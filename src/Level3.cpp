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
    constexpr float topLeftPlatformRight = 1104.0f;
    constexpr float topLeftPlatformTop = 218.0f;
    constexpr float topLeftPlatformBottom = 280.0f;

    constexpr float topRightPlatformLeft = 1296.0f;
    constexpr float topRightPlatformRight = 2088.0f;
    constexpr float topRightPlatformTop = 218.0f;
    constexpr float topRightPlatformBottom = 278.0f;

    constexpr float leftPillarLeft = 549.0f;
    constexpr float leftPillarRight = 609.0f;
    constexpr float leftPillarTop = 276.0f;
    constexpr float leftPillarBottom = 1086.0f;

    constexpr float rightPillarLeft = 1790.0f;
    constexpr float rightPillarRight = 1848.0f;
    constexpr float rightPillarTop = 273.0f;
    constexpr float rightPillarBottom = 1086.0f;

    constexpr float leftLedgeOuterLeft = 607.0f;
    constexpr float leftLedgeLiquidLeft = 664.0f;
    constexpr float leftLedgeLiquidRight = 792.0f;
    constexpr float leftLedgeOuterRight = 857.0f;
    constexpr float leftLedgeTopLeftY = 780.0f;
    constexpr float leftLedgeTopRightY = 776.0f;
    constexpr float leftLedgeLiquidBottomLeftY = 813.0f;
    constexpr float leftLedgeLiquidBottomRightY = 810.0f;
    constexpr float leftLedgeBottomY = 831.0f;

    constexpr float rightLedgeOuterLeft = 1542.0f;
    constexpr float rightLedgeLiquidLeft = 1604.0f;
    constexpr float rightLedgeLiquidRight = 1729.0f;
    constexpr float rightLedgeOuterRight = 1787.0f;
    constexpr float rightLedgeTopLeftY = 778.0f;
    constexpr float rightLedgeTopRightY = 776.0f;
    constexpr float rightLedgeLiquidBottomLeftY = 808.0f;
    constexpr float rightLedgeLiquidBottomRightY = 808.0f;
    constexpr float rightLedgeBottomY = 831.0f;

    // Fill the shoulder mass under the peak, but keep that fill lower than the
    // visible ramp shell so it seals the object without flattening the slope.
    constexpr float centerLeftCavitySealX = 1103.0f;
    constexpr float centerLeftCavitySealY = 769.0f;
    constexpr float centerRightShoulderOuterX = 1288.0f;
    constexpr float centerRightShoulderOuterY = 773.0f;

    // Left shoulder geometry (imgX=982–1109, imgY=653–769) and right shoulder
    // (imgX=1227–1412, imgY=644–771) are intentionally NOT added as solid blocks.
    // Their slope guard bands provide all needed ground for those surfaces.
    // A competing flat block top caused ApplySlopeFollow to fire a one-frame
    // ~24 wu Y-pop whenever a character walked from the flat block onto the rising
    // peak slope — the same X range, but two different Y values in the same frame.

    constexpr float centerPeakLeftX = 1162.0f;
    constexpr float centerPeakRightX = 1236.0f;
    constexpr float centerPeakY = 471.0f;
    constexpr float centerPeakBottomY = 690.0f;

    constexpr float centerOuterLeftX = 985.0f;
    constexpr float centerOuterLeftY = 656.0f;
    constexpr float centerOuterRightX = 1403.0f;
    constexpr float centerOuterRightY = 649.0f;

    constexpr float centerStemLeft = 1170.0f;
    constexpr float centerStemRight = 1229.0f;
    constexpr float centerStemBottom = 952.0f;
    constexpr float centerStemNeckTop = 690.0f;

    constexpr float centerLeftUpperWingLeft = 1103.0f;
    constexpr float centerLeftUpperWingRight = 1172.0f;
    constexpr float centerLeftUpperWingTop = 769.0f;
    constexpr float centerLeftUpperWingBottom = 952.0f;

    constexpr float centerRightUpperWingLeft = 1233.0f;
    constexpr float centerRightUpperWingRight = 1288.0f;
    constexpr float centerRightUpperWingTop = 767.0f;
    constexpr float centerRightUpperWingBottom = 952.0f;

    constexpr float centerLowerLeftSupportLeft = 1040.0f;
    constexpr float centerLowerLeftSupportTop = 1084.0f;
    constexpr float centerLowerLeftFillRight = 1172.0f;
    constexpr float centerLowerLeftFillTop = 952.0f;

    constexpr float centerLowerRightSupportRight = 1349.0f;
    constexpr float centerLowerRightSupportTop = 1086.0f;
    constexpr float centerLowerRightFillLeft = 1229.0f;
    constexpr float centerLowerRightFillTop = 952.0f;

    constexpr float baseBottomStripLeft = 366.0f;
    constexpr float baseBottomStripRight = 2040.0f;
    constexpr float baseBottomStripTop = 1086.0f;
    constexpr float baseBottomStripBottom = 1139.0f;
    constexpr float leftBaseRaisedLeft = 366.0f;
    constexpr float leftBaseRaisedRight = 551.0f;
    constexpr float leftBaseRaisedTop = 1079.0f;
    constexpr float rightBaseRaisedLeft = 1357.0f;
    constexpr float rightBaseRaisedRight = 1792.0f;
    constexpr float rightBaseRaisedTop = 1081.0f;
    constexpr float rightOuterBaseRaisedLeft = 1848.0f;
    constexpr float rightOuterBaseRaisedRight = 2040.0f;
    constexpr float rightOuterBaseRaisedTop = 1084.0f;
    constexpr float baseBottomLeftX = 308.0f;
    constexpr float baseBottomRightX = 2091.0f;
    constexpr float baseBottomY = 1139.0f;

    // Keep one named floor Y for the center doors below.
    constexpr float baseBridgeTopY = baseBottomStripTop;

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

    // Center peak solid rebuilt from the full image-map contour.
    //
    // Shoulder fills are placed with their TOP at the walkable shoulder Y so
    // that a character descending slope S7 (which ends at y=773) or walking
    // the left shoulder (y≈776) lands directly on the fill surface.
    // Previously the fills had tops at y=690 — 33 world-units above the slope
    // endpoints — meaning characters couldn't step up onto them (StepUpHeight=14).
    //
    // Gap fills bridge from each slope's low endpoint down to the shoulder fill
    // top, sealing the interior seam without competing with the walkable shell.
    // Ceiling slopes in m_CeilingSlopes (added below) cover the outer ascending
    // surface and make the entire underside impenetrable from below.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerPeakLeftX, centerPeakY, centerPeakRightX, centerPeakBottomY
    ));
    // Narrow center neck plus inner wings only; no broad hidden shoulder fill.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerStemLeft, centerStemNeckTop, centerStemRight, centerStemBottom, true
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerLeftUpperWingLeft, centerLeftUpperWingTop,
        centerLeftUpperWingRight, centerLeftUpperWingBottom,
        true
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerRightUpperWingLeft, centerRightUpperWingTop,
        centerRightUpperWingRight, centerRightUpperWingBottom,
        true
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerLowerLeftSupportLeft, centerLowerLeftFillTop,
        centerLowerLeftFillRight, centerLowerLeftSupportTop,
        true
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerLowerRightFillLeft, centerLowerRightFillTop,
        centerLowerRightSupportRight, centerLowerRightSupportTop,
        true
    ));
    // Keep the bottom strip continuous underneath the connected shape, but only
    // raise the top where the VIA actually shows a ledge. That preserves the
    // open pillar gaps without inventing a giant flat bridge at y=1084.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        baseBottomStripLeft, baseBottomStripTop, baseBottomStripRight, baseBottomStripBottom
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        leftBaseRaisedLeft, leftBaseRaisedTop, leftBaseRaisedRight, baseBottomStripTop
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        rightBaseRaisedLeft, rightBaseRaisedTop, rightBaseRaisedRight, baseBottomStripTop
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        rightOuterBaseRaisedLeft, rightOuterBaseRaisedTop, rightOuterBaseRaisedRight, baseBottomStripTop
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
        {ImagePointToWorldPoint(leftLedgeOuterLeft, leftLedgeTopLeftY), ImagePointToWorldPoint(leftLedgeLiquidLeft, leftLedgeLiquidBottomLeftY)},
        {ImagePointToWorldPoint(leftLedgeLiquidRight, leftLedgeLiquidBottomRightY), ImagePointToWorldPoint(leftLedgeOuterRight, leftLedgeTopRightY)},
        {ImagePointToWorldPoint(leftLedgeOuterRight, leftLedgeTopRightY), ImagePointToWorldPoint(centerOuterLeftX, centerOuterLeftY)},
        {ImagePointToWorldPoint(centerOuterLeftX, centerOuterLeftY), ImagePointToWorldPoint(centerPeakLeftX, centerPeakY)},
        // Inner shoulder slope {(986,648)→(1107,776)} intentionally omitted:
        // creates a triple junction at (986,648) with the two ascending slopes above,
        // causing ApplySlopeFollow oscillation — same class of bug as the removed (982,653) junction.
        {ImagePointToWorldPoint(centerPeakRightX, centerPeakY), ImagePointToWorldPoint(centerOuterRightX, centerOuterRightY)},
        {ImagePointToWorldPoint(centerOuterRightX, centerOuterRightY), ImagePointToWorldPoint(centerRightShoulderOuterX, centerRightShoulderOuterY)},
        {ImagePointToWorldPoint(rightLedgeOuterLeft, rightLedgeTopLeftY), ImagePointToWorldPoint(rightLedgeLiquidLeft, rightLedgeLiquidBottomLeftY)},
        {ImagePointToWorldPoint(rightLedgeLiquidRight, rightLedgeLiquidBottomRightY), ImagePointToWorldPoint(rightLedgeOuterRight, rightLedgeTopRightY)},

        // Block 6 lower supports.
        {ImagePointToWorldPoint(baseBottomLeftX, baseBottomY), ImagePointToWorldPoint(leftBaseRaisedLeft, leftBaseRaisedTop)},
        {ImagePointToWorldPoint(centerLowerLeftSupportLeft, centerLowerLeftSupportTop), ImagePointToWorldPoint(centerStemLeft, centerStemBottom)},
        {ImagePointToWorldPoint(centerStemRight, centerStemBottom), ImagePointToWorldPoint(centerLowerRightSupportRight, centerLowerRightSupportTop)},
        {ImagePointToWorldPoint(rightOuterBaseRaisedLeft, rightOuterBaseRaisedTop), ImagePointToWorldPoint(baseBottomRightX, baseBottomY)},
    };

    // Ceiling slopes: the outer ascending shell of the central peak structure.
    // Adding these makes the full underside of each slope a real ceiling collision
    // surface — characters cannot jump up through the shell from any X position.
    // This mirrors the 4 floor slopes that form the walkable outer surface; the
    // same coordinates are safe as both floor and ceiling because ceiling collision
    // only fires when velocity.y > 0 AND the head is approaching from below, which
    // is never true when standing on the slope (head is ~50wu above the surface).
    m_CeilingSlopes = {
        // Left outer ascending: ledge right tip → left outer junction
        {ImagePointToWorldPoint(leftLedgeOuterRight, leftLedgeTopRightY),
         ImagePointToWorldPoint(centerOuterLeftX, centerOuterLeftY)},
        // Left inner ascending: left outer junction → peak left base
        {ImagePointToWorldPoint(centerOuterLeftX, centerOuterLeftY),
         ImagePointToWorldPoint(centerPeakLeftX, centerPeakY)},
        {ImagePointToWorldPoint(centerOuterLeftX, centerOuterLeftY),
         ImagePointToWorldPoint(centerLeftCavitySealX, centerLeftCavitySealY)},
        // Right inner descending: peak right base → right outer junction
        {ImagePointToWorldPoint(centerPeakRightX, centerPeakY),
         ImagePointToWorldPoint(centerOuterRightX, centerOuterRightY)},
        // Right outer descending: right outer junction → right shoulder
        {ImagePointToWorldPoint(centerOuterRightX, centerOuterRightY),
         ImagePointToWorldPoint(centerRightShoulderOuterX, centerRightShoulderOuterY)},
        {ImagePointToWorldPoint(centerLowerLeftSupportLeft, centerLowerLeftSupportTop),
         ImagePointToWorldPoint(centerLeftUpperWingRight, centerLeftUpperWingBottom)},
        {ImagePointToWorldPoint(centerLowerRightFillLeft, centerLowerRightFillTop),
         ImagePointToWorldPoint(centerLowerRightSupportRight, centerLowerRightSupportTop)},
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
        609.0f, 780.0f,
        857.0f, 776.0f,
        664.0f, 813.0f,
        792.0f, 810.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterPaths, HazardRect::Type::Water,
        178.0f, 60.0f,
        1547.0f, 778.0f,
        1787.0f, 776.0f,
        1604.0f, 808.0f,
        1729.0f, 808.0f,
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
