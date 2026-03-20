#include "App.hpp"

#include "Util/Logger.hpp"

#include "HeadBodyCharacter.hpp"

#include <filesystem>

#include <algorithm>

static std::unordered_map<HeadBodyCharacter*, glm::vec2> s_LastPos;
const float IDLE_EPSILON = 0.5f;

void App::Start() {

    LOG_TRACE("Start");
    m_BackgroundOriginalSize = { 2380.0f, 1760.0f };
    m_BackgroundDisplayedSize = { 1244.16f, 699.84f };
    m_FireboyHitboxSize = { 25.0f, 30.0f };


    m_Background = std::make_shared<BackgroundImage>(

        GA_RESOURCE_DIR "/Image/Background/background0.png"

    );

    m_Background->SetPosition({ 0.0f, 0.0f });

    m_Root.AddChild(m_Background);



    m_SolidBlocks.clear();



    // World borders from Gemini image:

    // floor, roof, left wall, right wall are all 45 px thick in ORIGINAL image coordinates



    // Roof: y = 0 to 45

    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 2380.0f, 45.0f));



    // Floor: y = 1715 to 1760

    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 1715.0f, 2380.0f, 1760.0f));



    // Left wall: x = 0 to 45

    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 45.0f, 1760.0f));



    // Right wall: x = 2335 to 2380

    m_SolidBlocks.push_back(ImageRectToWorldRect(2335.0f, 0.0f, 2380.0f, 1760.0f));



    // Main uneven ground mass from your image-map polygon.

    // We keep runtime collision rectangular by decomposing the perimeter

    // into a handful of solid blocks that approximate the traced shape.

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



    {

        const std::string bodyDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Body";

        const std::string idleDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Idle";



        std::vector<std::string> idlePaths;

        std::vector<std::string> bodyPaths;



        try {

            for (const auto& entry : std::filesystem::directory_iterator(idleDir)) {

                if (!entry.is_regular_file()) continue;

                idlePaths.push_back(entry.path().string());

            }

        }
        catch (const std::exception&) {

            // fallback below

        }



        try {

            for (const auto& entry : std::filesystem::directory_iterator(bodyDir)) {

                if (!entry.is_regular_file()) continue;

                bodyPaths.push_back(entry.path().string());

            }

        }
        catch (const std::exception&) {

            // fallback below

        }



        std::sort(idlePaths.begin(), idlePaths.end());

        std::sort(bodyPaths.begin(), bodyPaths.end());



        if (idlePaths.empty()) {

            idlePaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Idle/fireboy_idle_00.png");

        }



        if (bodyPaths.empty()) {

            // 若沒有多張 body，退回單張

            bodyPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Body/fireboy_body_00.png");

        }



        // 建立支援 body + head 動畫的角色

        m_Fireboy = std::make_shared<HeadBodyCharacter>(bodyPaths, idlePaths, 10.0f, 120, true, 120, true);



        // 設定尺寸、位置並加入場景

        m_Fireboy->SetSize({ 50.0f, 60.0f });    // 設定 body 大小（會觸發 head 自動縮放）

        m_Fireboy->SetHeadScale(1.25f);          // 讓頭比預設再大 25%

        m_Fireboy->SetHeadOffset({ 0.0f, -4.0f });// 微調頭的位置



        // 假設 m_SolidBlocks[1] 是地板，m_SolidBlocks[2] 是左牆
        float spawnX = m_SolidBlocks[2].center.x + (m_SolidBlocks[2].size.x * 0.5f) + (m_FireboyHitboxSize.x * 0.5f) + 10.0f;
        float spawnY = m_SolidBlocks[1].center.y + (m_SolidBlocks[1].size.y * 0.5f) + (m_FireboyHitboxSize.y * 0.5f);

        m_Fireboy->SetPosition({ spawnX, spawnY });
        s_LastPos[m_Fireboy.get()] = { spawnX, spawnY }; // 記得還是要同步這行給 Update 判定



        m_Root.AddChild(m_Fireboy);

        if (m_SolidBlocks.size() >= 3) {
            const RectObject& floor = m_SolidBlocks[1];     // 地板
            const RectObject& leftWall = m_SolidBlocks[2];  // 左牆

            float floorTop = floor.center.y + (floor.size.y * 0.5f);
            float wallRight = leftWall.center.x + (leftWall.size.x * 0.5f);

            glm::vec2 spawnPos;
            spawnPos.x = wallRight + (m_FireboyHitboxSize.x * 0.5f) + 5.0f;
            spawnPos.y = floorTop + (m_FireboyHitboxSize.y * 0.5f);

            // 2. 同步設定位置與紀錄
            m_Fireboy->SetPosition(spawnPos);

            // 解決「未定義」：確保這行執行時，s_LastPos 的定義已經在 App.cpp 的最上方
            s_LastPos[m_Fireboy.get()] = spawnPos;

            LOG_DEBUG("SpawnPos Calculated: x={}, y={}", spawnPos.x, spawnPos.y);
        }

        // 3. 設定動畫與狀態
        m_Fireboy->SetIdleBodyImage(bodyPaths.front());
        m_Fireboy->SetIdleState(true);

    }



    m_Diamond = std::make_shared<Character>(

        GA_RESOURCE_DIR "/Image/Character/RedDiamonds.png"

    );

    m_Diamond->SetZIndex(8);

    m_Diamond->SetSize({ 18.0f, 18.0f });



    // Place diamond on top of the invisible platform block

    float blockTop = m_TestBlock.center.y + m_TestBlock.size.y * 0.5f;

    float diamondY = blockTop + m_DiamondHitboxSize.y * 0.5f + 5.0f;



    m_Diamond->SetPosition({ m_TestBlock.center.x, diamondY });

    m_Root.AddChild(m_Diamond);



    m_FireboyVelocity = { 0.0f, 0.0f };

    m_FireboyOnGround = true;


    LOG_DEBUG("Background Size: {}x{}", m_BackgroundDisplayedSize.x, m_BackgroundDisplayedSize.y);
    LOG_DEBUG("Fireboy Hitbox: {}x{}", m_FireboyHitboxSize.x, m_FireboyHitboxSize.y);

    m_CurrentState = State::UPDATE;



}
