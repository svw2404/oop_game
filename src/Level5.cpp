#include "App.hpp"

void App::BuildLevel5() {
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

    auto addSupportRect = [this](float left, float top, float right, float bottom) {
        SolidRect rect = ImageRectToWorldRect(left, top, right, bottom);
        rect.blockBottom = false;
        m_SolidBlocks.push_back(rect);
    };

    // Main shell rebuilt from the user image-map outline. The polygon traces
    // the navigable void; everything outside it is stone mass.
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 1453.0f, 48.0f));
    m_LeftWallSpawnRect = ImageRectToWorldRect(0.0f, 0.0f, 44.0f, 1082.0f);
    m_SolidBlocks.push_back(m_LeftWallSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(1403.0f, 0.0f, 1453.0f, 1082.0f));

    // Left tower with top water basin.
    m_LeftFloorSpawnRect = ImageRectToWorldRect(217.0f, 149.0f, 264.0f, 189.0f);
    m_SolidBlocks.push_back(m_LeftFloorSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(217.0f, 143.0f, 560.0f, 191.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(403.0f, 191.0f, 450.0f, 1040.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(261.0f, 481.0f, 407.0f, 523.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(264.0f, 632.0f, 405.0f, 675.0f));
    addSupportRect(260.0f, 190.0f, 320.0f, 244.0f);
    addSupportRect(320.0f, 244.0f, 366.0f, 294.0f);
    addSupportRect(366.0f, 294.0f, 409.0f, 345.0f);

    m_SolidBlocks.push_back(ImageRectToWorldRect(456.0f, 369.0f, 560.0f, 417.0f));
    addSupportRect(560.0f, 392.0f, 594.0f, 417.0f);
    m_SolidBlocks.push_back(ImageRectToWorldRect(452.0f, 590.0f, 559.0f, 636.0f));
    addSupportRect(559.0f, 612.0f, 596.0f, 636.0f);
    m_SolidBlocks.push_back(ImageRectToWorldRect(450.0f, 814.0f, 559.0f, 858.0f));
    addSupportRect(559.0f, 836.0f, 598.0f, 858.0f);

    // Center tower and bottom center floor strips.
    m_SolidBlocks.push_back(ImageRectToWorldRect(591.0f, 258.0f, 854.0f, 305.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(703.0f, 302.0f, 750.0f, 1041.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(44.0f, 1023.0f, 405.0f, 1082.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(448.0f, 1040.0f, 702.0f, 1082.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(750.0f, 1037.0f, 997.0f, 1082.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1067.0f, 1025.0f, 1406.0f, 1082.0f));

    // Right tower with top lava basin.
    m_SolidBlocks.push_back(ImageRectToWorldRect(886.0f, 148.0f, 1222.0f, 194.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(997.0f, 191.0f, 1043.0f, 1037.0f));
    addSupportRect(1043.0f, 189.0f, 1089.0f, 240.0f);
    addSupportRect(1089.0f, 240.0f, 1136.0f, 289.0f);
    addSupportRect(1136.0f, 289.0f, 1186.0f, 337.0f);

    m_SolidBlocks.push_back(ImageRectToWorldRect(851.0f, 372.0f, 998.0f, 417.0f));
    addSupportRect(851.0f, 391.0f, 884.0f, 417.0f);
    m_SolidBlocks.push_back(ImageRectToWorldRect(847.0f, 591.0f, 998.0f, 637.0f));
    addSupportRect(847.0f, 614.0f, 902.0f, 637.0f);
    m_SolidBlocks.push_back(ImageRectToWorldRect(848.0f, 814.0f, 1000.0f, 860.0f));
    addSupportRect(848.0f, 836.0f, 893.0f, 860.0f);
    m_SolidBlocks.push_back(ImageRectToWorldRect(1042.0f, 480.0f, 1188.0f, 523.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1037.0f, 632.0f, 1184.0f, 673.0f));

    m_Slopes = {
        {ImagePointToWorldPoint(260.0f, 190.0f), ImagePointToWorldPoint(409.0f, 345.0f)},
        {ImagePointToWorldPoint(450.0f, 226.0f), ImagePointToWorldPoint(493.0f, 191.0f)},
        {ImagePointToWorldPoint(560.0f, 372.0f), ImagePointToWorldPoint(594.0f, 410.0f)},
        {ImagePointToWorldPoint(559.0f, 590.0f), ImagePointToWorldPoint(596.0f, 634.0f)},
        {ImagePointToWorldPoint(559.0f, 816.0f), ImagePointToWorldPoint(598.0f, 858.0f)},
        {ImagePointToWorldPoint(851.0f, 412.0f), ImagePointToWorldPoint(884.0f, 372.0f)},
        {ImagePointToWorldPoint(847.0f, 637.0f), ImagePointToWorldPoint(902.0f, 591.0f)},
        {ImagePointToWorldPoint(848.0f, 859.0f), ImagePointToWorldPoint(893.0f, 814.0f)},
        {ImagePointToWorldPoint(1043.0f, 337.0f), ImagePointToWorldPoint(1186.0f, 189.0f)},
        {ImagePointToWorldPoint(44.0f, 1004.0f), ImagePointToWorldPoint(73.0f, 1023.0f)},
        {ImagePointToWorldPoint(371.0f, 1023.0f), ImagePointToWorldPoint(405.0f, 1001.0f)},
        {ImagePointToWorldPoint(1037.0f, 1005.0f), ImagePointToWorldPoint(1067.0f, 1025.0f)},
        {ImagePointToWorldPoint(1370.0f, 1026.0f), ImagePointToWorldPoint(1406.0f, 1004.0f)},
    };

    // The two long outer diagonals on each tower need ceiling slopes so players
    // cannot jump up through them from below (same issue that was fixed in Level 3).
    // Only these two are long enough to matter; the short platform-edge stubs are
    // already covered by the existing high-end ceiling logic.
    m_CeilingSlopes = {
        {ImagePointToWorldPoint(260.0f, 190.0f), ImagePointToWorldPoint(409.0f, 345.0f)},
        {ImagePointToWorldPoint(1043.0f, 337.0f), ImagePointToWorldPoint(1186.0f, 189.0f)},
    };

    AddCurrentSlopeGuardBands();

    // Visible liquids in the source art.
    AddAnimatedHazardInImageTrap(
        waterPaths, HazardRect::Type::Water,
        178.0f, 60.0f,
        289.0f, 164.0f,
        478.0f, 165.0f,
        323.0f, 186.0f,
        444.0f, 186.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        lavaPaths, HazardRect::Type::Lava,
        177.0f, 61.0f,
        934.0f, 147.0f,
        1148.0f, 167.0f,
        972.0f, 189.0f,
        1110.0f, 189.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        lavaPaths, HazardRect::Type::Lava,
        177.0f, 61.0f,
        77.0f, 1005.0f,
        370.0f, 1023.0f,
        114.0f, 1025.0f,
        333.0f, 1025.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        waterPaths, HazardRect::Type::Water,
        178.0f, 60.0f,
        1076.0f, 1025.0f,
        1369.0f, 1026.0f,
        1114.0f, 1042.0f,
        1331.0f, 1042.0f,
        10.2f
    );

    // Level 5 puzzle: cross-convergence.
    // Fireboy starts top-right (lava), Watergirl starts top-left (water).
    // Each must descend and cross to the opposite side of the center pillar
    // to reach their door. Both doors sit at the base of the center pillar
    // (x=703-743) so both characters must meet at the same bottom zone.
    //
    // Door scale mirrors Level 4 — background is 1453x1082 vs 2380x1760 standard,
    // so props need the same size correction to appear consistent.
    const float level5ToStandardScale = 1453.0f / 2380.0f;
    const float propScale = 1.0f / 1.5f;
    const float doorScale  = propScale * 1.6f * level5ToStandardScale;
    const float diamondScale = propScale * 0.90f;

    auto addDoorFrame = [&](float cx, float bottomY) {
        AddPropAtBottom(
            std::string(GA_RESOURCE_DIR) + "/Image/Character/door frame.png",
            cx, bottomY, 138.0f, 147.0f, 7.1f, doorScale
        );
    };
    // Center-left floor top = image y=1040, center-right = y=1042.
    addDoorFrame(610.0f, 1040.0f);
    addDoorFrame(870.0f, 1042.0f);

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
        610.0f, 1040.0f, 160.0f, 205.0f, 7.2f, doorScale
    );
    const glm::vec2 fireboyDoorSize = ImageSizeToWorldSize(160.0f, 205.0f, doorScale);
    m_FireboyDoor.triggerRect.center = m_FireboyDoor.sprite->GetPosition()
        + glm::vec2{0.0f, -fireboyDoorSize.y * 0.14f};
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
        870.0f, 1042.0f, 162.0f, 205.0f, 7.2f, doorScale
    );
    const glm::vec2 watergirlDoorSize = ImageSizeToWorldSize(162.0f, 205.0f, doorScale);
    m_WatergirlDoor.triggerRect.center = m_WatergirlDoor.sprite->GetPosition()
        + glm::vec2{watergirlDoorSize.x * 0.02f, -watergirlDoorSize.y * 0.10f};
    m_WatergirlDoor.triggerRect.size = {
        watergirlDoorSize.x * 0.60f,
        watergirlDoorSize.y * 0.82f,
    };

    auto addRedDiamond = [&](float cx, float cy) {
        AddCollectibleDiamond(
            std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
            DiamondType::Fire, cx, cy, 121.0f, 111.0f, 8.4f, diamondScale
        );
    };
    auto addBlueDiamond = [&](float cx, float cy) {
        AddCollectibleDiamond(
            std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
            DiamondType::Water, cx, cy, 117.0f, 111.0f, 8.4f, diamondScale
        );
    };

    // Red diamonds on left tower platforms + top-right lava basin.
    // Fireboy must cross to the left side to collect these.
    addRedDiamond(501.0f, 350.0f);
    addRedDiamond(504.0f, 560.0f);
    addRedDiamond(506.0f, 780.0f);
    addRedDiamond(1060.0f, 160.0f);

    // Blue diamonds on right tower platforms + top-left water basin.
    // Watergirl must cross to the right side to collect these.
    addBlueDiamond(944.0f, 350.0f);
    addBlueDiamond(945.0f, 560.0f);
    addBlueDiamond(945.0f, 778.0f);
    addBlueDiamond(380.0f, 160.0f);
}
