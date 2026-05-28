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
    // Block 6 comes from the latest VIA polyline traced directly on the real
    // 2380 x 1760 background. We keep the playable silhouette from that trace,
    // but still decompose it into stable solids + explicit slopes for this
    // engine rather than trying to do literal polygon collision.
    constexpr float topLeftPlatformLeft = 245.0f;
    constexpr float topLeftPlatformRight = 1100.0f;
    constexpr float topLeftPlatformTop = 220.0f;
    constexpr float topLeftPlatformBottom = 280.0f;

    constexpr float topRightPlatformLeft = 1296.0f;
    constexpr float topRightPlatformRight = 2088.0f;
    constexpr float topRightPlatformTop = 220.0f;
    constexpr float topRightPlatformBottom = 276.0f;

    constexpr float leftPillarLeft = 553.0f;
    constexpr float leftPillarRight = 611.0f;
    constexpr float leftPillarTop = 278.0f;
    constexpr float leftPillarBottom = 1084.0f;

    constexpr float rightPillarLeft = 1785.0f;
    constexpr float rightPillarRight = 1848.0f;
    constexpr float rightPillarTop = 276.0f;
    constexpr float rightPillarBottom = 1084.0f;

    constexpr float leftLedgeOuterLeft = 607.0f;
    constexpr float leftLedgeLiquidLeft = 669.0f;
    constexpr float leftLedgeLiquidRight = 789.0f;
    constexpr float leftLedgeOuterRight = 857.0f;
    constexpr float leftLedgeTopLeftY = 778.0f;
    constexpr float leftLedgeTopRightY = 776.0f;
    constexpr float leftLedgeLiquidBottomLeftY = 808.0f;
    constexpr float leftLedgeLiquidBottomRightY = 810.0f;
    constexpr float leftLedgeBottomY = 836.0f;

    constexpr float rightLedgeOuterLeft = 1542.0f;
    constexpr float rightLedgeLiquidLeft = 1597.0f;
    constexpr float rightLedgeLiquidRight = 1725.0f;
    constexpr float rightLedgeOuterRight = 1790.0f;
    constexpr float rightLedgeTopLeftY = 776.0f;
    constexpr float rightLedgeTopRightY = 778.0f;
    constexpr float rightLedgeLiquidBottomLeftY = 810.0f;
    constexpr float rightLedgeLiquidBottomRightY = 810.0f;
    constexpr float rightLedgeBottomY = 836.0f;

    // Fill the shoulder mass under the peak, but keep that fill lower than the
    // visible ramp shell so it seals the object without flattening the slope.
    constexpr float centerLeftShoulderFillLeft = 982.0f;
    constexpr float centerLeftShoulderFillRight = 1109.0f;
    constexpr float centerLeftShoulderFillTop = 690.0f;
    constexpr float centerLeftShoulderFillBottom = 769.0f;

    constexpr float centerRightShoulderFillLeft = 1227.0f;
    constexpr float centerRightShoulderFillRight = 1412.0f;
    constexpr float centerRightShoulderFillTop = 684.0f;
    constexpr float centerRightShoulderFillBottom = 771.0f;

    // Left shoulder geometry (imgX=982–1109, imgY=653–769) and right shoulder
    // (imgX=1227–1412, imgY=644–771) are intentionally NOT added as solid blocks.
    // Their slope guard bands provide all needed ground for those surfaces.
    // A competing flat block top caused ApplySlopeFollow to fire a one-frame
    // ~24 wu Y-pop whenever a character walked from the flat block onto the rising
    // peak slope — the same X range, but two different Y values in the same frame.

    constexpr float centerTopCapLeft = 1165.0f;
    constexpr float centerTopCapRight = 1227.0f;
    constexpr float centerTopCapTop = 470.0f;
    constexpr float centerTopCapBottom = 653.0f;

    constexpr float centerStemLeft = 1109.0f;
    constexpr float centerStemRight = 1227.0f;
    constexpr float centerStemTop = 769.0f;
    constexpr float centerStemBottom = 958.0f;

    constexpr float centerLowerLeftSupportLeft = 1037.0f;
    constexpr float centerLowerLeftSupportRight = 1171.0f;
    constexpr float centerLowerLeftSupportTop = 961.0f;
    constexpr float centerLowerLeftSupportBottom = 1086.0f;

    constexpr float centerLowerRightSupportLeft = 1227.0f;
    constexpr float centerLowerRightSupportRight = 1352.0f;
    constexpr float centerLowerRightSupportTop = 958.0f;
    constexpr float centerLowerRightSupportBottom = 1081.0f;

    constexpr float baseBridgeTopLeftX = 366.0f;
    constexpr float baseBridgeTopRightX = 2037.0f;
    constexpr float baseBridgeTopY = 1084.0f;
    constexpr float baseBridgeBottomLeftX = 301.0f;
    constexpr float baseBridgeBottomRightX = 2086.0f;
    constexpr float baseBridgeBottomY = 1137.0f;

    // Level 3 uses the same collision model as the other levels:
    // - SolidRect for the stable terrain mass
    // - SlopeSurface only for the meaningful playable diagonals
    // Because this map was traced by eye from the background, small micro-tilts
    // are flattened into shelves when they are more likely mapping noise than
    // real gameplay ramps.

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
    // Block 6: upper structure.
    // This one is intentionally decomposed into:
    // - top slabs and hanging columns
    // - two upper liquid ledges
    // - the center peak/stem
    // - lower support shelves that sit *under* the traced outer slopes
    // The goal is to preserve the playable silhouette without flattening the
    // ramps into one giant invisible rectangle.
    // ---------------------------------------------------------------------
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        topLeftPlatformLeft, topLeftPlatformTop, topLeftPlatformRight, topLeftPlatformBottom
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        topRightPlatformLeft, topRightPlatformTop, topRightPlatformRight, topRightPlatformBottom
    ));

    m_SolidBlocks.push_back(ImageRectToWorldRect(
        leftPillarLeft, leftPillarTop, leftPillarRight, leftPillarBottom
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        rightPillarLeft, rightPillarTop, rightPillarRight, rightPillarBottom
    ));

    // Split the shallow upper pools into lips + lower bed so the liquids can
    // actually exist in the geometry instead of sitting on one flat solid.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        leftLedgeOuterLeft, leftLedgeTopLeftY, leftLedgeLiquidLeft, leftLedgeBottomY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        leftLedgeLiquidLeft, leftLedgeLiquidBottomLeftY, leftLedgeLiquidRight, leftLedgeBottomY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        leftLedgeLiquidRight, leftLedgeTopRightY, leftLedgeOuterRight, leftLedgeBottomY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        rightLedgeOuterLeft, rightLedgeTopLeftY, rightLedgeLiquidLeft, rightLedgeBottomY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        rightLedgeLiquidLeft, rightLedgeLiquidBottomLeftY, rightLedgeLiquidRight, rightLedgeBottomY
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        rightLedgeLiquidRight, rightLedgeTopRightY, rightLedgeOuterRight, rightLedgeBottomY
    ));

    // Center peak solid: top cap + stem + low underfills inside the shoulders.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerTopCapLeft, centerTopCapTop, centerTopCapRight, centerTopCapBottom
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerLeftShoulderFillLeft,
        centerLeftShoulderFillTop,
        centerLeftShoulderFillRight,
        centerLeftShoulderFillBottom,
        false
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerRightShoulderFillLeft,
        centerRightShoulderFillTop,
        centerRightShoulderFillRight,
        centerRightShoulderFillBottom,
        false
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerStemLeft, centerStemTop, centerStemRight, centerStemBottom
    ));
    // Do not fill the lower support ramps with flat-topped rectangles.
    // Those tops sit above the visible diagonal shell across most of their width
    // and create uphill-only snags / pops when characters try to climb Level 3.
    // The bridge, stem, ramp slopes, guard bands, and underside blocking are
    // enough to keep this mass sealed without flattening the ramps.

    // Long connected base below the floating center.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        baseBridgeTopLeftX, baseBridgeTopY, baseBridgeTopRightX, baseBridgeBottomY
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

        // Block 6 upper ledges and peak
        {ImagePointToWorldPoint(609.0f, 778.0f), ImagePointToWorldPoint(669.0f, 808.0f)},
        {ImagePointToWorldPoint(789.0f, 810.0f), ImagePointToWorldPoint(857.0f, 776.0f)},
        {ImagePointToWorldPoint(857.0f, 776.0f), ImagePointToWorldPoint(982.0f, 653.0f)},
        // Both peak slopes share one endpoint at topCap center (imgX=1196, midpoint of 1165–1227).
        // This eliminates the 62-px gap where neither slope owned the surface, stopping
        // ApplySlopeFollow from extrapolating ~9 wu above the topCap block and oscillating.
        {ImagePointToWorldPoint(982.0f, 653.0f), ImagePointToWorldPoint(1196.0f, 470.0f)},
        {ImagePointToWorldPoint(1196.0f, 470.0f), ImagePointToWorldPoint(1412.0f, 644.0f)},
        {ImagePointToWorldPoint(1412.0f, 644.0f), ImagePointToWorldPoint(1296.0f, 766.0f)},
        // End pinned to centerStemTop: was 771 (0.8 wu below stem top 769), caused oscillation.
        {ImagePointToWorldPoint(1296.0f, 766.0f), ImagePointToWorldPoint(1227.0f, centerStemTop)},
        // Removed: {(1109,769)→(982,653)} — inner-left shoulder slope.
        // It created a triple junction at (982,653) alongside the outer-left and
        // left-peak slopes. The left-peak slope guard bands cover this zone.
        {ImagePointToWorldPoint(1542.0f, 776.0f), ImagePointToWorldPoint(1597.0f, 810.0f)},
        {ImagePointToWorldPoint(1725.0f, 810.0f), ImagePointToWorldPoint(1790.0f, 778.0f)},

        // Block 6 lower supports.
        // Endpoint Y values pinned to adjacent surfaces so ResolveSlopeGrounding
        // and the block-top detector agree at each seam and do not oscillate.
        {ImagePointToWorldPoint(baseBridgeBottomLeftX, baseBridgeBottomY), ImagePointToWorldPoint(baseBridgeTopLeftX, baseBridgeTopY)},
        // Start pinned to baseBridgeTopY: was 1086 (0.8 wu below bridge), caused oscillation.
        {ImagePointToWorldPoint(centerLowerLeftSupportLeft, baseBridgeTopY), ImagePointToWorldPoint(centerLowerLeftSupportRight, centerLowerLeftSupportTop)},
        // End pinned to baseBridgeTopY: was 1081 (1.2 wu above bridge), caused pop.
        {ImagePointToWorldPoint(centerLowerRightSupportLeft, centerLowerRightSupportTop), ImagePointToWorldPoint(centerLowerRightSupportRight, baseBridgeTopY)},
        {ImagePointToWorldPoint(baseBridgeTopRightX, baseBridgeTopY), ImagePointToWorldPoint(baseBridgeBottomRightX, baseBridgeBottomY)},
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
        615.0f, 771.0f,
        851.0f, 769.0f,
        671.0f, 803.0f,
        785.0f, 805.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterPaths, HazardRect::Type::Water,
        178.0f, 60.0f,
        1548.0f, 769.0f,
        1784.0f, 771.0f,
        1601.0f, 805.0f,
        1721.0f, 805.0f,
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
