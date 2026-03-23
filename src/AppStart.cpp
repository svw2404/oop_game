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
    m_Slopes.clear();
    m_Hazards.clear();

    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 2380.0f, 45.0f));       // Roof
    // Left floor segment before the two lower depressions. Keep this as index 1
    // because spawn logic below expects m_SolidBlocks[1] to be floor.
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 1715.0f, 1107.0f, 1760.0f));  // Floor
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 45.0f, 1760.0f));        // Left
    m_SolidBlocks.push_back(ImageRectToWorldRect(2335.0f, 0.0f, 2380.0f, 1760.0f));   // Right

    m_TestBlock = ImageRectToWorldRect(46.0f, 1466.0f, 784.0f, 1519.0f);
    m_SolidBlocks.push_back(m_TestBlock);

    const std::vector<SolidRect> perimeterBlocks = {
        // Flat floor between / after the two lower depressions.
        ImageRectToWorldRect(1155.0f, 1743.0f, 1342.0f, 1760.0f),
        ImageRectToWorldRect(1382.0f, 1712.0f, 1604.0f, 1760.0f),
        ImageRectToWorldRect(1652.0f, 1743.0f, 1839.0f, 1760.0f),
        ImageRectToWorldRect(1879.0f, 1712.0f, 2380.0f, 1760.0f),

        // High upper block:
        // preserve the stepped/slope top silhouette, but keep the lower edge
        // simplified as support mass so collision stays stable.
        ImageRectToWorldRect(55.0f, 423.0f, 355.0f, 723.0f),
        ImageRectToWorldRect(351.0f, 672.0f, 426.0f, 723.0f),
        ImageRectToWorldRect(426.0f, 708.0f, 483.0f, 723.0f),
        ImageRectToWorldRect(483.0f, 704.0f, 668.0f, 723.0f),
        ImageRectToWorldRect(668.0f, 704.0f, 712.0f, 723.0f),
        ImageRectToWorldRect(712.0f, 664.0f, 1141.0f, 723.0f),
        ImageRectToWorldRect(1139.0f, 639.0f, 1187.0f, 723.0f),
        ImageRectToWorldRect(1183.0f, 605.0f, 1216.0f, 723.0f),
        ImageRectToWorldRect(1220.0f, 546.0f, 1641.0f, 723.0f),
        ImageRectToWorldRect(1641.0f, 670.0f, 1771.0f, 723.0f),
        ImageRectToWorldRect(1771.0f, 670.0f, 2069.0f, 788.0f),

        // Upper bridge block:
        // map the clear top silhouette as flats + slopes and keep the hanging
        // lower notch as support mass only.
        ImageRectToWorldRect(594.0f, 299.0f, 643.0f, 423.0f),
        ImageRectToWorldRect(643.0f, 236.0f, 784.0f, 423.0f),
        ImageRectToWorldRect(784.0f, 303.0f, 851.0f, 423.0f),
        ImageRectToWorldRect(851.0f, 301.0f, 914.0f, 423.0f),
        ImageRectToWorldRect(914.0f, 357.0f, 958.0f, 423.0f),
        ImageRectToWorldRect(958.0f, 357.0f, 1031.0f, 423.0f),
        ImageRectToWorldRect(729.0f, 423.0f, 1027.0f, 544.0f),
        ImageRectToWorldRect(1031.0f, 362.0f, 1536.0f, 423.0f),
        ImageRectToWorldRect(1536.0f, 395.0f, 1588.0f, 423.0f),
        ImageRectToWorldRect(1588.0f, 393.0f, 1771.0f, 423.0f),
        ImageRectToWorldRect(1771.0f, 393.0f, 1832.0f, 423.0f),
        ImageRectToWorldRect(1832.0f, 362.0f, 2319.0f, 423.0f),

        // Upper mega-platform block:
        // keep the walkable top as flats + explicit slopes, and use broad
        // support rectangles underneath so the shape behaves as one solid mass.
        ImageRectToWorldRect(300.0f, 912.0f, 1220.0f, 964.0f),
        ImageRectToWorldRect(1220.0f, 973.0f, 1275.0f, 1032.0f),
        ImageRectToWorldRect(1275.0f, 973.0f, 1468.0f, 1032.0f),
        ImageRectToWorldRect(1468.0f, 1015.0f, 1527.0f, 1032.0f),
        ImageRectToWorldRect(1527.0f, 1013.0f, 1712.0f, 1032.0f),
        ImageRectToWorldRect(1712.0f, 1013.0f, 1767.0f, 1032.0f),
        ImageRectToWorldRect(1767.0f, 973.0f, 2321.0f, 1216.0f),

        ImageRectToWorldRect(46.0f, 1216.0f, 426.0f, 1273.0f),
        ImageRectToWorldRect(481.0f, 1250.0f, 670.0f, 1273.0f),
        ImageRectToWorldRect(721.0f, 1223.0f, 1029.0f, 1273.0f),

        // Bridge into Slope 3 so there is no blank gap between the upper flat
        // ledge and the diagonal ramp start.
        ImageRectToWorldRect(1029.0f, 1223.0f, 1044.0f, 1273.0f),

        // Slope 3 cleanup:
        // keep the left plateau high, but lower the support under the diagonal.
        ImageRectToWorldRect(901.0f, 1273.0f, 1044.0f, 1395.0f),
        ImageRectToWorldRect(1044.0f, 1340.0f, 1160.0f, 1395.0f),

        // Main middle platform before slope 4.
        ImageRectToWorldRect(1036.0f, 1340.0f, 1475.0f, 1395.0f),

        // Wider transition cap into Slope 4. This prevents a fall-through when
        // walking off the long middle platform and starting the next diagonal.
        ImageRectToWorldRect(1475.0f, 1344.0f, 1494.0f, 1395.0f),

        // Slope 4 cleanup:
        // lower the under-slope support and delay the next flat top slightly.
        ImageRectToWorldRect(1475.0f, 1378.0f, 1529.0f, 1395.0f),
        ImageRectToWorldRect(1529.0f, 1372.0f, 1704.0f, 1395.0f),

        // Wider transition cap into Slope 5 for the same reason.
        ImageRectToWorldRect(1704.0f, 1374.0f, 1722.0f, 1395.0f),

        // Slope 5 cleanup:
        // lower the support under the slope, then start the upper flat top at the slope end.
        ImageRectToWorldRect(1704.0f, 1374.0f, 1762.0f, 1395.0f),
        ImageRectToWorldRect(1762.0f, 1342.0f, 2021.0f, 1393.0f),

        // Wider transition cap into Slope 6 plus a lower support strip below.
        ImageRectToWorldRect(2021.0f, 1344.0f, 2038.0f, 1399.0f),
        ImageRectToWorldRect(2021.0f, 1393.0f, 2067.0f, 1399.0f),

        // Slope 7 cleanup:
        // keep support under most of the ramp, but stop it a little before the
        // top endpoint so coming back down from the right does not hit a wall
        // before entering the slope surface.
        ImageRectToWorldRect(2138.0f, 1586.0f, 2188.0f, 1704.0f),
        ImageRectToWorldRect(2197.0f, 1525.0f, 2325.0f, 1704.0f),
    };
    m_SolidBlocks.insert(m_SolidBlocks.end(), perimeterBlocks.begin(), perimeterBlocks.end());

    m_Slopes = {
        // Slope 1: leftmost upper-left small connector
        {ImagePointToWorldPoint(422.0f, 1216.0f), ImagePointToWorldPoint(479.0f, 1250.0f)},
        // Slope 2: upper-left small connector just right of Slope 1
        {ImagePointToWorldPoint(670.0f, 1254.0f), ImagePointToWorldPoint(721.0f, 1225.0f)},
        // Slope 3: middle-left long diagonal ramp
        {ImagePointToWorldPoint(1044.0f, 1223.0f), ImagePointToWorldPoint(1155.0f, 1340.0f)},
        // Slope 4: center small connector
        {ImagePointToWorldPoint(1477.0f, 1344.0f), ImagePointToWorldPoint(1529.0f, 1378.0f)},
        // Slope 5: middle-right small connector
        {ImagePointToWorldPoint(1712.0f, 1374.0f), ImagePointToWorldPoint(1762.0f, 1344.0f)},
        // Slope 6: rightmost lower connector
        {ImagePointToWorldPoint(2021.0f, 1344.0f), ImagePointToWorldPoint(2067.0f, 1397.0f)},
        // Slope 7: uppermost top-right raised block ramp
        {ImagePointToWorldPoint(2138.0f, 1586.0f), ImagePointToWorldPoint(2197.0f, 1525.0f)},
        // Slope 8: lower-left depression left wall
        {ImagePointToWorldPoint(1107.0f, 1714.0f), ImagePointToWorldPoint(1155.0f, 1743.0f)},
        // Slope 9: lower-left depression right wall
        {ImagePointToWorldPoint(1342.0f, 1747.0f), ImagePointToWorldPoint(1382.0f, 1714.0f)},
        // Slope 10: lower-right depression left wall
        {ImagePointToWorldPoint(1604.0f, 1712.0f), ImagePointToWorldPoint(1652.0f, 1741.0f)},
        // Slope 11: lower-right depression right wall
        {ImagePointToWorldPoint(1839.0f, 1745.0f), ImagePointToWorldPoint(1879.0f, 1712.0f)},
        // Slope 12: upper mega-platform left connector
        {ImagePointToWorldPoint(1220.0f, 912.0f), ImagePointToWorldPoint(1275.0f, 973.0f)},
        // Slope 13: upper mega-platform middle connector
        {ImagePointToWorldPoint(1468.0f, 977.0f), ImagePointToWorldPoint(1527.0f, 1015.0f)},
        // Slope 14: upper mega-platform right connector
        {ImagePointToWorldPoint(1712.0f, 1013.0f), ImagePointToWorldPoint(1767.0f, 975.0f)},
        // Slope 15: high upper block left-middle dip
        {ImagePointToWorldPoint(426.0f, 672.0f), ImagePointToWorldPoint(483.0f, 708.0f)},
        // Slope 16: high upper block middle rise
        {ImagePointToWorldPoint(668.0f, 704.0f), ImagePointToWorldPoint(712.0f, 670.0f)},
        // Slope 17: high upper block long right descent
        {ImagePointToWorldPoint(1641.0f, 546.0f), ImagePointToWorldPoint(1771.0f, 670.0f)},
        // Slope 18: upper bridge left rise
        {ImagePointToWorldPoint(594.0f, 299.0f), ImagePointToWorldPoint(643.0f, 236.0f)},
        // Slope 19: upper bridge left-center drop
        {ImagePointToWorldPoint(784.0f, 238.0f), ImagePointToWorldPoint(851.0f, 303.0f)},
        // Slope 20: upper bridge middle drop
        {ImagePointToWorldPoint(914.0f, 301.0f), ImagePointToWorldPoint(958.0f, 357.0f)},
        // Slope 21: upper bridge right-center drop
        {ImagePointToWorldPoint(1536.0f, 366.0f), ImagePointToWorldPoint(1588.0f, 395.0f)},
        // Slope 22: upper bridge right rise
        {ImagePointToWorldPoint(1771.0f, 393.0f), ImagePointToWorldPoint(1832.0f, 364.0f)},
    };

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
            const SolidRect& floor = m_SolidBlocks[1];
            const SolidRect& leftWall = m_SolidBlocks[2];

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
