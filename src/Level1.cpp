#include "App.hpp"

#include <algorithm>

void App::BuildLevel1() {
    // Base collision setup:
    // - m_SolidBlocks stores the terrain mass used by AABB collision.
    // - m_Slopes stores only the smooth walkable diagonals.
    // Together they let us fake irregular temple geometry with stable runtime
    // collision while still preserving smooth ramps for characters/cube.
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 2380.0f, 45.0f));       // Roof
    m_LeftFloorSpawnRect = ImageRectToWorldRect(0.0f, 1715.0f, 1107.0f, 1760.0f);
    m_SolidBlocks.push_back(m_LeftFloorSpawnRect);                                     // Floor
    m_LeftWallSpawnRect = ImageRectToWorldRect(0.0f, 0.0f, 45.0f, 1760.0f);
    m_SolidBlocks.push_back(m_LeftWallSpawnRect);                                      // Left
    m_SolidBlocks.push_back(ImageRectToWorldRect(2335.0f, 0.0f, 2380.0f, 1760.0f));   // Right

    m_TestBlock = ImageRectToWorldRect(46.0f, 1466.0f, 784.0f, 1519.0f);
    m_SolidBlocks.push_back(m_TestBlock);

    const std::vector<SolidRect> perimeterBlocks = {
        ImageRectToWorldRect(1155.0f, 1743.0f, 1342.0f, 1760.0f),
        ImageRectToWorldRect(1382.0f, 1714.0f, 1392.0f, 1760.0f),
        ImageRectToWorldRect(1390.0f, 1712.0f, 1594.0f, 1760.0f),
        ImageRectToWorldRect(1592.0f, 1714.0f, 1604.0f, 1760.0f),
        ImageRectToWorldRect(1652.0f, 1743.0f, 1839.0f, 1760.0f),
        ImageRectToWorldRect(1879.0f, 1714.0f, 1889.0f, 1760.0f),
        ImageRectToWorldRect(1887.0f, 1712.0f, 2380.0f, 1760.0f),

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
        ImageRectToWorldRect(1029.0f, 1223.0f, 1044.0f, 1273.0f),
        ImageRectToWorldRect(901.0f, 1273.0f, 910.0f, 1395.0f),
        ImageRectToWorldRect(910.0f, 1273.0f, 1032.0f, 1399.0f, false),
        ImageRectToWorldRect(1032.0f, 1273.0f, 1044.0f, 1395.0f),
        ImageRectToWorldRect(1044.0f, 1340.0f, 1160.0f, 1395.0f, false),
        ImageRectToWorldRect(1036.0f, 1340.0f, 1469.0f, 1395.0f),
        ImageRectToWorldRect(1469.0f, 1360.0f, 1477.0f, 1395.0f, false),
        ImageRectToWorldRect(1475.0f, 1344.0f, 1494.0f, 1395.0f, false),
        ImageRectToWorldRect(1475.0f, 1378.0f, 1529.0f, 1395.0f, false),
        ImageRectToWorldRect(1529.0f, 1372.0f, 1539.0f, 1395.0f, false),
        ImageRectToWorldRect(1537.0f, 1372.0f, 1704.0f, 1395.0f),
        ImageRectToWorldRect(1704.0f, 1374.0f, 1722.0f, 1395.0f, false),
        ImageRectToWorldRect(1704.0f, 1374.0f, 1762.0f, 1395.0f, false),
        ImageRectToWorldRect(1762.0f, 1342.0f, 2021.0f, 1393.0f),
        ImageRectToWorldRect(2021.0f, 1344.0f, 2038.0f, 1399.0f, false),
        ImageRectToWorldRect(2021.0f, 1393.0f, 2067.0f, 1399.0f, false),
        ImageRectToWorldRect(2138.0f, 1586.0f, 2188.0f, 1704.0f, false),
        ImageRectToWorldRect(2197.0f, 1525.0f, 2325.0f, 1704.0f),
    };
    m_SolidBlocks.insert(m_SolidBlocks.end(), perimeterBlocks.begin(), perimeterBlocks.end());

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

    m_GreenPlatformRestRect2 = ImageRectToWorldRect(2081.0f, 676.0f, 2321.0f, 748.0f);
    m_GreenPlatformPressedRect2 = ImageRectToWorldRect(2081.0f, 897.0f, 2321.0f, 969.0f);
    m_GreenPlatformCurrentRect2 = m_GreenPlatformRestRect2;
    m_GreenPlatformBlockIndex2 = m_SolidBlocks.size();
    m_HasGreenPlatformBlock2 = true;
    m_SolidBlocks.push_back(m_GreenPlatformCurrentRect2);

    m_GreenPlatform2 = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/platform_green.png"
    );
    m_GreenPlatform2->SetZIndex(7);
    m_GreenPlatform2->SetSize({
        m_GreenPlatformCurrentRect2.size.x,
        m_GreenPlatformCurrentRect2.size.x * (107.0f / 333.0f),
    });
    m_GreenPlatform2->SetPosition(m_GreenPlatformCurrentRect2.center);
    m_Root.AddChild(m_GreenPlatform2);

    m_GreenSwitchHitbox = ImageRectToWorldRect(1180.0f, 1280.0f, 1345.0f, 1340.0f);
    m_GreenButtonAfterHitbox = ImageRectToWorldRect(365.0f, 850.0f, 455.0f, 912.0f);
    m_GreenButtonAfterHitbox2 = ImageRectToWorldRect(1900.0f, 980.0f, 2000.0f, 1040.0f);

    const float propScale = 1.0f / 1.5f;
    const float greenButtonVisualScale = (propScale * 0.82f) / 1.5f;
    const glm::vec2 greenButtonSize = ImageSizeToWorldSize(189.0f, 120.0f, greenButtonVisualScale);
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

    AddAnimatedHazardInImageTrap(
        lavaPaths, HazardRect::Type::Lava,
        177.0f, 61.0f,
        436.0f, 1225.0f,
        715.0f, 1223.0f,
        490.0f, 1255.0f,
        669.0f, 1255.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        lavaPaths, HazardRect::Type::Lava,
        177.0f, 61.0f,
        426.0f, 673.0f,
        705.0f, 671.0f,
        480.0f, 703.0f,
        659.0f, 703.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        lavaPaths, HazardRect::Type::Lava,
        177.0f, 61.0f,
        1110.0f, 1714.0f,
        1389.0f, 1712.0f,
        1164.0f, 1744.0f,
        1343.0f, 1744.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterPaths, HazardRect::Type::Water,
        178.0f, 60.0f,
        1542.0f, 364.0f,
        1821.0f, 362.0f,
        1596.0f, 394.0f,
        1775.0f, 394.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterPaths, HazardRect::Type::Water,
        178.0f, 60.0f,
        1473.0f, 979.0f,
        1752.0f, 977.0f,
        1527.0f, 1009.0f,
        1706.0f, 1009.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        venomPaths, HazardRect::Type::Venom,
        178.0f, 60.0f,
        1481.0f, 1343.0f,
        1760.0f, 1341.0f,
        1535.0f, 1373.0f,
        1714.0f, 1373.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterPaths, HazardRect::Type::Water,
        178.0f, 60.0f,
        1601.0f, 1713.0f,
        1880.0f, 1711.0f,
        1655.0f, 1743.0f,
        1834.0f, 1743.0f,
        10.2f
    );

    m_GreenSwitch = AddPropAtBottom(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/switch_green_off.png",
        1262.5f, 1340.0f, 148.0f, 124.0f, 7.6f, propScale
    );
    const glm::vec2 switchSpriteSize = ImageSizeToWorldSize(148.0f, 124.0f, propScale);
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

    m_GreenButtonAfter2 = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/button_green.png"
    );
    m_GreenButtonAfter2->SetZIndex(7.0f);
    m_GreenButtonAfter2->SetSize(greenButtonSize);
    m_GreenButtonAfterBasePosition2 = {
        m_GreenButtonAfterHitbox2.center.x,
        ImagePointToWorldPoint(0.0f, 665.0f).y + greenButtonSize.y * 0.5f,
    };
    m_GreenButtonAfterBaseSize2 = greenButtonSize;
    m_GreenButtonAfter2->SetPosition(m_GreenButtonAfterBasePosition2);
    m_Root.AddChild(m_GreenButtonAfter2);
    m_LevelProps.push_back(m_GreenButtonAfter2);

    m_GreenButtonAfterHitbox2.center = {
        m_GreenButtonAfterBasePosition2.x,
        ImagePointToWorldPoint(0.0f, 665.0f).y + std::max(4.0f, greenButtonSize.y * 0.16f) * 0.5f,
    };
    m_GreenButtonAfterHitbox2.size = {
        greenButtonSize.x * 0.72f,
        std::max(4.0f, greenButtonSize.y * 0.16f),
    };

    const float doorScale = propScale * 1.6f;

    m_FireboyDoor.closedImagePath =
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lavaboy_door_closed.png";
    m_FireboyDoor.openingImagePaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lavaboy_door_opening_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lavaboy_door_opening_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lavaboy_door_opening_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lavaboy_door_opening_03.png",
    };
    m_FireboyDoor.sprite = AddPropAtBottom(
        m_FireboyDoor.closedImagePath,
        1965.0f, 362.0f, 160.0f, 205.0f, 7.2f, doorScale
    );
    const glm::vec2 fireboyDoorSize = ImageSizeToWorldSize(160.0f, 205.0f, doorScale);
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
    m_WatergirlDoor.sprite = AddPropAtBottom(
        m_WatergirlDoor.closedImagePath,
        2140.0f, 362.0f, 162.0f, 205.0f, 7.2f, doorScale
    );
    const glm::vec2 watergirlDoorSize = ImageSizeToWorldSize(162.0f, 205.0f, doorScale);
    m_WatergirlDoor.triggerRect.center = m_WatergirlDoor.sprite->GetPosition() + glm::vec2{
        watergirlDoorSize.x * 0.02f,
        -watergirlDoorSize.y * 0.10f,
    };
    m_WatergirlDoor.triggerRect.size = {
        watergirlDoorSize.x * 0.60f,
        watergirlDoorSize.y * 0.82f,
    };

    const float cubeFitScale = (123.0f - 4.0f) / 124.0f;
    const glm::vec2 cubeSpriteSize = ImageSizeToWorldSize(123.0f, 124.0f, cubeFitScale);
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

    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        720.0f, 170.0f, 121.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        130.0f, 310.0f, 117.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        1400.0f, 292.0f, 117.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        1200.0f, 292.0f, 117.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        1260.0f, 1600.0f, 121.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        1750.0f, 1600.0f, 117.0f, 111.0f, 8.4f, diamondScale
    );

    m_Slopes = {
        {ImagePointToWorldPoint(422.0f, 1216.0f), ImagePointToWorldPoint(479.0f, 1250.0f)},
        {ImagePointToWorldPoint(670.0f, 1254.0f), ImagePointToWorldPoint(721.0f, 1225.0f)},
        {ImagePointToWorldPoint(1044.0f, 1223.0f), ImagePointToWorldPoint(1155.0f, 1340.0f)},
        {ImagePointToWorldPoint(1477.0f, 1344.0f), ImagePointToWorldPoint(1529.0f, 1378.0f)},
        {ImagePointToWorldPoint(1712.0f, 1374.0f), ImagePointToWorldPoint(1762.0f, 1344.0f)},
        {ImagePointToWorldPoint(2021.0f, 1344.0f), ImagePointToWorldPoint(2067.0f, 1397.0f)},
        {ImagePointToWorldPoint(2138.0f, 1586.0f), ImagePointToWorldPoint(2197.0f, 1525.0f)},
        {ImagePointToWorldPoint(1107.0f, 1714.0f), ImagePointToWorldPoint(1155.0f, 1743.0f)},
        {ImagePointToWorldPoint(1342.0f, 1747.0f), ImagePointToWorldPoint(1382.0f, 1714.0f)},
        {ImagePointToWorldPoint(1604.0f, 1712.0f), ImagePointToWorldPoint(1652.0f, 1741.0f)},
        {ImagePointToWorldPoint(1839.0f, 1745.0f), ImagePointToWorldPoint(1879.0f, 1712.0f)},
        {ImagePointToWorldPoint(1220.0f, 912.0f), ImagePointToWorldPoint(1275.0f, 973.0f)},
        {ImagePointToWorldPoint(1468.0f, 977.0f), ImagePointToWorldPoint(1527.0f, 1015.0f)},
        {ImagePointToWorldPoint(1712.0f, 1013.0f), ImagePointToWorldPoint(1767.0f, 975.0f)},
        {ImagePointToWorldPoint(426.0f, 672.0f), ImagePointToWorldPoint(483.0f, 708.0f)},
        {ImagePointToWorldPoint(668.0f, 704.0f), ImagePointToWorldPoint(712.0f, 670.0f)},
        {ImagePointToWorldPoint(1641.0f, 546.0f), ImagePointToWorldPoint(1771.0f, 670.0f)},
        {ImagePointToWorldPoint(594.0f, 299.0f), ImagePointToWorldPoint(643.0f, 236.0f)},
        {ImagePointToWorldPoint(784.0f, 238.0f), ImagePointToWorldPoint(851.0f, 303.0f)},
        {ImagePointToWorldPoint(914.0f, 301.0f), ImagePointToWorldPoint(958.0f, 357.0f)},
        {ImagePointToWorldPoint(1536.0f, 366.0f), ImagePointToWorldPoint(1588.0f, 395.0f)},
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

    AddCurrentSlopeGuardBands();
}
