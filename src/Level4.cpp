#include "App.hpp"

void App::BuildLevel4() {
    const std::vector<std::string> lavaPaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_04.png",
    };
    const std::vector<std::string> venomPaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_04.png",
    };

    const float level4ToStandardScale = 1455.0f / 2380.0f;
    const float propScale = 1.0f / 1.5f;
    const float doorScale = propScale * 1.6f * level4ToStandardScale;
    const float diamondScale = propScale * 0.90f;

    // Base shell from the user-mapped outer level rectangle.
    m_SolidBlocks.push_back(ImageRectToWorldRect(26.0f, 27.0f, 1419.0f, 29.0f));
    m_LeftWallSpawnRect = ImageRectToWorldRect(0.0f, 0.0f, 28.0f, 1081.0f);
    m_SolidBlocks.push_back(m_LeftWallSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(1419.0f, 0.0f, 1455.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(26.0f, 1048.0f, 1419.0f, 1053.0f));

    // Main connected mass rebuilt from the latest image-map outline. The poly
    // describes the big navigable void; everything outside that contour is the
    // stone shell we need to represent here.
    m_LeftFloorSpawnRect = ImageRectToWorldRect(26.0f, 934.0f, 253.0f, 1049.0f);
    m_SolidBlocks.push_back(m_LeftFloorSpawnRect);

    // Left outer tower and upper lava shelf.
    m_SolidBlocks.push_back(ImageRectToWorldRect(26.0f, 179.0f, 143.0f, 1049.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(139.0f, 396.0f, 179.0f, 934.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(170.0f, 370.0f, 257.0f, 407.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(286.0f, 389.0f, 399.0f, 407.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(439.0f, 369.0f, 551.0f, 407.0f));

    // Stepped center ascent leading to the top shelf.
    m_SolidBlocks.push_back(ImageRectToWorldRect(476.0f, 406.0f, 512.0f, 708.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(512.0f, 330.0f, 589.0f, 708.0f, false));
    m_SolidBlocks.push_back(ImageRectToWorldRect(589.0f, 673.0f, 634.0f, 708.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(551.0f, 296.0f, 667.0f, 369.0f, false));
    m_SolidBlocks.push_back(ImageRectToWorldRect(589.0f, 253.0f, 701.0f, 330.0f, false));
    m_SolidBlocks.push_back(ImageRectToWorldRect(667.0f, 217.0f, 850.0f, 253.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(701.0f, 253.0f, 850.0f, 325.0f, false));

    // Central pillar island and its cap.
    m_SolidBlocks.push_back(ImageRectToWorldRect(591.0f, 258.0f, 852.0f, 305.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(703.0f, 302.0f, 750.0f, 1041.0f));

    // Right-side divider column and the small shelves hanging off it.
    m_SolidBlocks.push_back(ImageRectToWorldRect(997.0f, 191.0f, 1043.0f, 1037.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(884.0f, 372.0f, 998.0f, 417.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(902.0f, 591.0f, 998.0f, 637.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(893.0f, 814.0f, 1000.0f, 860.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1040.0f, 480.0f, 1188.0f, 523.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1037.0f, 632.0f, 1184.0f, 673.0f));

    // Upper-right U-shaped tower.
    m_SolidBlocks.push_back(ImageRectToWorldRect(1113.0f, 217.0f, 1153.0f, 556.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1148.0f, 179.0f, 1416.0f, 217.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1152.0f, 519.0f, 1297.0f, 556.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1306.0f, 633.0f, 1415.0f, 669.0f, false));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1339.0f, 179.0f, 1415.0f, 941.0f));

    // Bottom venom-basin frame and right return path.
    m_SolidBlocks.push_back(ImageRectToWorldRect(176.0f, 860.0f, 399.0f, 934.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(399.0f, 866.0f, 440.0f, 1034.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(440.0f, 1034.0f, 1002.0f, 1051.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1038.0f, 973.0f, 1262.0f, 1017.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1298.0f, 940.0f, 1339.0f, 975.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1339.0f, 859.0f, 1415.0f, 941.0f));

    // Floating/helper platforms mapped directly from the new image-map notes.
    // Miniblock 4 is trapezoidal, so we split it into a flat shelf plus a
    // short descending tip that the slope system can represent smoothly.
    m_SolidBlocks.push_back(ImageRectToWorldRect(107.0f, 141.0f, 248.0f, 179.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(293.0f, 180.0f, 438.0f, 217.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(438.0f, 180.0f, 446.0f, 217.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(446.0f, 189.0f, 454.0f, 217.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(454.0f, 198.0f, 462.0f, 217.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(462.0f, 207.0f, 470.0f, 217.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(744.0f, 560.0f, 849.0f, 595.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(515.0f, 939.0f, 623.0f, 973.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(780.0f, 902.0f, 926.0f, 935.0f));

    m_Slopes = {
        {ImagePointToWorldPoint(139.0f, 396.0f), ImagePointToWorldPoint(170.0f, 370.0f)},
        {ImagePointToWorldPoint(257.0f, 370.0f), ImagePointToWorldPoint(286.0f, 389.0f)},
        {ImagePointToWorldPoint(399.0f, 389.0f), ImagePointToWorldPoint(439.0f, 370.0f)},
        {ImagePointToWorldPoint(634.0f, 706.0f), ImagePointToWorldPoint(659.0f, 674.0f)},
        {ImagePointToWorldPoint(851.0f, 412.0f), ImagePointToWorldPoint(884.0f, 372.0f)},
        {ImagePointToWorldPoint(847.0f, 637.0f), ImagePointToWorldPoint(902.0f, 591.0f)},
        {ImagePointToWorldPoint(848.0f, 859.0f), ImagePointToWorldPoint(893.0f, 814.0f)},
        {ImagePointToWorldPoint(1115.0f, 217.0f), ImagePointToWorldPoint(1148.0f, 184.0f)},
        {ImagePointToWorldPoint(1297.0f, 523.0f), ImagePointToWorldPoint(1266.0f, 556.0f)},
        {ImagePointToWorldPoint(1306.0f, 669.0f), ImagePointToWorldPoint(1339.0f, 636.0f)},
        {ImagePointToWorldPoint(1002.0f, 1034.0f), ImagePointToWorldPoint(1038.0f, 1012.0f)},
        {ImagePointToWorldPoint(1262.0f, 975.0f), ImagePointToWorldPoint(1298.0f, 940.0f)},
        {ImagePointToWorldPoint(438.0f, 180.0f), ImagePointToWorldPoint(470.0f, 216.0f)},
    };

    AddCurrentSlopeGuardBands();

    // Liquids visible in the art:
    // - upper-left orange basin = lava
    // - large bottom green basin = venom
    AddAnimatedHazardInImageTrap(
        lavaPaths, HazardRect::Type::Lava,
        177.0f, 61.0f,
        257.0f, 374.0f,
        435.0f, 371.0f,
        293.0f, 393.0f,
        403.0f, 393.0f,
        10.2f
    );
    AddAnimatedHazardInImageTrap(
        venomPaths, HazardRect::Type::Venom,
        178.0f, 60.0f,
        404.0f, 1017.0f,
        1041.0f, 1016.0f,
        441.0f, 1038.0f,
        1004.0f, 1035.0f,
        10.2f
    );

    auto addDoorFrame = [&](float centerXImage, float bottomYImage) {
        AddPropAtBottom(
            std::string(GA_RESOURCE_DIR) + "/Image/Character/door frame.png",
            centerXImage, bottomYImage, 138.0f, 147.0f, 7.1f, doorScale
        );
    };

    const float doorBottomYImage = 865.0f;

    addDoorFrame(225.0f, doorBottomYImage);
    addDoorFrame(330.0f, doorBottomYImage);

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
        225.0f, doorBottomYImage, 160.0f, 205.0f, 7.2f, doorScale
    );
    const glm::vec2 fireboyDoorSize = ImageSizeToWorldSize(160.0f, 205.0f, doorScale);
    m_FireboyDoor.triggerRect.center = m_FireboyDoor.sprite->GetPosition();
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
        330.0f, doorBottomYImage, 162.0f, 205.0f, 7.2f, doorScale
    );
    const glm::vec2 watergirlDoorSize = ImageSizeToWorldSize(162.0f, 205.0f, doorScale);
    m_WatergirlDoor.triggerRect.center = m_WatergirlDoor.sprite->GetPosition();
    m_WatergirlDoor.triggerRect.size = {
        watergirlDoorSize.x * 0.60f,
        watergirlDoorSize.y * 0.82f,
    };

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

    // Keep the current collectible layout for now; this pass is about getting
    // the terrain shell and the liquid basins much closer to the mapped image.
    addRedDiamond(91.0f, 92.0f);
    addRedDiamond(90.0f, 510.0f);
    addRedDiamond(89.0f, 696.0f);
    addRedDiamond(594.0f, 561.0f);
    addRedDiamond(578.0f, 884.0f);
    addRedDiamond(810.0f, 849.0f);

    addBlueDiamond(1065.0f, 343.0f);
    addBlueDiamond(1064.0f, 453.0f);
    addBlueDiamond(1362.0f, 584.0f);
    addBlueDiamond(689.0f, 775.0f);
    addBlueDiamond(907.0f, 842.0f);
    addBlueDiamond(1176.0f, 920.0f);
}
