#include "App.hpp"

void App::BuildLevel2() {
    // Level 2 bootstrap:
    // keep the exact same engine/controller as Level 1, but switch to the
    // new background and use the traced inner-map rectangle for the outer
    // boundaries. Inside that square, we then place the mapped terrain.
    // Because this level is traced by eye from the image, we intentionally
    // overlap adjoining solids by a few pixels and only keep the diagonals
    // that are visually meaningful as true slope surfaces.
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 2380.0f, 47.0f));      // Roof
    m_LeftWallSpawnRect = ImageRectToWorldRect(0.0f, 0.0f, 52.0f, 1760.0f);         // Left wall
    m_SolidBlocks.push_back(m_LeftWallSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(2327.0f, 0.0f, 2380.0f, 1760.0f)); // Right wall

    // Main inner-map floor boundary, carved into real liquid basins instead of
    // one giant solid block. Safe characters should be able to enter/exit the
    // element pools, so the terrain needs upper lips, side slopes, and lower
    // liquid beds rather than a flat collider under the whole strip.
    m_LeftFloorSpawnRect = ImageRectToWorldRect(52.0f, 1705.0f, 374.0f, 1760.0f);
    m_SolidBlocks.push_back(m_LeftFloorSpawnRect);                                      // left spawn floor
    m_SolidBlocks.push_back(ImageRectToWorldRect(426.0f, 1743.0f, 851.0f, 1760.0f));   // lava basin bed
    m_SolidBlocks.push_back(ImageRectToWorldRect(903.0f, 1705.0f, 1358.0f, 1760.0f));  // middle bridge
    m_SolidBlocks.push_back(ImageRectToWorldRect(1410.0f, 1745.0f, 1835.0f, 1760.0f)); // water basin bed
    m_SolidBlocks.push_back(ImageRectToWorldRect(1887.0f, 1705.0f, 2143.0f, 1760.0f)); // right floor before ramp

    // Support mass under the right-hand ramp and the raised top-right floor.
    m_SolidBlocks.push_back(ImageRectToWorldRect(2139.0f, 1585.0f, 2250.0f, 1760.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(2246.0f, 1463.0f, 2325.0f, 1760.0f));

    // Higher middle block:
    // - slight upper-left sloped top
    // - sharp internal drop
    // - long flat run
    // - right sloped descent
    m_SolidBlocks.push_back(ImageRectToWorldRect(50.0f, 1024.0f, 58.0f, 1272.0f));    // left wall
    m_SolidBlocks.push_back(ImageRectToWorldRect(53.0f, 1032.0f, 231.0f, 1272.0f));   // support under left top slope
    m_SolidBlocks.push_back(ImageRectToWorldRect(227.0f, 1032.0f, 241.0f, 1219.0f));  // internal drop wall
    m_SolidBlocks.push_back(ImageRectToWorldRect(237.0f, 1215.0f, 2094.0f, 1272.0f)); // long flat top block
    m_SolidBlocks.push_back(ImageRectToWorldRect(2090.0f, 1217.0f, 2143.0f, 1272.0f));// support under right slope

    // Next higher block:
    // - gentle left top rise
    // - narrow step-up into a higher plateau
    // - long middle shoulder: flatten the tiny traced height differences
    //   because they are within hand-mapping error and play better as one shelf
    // - steep rise into the upper-right shelf / cap
    m_SolidBlocks.push_back(ImageRectToWorldRect(363.0f, 913.0f, 370.0f, 967.0f));    // left outer wall
    m_SolidBlocks.push_back(ImageRectToWorldRect(366.0f, 913.0f, 487.0f, 967.0f));    // support under left gentle slope
    m_SolidBlocks.push_back(ImageRectToWorldRect(483.0f, 908.0f, 494.0f, 967.0f));    // left lip before venom basin
    m_SolidBlocks.push_back(ImageRectToWorldRect(548.0f, 942.0f, 977.0f, 967.0f));    // left venom basin bed
    m_SolidBlocks.push_back(ImageRectToWorldRect(1033.0f, 908.0f, 1038.0f, 967.0f));  // tiny right lip after venom basin
    m_SolidBlocks.push_back(ImageRectToWorldRect(1034.0f, 852.0f, 1042.0f, 967.0f));  // narrow step-up wall
    m_SolidBlocks.push_back(ImageRectToWorldRect(1038.0f, 852.0f, 1403.0f, 967.0f));  // upper middle plateau
    m_SolidBlocks.push_back(ImageRectToWorldRect(1396.0f, 852.0f, 1404.0f, 967.0f));  // step-down wall
    m_SolidBlocks.push_back(ImageRectToWorldRect(1399.0f, 906.0f, 1412.0f, 967.0f));  // lead-in before right venom basin
    m_SolidBlocks.push_back(ImageRectToWorldRect(1460.0f, 942.0f, 1771.0f, 967.0f));  // right venom basin bed
    m_SolidBlocks.push_back(ImageRectToWorldRect(1828.0f, 906.0f, 2021.0f, 967.0f));  // right shoulder after venom basin
    m_SolidBlocks.push_back(ImageRectToWorldRect(2017.0f, 723.0f, 2027.0f, 967.0f));  // steep rise wall
    m_SolidBlocks.push_back(ImageRectToWorldRect(2021.0f, 723.0f, 2145.0f, 967.0f));  // upper-right shelf
    m_SolidBlocks.push_back(ImageRectToWorldRect(2140.0f, 541.0f, 2151.0f, 969.0f));  // tower step-up wall
    m_SolidBlocks.push_back(ImageRectToWorldRect(2141.0f, 547.0f, 2321.0f, 969.0f));  // support under top-right cap slope

    // Upper suspended block:
    // - almost-flat top ledge: keep it flat instead of honoring the tiny
    //   traced tilt because the image-map error is larger than the tilt itself
    // - steep descending ramp on the right
    // - short right cap
    // - stepped fill below the ramp so the interior stays solid
    m_SolidBlocks.push_back(ImageRectToWorldRect(1342.0f, 295.0f, 1846.0f, 356.0f));  // thin top slab under the traced top edge
    m_SolidBlocks.push_back(ImageRectToWorldRect(1708.0f, 352.0f, 1773.0f, 419.0f));  // left hanging support chunk
    m_SolidBlocks.push_back(ImageRectToWorldRect(1769.0f, 415.0f, 1953.0f, 478.0f));  // lower body under the right side
    m_SolidBlocks.push_back(ImageRectToWorldRect(1949.0f, 419.0f, 2016.0f, 478.0f));  // short right cap support
    m_SolidBlocks.push_back(ImageRectToWorldRect(1842.0f, 344.0f, 1882.0f, 419.0f));  // stepped fill below the ramp (upper step)
    m_SolidBlocks.push_back(ImageRectToWorldRect(1878.0f, 386.0f, 1918.0f, 419.0f));  // stepped fill below the ramp (lower step)

    // Final upper-left slab.
    m_SolidBlocks.push_back(ImageRectToWorldRect(55.0f, 297.0f, 1035.0f, 350.0f));

    // Explicit walkable slopes inferred from the polygons.
    m_Slopes = {
        {ImagePointToWorldPoint(2141.0f, 1587.0f), ImagePointToWorldPoint(2248.0f, 1469.0f)},
        {ImagePointToWorldPoint(374.0f, 1710.0f), ImagePointToWorldPoint(426.0f, 1743.0f)},
        {ImagePointToWorldPoint(851.0f, 1747.0f), ImagePointToWorldPoint(903.0f, 1710.0f)},
        {ImagePointToWorldPoint(1358.0f, 1712.0f), ImagePointToWorldPoint(1410.0f, 1745.0f)},
        {ImagePointToWorldPoint(1835.0f, 1749.0f), ImagePointToWorldPoint(1887.0f, 1712.0f)},
        {ImagePointToWorldPoint(55.0f, 1026.0f), ImagePointToWorldPoint(229.0f, 1034.0f)},
        {ImagePointToWorldPoint(2092.0f, 1219.0f), ImagePointToWorldPoint(2141.0f, 1270.0f)},
        {ImagePointToWorldPoint(368.0f, 915.0f), ImagePointToWorldPoint(485.0f, 910.0f)},
        {ImagePointToWorldPoint(494.0f, 912.0f), ImagePointToWorldPoint(548.0f, 944.0f)},
        {ImagePointToWorldPoint(977.0f, 944.0f), ImagePointToWorldPoint(1033.0f, 912.0f)},
        {ImagePointToWorldPoint(1412.0f, 912.0f), ImagePointToWorldPoint(1460.0f, 942.0f)},
        {ImagePointToWorldPoint(1771.0f, 945.0f), ImagePointToWorldPoint(1828.0f, 912.0f)},
        {ImagePointToWorldPoint(2143.0f, 549.0f), ImagePointToWorldPoint(2319.0f, 543.0f)},
        {ImagePointToWorldPoint(1844.0f, 297.0f), ImagePointToWorldPoint(1951.0f, 421.0f)},
        {ImagePointToWorldPoint(376.0f, 1524.0f), ImagePointToWorldPoint(428.0f, 1557.0f)},
        {ImagePointToWorldPoint(853.0f, 1561.0f), ImagePointToWorldPoint(905.0f, 1524.0f)},
        {ImagePointToWorldPoint(1354.0f, 1524.0f), ImagePointToWorldPoint(1406.0f, 1557.0f)},
        {ImagePointToWorldPoint(1831.0f, 1561.0f), ImagePointToWorldPoint(1883.0f, 1524.0f)},
    };

    // Level 2 floating support blocks, also carved into true liquid basins.
    m_SolidBlocks.push_back(ImageRectToWorldRect(363.0f, 1522.0f, 376.0f, 1583.0f));   // left floating lip
    m_SolidBlocks.push_back(ImageRectToWorldRect(428.0f, 1557.0f, 853.0f, 1583.0f));   // floating water bed
    m_SolidBlocks.push_back(ImageRectToWorldRect(905.0f, 1522.0f, 914.0f, 1583.0f));   // right floating lip
    m_SolidBlocks.push_back(ImageRectToWorldRect(1345.0f, 1526.0f, 1354.0f, 1587.0f)); // left floating lip
    m_SolidBlocks.push_back(ImageRectToWorldRect(1406.0f, 1557.0f, 1831.0f, 1587.0f)); // floating lava bed
    m_SolidBlocks.push_back(ImageRectToWorldRect(1883.0f, 1526.0f, 1896.0f, 1587.0f)); // right floating lip

    AddCurrentSlopeGuardBands();

    // Level 2 uses the second green-platform system as a vertical gate wall.
    // The wall starts in the center shaft, then rises upward into the upper
    // block when either of the two pressure buttons is held.
    constexpr float gateVisualLeftImage = 1141.0f;
    constexpr float gateVisualTopImage = 966.0f;
    constexpr float gateVisualRightImage = 1202.0f;
    constexpr float gateVisualBottomImage = 1214.0f;
    constexpr float gateSideOverlapImage = 10.0f;
    constexpr float gateVerticalOverlapImage = 4.0f;

    const SolidRect gateVisualRect = ImageRectToWorldRect(
        gateVisualLeftImage,
        gateVisualTopImage,
        gateVisualRightImage,
        gateVisualBottomImage
    );
    m_GreenPlatform2VisualSize = {
        gateVisualRect.size.y,
        gateVisualRect.size.x,
    };

    m_GreenPlatformRestRect2 = ImageRectToWorldRect(
        gateVisualLeftImage - gateSideOverlapImage,
        gateVisualTopImage - gateVerticalOverlapImage,
        gateVisualRightImage + gateSideOverlapImage,
        gateVisualBottomImage + gateVerticalOverlapImage
    );
    const float wallHeightImage = gateVisualBottomImage - gateVisualTopImage;
    // The gate should rise through the upper block from below, with the block
    // art hiding the overlap. The stop point is when the gate's bottom reaches
    // the bottom of that upper block, leaving
    //   wall height - block height
    // visible above the block.
    const float upperBlockBottomImage = 967.0f;
    const float wallTravelImage = gateVisualBottomImage - upperBlockBottomImage;
    m_GreenPlatformPressedRect2 = ImageRectToWorldRect(
        gateVisualLeftImage - gateSideOverlapImage,
        gateVisualTopImage - gateVerticalOverlapImage - wallTravelImage,
        gateVisualRightImage + gateSideOverlapImage,
        gateVisualBottomImage + gateVerticalOverlapImage - wallTravelImage
    );
    m_GreenPlatformCurrentRect2 = m_GreenPlatformRestRect2;
    m_GreenPlatformBlockIndex2 = m_SolidBlocks.size();
    m_HasGreenPlatformBlock2 = true;
    m_GreenPlatform2UseVerticalVisualClip = true;
    m_GreenPlatform2VisualCoverY = ImagePointToWorldPoint(0.0f, 967.0f).y;
    m_SolidBlocks.push_back(m_GreenPlatformCurrentRect2);

    m_GreenPlatform2 = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/platform_green.png"
    );
    m_GreenPlatform2->SetZIndex(7.0f);
    m_GreenPlatform2->SetSize(m_GreenPlatform2VisualSize);
    m_GreenPlatform2->SetPosition(m_GreenPlatformCurrentRect2.center);
    m_GreenPlatform2->m_Transform.rotation = 1.57079632679f;
    m_Root.AddChild(m_GreenPlatform2);

    auto gateCover = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/level2_wall_cover.png"
    );
    gateCover->SetZIndex(7.5f);
    gateCover->SetSize(ImageSizeToWorldSize(369.0f, 115.0f));
    gateCover->SetPosition(ImagePointToWorldPoint((1034.0f + 1403.0f) * 0.5f, (852.0f + 967.0f) * 0.5f));
    m_Root.AddChild(gateCover);
    m_LevelProps.push_back(gateCover);

    // Level 2 hidden bridge:
    // starts concealed inside the upper-right suspended block, then slides
    // left into the top gap when either top button is held.
    // Keep the bridge slightly lower than the surrounding top floor so that,
    // when hidden inside the right block, the real block floor remains the
    // walk surface instead of the concealed moving platform.
    m_Level2HiddenPlatformShownRect = ImageRectToWorldRect(1035.0f, 300.0f, 1342.0f, 360.0f);
    m_Level2HiddenPlatformRestRect = ImageRectToWorldRect(1342.0f, 300.0f, 1649.0f, 360.0f);
    m_Level2HiddenPlatformCurrentRect = m_Level2HiddenPlatformRestRect;
    m_Level2HiddenPlatformVisualSize = m_Level2HiddenPlatformShownRect.size;
    m_Level2HiddenPlatformBlockIndex = m_SolidBlocks.size();
    m_HasLevel2HiddenPlatformBlock = true;
    m_SolidBlocks.push_back(m_Level2HiddenPlatformCurrentRect);

    m_Level2HiddenPlatform = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/platform_green.png"
    );
    m_Level2HiddenPlatform->SetZIndex(6.9f);
    m_Level2HiddenPlatform->SetSize(m_Level2HiddenPlatformVisualSize);
    m_Level2HiddenPlatform->SetPosition(m_Level2HiddenPlatformCurrentRect.center);
    m_Root.AddChild(m_Level2HiddenPlatform);

    const std::vector<std::string> lavaLargePaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_04.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_05.png",
    };
    const std::vector<std::string> waterLargePaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/water_large_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/water_large_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/water_large_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/water_large_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/water_large_04.png",
    };
    const std::vector<std::string> venomPaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_04.png",
    };
    const std::vector<std::string> venomLargePaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_large_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_large_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_large_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_large_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_large_04.png",
    };
    const float propScale = 1.0f / 1.5f;
    const float greenButtonVisualScale = (propScale * 0.82f) / 1.5f;
    const float diamondScale = propScale * 0.90f;
    const float liquidPairOffset = 126.0f;
    const float floatingLiquidDiamondY = 1462.0f;
    const float bottomLiquidDiamondY = 1648.0f;
    const float secondBlockDiamondY = 1104.0f;
    const float secondBlockPairOffset = 155.0f;
    const float secondBlockLeftPairCenterX = 698.5f;
    const float secondBlockRightPairCenterX = 1526.5f;
    const float upperTopPairCenterX = (1035.0f + 1342.0f) * 0.5f;
    const float upperTopPairOffset = 74.0f;
    const float upperMidPairCenterX = (1038.0f + 1403.0f) * 0.5f;
    const float upperMidPairOffset = 111.0f;
    const glm::vec2 greenButtonSize = ImageSizeToWorldSize(189.0f, 120.0f, greenButtonVisualScale);
    const glm::vec2 topButtonSize = greenButtonSize;

    m_GreenButtonAfterHitbox = ImageRectToWorldRect(494.0f, 1210.0f, 571.0f, 1270.0f);
    m_GreenButtonAfterHitbox2 = ImageRectToWorldRect(1536.0f, 1210.0f, 1613.0f, 1270.0f);

    m_GreenButtonAfter = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/button_green.png"
    );
    m_GreenButtonAfter->SetZIndex(7.0f);
    m_GreenButtonAfter->SetSize(greenButtonSize);
    m_GreenButtonAfterBasePosition = {
        m_GreenButtonAfterHitbox.center.x,
        ImagePointToWorldPoint(0.0f, 1215.0f).y + greenButtonSize.y * 0.5f,
    };
    m_GreenButtonAfterBaseSize = greenButtonSize;
    m_GreenButtonAfter->SetPosition(m_GreenButtonAfterBasePosition);
    m_Root.AddChild(m_GreenButtonAfter);
    m_LevelProps.push_back(m_GreenButtonAfter);

    m_GreenButtonAfterHitbox.center = {
        m_GreenButtonAfterBasePosition.x,
        ImagePointToWorldPoint(0.0f, 1215.0f).y + std::max(4.0f, greenButtonSize.y * 0.16f) * 0.5f,
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
        ImagePointToWorldPoint(0.0f, 1215.0f).y + greenButtonSize.y * 0.5f,
    };
    m_GreenButtonAfterBaseSize2 = greenButtonSize;
    m_GreenButtonAfter2->SetPosition(m_GreenButtonAfterBasePosition2);
    m_Root.AddChild(m_GreenButtonAfter2);
    m_LevelProps.push_back(m_GreenButtonAfter2);

    m_GreenButtonAfterHitbox2.center = {
        m_GreenButtonAfterBasePosition2.x,
        ImagePointToWorldPoint(0.0f, 1215.0f).y + std::max(4.0f, greenButtonSize.y * 0.16f) * 0.5f,
    };
    m_GreenButtonAfterHitbox2.size = {
        greenButtonSize.x * 0.72f,
        std::max(4.0f, greenButtonSize.y * 0.16f),
    };

    // Top buttons for the hidden bridge.
    const float topButtonFloorYImage = 299.0f;
    const float topButtonStripHeight = std::max(4.0f, topButtonSize.y * 0.16f);

    m_Level2TopButtonLeftHitbox = ImageRectToWorldRect(536.0f, 261.0f, 602.0f, 292.0f);
    m_Level2TopButtonLeft = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/button_green.png"
    );
    m_Level2TopButtonLeft->SetZIndex(7.6f);
    m_Level2TopButtonLeft->SetSize(topButtonSize);
    m_Level2TopButtonBaseSize = topButtonSize;
    m_Level2TopButtonLeftBasePosition = {
        m_Level2TopButtonLeftHitbox.center.x,
        ImagePointToWorldPoint(0.0f, topButtonFloorYImage).y + topButtonSize.y * 0.5f,
    };
    m_Level2TopButtonLeft->SetPosition(m_Level2TopButtonLeftBasePosition);
    m_Root.AddChild(m_Level2TopButtonLeft);
    m_LevelProps.push_back(m_Level2TopButtonLeft);
    m_Level2TopButtonLeftHitbox.center = {
        m_Level2TopButtonLeftBasePosition.x,
        ImagePointToWorldPoint(0.0f, topButtonFloorYImage).y + topButtonStripHeight * 0.5f,
    };
    m_Level2TopButtonLeftHitbox.size = {
        topButtonSize.x * 0.72f,
        topButtonStripHeight,
    };

    m_Level2TopButtonRightHitbox = ImageRectToWorldRect(1708.0f, 259.0f, 1774.0f, 290.0f);
    m_Level2TopButtonRight = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/button_green.png"
    );
    m_Level2TopButtonRight->SetZIndex(7.6f);
    m_Level2TopButtonRight->SetSize(topButtonSize);
    m_Level2TopButtonRightBasePosition = {
        m_Level2TopButtonRightHitbox.center.x,
        ImagePointToWorldPoint(0.0f, topButtonFloorYImage).y + topButtonSize.y * 0.5f,
    };
    m_Level2TopButtonRight->SetPosition(m_Level2TopButtonRightBasePosition);
    m_Root.AddChild(m_Level2TopButtonRight);
    m_LevelProps.push_back(m_Level2TopButtonRight);
    m_Level2TopButtonRightHitbox.center = {
        m_Level2TopButtonRightBasePosition.x,
        ImagePointToWorldPoint(0.0f, topButtonFloorYImage).y + topButtonStripHeight * 0.5f,
    };
    m_Level2TopButtonRightHitbox.size = {
        topButtonSize.x * 0.72f,
        topButtonStripHeight,
    };

    const float doorScale = propScale * 1.6f;
    const float topLeftDoorFloorYImage = 299.0f;

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
        (101.0f + 189.0f) * 0.5f, topLeftDoorFloorYImage, 160.0f, 205.0f, 7.2f, doorScale
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
        (278.0f + 366.0f) * 0.5f, topLeftDoorFloorYImage, 162.0f, 205.0f, 7.2f, doorScale
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

    auto hiddenPlatformCover = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/level2_hidden_platform_cover.png"
    );
    hiddenPlatformCover->SetZIndex(7.4f);
    hiddenPlatformCover->SetSize(ImageSizeToWorldSize(504.0f, 61.0f));
    hiddenPlatformCover->SetPosition(ImagePointToWorldPoint(
        (1342.0f + 1846.0f) * 0.5f,
        (295.0f + 356.0f) * 0.5f
    ));
    m_Root.AddChild(hiddenPlatformCover);
    m_LevelProps.push_back(hiddenPlatformCover);

    // Level 2 diamonds:
    // most of these coordinates were traced "a ojimetro", so for the flat-line
    // mappings we place the diamond slightly above that line instead of
    // treating it as the exact center. We can fine-tune each one after the
    // first in-game pass.
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        640.5f - liquidPairOffset, floatingLiquidDiamondY, 117.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        640.5f + liquidPairOffset, floatingLiquidDiamondY, 117.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        1622.5f - liquidPairOffset, bottomLiquidDiamondY, 117.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        1622.5f + liquidPairOffset, bottomLiquidDiamondY, 117.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        secondBlockRightPairCenterX - secondBlockPairOffset, secondBlockDiamondY, 117.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        secondBlockLeftPairCenterX - secondBlockPairOffset, secondBlockDiamondY, 117.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        upperTopPairCenterX + upperTopPairOffset, (203.0f + 161.0f) * 0.5f, 117.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        DiamondType::Water,
        upperTopPairCenterX + upperTopPairOffset, (785.0f + 743.0f) * 0.5f, 117.0f, 111.0f, 8.4f, diamondScale
    );

    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        638.5f - liquidPairOffset, bottomLiquidDiamondY, 121.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        638.5f + liquidPairOffset, bottomLiquidDiamondY, 121.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        1618.5f - liquidPairOffset, floatingLiquidDiamondY, 121.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        1618.5f + liquidPairOffset, floatingLiquidDiamondY, 121.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        secondBlockRightPairCenterX + secondBlockPairOffset, secondBlockDiamondY, 121.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        secondBlockLeftPairCenterX + secondBlockPairOffset, secondBlockDiamondY, 121.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        upperMidPairCenterX - upperMidPairOffset, (785.0f + 743.0f) * 0.5f, 121.0f, 111.0f, 8.4f, diamondScale
    );
    AddCollectibleDiamond(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        DiamondType::Fire,
        upperTopPairCenterX - upperTopPairOffset, (203.0f + 161.0f) * 0.5f, 121.0f, 111.0f, 8.4f, diamondScale
    );

    // Base-floor liquids.
    AddAnimatedHazardInImageTrap(
        lavaLargePaths,
        HazardRect::Type::Lava,
        360.0f, 80.0f,
        374.0f, 1709.0f,
        903.0f, 1711.0f,
        426.0f, 1743.0f,
        851.0f, 1747.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterLargePaths,
        HazardRect::Type::Water,
        360.0f, 80.0f,
        1358.0f, 1711.0f,
        1887.0f, 1713.0f,
        1410.0f, 1745.0f,
        1835.0f, 1749.0f,
        10.2f
    );

    // Floating-block liquids.
    AddAnimatedHazardInImageTrap(
        lavaLargePaths,
        HazardRect::Type::Lava,
        360.0f, 80.0f,
        1354.0f, 1523.0f,
        1883.0f, 1525.0f,
        1406.0f, 1557.0f,
        1831.0f, 1561.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterLargePaths,
        HazardRect::Type::Water,
        360.0f, 80.0f,
        376.0f, 1523.0f,
        905.0f, 1525.0f,
        428.0f, 1557.0f,
        853.0f, 1561.0f,
        10.2f
    );

    // Mid-height venom strips.
    AddAnimatedHazardInImageTrap(
        venomLargePaths,
        HazardRect::Type::Venom,
        360.0f, 80.0f,
        494.0f, 915.0f,
        1033.0f, 908.0f,
        548.0f, 946.0f,
        977.0f, 942.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        venomPaths,
        HazardRect::Type::Venom,
        178.0f, 45.0f,
        1412.0f, 915.0f,
        1828.0f, 910.0f,
        1460.0f, 942.0f,
        1771.0f, 948.0f,
        10.2f
    );
}
