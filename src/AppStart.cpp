#include "App.hpp"

#include "HeadBodyCharacter.hpp"
#include "Util/Animation.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

#include <algorithm>
#include <filesystem>
#include <unordered_map>

static std::unordered_map<HeadBodyCharacter*, glm::vec2> s_LastPos;

// TODO(Roadmap):
// - Build the real front-end flow: intro screen, home/menu, options, sound,
//   credits, and pause UI, based on the original game's structure.
// - Add a level-select world map with connected-path progression and 5 total
//   levels represented by emerald/gem nodes.
// - Prepare 5 clean level backgrounds without baked gameplay props, then place
//   doors, liquids, diamonds, switches, cubes, and platforms in code.
// - Reposition and recenter our diamonds to match the original Level 1 layout
//   more closely after comparing against gameplay footage.
// - Clean the liquid assets by trimming the side edges so no thin line floats
//   above the floor.
// - Clean the door assets by trimming the screenshot borders on both sides.
// - Add full game-feel polish: music, SFX, pause/home/restart/mute controls,
//   stored progression, stored rank, and stored diamond totals.
// - Keep polishing gameplay edge cases: remaining slope seams, cube-on-slope
//   behavior, door trigger feel, liquid death feel, and z-order/hitbox cleanup.
// - Decide whether to preserve the current controls or switch to the original
//   default mapping (Fireboy arrows, Watergirl A/W/D).

void App::Start() {
    LOG_TRACE("Start");
    // Rebuild the whole scene graph on every Start() so restarting the level
    // creates a fresh world instead of stacking duplicate drawables.
    m_Root = Util::Renderer();

    // -------------------------------------------------------------------------
    // 1) 基本場景參數
    // -------------------------------------------------------------------------
    m_BackgroundOriginalSize = {2380.0f, 1760.0f};
    m_BackgroundDisplayedSize = {1244.16f, 699.84f};

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
    m_CeilingSlopes.clear();
    m_Hazards.clear();
    m_LevelProps.clear();
    m_AnimatedLevelProps.clear();
    m_FailOverlayObjects.clear();
    m_Diamonds.clear();
    m_GreenButton.reset();
    m_GreenButtonAfter.reset();
    m_GreenSwitch.reset();
    m_Cube.reset();
    m_HasGreenPlatformBlock = false;
    m_HasCubeBlock = false;
    m_GreenButtonPressed = false;
    m_GreenSwitchOn = false;
    m_GreenSwitchTouchLatch = false;
    m_GreenButtonAfterPressVisual = 0.0f;
    m_FireDiamondsCollected = 0;
    m_WaterDiamondsCollected = 0;
    m_GreenDiamondsCollected = 0;
    m_FireDiamondsTotal = 0;
    m_WaterDiamondsTotal = 0;
    m_GreenDiamondsTotal = 0;
    m_CubeVelocity = {0.0f, 0.0f};
    m_FireboyDeathStartScale = {1.0f, 1.0f};
    m_WatergirlDeathStartScale = {1.0f, 1.0f};
    m_FireboyVictoryStartPosition = {0.0f, 0.0f};
    m_WatergirlVictoryStartPosition = {0.0f, 0.0f};
    m_FireboyVictoryStartScale = {1.0f, 1.0f};
    m_WatergirlVictoryStartScale = {1.0f, 1.0f};
    m_CubeOnGround = false;
    m_FireboyDeathTimer = 0.0f;
    m_WatergirlDeathTimer = 0.0f;
    m_FireboyCeilingCarryTimer = 0.0f;
    m_WatergirlCeilingCarryTimer = 0.0f;
    m_FireboyCeilingCarrySpeed = 0.0f;
    m_WatergirlCeilingCarrySpeed = 0.0f;
    m_FireboyDoor = {};
    m_WatergirlDoor = {};
    m_VictoryPhase = VictoryPhase::None;
    m_VictoryTimer = 0.0f;
    m_VictoryCelebrateDuration = 0.0f;
    m_FailOverlayVisible = false;
    m_FailOverlayMouseLatch = false;
    m_FailOverlayPanel.reset();
    m_FailOverlayTitle.reset();
    m_FailRestartButton = {};
    m_FailHomeButton = {};
    m_FailExitButton = {};
    m_VictoryOverlayVisible = false;
    m_VictoryOverlayPanel.reset();
    m_VictoryOverlayTitle.reset();
    m_VictoryTimeText.reset();
    m_VictoryBlueDiamondIcon.reset();
    m_VictoryRedDiamondIcon.reset();
    m_VictoryBlueDiamondText.reset();
    m_VictoryRedDiamondText.reset();
    m_VictoryRankText.reset();
    m_VictoryContinueButton = {};
    m_VictoryOverlayObjects.clear();
    m_LevelStartTimeMs = 0.0f;
    m_LevelCompleteTimeMs = 0.0f;
    m_GreenButtonAfterBaseSize = {0.0f, 0.0f};

    // Base collision setup:
    // - m_SolidBlocks stores the terrain mass used by AABB collision.
    // - m_Slopes stores only the smooth walkable diagonals.
    // Together they let us fake irregular temple geometry with stable runtime
    // collision while still preserving smooth ramps for characters/cube.
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 2380.0f, 45.0f));       // Roof
    // Left floor segment before the two lower depressions. Spawn logic now
    // stores explicit references to this floor/wall instead of depending on
    // vector indices staying fixed while the level is remapped.
    m_LeftFloorSpawnRect = ImageRectToWorldRect(0.0f, 1715.0f, 1107.0f, 1760.0f);
    m_SolidBlocks.push_back(m_LeftFloorSpawnRect);                                     // Floor
    m_LeftWallSpawnRect = ImageRectToWorldRect(0.0f, 0.0f, 45.0f, 1760.0f);
    m_SolidBlocks.push_back(m_LeftWallSpawnRect);                                      // Left
    m_SolidBlocks.push_back(ImageRectToWorldRect(2335.0f, 0.0f, 2380.0f, 1760.0f));   // Right

    m_TestBlock = ImageRectToWorldRect(46.0f, 1466.0f, 784.0f, 1519.0f);
    m_SolidBlocks.push_back(m_TestBlock);

    // Hand-authored level decomposition for Level 1.
    // Most terrain is approximated with rectangles, then the visible diagonal
    // connectors are added separately as slope surfaces later in this file.
    const std::vector<SolidRect> perimeterBlocks = {
        // Flat floor between / after the two lower depressions.
        ImageRectToWorldRect(1155.0f, 1743.0f, 1342.0f, 1760.0f),
        ImageRectToWorldRect(1382.0f, 1714.0f, 1392.0f, 1760.0f),
        ImageRectToWorldRect(1390.0f, 1712.0f, 1594.0f, 1760.0f),
        ImageRectToWorldRect(1592.0f, 1714.0f, 1604.0f, 1760.0f),
        ImageRectToWorldRect(1652.0f, 1743.0f, 1839.0f, 1760.0f),
        ImageRectToWorldRect(1879.0f, 1714.0f, 1889.0f, 1760.0f),
        ImageRectToWorldRect(1887.0f, 1712.0f, 2380.0f, 1760.0f),

        // High upper block:
        // preserve the stepped/slope top silhouette, but keep the lower edge
        // simplified as support mass so collision stays stable.
        ImageRectToWorldRect(55.0f, 423.0f, 355.0f, 723.0f),
        ImageRectToWorldRect(351.0f, 672.0f, 418.0f, 723.0f),
        ImageRectToWorldRect(418.0f, 688.0f, 426.0f, 723.0f, false),
        ImageRectToWorldRect(426.0f, 708.0f, 483.0f, 723.0f, false),
        ImageRectToWorldRect(483.0f, 704.0f, 493.0f, 723.0f, false),
        ImageRectToWorldRect(491.0f, 704.0f, 668.0f, 723.0f),
        ImageRectToWorldRect(668.0f, 704.0f, 712.0f, 723.0f, false),
        ImageRectToWorldRect(712.0f, 664.0f, 1141.0f, 723.0f),
        ImageRectToWorldRect(1139.0f, 639.0f, 1187.0f, 723.0f),
        ImageRectToWorldRect(1183.0f, 605.0f, 1216.0f, 723.0f),
        ImageRectToWorldRect(1220.0f, 546.0f, 1641.0f, 723.0f),
        ImageRectToWorldRect(1641.0f, 670.0f, 1771.0f, 723.0f, false),
        ImageRectToWorldRect(1771.0f, 670.0f, 2069.0f, 788.0f),

        // Upper bridge block:
        // map the clear top silhouette as flats + slopes and keep the hanging
        // lower notch as support mass only.
        ImageRectToWorldRect(594.0f, 299.0f, 643.0f, 423.0f, false),
        ImageRectToWorldRect(643.0f, 236.0f, 776.0f, 423.0f, false),
        ImageRectToWorldRect(776.0f, 255.0f, 784.0f, 423.0f, false),
        ImageRectToWorldRect(784.0f, 303.0f, 851.0f, 423.0f, false),
        ImageRectToWorldRect(851.0f, 301.0f, 861.0f, 423.0f, false),
        ImageRectToWorldRect(859.0f, 301.0f, 914.0f, 423.0f),
        ImageRectToWorldRect(914.0f, 357.0f, 958.0f, 423.0f, false),
        ImageRectToWorldRect(958.0f, 357.0f, 1031.0f, 423.0f),
        ImageRectToWorldRect(729.0f, 423.0f, 1027.0f, 544.0f),
        ImageRectToWorldRect(1031.0f, 362.0f, 1528.0f, 423.0f),
        ImageRectToWorldRect(1528.0f, 378.0f, 1536.0f, 423.0f, false),
        ImageRectToWorldRect(1536.0f, 395.0f, 1588.0f, 423.0f, false),
        ImageRectToWorldRect(1588.0f, 393.0f, 1598.0f, 423.0f, false),
        ImageRectToWorldRect(1596.0f, 393.0f, 1771.0f, 423.0f),
        ImageRectToWorldRect(1771.0f, 393.0f, 1832.0f, 423.0f, false),
        ImageRectToWorldRect(1832.0f, 362.0f, 2319.0f, 423.0f),

        // Upper mega-platform block:
        // keep the walkable top as flats + explicit slopes, and use broad
        // support rectangles underneath so the shape behaves as one solid mass.
        ImageRectToWorldRect(300.0f, 912.0f, 1097.0f, 964.0f),
        ImageRectToWorldRect(1097.0f, 912.0f, 1151.0f, 1030.0f, false),
        ImageRectToWorldRect(1151.0f, 912.0f, 1220.0f, 1032.0f),
        ImageRectToWorldRect(1220.0f, 973.0f, 1275.0f, 1032.0f, false),
        ImageRectToWorldRect(1275.0f, 973.0f, 1460.0f, 1032.0f),
        ImageRectToWorldRect(1460.0f, 992.0f, 1468.0f, 1032.0f, false),
        ImageRectToWorldRect(1468.0f, 1015.0f, 1527.0f, 1032.0f, false),
        ImageRectToWorldRect(1527.0f, 1013.0f, 1537.0f, 1032.0f, false),
        ImageRectToWorldRect(1535.0f, 1013.0f, 1712.0f, 1032.0f),
        ImageRectToWorldRect(1712.0f, 1013.0f, 1767.0f, 1032.0f, false),
        ImageRectToWorldRect(1767.0f, 973.0f, 1958.0f, 1032.0f),
        ImageRectToWorldRect(1958.0f, 973.0f, 2014.0f, 1090.0f, false),
        ImageRectToWorldRect(2014.0f, 973.0f, 2082.0f, 1093.0f),
        ImageRectToWorldRect(2082.0f, 973.0f, 2193.0f, 1214.0f, false),
        ImageRectToWorldRect(2193.0f, 973.0f, 2321.0f, 1216.0f),

        ImageRectToWorldRect(46.0f, 1216.0f, 426.0f, 1273.0f),
        ImageRectToWorldRect(481.0f, 1250.0f, 670.0f, 1273.0f),
        ImageRectToWorldRect(721.0f, 1223.0f, 1029.0f, 1273.0f),

        // Bridge into Slope 3 so there is no blank gap between the upper flat
        // ledge and the diagonal ramp start.
        ImageRectToWorldRect(1029.0f, 1223.0f, 1044.0f, 1273.0f),

        // Slope 3 cleanup:
        // keep the left plateau high, but lower the support under the diagonal.
        ImageRectToWorldRect(901.0f, 1273.0f, 910.0f, 1395.0f),
        ImageRectToWorldRect(910.0f, 1273.0f, 1032.0f, 1399.0f, false),
        ImageRectToWorldRect(1032.0f, 1273.0f, 1044.0f, 1395.0f),
        ImageRectToWorldRect(1044.0f, 1340.0f, 1160.0f, 1395.0f, false),

        // Main middle platform before slope 4.
        ImageRectToWorldRect(1036.0f, 1340.0f, 1469.0f, 1395.0f),
        ImageRectToWorldRect(1469.0f, 1360.0f, 1477.0f, 1395.0f, false),

        // Wider transition cap into Slope 4. This prevents a fall-through when
        // walking off the long middle platform and starting the next diagonal.
        ImageRectToWorldRect(1475.0f, 1344.0f, 1494.0f, 1395.0f, false),

        // Slope 4 cleanup:
        // lower the under-slope support and delay the next flat top slightly.
        ImageRectToWorldRect(1475.0f, 1378.0f, 1529.0f, 1395.0f, false),
        ImageRectToWorldRect(1529.0f, 1372.0f, 1539.0f, 1395.0f, false),
        ImageRectToWorldRect(1537.0f, 1372.0f, 1704.0f, 1395.0f),

        // Wider transition cap into Slope 5 for the same reason.
        ImageRectToWorldRect(1704.0f, 1374.0f, 1722.0f, 1395.0f, false),

        // Slope 5 cleanup:
        // lower the support under the slope, then start the upper flat top at the slope end.
        ImageRectToWorldRect(1704.0f, 1374.0f, 1762.0f, 1395.0f, false),
        ImageRectToWorldRect(1762.0f, 1342.0f, 2021.0f, 1393.0f),

        // Wider transition cap into Slope 6 plus a lower support strip below.
        ImageRectToWorldRect(2021.0f, 1344.0f, 2038.0f, 1399.0f, false),
        ImageRectToWorldRect(2021.0f, 1393.0f, 2067.0f, 1399.0f, false),

        // Slope 7 cleanup:
        // keep support under most of the ramp, but stop it a little before the
        // top endpoint so coming back down from the right does not hit a wall
        // before entering the slope surface.
        ImageRectToWorldRect(2138.0f, 1586.0f, 2188.0f, 1704.0f, false),
        ImageRectToWorldRect(2197.0f, 1525.0f, 2325.0f, 1704.0f),
    };
    m_SolidBlocks.insert(m_SolidBlocks.end(), perimeterBlocks.begin(), perimeterBlocks.end());

    // Moving platform setup:
    // the platform is a real solid block inside m_SolidBlocks, while the sprite
    // is just the visual. Pressing either green button moves the block down.
    // The platform travels vertically through the hand-mapped passage
    // x=55..292, y=904..1225. It starts high to block the route, then lowers
    // while the nearby pressure button is held.
    m_GreenPlatformRestRect = ImageRectToWorldRect(55.0f, 904.0f, 292.0f, 1004.0f);
    m_GreenPlatformPressedRect = ImageRectToWorldRect(55.0f, 1125.0f, 292.0f, 1225.0f);
    m_GreenPlatformCurrentRect = m_GreenPlatformRestRect;
    m_GreenPlatformBlockIndex = m_SolidBlocks.size();
    m_HasGreenPlatformBlock = true;
    m_SolidBlocks.push_back(m_GreenPlatformCurrentRect);

    m_GreenPlatform = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/platform_green.png"
    );
    m_GreenPlatform->SetZIndex(7);
    m_GreenPlatform->SetSize({
        m_GreenPlatformCurrentRect.size.x,
        m_GreenPlatformCurrentRect.size.x * (107.0f / 333.0f),
    });
    m_GreenPlatform->SetPosition(m_GreenPlatformCurrentRect.center);
    m_Root.AddChild(m_GreenPlatform);

    // Control anchors for the green platform:
    // - the first mapped control is now a toggle switch
    // - the second mapped control stays a pressure button
    m_GreenSwitchHitbox = ImageRectToWorldRect(1180.0f, 1280.0f, 1345.0f, 1340.0f);
    m_GreenButtonAfterHitbox = ImageRectToWorldRect(365.0f, 850.0f, 455.0f, 912.0f);

    // -------------------------------------------------------------------------
    // 3.5) Level 1 prop pass
    // -------------------------------------------------------------------------
    // Everything here is visual or gameplay dressing placed on top of the
    // terrain data: hazards, doors, switch, cube, and collectible diamonds.
    const float scaleX = m_BackgroundDisplayedSize.x / m_BackgroundOriginalSize.x;
    const float scaleY = m_BackgroundDisplayedSize.y / m_BackgroundOriginalSize.y;

    auto imageSizeToWorldSize = [&](float imageWidth, float imageHeight, float scale = 1.0f) {
        return glm::vec2{imageWidth * scaleX * scale, imageHeight * scaleY * scale};
    };

    // Helper for static props whose intended authoring rule is "sit on this
    // floor Y in image space". We convert image coordinates into world space
    // and anchor the sprite by its bottom edge.
    auto addPropAtBottom = [&](const std::string& path,
                               float centerXImage,
                               float bottomYImage,
                               float imageWidth,
                               float imageHeight,
                               float zIndex,
                               float scale = 1.0f) {
        auto prop = std::make_shared<Character>(path);
        const glm::vec2 size = imageSizeToWorldSize(imageWidth, imageHeight, scale);
        glm::vec2 pos = ImagePointToWorldPoint(centerXImage, bottomYImage);
        pos.y += size.y * 0.5f;
        prop->SetZIndex(zIndex);
        prop->SetSize(size);
        prop->SetPosition(pos);
        m_Root.AddChild(prop);
        m_LevelProps.push_back(prop);
        return prop;
    };

    // Helper for animated liquid pools inside trapezoid-like depressions.
    // Visuals use a wider overlay so the sprite covers the baked background
    // art, while gameplay uses tighter hazard bands for fair collision.
    auto addAnimatedHazardInImageTrap = [&](const std::vector<std::string>& paths,
                                            HazardRect::Type type,
                                            float assetWidthImage,
                                            float assetHeightImage,
                                            float topLeftXImage,
                                            float topLeftYImage,
                                            float topRightXImage,
                                            float topRightYImage,
                                            float bottomLeftXImage,
                                            float bottomLeftYImage,
                                            float bottomRightXImage,
                                            float bottomRightYImage,
                                            float zIndex) {
        auto hazardAnimation = std::make_shared<Util::Animation>(paths, true, 120, true, 0);
        const float topWidthImage = std::abs(topRightXImage - topLeftXImage);
        const float bottomWidthImage = std::abs(bottomRightXImage - bottomLeftXImage);
        const float topYImage = (topLeftYImage + topRightYImage) * 0.5f;
        const float bottomYImage = (bottomLeftYImage + bottomRightYImage) * 0.5f;
        const float imageHeight = std::abs(bottomYImage - topYImage);
        const float overlayWidthImage = topWidthImage + std::min(36.0f, topWidthImage * 0.13f);
        const float overlayHeightImage =
            overlayWidthImage * (assetHeightImage / assetWidthImage) * 0.39f;
        const glm::vec2 size = imageSizeToWorldSize(overlayWidthImage, overlayHeightImage);
        hazardAnimation->SetSize(size);

        const float centerXImage =
            (topLeftXImage + topRightXImage + bottomLeftXImage + bottomRightXImage) * 0.25f;
        glm::vec2 pos = ImagePointToWorldPoint(centerXImage, bottomYImage);
        pos.y += size.y * 0.5f;

        auto hazardObject = std::make_shared<Util::GameObject>(hazardAnimation, zIndex);
        hazardObject->m_Transform.translation = pos;
        m_Root.AddChild(hazardObject);
        m_AnimatedLevelProps.push_back(hazardObject);

        const float surfaceInset = std::min(6.0f, topWidthImage * 0.03f);
        const float bodyInset = std::min(6.0f, bottomWidthImage * 0.04f);
        const float upperBandBottomY = topYImage + imageHeight * 0.42f;

        HazardRect surfaceHazard;
        {
            const SolidRect rect = ImageRectToWorldRect(
                std::min(topLeftXImage, topRightXImage) + surfaceInset,
                topYImage,
                std::max(topLeftXImage, topRightXImage) - surfaceInset,
                upperBandBottomY,
                false
            );
            surfaceHazard.center = rect.center;
            surfaceHazard.size = rect.size;
            surfaceHazard.type = type;
            m_Hazards.push_back(surfaceHazard);
        }

        HazardRect bodyHazard;
        {
            const SolidRect rect = ImageRectToWorldRect(
                std::min(bottomLeftXImage, bottomRightXImage) + bodyInset,
                upperBandBottomY - 1.0f,
                std::max(bottomLeftXImage, bottomRightXImage) - bodyInset,
                bottomYImage,
                false
            );
            bodyHazard.center = rect.center;
            bodyHazard.size = rect.size;
            bodyHazard.type = type;
            m_Hazards.push_back(bodyHazard);
        }

        return hazardObject;
    };

    // Diamonds are scene props with independent collection logic. They are not
    // terrain colliders and can be color-restricted per character.
    auto addCollectibleDiamond = [&](const std::string& path,
                                     DiamondType type,
                                     float centerXImage,
                                     float centerYImage,
                                     float imageWidth,
                                     float imageHeight,
                                     float zIndex,
                                     float scale = 1.0f,
                                     bool required = false) {
        auto diamond = std::make_shared<Character>(path);
        const glm::vec2 size = imageSizeToWorldSize(imageWidth, imageHeight, scale);
        diamond->SetZIndex(zIndex);
        diamond->SetSize(size);
        diamond->SetPosition(ImagePointToWorldPoint(centerXImage, centerYImage));
        m_Root.AddChild(diamond);
        m_Diamonds.push_back({diamond, type, required, false});

        switch (type) {
        case DiamondType::Fire:
            ++m_FireDiamondsTotal;
            break;
        case DiamondType::Water:
            ++m_WaterDiamondsTotal;
            break;
        case DiamondType::Neutral:
            ++m_GreenDiamondsTotal;
            break;
        }

        return diamond;
    };

    const float propScale = 1.0f / 1.5f;
    const float greenButtonVisualScale = (propScale * 0.82f) / 1.5f;
    const glm::vec2 greenButtonSize = imageSizeToWorldSize(189.0f, 120.0f, greenButtonVisualScale);
    const float diamondScale = propScale * 0.90f;
    const std::vector<std::string> lavaPaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_04.png",
    };
    const std::vector<std::string> waterPaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/water_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/water_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/water_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/water_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/water_04.png",
    };
    const std::vector<std::string> venomPaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_04.png",
    };

    // Animated hazard pools mapped from the hand-traced image regions.
    // These are the current runtime lava/water zones used by wrong-element death.
    addAnimatedHazardInImageTrap(
        lavaPaths,
        HazardRect::Type::Lava,
        177.0f, 61.0f,
        436.0f, 1225.0f,
        715.0f, 1223.0f,
        490.0f, 1255.0f,
        669.0f, 1255.0f,
        10.2f
    );
    addAnimatedHazardInImageTrap(
        lavaPaths,
        HazardRect::Type::Lava,
        177.0f, 61.0f,
        426.0f, 673.0f,
        705.0f, 671.0f,
        480.0f, 703.0f,
        659.0f, 703.0f,
        10.2f
    );
    addAnimatedHazardInImageTrap(
        lavaPaths,
        HazardRect::Type::Lava,
        177.0f, 61.0f,
        1110.0f, 1714.0f,
        1389.0f, 1712.0f,
        1164.0f, 1744.0f,
        1343.0f, 1744.0f,
        10.2f
    );
    addAnimatedHazardInImageTrap(
        waterPaths,
        HazardRect::Type::Water,
        178.0f, 60.0f,
        1542.0f, 364.0f,
        1821.0f, 362.0f,
        1596.0f, 394.0f,
        1775.0f, 394.0f,
        10.2f
    );
    addAnimatedHazardInImageTrap(
        waterPaths,
        HazardRect::Type::Water,
        178.0f, 60.0f,
        1473.0f, 979.0f,
        1752.0f, 977.0f,
        1527.0f, 1009.0f,
        1706.0f, 1009.0f,
        10.2f
    );
    addAnimatedHazardInImageTrap(
        venomPaths,
        HazardRect::Type::Venom,
        178.0f, 60.0f,
        1481.0f, 1343.0f,
        1760.0f, 1341.0f,
        1535.0f, 1373.0f,
        1714.0f, 1373.0f,
        10.2f
    );
    addAnimatedHazardInImageTrap(
        waterPaths,
        HazardRect::Type::Water,
        178.0f, 60.0f,
        1601.0f, 1713.0f,
        1880.0f, 1711.0f,
        1655.0f, 1743.0f,
        1834.0f, 1743.0f,
        10.2f
    );

    m_GreenSwitch = addPropAtBottom(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/switch_green_off.png",
        1262.5f, 1340.0f, 148.0f, 124.0f, 7.6f, propScale
    );
    const glm::vec2 switchSpriteSize = imageSizeToWorldSize(148.0f, 124.0f, propScale);
    m_GreenSwitchHitbox.center = m_GreenSwitch->GetPosition() + glm::vec2{
        -switchSpriteSize.x * 0.01f,
        -switchSpriteSize.y * 0.10f,
    };
    m_GreenSwitchHitbox.size = {
        switchSpriteSize.x * 0.72f,
        switchSpriteSize.y * 0.62f,
    };

    m_GreenButtonAfter = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/button_green.png"
    );
    m_GreenButtonAfter->SetZIndex(7.0f);
    m_GreenButtonAfter->SetSize(greenButtonSize);
    m_GreenButtonAfterBasePosition = {
        m_GreenButtonAfterHitbox.center.x,
        ImagePointToWorldPoint(0.0f, 912.0f).y + greenButtonSize.y * 0.5f,
    };
    m_GreenButtonAfterBaseSize = greenButtonSize;
    m_GreenButtonAfter->SetPosition(m_GreenButtonAfterBasePosition);
    m_Root.AddChild(m_GreenButtonAfter);
    m_LevelProps.push_back(m_GreenButtonAfter);

    m_GreenButtonAfterHitbox.center = {
        m_GreenButtonAfterBasePosition.x,
        ImagePointToWorldPoint(0.0f, 912.0f).y + std::max(4.0f, greenButtonSize.y * 0.16f) * 0.5f,
    };
    m_GreenButtonAfterHitbox.size = {
        greenButtonSize.x * 0.72f,
        std::max(4.0f, greenButtonSize.y * 0.16f),
    };

    // Top exit doors.
    // Each door has:
    // - a visual sprite with open/close animation frames
    // - a trigger rect checked against the matching character only
    m_FireboyDoor.closedImagePath =
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lavaboy_door_closed.png";
    m_FireboyDoor.openingImagePaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lavaboy_door_opening_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lavaboy_door_opening_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lavaboy_door_opening_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lavaboy_door_opening_03.png",
    };
    m_FireboyDoor.sprite = addPropAtBottom(
        m_FireboyDoor.closedImagePath,
        1965.0f, 362.0f, 160.0f, 205.0f, 7.2f, propScale
    );
    const glm::vec2 fireboyDoorSize = imageSizeToWorldSize(160.0f, 205.0f, propScale);
    m_FireboyDoor.triggerRect.center = m_FireboyDoor.sprite->GetPosition() + glm::vec2{
        0.0f,
        -fireboyDoorSize.y * 0.14f,
    };
    m_FireboyDoor.triggerRect.size = {
        fireboyDoorSize.x * 0.46f,
        fireboyDoorSize.y * 0.72f,
    };

    m_WatergirlDoor.closedImagePath =
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/watergirl_door_closed.png";
    m_WatergirlDoor.openingImagePaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/watergirl_door_opening_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/watergirl_door_opening_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/watergirl_door_opening_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/watergirl_door_opening_03.png",
    };
    m_WatergirlDoor.sprite = addPropAtBottom(
        m_WatergirlDoor.closedImagePath,
        2140.0f, 362.0f, 162.0f, 205.0f, 7.2f, propScale
    );
    const glm::vec2 watergirlDoorSize = imageSizeToWorldSize(162.0f, 205.0f, propScale);
    m_WatergirlDoor.triggerRect.center = m_WatergirlDoor.sprite->GetPosition() + glm::vec2{
        watergirlDoorSize.x * 0.02f,
        -watergirlDoorSize.y * 0.10f,
    };
    m_WatergirlDoor.triggerRect.size = {
        watergirlDoorSize.x * 0.60f,
        watergirlDoorSize.y * 0.82f,
    };

    // Central cube block. Fit it to the alley between the upper bridge
    // underside (y=423) and the floor below (y=546), leaving a tiny margin
    // so the push physics do not snag on the ceiling.
    const float cubeFitScale = (123.0f - 4.0f) / 124.0f;
    const glm::vec2 cubeSpriteSize = imageSizeToWorldSize(123.0f, 124.0f, cubeFitScale);
    const glm::vec2 cubeHitboxSize = {
        cubeSpriteSize.x * m_CubeHitboxScale.x,
        cubeSpriteSize.y * m_CubeHitboxScale.y,
    };
    const glm::vec2 cubeFloorPoint = ImagePointToWorldPoint(1415.0f, 546.0f);
    m_CubeRect.center = cubeFloorPoint + glm::vec2{0.0f, cubeHitboxSize.y * 0.5f};
    m_CubeRect.size = cubeHitboxSize;
    m_CubeSpawnRect = m_CubeRect;
    m_CubeBlockIndex = m_SolidBlocks.size();
    m_HasCubeBlock = true;
    m_SolidBlocks.push_back(m_CubeRect);

    m_Cube = std::make_shared<Character>(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/cube.png"
    );
    m_Cube->SetZIndex(8.1f);
    m_Cube->SetSize(cubeSpriteSize);
    m_Cube->SetPosition(cubeFloorPoint + glm::vec2{0.0f, cubeSpriteSize.y * 0.5f});
    m_CubeSpawnPosition = m_Cube->GetPosition();
    m_Root.AddChild(m_Cube);

    // Collectible diamonds. These are not terrain colliders; pickup is handled
    // separately so the characters can pass through them and collect by color.
    addCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        800.0f, 230.0f, 121.0f, 111.0f, 8.4f, diamondScale
    );
    addCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        120.0f, 672.0f, 117.0f, 111.0f, 8.4f, diamondScale
    );
    addCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        1450.0f, 292.0f, 117.0f, 111.0f, 8.4f, diamondScale
    );
    addCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        1657.0f, 1153.0f, 117.0f, 111.0f, 8.4f, diamondScale
    );
    addCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        1335.0f, 1643.0f, 121.0f, 111.0f, 8.4f, diamondScale
    );
    addCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        1880.0f, 1643.0f, 117.0f, 111.0f, 8.4f, diamondScale
    );
    addCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_green.png",
        DiamondType::Neutral,
        975.0f, 1125.0f, 113.0f, 106.0f, 8.4f, diamondScale
    );

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

    m_CeilingSlopes = {
        {ImagePointToWorldPoint(660.0f, 297.0f), ImagePointToWorldPoint(729.0f, 360.0f)},
        {ImagePointToWorldPoint(1704.0f, 722.0f), ImagePointToWorldPoint(1762.0f, 781.0f)},
        {ImagePointToWorldPoint(1097.0f, 969.0f), ImagePointToWorldPoint(1151.0f, 1030.0f)},
        {ImagePointToWorldPoint(1958.0f, 1034.0f), ImagePointToWorldPoint(2014.0f, 1090.0f)},
        {ImagePointToWorldPoint(2082.0f, 1095.0f), ImagePointToWorldPoint(2193.0f, 1214.0f)},
        {ImagePointToWorldPoint(910.0f, 1285.0f), ImagePointToWorldPoint(1027.0f, 1398.0f)},
    };

    // Solidify slope seams and underside gaps. The walkable line slope logic is
    // still the main behavior, but these overlapping guard pieces make the
    // terrain mass continuous so stronger jumps cannot ghost through tiny
    // blanks at slope starts, ends, or undersides.
    auto addSlopeGuardBand = [&](const auto& slopes, float solidSideSign) {
        constexpr float guardHeight = 9.0f;
        constexpr float guardExtraOverlap = 7.0f;
        constexpr float endpointGuardSize = 12.0f;

        for (const auto& slope : slopes) {
            const glm::vec2 delta = slope.end - slope.start;
            const float slopeLength = glm::length(delta);
            if (slopeLength < 0.001f) {
                continue;
            }

            const int segments = std::max(3, static_cast<int>(std::ceil(slopeLength / 10.0f)));
            const float guardWidth = (slopeLength / static_cast<float>(segments)) + guardExtraOverlap;

            for (int i = 0; i <= segments; ++i) {
                const float t = static_cast<float>(i) / static_cast<float>(segments);
                const glm::vec2 point = slope.start + delta * t;

                SolidRect guard;
                guard.center = {
                    point.x,
                    point.y + solidSideSign * (guardHeight * 0.5f - 0.35f),
                };
                guard.size = {guardWidth, guardHeight};
                guard.blockBottom = true;
                m_SolidBlocks.push_back(guard);
            }

            for (const glm::vec2 endpoint : {slope.start, slope.end}) {
                SolidRect endpointGuard;
                endpointGuard.center = {
                    endpoint.x,
                    endpoint.y + solidSideSign * (endpointGuardSize * 0.5f - 0.35f),
                };
                endpointGuard.size = {endpointGuardSize, endpointGuardSize};
                endpointGuard.blockBottom = true;
                m_SolidBlocks.push_back(endpointGuard);
            }
        }
    };

    // Floor slopes: solid mass exists under the walkable line.
    addSlopeGuardBand(m_Slopes, -1.0f);
    // Ceiling slopes: solid mass exists above the line.
    addSlopeGuardBand(m_CeilingSlopes, 1.0f);

    // -------------------------------------------------------------------------
    // 4) Fireboy
    // -------------------------------------------------------------------------
    // Fireboy/Watergirl share the same body+head controller architecture.
    // The difference is mainly asset folders, controls, and element rules.
    {
        const std::string bodyDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Body";
        const std::string idleDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Idle";
        const std::string runDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Run";
        const std::string jumpDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Jump";
        const std::string fallDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Fall";
        const std::string winDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Win";

        std::vector<std::string> bodyPaths;
        std::vector<std::string> idleHeadPaths;
        std::vector<std::string> runHeadPaths;
        std::vector<std::string> jumpHeadPaths;
        std::vector<std::string> fallHeadPaths;
        std::vector<std::string> winHeadPaths;

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

        try {
            for (const auto& entry : std::filesystem::directory_iterator(jumpDir)) {
                if (entry.is_regular_file()) {
                    jumpHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(fallDir)) {
                if (entry.is_regular_file()) {
                    fallHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(winDir)) {
                if (entry.is_regular_file()) {
                    winHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        std::sort(bodyPaths.begin(), bodyPaths.end());
        std::sort(idleHeadPaths.begin(), idleHeadPaths.end());
        std::sort(runHeadPaths.begin(), runHeadPaths.end());
        std::sort(jumpHeadPaths.begin(), jumpHeadPaths.end());
        std::sort(fallHeadPaths.begin(), fallHeadPaths.end());
        std::sort(winHeadPaths.begin(), winHeadPaths.end());

        if (bodyPaths.empty()) {
            bodyPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Body/fireboy_body_00.png");
        }
        if (idleHeadPaths.empty()) {
            idleHeadPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Idle/fireboy_idle_00.png");
        }
        if (runHeadPaths.empty()) {
            runHeadPaths = idleHeadPaths;
        }
        if (jumpHeadPaths.empty()) {
            jumpHeadPaths = runHeadPaths;
        }
        if (fallHeadPaths.empty()) {
            fallHeadPaths = runHeadPaths;
        }
        if (winHeadPaths.empty()) {
            winHeadPaths = idleHeadPaths;
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
        m_Fireboy->SetSize({36.0f, 40.0f});
        m_Fireboy->SetHeadScale(1.25f / 1.4f);
        m_Fireboy->SetBodySize({30.0f / 1.4f, 31.0f / 1.4f});
        m_Fireboy->SetHeadOffset({0.0f, -4.0f});
        RecalculateFireboyCollisionBoxes();

        m_Fireboy->SetIdleBodyImage(bodyPaths.front());
        m_Fireboy->SetIdleHeadImage(idleHeadPaths, 120, true);
        m_Fireboy->SetJumpHeadImage(jumpHeadPaths, 120, true);
        m_Fireboy->SetFallHeadImage(fallHeadPaths, 120, true);
        m_Fireboy->SetWinHeadImage(winHeadPaths, 360, false);
        m_VictoryCelebrateDuration = std::max(
            m_VictoryCelebrateDuration,
            static_cast<float>(std::max<std::size_t>(1, winHeadPaths.size()) - 1) * 0.36f
        );
        m_Fireboy->SetLifeState(HeadBodyCharacter::LifeState::Alive);
        m_Fireboy->SetMotionState(HeadBodyCharacter::MotionState::Idle);

        const float floorTop = m_LeftFloorSpawnRect.center.y + (m_LeftFloorSpawnRect.size.y * 0.5f);
        const float wallRight = m_LeftWallSpawnRect.center.x + (m_LeftWallSpawnRect.size.x * 0.5f);

        glm::vec2 spawnPos;
        const float leftExtent = -GetFireboyLeftEdge({0.0f, 0.0f});
        const float bodyBottom = GetFireboyBodyBottom({0.0f, 0.0f});
        spawnPos.x = wallRight + leftExtent + 5.0f;
        spawnPos.y = floorTop - bodyBottom;

        m_Fireboy->SetPosition(spawnPos);
        m_FireboySpawnPosition = spawnPos;
        s_LastPos[m_Fireboy.get()] = spawnPos;

        m_Root.AddChild(m_Fireboy);
    }

    // -------------------------------------------------------------------------
    // 5) Watergirl
    // -------------------------------------------------------------------------
    {
        const std::string bodyDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Body";
        const std::string idleDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Idle";
        const std::string runDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Run";
        const std::string jumpDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Jump";
        const std::string fallDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Fall";
        const std::string winDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Win";

        std::vector<std::string> bodyPaths;
        std::vector<std::string> idleHeadPaths;
        std::vector<std::string> runHeadPaths;
        std::vector<std::string> jumpHeadPaths;
        std::vector<std::string> fallHeadPaths;
        std::vector<std::string> winHeadPaths;

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

        try {
            for (const auto& entry : std::filesystem::directory_iterator(jumpDir)) {
                if (entry.is_regular_file()) {
                    jumpHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(fallDir)) {
                if (entry.is_regular_file()) {
                    fallHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(winDir)) {
                if (entry.is_regular_file()) {
                    winHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        std::sort(bodyPaths.begin(), bodyPaths.end());
        std::sort(idleHeadPaths.begin(), idleHeadPaths.end());
        std::sort(runHeadPaths.begin(), runHeadPaths.end());
        std::sort(jumpHeadPaths.begin(), jumpHeadPaths.end());
        std::sort(fallHeadPaths.begin(), fallHeadPaths.end());
        std::sort(winHeadPaths.begin(), winHeadPaths.end());

        if (bodyPaths.empty()) {
            bodyPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Body/watergirl_body_00.png");
        }
        if (idleHeadPaths.empty()) {
            idleHeadPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Idle/watergirl_idle_00.png");
        }
        if (runHeadPaths.empty()) {
            runHeadPaths = idleHeadPaths;
        }
        if (jumpHeadPaths.empty()) {
            jumpHeadPaths = runHeadPaths;
        }
        if (fallHeadPaths.empty()) {
            fallHeadPaths = runHeadPaths;
        }
        if (winHeadPaths.empty()) {
            winHeadPaths = idleHeadPaths;
        }

        std::vector<std::string> moveBodyPaths;
        if (bodyPaths.size() > 1) {
            moveBodyPaths.assign(bodyPaths.begin() + 1, bodyPaths.end());
        } else {
            moveBodyPaths.push_back(bodyPaths.front());
        }

        m_Watergirl = std::make_shared<HeadBodyCharacter>(
            moveBodyPaths, runHeadPaths, 10.0f, 120, true, 120, true
        );
        m_Watergirl->SetSize({36.0f, 40.0f});
        m_Watergirl->SetHeadScale(1.25f / 1.4f);
        m_Watergirl->SetMoveHeadWidthScale(1.18f);
        m_Watergirl->SetBodySize({30.0f / 1.4f, 31.0f / 1.4f});
        m_Watergirl->SetHeadOffset({0.0f, -4.0f});
        RecalculateWatergirlCollisionBoxes();

        m_Watergirl->SetIdleBodyImage(bodyPaths.front());
        m_Watergirl->SetIdleHeadImage(idleHeadPaths, 120, true);
        m_Watergirl->SetJumpHeadImage(jumpHeadPaths, 120, true);
        m_Watergirl->SetFallHeadImage(fallHeadPaths, 120, true);
        m_Watergirl->SetWinHeadImage(winHeadPaths, 360, false);
        m_VictoryCelebrateDuration = std::max(
            m_VictoryCelebrateDuration,
            static_cast<float>(std::max<std::size_t>(1, winHeadPaths.size()) - 1) * 0.36f
        );
        m_Watergirl->SetLifeState(HeadBodyCharacter::LifeState::Alive);
        m_Watergirl->SetMotionState(HeadBodyCharacter::MotionState::Idle);

        const float floorTop = m_LeftFloorSpawnRect.center.y + (m_LeftFloorSpawnRect.size.y * 0.5f);
        const float wallRight = m_LeftWallSpawnRect.center.x + (m_LeftWallSpawnRect.size.x * 0.5f);

        glm::vec2 spawnPos;
        const float leftExtent = -GetWatergirlLeftEdge({0.0f, 0.0f});
        const float bodyBottom = GetWatergirlBodyBottom({0.0f, 0.0f});
        spawnPos.x = wallRight + leftExtent + 45.0f;
        spawnPos.y = floorTop - bodyBottom;

        m_Watergirl->SetPosition(spawnPos);
        m_WatergirlSpawnPosition = spawnPos;
        s_LastPos[m_Watergirl.get()] = spawnPos;

        m_Root.AddChild(m_Watergirl);
    }

    // -------------------------------------------------------------------------
    // 6) 初始狀態
    // -------------------------------------------------------------------------
    m_FireboyVelocity = {0.0f, 0.0f};
    m_WatergirlVelocity = {0.0f, 0.0f};
    m_FireboyOnGround = true;
    m_WatergirlOnGround = true;

    LOG_DEBUG("Background Size: {}x{}", m_BackgroundDisplayedSize.x, m_BackgroundDisplayedSize.y);
    LOG_DEBUG(
        "Fireboy Body Box: {}x{} Head Box: {}x{}",
        m_FireboyCollision.bodyHitboxSize.x,
        m_FireboyCollision.bodyHitboxSize.y,
        m_FireboyCollision.headHitboxSize.x,
        m_FireboyCollision.headHitboxSize.y
    );
    LOG_DEBUG(
        "Watergirl Body Box: {}x{} Head Box: {}x{}",
        m_WatergirlCollision.bodyHitboxSize.x,
        m_WatergirlCollision.bodyHitboxSize.y,
        m_WatergirlCollision.headHitboxSize.x,
        m_WatergirlCollision.headHitboxSize.y
    );

    // -------------------------------------------------------------------------
    // 7) Fail overlay
    // -------------------------------------------------------------------------
    // This is intentionally structured like future UI buttons:
    // - current branch uses text labels
    // - each button already stores its future asset path so swapping to real
    //   sprites later should not require rewriting the fail-state logic.
    m_FailOverlayPanel = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/ui_fail_overlay_panel.png"
    );
    m_FailOverlayPanel->SetZIndex(50.0f);
    m_FailOverlayPanel->SetSize({760.0f, 430.0f});
    m_FailOverlayPanel->SetPosition({0.0f, 20.0f});
    m_FailOverlayPanel->SetVisible(false);
    m_Root.AddChild(m_FailOverlayPanel);
    m_FailOverlayObjects.push_back(m_FailOverlayPanel);

    auto makeOverlayText = [&](const std::string& text,
                               const glm::vec2& position,
                               int fontSize,
                               const Util::Color& color,
                               float zIndex) {
        auto object = std::make_shared<OverlayText>(text, fontSize, color, zIndex);
        object->m_Transform.translation = position;
        object->SetVisible(false);
        m_Root.AddChild(object);
        m_FailOverlayObjects.push_back(object);
        return object;
    };

    m_FailOverlayTitle = makeOverlayText(
        "Both heroes fell",
        {0.0f, 120.0f},
        34,
        Util::Color(255, 245, 220, 255),
        51.0f
    );

    auto makeFailButton = [&](const std::string& label,
                              const std::string& futureAssetPath,
                              const glm::vec2& center,
                              const glm::vec2& size) {
        FailOverlayButton button;
        button.label = label;
        button.futureAssetPath = futureAssetPath;
        button.rect.center = center;
        button.rect.size = size;
        button.labelObject = makeOverlayText(
            label,
            center,
            26,
            Util::Color(255, 255, 255, 255),
            51.0f
        );
        return button;
    };

    m_FailRestartButton = makeFailButton(
        "RESTART",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/button_restart.png",
        {0.0f, 25.0f},
        {240.0f, 64.0f}
    );
    m_FailHomeButton = makeFailButton(
        "HOME",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/button_home.png",
        {0.0f, -55.0f},
        {240.0f, 64.0f}
    );
    m_FailExitButton = makeFailButton(
        "EXIT",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/button_exit.png",
        {0.0f, -135.0f},
        {240.0f, 64.0f}
    );

    UpdateFailOverlayVisuals();

    // -------------------------------------------------------------------------
    // 8) Victory overlay
    // -------------------------------------------------------------------------
    // Mirrors the fail overlay structure so swapping text placeholders for
    // real UI button art later will be straightforward.
    m_VictoryOverlayPanel = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/ui_fail_overlay_panel.png"
    );
    m_VictoryOverlayPanel->SetZIndex(50.0f);
    m_VictoryOverlayPanel->SetSize({760.0f, 460.0f});
    m_VictoryOverlayPanel->SetPosition({0.0f, 20.0f});
    m_VictoryOverlayPanel->SetVisible(false);
    m_Root.AddChild(m_VictoryOverlayPanel);
    m_VictoryOverlayObjects.push_back(m_VictoryOverlayPanel);

    auto makeVictoryOverlayText = [&](const std::string& text,
                                      const glm::vec2& position,
                                      int fontSize,
                                      const Util::Color& color,
                                      float zIndex) {
        auto object = std::make_shared<OverlayText>(text, fontSize, color, zIndex);
        object->m_Transform.translation = position;
        object->SetVisible(false);
        m_Root.AddChild(object);
        m_VictoryOverlayObjects.push_back(object);
        return object;
    };

    auto makeVictoryOverlayIcon = [&](const std::string& path,
                                      const glm::vec2& position,
                                      const glm::vec2& size,
                                      float zIndex) {
        auto object = std::make_shared<Character>(path);
        object->SetZIndex(zIndex);
        object->SetSize(size);
        object->SetPosition(position);
        object->SetVisible(false);
        m_Root.AddChild(object);
        m_VictoryOverlayObjects.push_back(object);
        return object;
    };

    m_VictoryOverlayTitle = makeVictoryOverlayText(
        "LEVEL COMPLETE",
        {0.0f, 128.0f},
        34,
        Util::Color(255, 245, 220, 255),
        51.0f
    );
    m_VictoryTimeText = makeVictoryOverlayText(
        "00:00",
        {0.0f, 72.0f},
        24,
        Util::Color(255, 255, 255, 255),
        51.0f
    );
    m_VictoryBlueDiamondIcon = makeVictoryOverlayIcon(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        {-102.0f, 18.0f},
        {42.0f, 38.0f},
        51.0f
    );
    m_VictoryBlueDiamondText = makeVictoryOverlayText(
        "x 0",
        {12.0f, 18.0f},
        24,
        Util::Color(240, 248, 255, 255),
        51.0f
    );
    m_VictoryRedDiamondIcon = makeVictoryOverlayIcon(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        {-102.0f, -38.0f},
        {42.0f, 38.0f},
        51.0f
    );
    m_VictoryRedDiamondText = makeVictoryOverlayText(
        "x 0",
        {12.0f, -38.0f},
        24,
        Util::Color(255, 235, 230, 255),
        51.0f
    );
    m_VictoryRankText = makeVictoryOverlayText(
        "Rank: C",
        {0.0f, -108.0f},
        26,
        Util::Color(255, 230, 170, 255),
        51.0f
    );

    m_VictoryContinueButton.label = "CONTINUE";
    m_VictoryContinueButton.futureAssetPath =
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/button_continue.png";
    m_VictoryContinueButton.rect.center = {0.0f, -190.0f};
    m_VictoryContinueButton.rect.size = {260.0f, 64.0f};
    m_VictoryContinueButton.labelObject = makeVictoryOverlayText(
        m_VictoryContinueButton.label,
        m_VictoryContinueButton.rect.center,
        26,
        Util::Color(255, 255, 255, 255),
        51.0f
    );

    UpdateVictoryOverlayVisuals();
    // Rebuild the switch latch from the new scene so restart does not inherit
    // an accidental pressed/not-pressed edge from the previous run.
    m_GreenSwitchTouchLatch = IsGreenSwitchTouched();
    m_LevelStartTimeMs = Util::Time::GetElapsedTimeMs();

    m_CurrentState = State::UPDATE;
}
