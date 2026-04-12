#include "App.hpp"

#include "HeadBodyCharacter.hpp"
#include "Util/Animation.hpp"
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
    m_Diamonds.clear();
    m_GreenSwitch.reset();
    m_Cube.reset();
    m_HasGreenPlatformBlock = false;
    m_HasCubeBlock = false;
    m_GreenButtonPressed = false;
    m_GreenSwitchOn = false;
    m_GreenSwitchTouchLatch = false;
    m_FireDiamondsCollected = 0;
    m_WaterDiamondsCollected = 0;
    m_GreenDiamondsCollected = 0;
    m_FireDiamondsTotal = 0;
    m_WaterDiamondsTotal = 0;
    m_GreenDiamondsTotal = 0;
    m_CubeVelocity = {0.0f, 0.0f};
    m_CubeOnGround = false;
    m_FireboyDoor = {};
    m_WatergirlDoor = {};
    m_VictoryPhase = VictoryPhase::None;
    m_VictoryTimer = 0.0f;

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

    // Green button platform:
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

    m_GreenButtonHitbox = ImageRectToWorldRect(805.0f, 1160.0f, 970.0f, 1220.0f);
    const glm::vec2 greenButtonSize = {
        m_GreenButtonHitbox.size.x / 2.0f,
        (m_GreenButtonHitbox.size.x / 2.0f) * (120.0f / 189.0f),
    };
    m_GreenButtonHitbox.size = greenButtonSize;
    m_GreenButtonHitbox.center.y =
        ImagePointToWorldPoint(0.0f, 1223.0f).y + greenButtonSize.y * 0.5f;
    m_SolidBlocks.push_back(m_GreenButtonHitbox);
    m_GreenButton = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/button_green.png"
    );
    m_GreenButton->SetZIndex(7);
    m_GreenButton->SetSize(greenButtonSize);
    m_GreenButton->SetPosition(m_GreenButtonHitbox.center);
    m_Root.AddChild(m_GreenButton);

    m_GreenButtonAfterHitbox = ImageRectToWorldRect(365.0f, 850.0f, 455.0f, 912.0f);
    m_GreenButtonAfterHitbox.size = greenButtonSize;
    m_GreenButtonAfterHitbox.center.y =
        ImagePointToWorldPoint(0.0f, 912.0f).y + greenButtonSize.y * 0.5f;
    m_SolidBlocks.push_back(m_GreenButtonAfterHitbox);
    m_GreenButtonAfter = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/button_green.png"
    );
    m_GreenButtonAfter->SetZIndex(7);
    m_GreenButtonAfter->SetSize(greenButtonSize);
    m_GreenButtonAfter->SetPosition(m_GreenButtonAfterHitbox.center);
    m_Root.AddChild(m_GreenButtonAfter);

    // -------------------------------------------------------------------------
    // 3.5) Level 1 prop pass
    // -------------------------------------------------------------------------
    const float scaleX = m_BackgroundDisplayedSize.x / m_BackgroundOriginalSize.x;
    const float scaleY = m_BackgroundDisplayedSize.y / m_BackgroundOriginalSize.y;

    auto imageSizeToWorldSize = [&](float imageWidth, float imageHeight, float scale = 1.0f) {
        return glm::vec2{imageWidth * scaleX * scale, imageHeight * scaleY * scale};
    };

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

    auto addAnimatedHazardAtBottom = [&](const std::vector<std::string>& paths,
                                         const HazardRect& hazardRect,
                                         float centerXImage,
                                         float bottomYImage,
                                         float imageWidth,
                                         float imageHeight,
                                         float zIndex) {
        auto hazardAnimation = std::make_shared<Util::Animation>(paths, true, 120, true, 0);
        const glm::vec2 size = imageSizeToWorldSize(imageWidth, imageHeight);
        hazardAnimation->SetSize(size);

        glm::vec2 pos = ImagePointToWorldPoint(centerXImage, bottomYImage);
        pos.y += size.y * 0.5f;

        auto hazardObject = std::make_shared<Util::GameObject>(hazardAnimation, zIndex);
        hazardObject->m_Transform.translation = pos;
        m_Root.AddChild(hazardObject);
        m_AnimatedLevelProps.push_back(hazardObject);
        m_Hazards.push_back(hazardRect);

        return hazardObject;
    };

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
    HazardRect lowerLavaHazard;
    {
        const SolidRect rect = ImageRectToWorldRect(1162.0f, 1738.0f, 1336.0f, 1760.0f);
        lowerLavaHazard.center = rect.center;
        lowerLavaHazard.size = rect.size;
        lowerLavaHazard.type = HazardRect::Type::Lava;
    }
    HazardRect lowerWaterHazard;
    {
        const SolidRect rect = ImageRectToWorldRect(1659.0f, 1738.0f, 1833.0f, 1760.0f);
        lowerWaterHazard.center = rect.center;
        lowerWaterHazard.size = rect.size;
        lowerWaterHazard.type = HazardRect::Type::Water;
    }

    // Approximate Level 1 dressing. We can fine-tune these image coordinates together.
    addAnimatedHazardAtBottom(
        lavaPaths,
        lowerLavaHazard,
        1244.5f, 1760.0f, 275.0f, 275.0f * (61.0f / 177.0f), 6.7f
    );
    addAnimatedHazardAtBottom(
        waterPaths,
        lowerWaterHazard,
        1741.5f, 1760.0f, 275.0f, 275.0f * (60.0f / 178.0f), 6.7f
    );

    m_GreenSwitch = addPropAtBottom(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/switch_green_off.png",
        550.0f, 1466.0f, 148.0f, 124.0f, 7.6f, propScale
    );
    const glm::vec2 switchSpriteSize = imageSizeToWorldSize(148.0f, 124.0f, propScale);
    m_GreenSwitchHitbox.center = m_GreenSwitch->GetPosition() + glm::vec2{
        -switchSpriteSize.x * 0.03f,
        -switchSpriteSize.y * 0.18f,
    };
    m_GreenSwitchHitbox.size = {
        switchSpriteSize.x * 0.62f,
        switchSpriteSize.y * 0.46f,
    };

    // Top exit doors.
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
        1965.0f, 362.0f, 160.0f, 188.0f, 7.2f, propScale
    );
    const glm::vec2 fireboyDoorSize = imageSizeToWorldSize(160.0f, 188.0f, propScale);
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
        2140.0f, 362.0f, 162.0f, 188.0f, 7.2f, propScale
    );
    const glm::vec2 watergirlDoorSize = imageSizeToWorldSize(162.0f, 188.0f, propScale);
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

    // -------------------------------------------------------------------------
    // 4) Fireboy
    // -------------------------------------------------------------------------
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
        m_Fireboy->SetWinHeadImage(winHeadPaths, 90, true);
        m_Fireboy->SetMotionState(HeadBodyCharacter::MotionState::Idle);

        if (m_SolidBlocks.size() >= 3) {
            const SolidRect& floor = m_SolidBlocks[1];
            const SolidRect& leftWall = m_SolidBlocks[2];

            const float floorTop = floor.center.y + (floor.size.y * 0.5f);
            const float wallRight = leftWall.center.x + (leftWall.size.x * 0.5f);

            glm::vec2 spawnPos;
            const float leftExtent = -GetFireboyLeftEdge({0.0f, 0.0f});
            const float bodyBottom = GetFireboyBodyBottom({0.0f, 0.0f});
            spawnPos.x = wallRight + leftExtent + 5.0f;
            spawnPos.y = floorTop - bodyBottom;

            m_Fireboy->SetPosition(spawnPos);
            m_FireboySpawnPosition = spawnPos;
            s_LastPos[m_Fireboy.get()] = spawnPos;
        }

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
        m_Watergirl->SetWinHeadImage(winHeadPaths, 90, true);
        m_Watergirl->SetMotionState(HeadBodyCharacter::MotionState::Idle);

        if (m_SolidBlocks.size() >= 3) {
            const SolidRect& floor = m_SolidBlocks[1];
            const SolidRect& leftWall = m_SolidBlocks[2];

            const float floorTop = floor.center.y + (floor.size.y * 0.5f);
            const float wallRight = leftWall.center.x + (leftWall.size.x * 0.5f);

            glm::vec2 spawnPos;
            const float leftExtent = -GetWatergirlLeftEdge({0.0f, 0.0f});
            const float bodyBottom = GetWatergirlBodyBottom({0.0f, 0.0f});
            spawnPos.x = wallRight + leftExtent + 45.0f;
            spawnPos.y = floorTop - bodyBottom;

            m_Watergirl->SetPosition(spawnPos);
            m_WatergirlSpawnPosition = spawnPos;
            s_LastPos[m_Watergirl.get()] = spawnPos;
        }

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

    m_CurrentState = State::UPDATE;
}
