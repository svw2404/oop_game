#include "App.hpp"

#include "HeadBodyCharacter.hpp"
#include "Util/Logger.hpp"

#include <algorithm>
#include <filesystem>
#include <unordered_map>

static std::unordered_map<HeadBodyCharacter*, glm::vec2> s_LastPos;

void App::Start() {
    LOG_TRACE("Start");

    // -------------------------------------------------------------------------
    // 1) 基本場景參數
    // -------------------------------------------------------------------------
    m_BackgroundOriginalSize = {2380.0f, 1760.0f};
    m_BackgroundDisplayedSize = {1244.16f, 699.84f};
    m_FireboyHitboxSize = {25.0f, 30.0f};

    // -------------------------------------------------------------------------
    // 2) 背景
    // -------------------------------------------------------------------------
    m_Background = std::make_shared<BackgroundImage>(
        GA_RESOURCE_DIR "/Image/Background/background0.png"
    );
    m_Background->SetPosition({0.0f, 0.0f});
    m_Root.AddChild(m_Background);

    // -------------------------------------------------------------------------
    // 3) 碰撞區塊（世界邊界 + 地形）
    // -------------------------------------------------------------------------
    m_SolidBlocks.clear();

    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 2380.0f, 45.0f));       // Roof
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 1715.0f, 2380.0f, 1760.0f));  // Floor
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 45.0f, 1760.0f));        // Left
    m_SolidBlocks.push_back(ImageRectToWorldRect(2335.0f, 0.0f, 2380.0f, 1760.0f));   // Right

    m_TestBlock = ImageRectToWorldRect(46.0f, 1466.0f, 784.0f, 1519.0f);
    m_SolidBlocks.push_back(m_TestBlock);

    const std::vector<RectObject> perimeterBlocks = {
        ImageRectToWorldRect(46.0f, 1216.0f, 426.0f, 1273.0f),
        ImageRectToWorldRect(481.0f, 1250.0f, 670.0f, 1273.0f),
        ImageRectToWorldRect(721.0f, 1223.0f, 1029.0f, 1273.0f),
        ImageRectToWorldRect(901.0f, 1273.0f, 1160.0f, 1395.0f),
        ImageRectToWorldRect(1036.0f, 1340.0f, 1475.0f, 1395.0f),
        ImageRectToWorldRect(1475.0f, 1342.0f, 1519.0f, 1395.0f),
        ImageRectToWorldRect(1519.0f, 1372.0f, 1704.0f, 1395.0f),
        ImageRectToWorldRect(1704.0f, 1342.0f, 1758.0f, 1395.0f),
        ImageRectToWorldRect(1758.0f, 1342.0f, 2063.0f, 1393.0f),
        ImageRectToWorldRect(2149.0f, 1578.0f, 2325.0f, 1704.0f),
        ImageRectToWorldRect(2208.0f, 1525.0f, 2321.0f, 1578.0f),
    };
    m_SolidBlocks.insert(m_SolidBlocks.end(), perimeterBlocks.begin(), perimeterBlocks.end());

    // -------------------------------------------------------------------------
    // 4) Fireboy
    // -------------------------------------------------------------------------
    {
        const std::string bodyDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Body";
        const std::string idleDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Idle";
        const std::string runDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Run";

        std::vector<std::string> bodyPaths;
        std::vector<std::string> idleHeadPaths;
        std::vector<std::string> runHeadPaths;

        try {
            for (const auto& entry : std::filesystem::directory_iterator(bodyDir)) {
                if (entry.is_regular_file()) {
                    bodyPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(idleDir)) {
                if (entry.is_regular_file()) {
                    idleHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(runDir)) {
                if (entry.is_regular_file()) {
                    runHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        std::sort(bodyPaths.begin(), bodyPaths.end());
        std::sort(idleHeadPaths.begin(), idleHeadPaths.end());
        std::sort(runHeadPaths.begin(), runHeadPaths.end());

        if (bodyPaths.empty()) {
            bodyPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Body/fireboy_body_00.png");
        }
        if (idleHeadPaths.empty()) {
            idleHeadPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Idle/fireboy_idle_00.png");
        }
        if (runHeadPaths.empty()) {
            runHeadPaths = idleHeadPaths;
        }

        std::vector<std::string> moveBodyPaths;
        if (bodyPaths.size() > 1) {
            moveBodyPaths.assign(bodyPaths.begin() + 1, bodyPaths.end());
        } else {
            moveBodyPaths.push_back(bodyPaths.front());
        }

        m_Fireboy = std::make_shared<HeadBodyCharacter>(
            moveBodyPaths, runHeadPaths, 10.0f, 120, true, 120, true
        );
        m_Fireboy->SetSize({50.0f, 60.0f});
        m_Fireboy->SetHeadScale(1.25f);
        m_Fireboy->SetHeadOffset({0.0f, -4.0f});

        m_Fireboy->SetIdleBodyImage(bodyPaths.front());
        m_Fireboy->SetIdleHeadImage(idleHeadPaths, 120, true);
        m_Fireboy->SetIdleState(true);

        if (m_SolidBlocks.size() >= 3) {
            const RectObject& floor = m_SolidBlocks[1];
            const RectObject& leftWall = m_SolidBlocks[2];

            const float floorTop = floor.center.y + (floor.size.y * 0.5f);
            const float wallRight = leftWall.center.x + (leftWall.size.x * 0.5f);

            glm::vec2 spawnPos;
            spawnPos.x = wallRight + (m_FireboyHitboxSize.x * 0.5f) + 5.0f;
            spawnPos.y = floorTop + (m_FireboyHitboxSize.y * 0.5f);

            m_Fireboy->SetPosition(spawnPos);
            s_LastPos[m_Fireboy.get()] = spawnPos;
        }

        m_Root.AddChild(m_Fireboy);
    }

    // -------------------------------------------------------------------------
    // 5) Diamond
    // -------------------------------------------------------------------------
    m_Diamond = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Character/RedDiamonds.png"
    );
    m_Diamond->SetZIndex(8);
    m_Diamond->SetSize({18.0f, 18.0f});

    const float blockTop = m_TestBlock.center.y + m_TestBlock.size.y * 0.5f;
    const float diamondY = blockTop + m_DiamondHitboxSize.y * 0.5f + 5.0f;
    m_Diamond->SetPosition({m_TestBlock.center.x, diamondY});
    m_Root.AddChild(m_Diamond);

    // -------------------------------------------------------------------------
    // 6) 初始狀態
    // -------------------------------------------------------------------------
    m_FireboyVelocity = {0.0f, 0.0f};
    m_FireboyOnGround = true;

    LOG_DEBUG("Background Size: {}x{}", m_BackgroundDisplayedSize.x, m_BackgroundDisplayedSize.y);
    LOG_DEBUG("Fireboy Hitbox: {}x{}", m_FireboyHitboxSize.x, m_FireboyHitboxSize.y);

    m_CurrentState = State::UPDATE;
}
