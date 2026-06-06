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

    // Outer contour and the small ledge attached to the upper-left wall.
    m_SolidBlocks.push_back(ImageRectToWorldRect(30.0f, 0.0f, 1416.0f, 29.0f));
    m_LeftWallSpawnRect = ImageRectToWorldRect(0.0f, 0.0f, 28.0f, 1081.0f);
    m_SolidBlocks.push_back(m_LeftWallSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(1416.0f, 0.0f, 1455.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(27.0f, 142.0f, 65.0f, 178.0f));

    m_LeftFloorSpawnRect = ImageRectToWorldRect(22.0f, 1051.0f, 256.0f, 1081.0f);
    m_SolidBlocks.push_back(m_LeftFloorSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(256.0f, 1051.0f, 1416.0f, 1081.0f));

    // Left wall, lava shelf, and stepped upper-left mass.
    m_SolidBlocks.push_back(ImageRectToWorldRect(141.0f, 397.0f, 177.0f, 934.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(172.0f, 371.0f, 258.0f, 408.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(258.0f, 392.0f, 290.0f, 408.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(290.0f, 388.0f, 406.0f, 408.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(407.0f, 397.0f, 438.0f, 408.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(439.0f, 371.0f, 551.0f, 408.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(551.0f, 293.0f, 557.0f, 408.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(557.0f, 293.0f, 589.0f, 404.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(589.0f, 293.0f, 665.0f, 331.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(665.0f, 222.0f, 672.0f, 331.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(672.0f, 218.0f, 849.0f, 255.0f));

    // Central hanging pillar and its lower wedge.
    m_SolidBlocks.push_back(ImageRectToWorldRect(476.0f, 408.0f, 515.0f, 707.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(512.0f, 671.0f, 631.0f, 707.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(631.0f, 674.0f, 660.0f, 683.0f, false));

    // Left side of the venom basin and the stone around the lower spawn room.
    m_SolidBlocks.push_back(ImageRectToWorldRect(177.0f, 860.0f, 402.0f, 934.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(254.0f, 934.0f, 402.0f, 1010.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(402.0f, 1043.0f, 442.0f, 1051.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(442.0f, 1034.0f, 1002.0f, 1081.0f));

    // Right return path and the two ledges attached to the outer wall.
    m_SolidBlocks.push_back(ImageRectToWorldRect(1002.0f, 1034.0f, 1041.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1041.0f, 976.0f, 1264.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1264.0f, 976.0f, 1298.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1298.0f, 934.0f, 1339.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1339.0f, 864.0f, 1345.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1345.0f, 864.0f, 1416.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1339.0f, 634.0f, 1416.0f, 670.0f));

    // Upper-right U-shaped tower. The angled lower tip is a ceiling slope.
    m_SolidBlocks.push_back(ImageRectToWorldRect(1115.0f, 210.0f, 1149.0f, 557.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1149.0f, 178.0f, 1416.0f, 218.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1149.0f, 521.0f, 1264.0f, 557.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1264.0f, 521.0f, 1296.0f, 530.0f, false));

    // Internal divider and the three shelves visibly attached to it.
    m_SolidBlocks.push_back(ImageRectToWorldRect(966.0f, 61.0f, 1000.0f, 749.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1000.0f, 105.0f, 1038.0f, 142.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(890.0f, 446.0f, 966.0f, 483.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1000.0f, 749.0f, 1300.0f, 783.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1112.0f, 744.0f, 1149.0f, 749.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1121.0f, 739.0f, 1130.0f, 744.0f, false));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1130.0f, 731.0f, 1139.0f, 744.0f, false));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1139.0f, 724.0f, 1147.0f, 744.0f, false));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1147.0f, 717.0f, 1149.0f, 744.0f, false));

    // Floating platforms inside the playable contour.
    m_SolidBlocks.push_back(ImageRectToWorldRect(109.0f, 146.0f, 248.0f, 179.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(295.0f, 181.0f, 446.0f, 215.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(446.0f, 193.0f, 454.0f, 215.0f, false));
    m_SolidBlocks.push_back(ImageRectToWorldRect(454.0f, 203.0f, 462.0f, 215.0f, false));
    m_SolidBlocks.push_back(ImageRectToWorldRect(462.0f, 213.0f, 471.0f, 215.0f, false));
    m_SolidBlocks.push_back(ImageRectToWorldRect(745.0f, 559.0f, 850.0f, 595.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(518.0f, 937.0f, 624.0f, 973.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(781.0f, 900.0f, 927.0f, 937.0f));

    m_Slopes = {
        {ImagePointToWorldPoint(141.0f, 397.0f), ImagePointToWorldPoint(172.0f, 372.0f)},
        {ImagePointToWorldPoint(258.0f, 371.0f), ImagePointToWorldPoint(290.0f, 392.0f)},
        {ImagePointToWorldPoint(406.0f, 388.0f), ImagePointToWorldPoint(439.0f, 371.0f)},
        {ImagePointToWorldPoint(402.0f, 1010.0f), ImagePointToWorldPoint(442.0f, 1034.0f)},
        {ImagePointToWorldPoint(1002.0f, 1033.0f), ImagePointToWorldPoint(1041.0f, 1014.0f)},
        {ImagePointToWorldPoint(1264.0f, 976.0f), ImagePointToWorldPoint(1298.0f, 937.0f)},
        {ImagePointToWorldPoint(1309.0f, 670.0f), ImagePointToWorldPoint(1339.0f, 635.0f)},
        {ImagePointToWorldPoint(1116.0f, 210.0f), ImagePointToWorldPoint(1144.0f, 181.0f)},
        {ImagePointToWorldPoint(446.0f, 184.0f), ImagePointToWorldPoint(471.0f, 215.0f)},
        {ImagePointToWorldPoint(1112.0f, 746.0f), ImagePointToWorldPoint(1147.0f, 717.0f)},
    };

    m_CeilingSlopes = {
        {ImagePointToWorldPoint(660.0f, 675.0f), ImagePointToWorldPoint(631.0f, 707.0f)},
        {ImagePointToWorldPoint(1296.0f, 521.0f), ImagePointToWorldPoint(1264.0f, 555.0f)},
        {ImagePointToWorldPoint(1416.0f, 670.0f), ImagePointToWorldPoint(1309.0f, 670.0f)},
        {ImagePointToWorldPoint(966.0f, 749.0f), ImagePointToWorldPoint(1000.0f, 783.0f)},
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
