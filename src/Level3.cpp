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

    constexpr float centerLeftShoulderLeft = 982.0f;
    constexpr float centerLeftShoulderRight = 1174.0f;
    constexpr float centerLeftShoulderTop = 653.0f;
    constexpr float centerLeftShoulderBottom = 769.0f;

    constexpr float centerTopCapLeft = 1165.0f;
    constexpr float centerTopCapRight = 1227.0f;
    constexpr float centerTopCapTop = 470.0f;
    constexpr float centerTopCapBottom = 653.0f;

    constexpr float centerRightShoulderLeft = 1227.0f;
    constexpr float centerRightShoulderRight = 1412.0f;
    constexpr float centerRightShoulderTop = 644.0f;
    constexpr float centerRightShoulderBottom = 771.0f;

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
    // Note: the prompt text conflicts with background2 here. The image itself
    // clearly shows left WATER and right LAVA, so we follow the background art.
    // ---------------------------------------------------------------------
    m_LeftFloorSpawnRect = ImageRectToWorldRect(46.0f, 1702.0f, 366.0f, 1760.0f);
    m_SolidBlocks.push_back(m_LeftFloorSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(418.0f, 1737.0f, 796.0f, 1760.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1040.0f, 1519.0f, 1353.0f, 1760.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1599.0f, 1731.0f, 1968.0f, 1760.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(2014.0f, 1702.0f, 2340.0f, 1760.0f));

    // ---------------------------------------------------------------------
    // Block 2: right middle basin shelf.
    // The prompt says lava, but the inspected background art shows WATER.
    // ---------------------------------------------------------------------
    m_SolidBlocks.push_back(ImageRectToWorldRect(1418.0f, 1328.0f, 1603.0f, 1389.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1603.0f, 1363.0f, 1979.0f, 1389.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1979.0f, 1328.0f, 2216.0f, 1389.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(2216.0f, 1208.0f, 2346.0f, 1389.0f));

    // ---------------------------------------------------------------------
    // Block 3: left middle basin shelf.
    // The image shows this one as LAVA.
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

    // Center floating block support mass. The tops of these supports stay under
    // the traced outer ramps, which avoids invisible walls while still keeping
    // the block sealed from side/under overlaps.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerTopCapLeft, centerTopCapTop, centerTopCapRight, centerTopCapBottom
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerLeftShoulderLeft, centerLeftShoulderTop, centerLeftShoulderRight, centerLeftShoulderBottom
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerRightShoulderLeft, centerRightShoulderTop, centerRightShoulderRight, centerRightShoulderBottom
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerStemLeft, centerStemTop, centerStemRight, centerStemBottom
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerLowerLeftSupportLeft,
        centerLowerLeftSupportTop,
        centerLowerLeftSupportRight,
        centerLowerLeftSupportBottom
    ));
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        centerLowerRightSupportLeft,
        centerLowerRightSupportTop,
        centerLowerRightSupportRight,
        centerLowerRightSupportBottom
    ));

    // Long connected base below the floating center.
    m_SolidBlocks.push_back(ImageRectToWorldRect(
        baseBridgeTopLeftX, baseBridgeTopY, baseBridgeTopRightX, baseBridgeBottomY
    ));

    m_Slopes = {
        // Block 1
        {ImagePointToWorldPoint(366.0f, 1702.0f), ImagePointToWorldPoint(418.0f, 1737.0f)},
        {ImagePointToWorldPoint(796.0f, 1742.0f), ImagePointToWorldPoint(853.0f, 1704.0f)},
        {ImagePointToWorldPoint(853.0f, 1704.0f), ImagePointToWorldPoint(1040.0f, 1519.0f)},
        {ImagePointToWorldPoint(1353.0f, 1523.0f), ImagePointToWorldPoint(1536.0f, 1704.0f)},
        {ImagePointToWorldPoint(1536.0f, 1704.0f), ImagePointToWorldPoint(1599.0f, 1735.0f)},
        {ImagePointToWorldPoint(1968.0f, 1731.0f), ImagePointToWorldPoint(2014.0f, 1708.0f)},

        // Block 2
        {ImagePointToWorldPoint(1544.0f, 1338.0f), ImagePointToWorldPoint(1603.0f, 1368.0f)},
        {ImagePointToWorldPoint(1979.0f, 1363.0f), ImagePointToWorldPoint(2029.0f, 1328.0f)},

        // Block 3
        {ImagePointToWorldPoint(372.0f, 1332.0f), ImagePointToWorldPoint(416.0f, 1368.0f)},
        {ImagePointToWorldPoint(796.0f, 1366.0f), ImagePointToWorldPoint(855.0f, 1334.0f)},

        // Block 6 upper ledges and peak
        {ImagePointToWorldPoint(609.0f, 778.0f), ImagePointToWorldPoint(669.0f, 808.0f)},
        {ImagePointToWorldPoint(789.0f, 810.0f), ImagePointToWorldPoint(857.0f, 776.0f)},
        {ImagePointToWorldPoint(857.0f, 776.0f), ImagePointToWorldPoint(982.0f, 653.0f)},
        {ImagePointToWorldPoint(982.0f, 653.0f), ImagePointToWorldPoint(1165.0f, 470.0f)},
        {ImagePointToWorldPoint(1227.0f, 470.0f), ImagePointToWorldPoint(1412.0f, 644.0f)},
        {ImagePointToWorldPoint(1412.0f, 644.0f), ImagePointToWorldPoint(1296.0f, 766.0f)},
        {ImagePointToWorldPoint(1296.0f, 766.0f), ImagePointToWorldPoint(1227.0f, 771.0f)},
        {ImagePointToWorldPoint(1109.0f, 769.0f), ImagePointToWorldPoint(982.0f, 653.0f)},
        {ImagePointToWorldPoint(1542.0f, 776.0f), ImagePointToWorldPoint(1597.0f, 810.0f)},
        {ImagePointToWorldPoint(1725.0f, 810.0f), ImagePointToWorldPoint(1790.0f, 778.0f)},

        // Block 6 lower supports
        {ImagePointToWorldPoint(baseBridgeBottomLeftX, baseBridgeBottomY), ImagePointToWorldPoint(baseBridgeTopLeftX, baseBridgeTopY)},
        {ImagePointToWorldPoint(1037.0f, 1086.0f), ImagePointToWorldPoint(1171.0f, 961.0f)},
        {ImagePointToWorldPoint(1227.0f, 958.0f), ImagePointToWorldPoint(1352.0f, 1081.0f)},
        {ImagePointToWorldPoint(baseBridgeTopRightX, 1086.0f), ImagePointToWorldPoint(baseBridgeBottomRightX, 1134.0f)},
    };

    AddCurrentSlopeGuardBands();

    // Liquids. Again, the actual background art wins where the prompt notes
    // and the traced text disagree.
    AddAnimatedHazardInImageTrap(
        waterLargePaths, HazardRect::Type::Water,
        360.0f, 80.0f,
        366.0f, 1702.0f,
        853.0f, 1704.0f,
        418.0f, 1737.0f,
        796.0f, 1742.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        lavaLargePaths, HazardRect::Type::Lava,
        360.0f, 80.0f,
        1536.0f, 1704.0f,
        2014.0f, 1708.0f,
        1599.0f, 1735.0f,
        1968.0f, 1731.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterLargePaths, HazardRect::Type::Water,
        360.0f, 80.0f,
        1544.0f, 1338.0f,
        2029.0f, 1328.0f,
        1603.0f, 1368.0f,
        1979.0f, 1363.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        lavaLargePaths, HazardRect::Type::Lava,
        360.0f, 80.0f,
        372.0f, 1332.0f,
        855.0f, 1334.0f,
        416.0f, 1368.0f,
        796.0f, 1366.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        lavaPaths, HazardRect::Type::Lava,
        177.0f, 61.0f,
        609.0f, 778.0f,
        857.0f, 776.0f,
        669.0f, 808.0f,
        789.0f, 810.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterPaths, HazardRect::Type::Water,
        178.0f, 60.0f,
        1542.0f, 776.0f,
        1790.0f, 778.0f,
        1597.0f, 810.0f,
        1725.0f, 810.0f,
        10.2f
    );

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
