#include "App.hpp"

#include "Util/Animation.hpp"

void App::BuildLevel3() {
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
    const std::vector<std::string> fanPaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/fan00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/fan01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/fan02.png",
    };
    // Block 6 is rebuilt from the new 2380 x 1760 image-map trace. The trace
    // follows the visible stone silhouette, so exposed upper edges become floor
    // slopes and exposed undersides become ceiling slopes.
    constexpr float topLeftSlabLeft = 242.0f;
    constexpr float topLeftSlabRight = 1101.0f;
    constexpr float topLeftSlabTop = 219.0f;
    constexpr float topSlabCoreBottom = 272.0f;

    constexpr float topRightSlabLeft = 1296.0f;
    constexpr float topRightSlabRight = 2090.0f;
    constexpr float topRightSlabTop = 221.0f;

    constexpr float leftPillarLeft = 550.0f;
    constexpr float leftPillarRight = 609.0f;
    constexpr float leftPillarTop = 272.0f;

    constexpr float rightPillarLeft = 1790.0f;
    constexpr float rightPillarRight = 1849.0f;
    constexpr float rightPillarTop = 272.0f;

    constexpr float leftLedgeLeft = 609.0f;
    constexpr float leftLedgeLiquidLeft = 670.0f;
    constexpr float leftLedgeLiquidRight = 796.0f;
    constexpr float leftLedgeRight = 855.0f;
    constexpr float leftLedgeLipY = 776.0f;
    constexpr float leftLedgeBedY = 810.0f;
    constexpr float leftLedgeBottomY = 833.0f;

    constexpr float rightLedgeLeft = 1546.0f;
    constexpr float rightLedgeLiquidLeft = 1601.0f;
    constexpr float rightLedgeLiquidRight = 1727.0f;
    constexpr float rightLedgeRight = 1792.0f;
    constexpr float rightLedgeLeftLipY = 776.0f;
    constexpr float rightLedgeRightLipY = 770.0f;
    constexpr float rightLedgeBedLeftY = 803.0f;
    constexpr float rightLedgeBedRightY = 801.0f;
    constexpr float rightLedgeBottomY = 828.0f;

    constexpr float diamondLeftX = 985.0f;
    constexpr float diamondMiddleY = 650.0f;
    constexpr float diamondTopLeftX = 1164.0f;
    constexpr float diamondTopRightX = 1235.0f;
    constexpr float diamondTopY = 469.0f;
    constexpr float diamondRightX = 1405.0f;
    constexpr float diamondLowerLeftX = 1099.0f;
    constexpr float diamondLowerLeftY = 763.0f;
    constexpr float diamondLowerRightX = 1286.0f;
    constexpr float diamondLowerY = 768.0f;

    constexpr float centerStemLeft = 1170.0f;
    constexpr float centerStemRight = 1227.0f;
    constexpr float centerStemTop = 768.0f;
    constexpr float centerStemBottom = 952.0f;

    constexpr float baseLeftTipX = 307.0f;
    constexpr float baseLeftRampTopX = 366.0f;
    constexpr float baseTopY = 1083.0f;
    constexpr float baseCoreTopY = 1085.0f;
    constexpr float baseRightRampTopX = 2040.0f;
    constexpr float baseRightRampTopY = 1085.0f;
    constexpr float baseRightTipX = 2082.0f;
    constexpr float baseBottomY = 1137.0f;

    // Preserve the established visual door anchor while rebuilding the floor.
    constexpr float baseBridgeTopY = 1086.0f;

    // Main outer frame.
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 2380.0f, 26.0f));
    m_LeftWallSpawnRect = ImageRectToWorldRect(0.0f, 0.0f, 46.0f, 1760.0f);
    m_SolidBlocks.push_back(m_LeftWallSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(2348.0f, 0.0f, 2380.0f, 1760.0f));

    // ---------------------------------------------------------------------
    // Block 1: lower floor with two basins and a central climb.
    // Latest level-design direction wins here:
    // - left down = lava
    // - right down = water
    // ---------------------------------------------------------------------
    m_LeftFloorSpawnRect = ImageRectToWorldRect(46.0f, 1702.0f, 366.0f, 1760.0f);
    m_SolidBlocks.push_back(m_LeftFloorSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(418.0f, 1737.0f, 796.0f, 1760.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1040.0f, 1519.0f, 1353.0f, 1760.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1599.0f, 1731.0f, 1968.0f, 1760.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(2014.0f, 1702.0f, 2340.0f, 1760.0f));

    // ---------------------------------------------------------------------
    // Block 2: right middle basin shelf.
    // Current direction keeps this upper-right pool as lava.
    // ---------------------------------------------------------------------
    m_SolidBlocks.push_back(ImageRectToWorldRect(1418.0f, 1328.0f, 1603.0f, 1389.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1603.0f, 1363.0f, 1979.0f, 1389.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1979.0f, 1328.0f, 2216.0f, 1389.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(2216.0f, 1208.0f, 2346.0f, 1389.0f));

    // ---------------------------------------------------------------------
    // Block 3: left middle basin shelf.
    // Latest direction sets the upper-left pool to water.
    // ---------------------------------------------------------------------
    m_SolidBlocks.push_back(ImageRectToWorldRect(48.0f, 1206.0f, 170.0f, 1389.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(166.0f, 1332.0f, 416.0f, 1389.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(416.0f, 1366.0f, 796.0f, 1389.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(796.0f, 1334.0f, 973.0f, 1389.0f));

    // Side floating slabs.
    m_SolidBlocks.push_back(ImageRectToWorldRect(53.0f, 897.0f, 359.0f, 958.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(2039.0f, 899.0f, 2345.0f, 960.0f));

    // ---------------------------------------------------------------------
    // Block 6: polygon-derived main structure.
    // ---------------------------------------------------------------------
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        topLeftSlabLeft, topLeftSlabTop, topLeftSlabRight, topSlabCoreBottom, false
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        topRightSlabLeft, topRightSlabTop, topRightSlabRight, topSlabCoreBottom, false
    ));

    m_SolidBlocks.push_back(ImageRectToWorldRect(
        leftPillarLeft, leftPillarTop, leftPillarRight, baseTopY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        rightPillarLeft, rightPillarTop, rightPillarRight, baseTopY
    ));

    // Pool ledges start at the liquid beds. The lips and undersides are slopes.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        leftLedgeLeft, leftLedgeBedY, leftLedgeRight, leftLedgeBottomY, false
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        rightLedgeLeft, rightLedgeBedLeftY, rightLedgeRight, rightLedgeBottomY, false
    ));

    // Diamond fills remain inside the traced shell. Their tops are below the
    // upper slopes, avoiding competing ground surfaces.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        diamondTopLeftX, diamondTopY, diamondTopRightX, diamondLowerY, false
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        1023.0f, 612.0f, 1061.0f, 687.0f, false
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        1061.0f, 574.0f, diamondLowerLeftX, 725.0f, false
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        diamondLowerLeftX, 535.0f, diamondTopLeftX, diamondLowerLeftY, false
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        diamondTopRightX, 524.0f, diamondLowerRightX, diamondLowerY, false
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        diamondLowerRightX, 566.0f, 1326.0f, 728.0f, false
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        1326.0f, 609.0f, 1366.0f, 688.0f, false
    ));

    // The narrow stem is the only full-height center rectangle.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerStemLeft, centerStemTop, centerStemRight, centerStemBottom
    ));

    // Internal wedge fills stay strictly below the two lower walkable slopes.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        1086.0f, 1040.0f, 1128.0f, baseCoreTopY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        1128.0f, 996.0f, centerStemLeft, baseCoreTopY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerStemRight, 995.0f, 1271.0f, baseCoreTopY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        1271.0f, 1040.0f, 1315.0f, baseCoreTopY
    ));

    // The base core meets the right ramp at y=1085. Full-depth left sections
    // retain the traced y=1083 floor without adding thin sliver rectangles.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        baseLeftRampTopX, baseCoreTopY, baseRightRampTopX, baseBottomY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        baseLeftRampTopX, baseTopY, 552.0f, baseBottomY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        605.0f, baseTopY, 1044.0f, baseBottomY
    ));

    m_Slopes = {
        // Block 1
        {ImagePointToWorldPoint(366.0f, 1702.0f), ImagePointToWorldPoint(418.0f, 1737.0f)},
        {ImagePointToWorldPoint(796.0f, 1737.0f), ImagePointToWorldPoint(853.0f, 1704.0f)},
        {ImagePointToWorldPoint(853.0f, 1704.0f), ImagePointToWorldPoint(1040.0f, 1519.0f)},
        {ImagePointToWorldPoint(1353.0f, 1519.0f), ImagePointToWorldPoint(1536.0f, 1704.0f)},
        {ImagePointToWorldPoint(1536.0f, 1704.0f), ImagePointToWorldPoint(1599.0f, 1731.0f)},
        {ImagePointToWorldPoint(1968.0f, 1731.0f), ImagePointToWorldPoint(2014.0f, 1702.0f)},

        // Block 2
        {ImagePointToWorldPoint(1544.0f, 1338.0f), ImagePointToWorldPoint(1603.0f, 1363.0f)},
        {ImagePointToWorldPoint(1979.0f, 1363.0f), ImagePointToWorldPoint(2029.0f, 1328.0f)},

        // Block 3
        {ImagePointToWorldPoint(372.0f, 1332.0f), ImagePointToWorldPoint(416.0f, 1366.0f)},
        {ImagePointToWorldPoint(796.0f, 1366.0f), ImagePointToWorldPoint(855.0f, 1334.0f)},

        // Block 6 liquid ledges.
        {ImagePointToWorldPoint(leftLedgeLeft, leftLedgeLipY),
         ImagePointToWorldPoint(leftLedgeLiquidLeft, leftLedgeBedY)},
        {ImagePointToWorldPoint(leftLedgeLiquidRight, leftLedgeBedY),
         ImagePointToWorldPoint(849.0f, leftLedgeLipY)},
        {ImagePointToWorldPoint(rightLedgeLeft, rightLedgeLeftLipY),
         ImagePointToWorldPoint(rightLedgeLiquidLeft, rightLedgeBedLeftY)},
        {ImagePointToWorldPoint(rightLedgeLiquidRight, rightLedgeBedRightY),
         ImagePointToWorldPoint(rightLedgeRight, rightLedgeRightLipY)},

        // Block 6 center diamond: only the visible upper edges are floors.
        {ImagePointToWorldPoint(diamondLeftX, diamondMiddleY),
         ImagePointToWorldPoint(diamondTopLeftX, diamondTopY)},
        {ImagePointToWorldPoint(diamondTopRightX, diamondTopY),
         ImagePointToWorldPoint(diamondRightX, diamondMiddleY)},

        // Block 6 base and lower mound.
        {ImagePointToWorldPoint(baseLeftTipX, 1135.0f),
         ImagePointToWorldPoint(baseLeftRampTopX, baseTopY)},
        {ImagePointToWorldPoint(1044.0f, baseTopY),
         ImagePointToWorldPoint(centerStemLeft, centerStemBottom)},
        {ImagePointToWorldPoint(centerStemRight, 950.0f),
         ImagePointToWorldPoint(1359.0f, baseRightRampTopY)},
        {ImagePointToWorldPoint(baseRightRampTopX, baseRightRampTopY),
         ImagePointToWorldPoint(baseRightTipX, baseBottomY)},
    };

    // Ceiling slopes are only roof-like exposed undersides. No lower mound
    // slope is duplicated here as a ceiling.
    m_CeilingSlopes = {
        // Top slab undersides.
        {ImagePointToWorldPoint(239.0f, 272.0f),
         ImagePointToWorldPoint(leftPillarLeft, 280.0f)},
        {ImagePointToWorldPoint(leftPillarRight, 282.0f),
         ImagePointToWorldPoint(1099.0f, 272.0f)},
        {ImagePointToWorldPoint(1294.0f, 274.0f),
         ImagePointToWorldPoint(rightPillarLeft, 272.0f)},
        {ImagePointToWorldPoint(rightPillarRight, 276.0f),
         ImagePointToWorldPoint(topRightSlabRight, 272.0f)},

        // Liquid ledge undersides.
        {ImagePointToWorldPoint(leftLedgeLeft, 828.0f),
         ImagePointToWorldPoint(leftLedgeRight, leftLedgeBottomY)},
        {ImagePointToWorldPoint(rightLedgeLeft, 826.0f),
         ImagePointToWorldPoint(rightLedgeRight, rightLedgeBottomY)},

        // Diamond lower shell.
        {ImagePointToWorldPoint(diamondLeftX, diamondMiddleY),
         ImagePointToWorldPoint(diamondLowerLeftX, diamondLowerLeftY)},
        {ImagePointToWorldPoint(diamondLowerLeftX, diamondLowerLeftY),
         ImagePointToWorldPoint(centerStemLeft, centerStemTop)},
        {ImagePointToWorldPoint(1225.0f, diamondLowerY),
         ImagePointToWorldPoint(diamondLowerRightX, diamondLowerY)},
        {ImagePointToWorldPoint(diamondLowerRightX, diamondLowerY),
         ImagePointToWorldPoint(diamondRightX, diamondMiddleY)},
    };

    AddCurrentSlopeGuardBands();

    // Liquids. Level 3 direction:
    // - left up (center structure left ledge) = lava
    // - left down (lower-left basin) = lava
    // - right up (center structure right ledge) = water
    // - right down (lower-right basin) = water
    // - upper-left shelf (Block 3) = water
    // - upper-right shelf (Block 2) = lava
    AddAnimatedHazardInImageTrap(
        lavaLargePaths, HazardRect::Type::Lava,
        360.0f, 80.0f,
        366.0f, 1702.0f,
        853.0f, 1704.0f,
        418.0f, 1737.0f,
        796.0f, 1742.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterLargePaths, HazardRect::Type::Water,
        360.0f, 80.0f,
        1536.0f, 1704.0f,
        2014.0f, 1708.0f,
        1599.0f, 1735.0f,
        1968.0f, 1731.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        lavaLargePaths, HazardRect::Type::Lava,
        360.0f, 80.0f,
        1544.0f, 1338.0f,
        2029.0f, 1328.0f,
        1603.0f, 1368.0f,
        1979.0f, 1363.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterLargePaths, HazardRect::Type::Water,
        360.0f, 80.0f,
        372.0f, 1332.0f,
        855.0f, 1334.0f,
        416.0f, 1368.0f,
        796.0f, 1366.0f,
        10.2f
    );
    // Upper-left small pool on center floating structure = lava per level design.
    AddAnimatedHazardInImageTrap(
        lavaPaths, HazardRect::Type::Lava,
        177.0f, 61.0f,
        leftLedgeLeft, leftLedgeLipY,
        849.0f, leftLedgeLipY,
        leftLedgeLiquidLeft, leftLedgeBedY,
        leftLedgeLiquidRight, leftLedgeBedY,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterPaths, HazardRect::Type::Water,
        178.0f, 60.0f,
        rightLedgeLeft, rightLedgeLeftLipY,
        rightLedgeRight, rightLedgeRightLipY,
        rightLedgeLiquidLeft, rightLedgeBedLeftY,
        rightLedgeLiquidRight, rightLedgeBedRightY,
        10.2f
    );

    const float propScale = 1.0f / 1.5f;
    const float doorScale = propScale * 1.6f;
    const float centerDoorFloorYImage = baseBridgeTopY;

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
        770.0f, centerDoorFloorYImage, 160.0f, 205.0f, 7.2f, doorScale
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
        1636.0f, centerDoorFloorYImage, 162.0f, 205.0f, 7.2f, doorScale
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

    const float diamondScale = propScale * 0.90f;
    auto addBlueDiamond = [&](float centerXImage, float centerYImage) {
        AddCollectibleDiamond(
            std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
            DiamondType::Water,
            centerXImage, centerYImage, 117.0f, 111.0f, 8.4f, diamondScale
        );
    };
    auto addRedDiamond = [&](float centerXImage, float centerYImage) {
        AddCollectibleDiamond(
            std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
            DiamondType::Fire,
            centerXImage, centerYImage, 121.0f, 111.0f, 8.4f, diamondScale
        );
    };

    // Level 3 diamonds mapped from the original layout reference.
    addBlueDiamond(129.5f, 335.0f);
    addBlueDiamond(129.5f, 532.0f);
    addBlueDiamond(129.5f, 715.0f);
    addBlueDiamond(771.0f, 156.0f);
    addBlueDiamond(951.0f, 594.0f);
    addBlueDiamond(1676.0f, 720.0f);
    addBlueDiamond(519.0f, 1278.0f);
    addBlueDiamond(713.0f, 1278.0f);
    addBlueDiamond(1499.0f, 1581.0f);
    addBlueDiamond(1679.0f, 1658.0f);
    addBlueDiamond(1924.0f, 1658.0f);

    addRedDiamond(1620.0f, 156.0f);
    addRedDiamond(2265.5f, 335.0f);
    addRedDiamond(2265.5f, 532.0f);
    addRedDiamond(2265.5f, 715.0f);
    addRedDiamond(711.0f, 720.0f);
    addRedDiamond(1435.0f, 594.0f);
    addRedDiamond(1629.0f, 1278.0f);
    addRedDiamond(1859.0f, 1278.0f);
    addRedDiamond(896.0f, 1581.0f);
    addRedDiamond(528.0f, 1658.0f);
    addRedDiamond(711.0f, 1658.0f);

    // Side fans pushing inward across the middle chamber.
    {
        auto leftFanAnimation = std::make_shared<Util::Animation>(fanPaths, true, 125, true, 0);
        leftFanAnimation->SetSize(ImageSizeToWorldSize(161.0f, 80.0f));
        auto leftFan = std::make_shared<Util::GameObject>(leftFanAnimation, 7.2f);
        leftFan->m_Transform.translation = ImagePointToWorldPoint((49.0f + 210.0f) * 0.5f, (820.0f + 900.0f) * 0.5f);
        m_Root.AddChild(leftFan);
        m_AnimatedLevelProps.push_back(leftFan);
        m_Fans.push_back({
            leftFan,
            ImageRectToWorldRect(49.0f, 238.0f, 210.0f, 934.0f, false),
            {0.0f, 0.24f}
        });
    }

    {
        auto rightFanAnimation = std::make_shared<Util::Animation>(fanPaths, true, 125, true, 0);
        rightFanAnimation->SetSize(ImageSizeToWorldSize(161.0f, 80.0f));
        auto rightFan = std::make_shared<Util::GameObject>(rightFanAnimation, 7.2f);
        rightFan->m_Transform.translation = ImagePointToWorldPoint((2185.0f + 2346.0f) * 0.5f, (820.0f + 900.0f) * 0.5f);
        rightFan->m_Transform.scale = {-1.0f, 1.0f};
        m_Root.AddChild(rightFan);
        m_AnimatedLevelProps.push_back(rightFan);
        m_Fans.push_back({
            rightFan,
            ImageRectToWorldRect(2185.0f, 238.0f, 2346.0f, 934.0f, false),
            {0.0f, 0.24f}
        });
    }
}
