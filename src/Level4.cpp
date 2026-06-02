#include "App.hpp"

void App::BuildLevel4() {
    const float level4ToStandardScale = 1455.0f / 2380.0f;
    const float propScale = 1.0f / 1.5f;
    const float doorScale = propScale * 1.6f * level4ToStandardScale;
    const float diamondScale = propScale * 0.90f;

    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 1455.0f, 28.0f));
    m_LeftWallSpawnRect = ImageRectToWorldRect(0.0f, 0.0f, 28.0f, 1081.0f);
    m_SolidBlocks.push_back(m_LeftWallSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(1427.0f, 0.0f, 1455.0f, 1081.0f));

    m_LeftFloorSpawnRect = ImageRectToWorldRect(28.0f, 1040.0f, 250.0f, 1081.0f);
    m_SolidBlocks.push_back(m_LeftFloorSpawnRect);
    m_SolidBlocks.push_back(ImageRectToWorldRect(250.0f, 906.0f, 402.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(402.0f, 1041.0f, 1010.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1010.0f, 1000.0f, 1320.0f, 1081.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1320.0f, 860.0f, 1427.0f, 1081.0f));

    m_SolidBlocks.push_back(ImageRectToWorldRect(55.0f, 875.0f, 272.0f, 908.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(518.0f, 948.0f, 635.0f, 980.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(780.0f, 902.0f, 930.0f, 936.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1110.0f, 946.0f, 1310.0f, 980.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1310.0f, 882.0f, 1427.0f, 946.0f));

    m_SolidBlocks.push_back(ImageRectToWorldRect(195.0f, 502.0f, 655.0f, 542.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(555.0f, 300.0f, 646.0f, 542.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(646.0f, 252.0f, 940.0f, 300.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(936.0f, 40.0f, 978.0f, 780.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(978.0f, 742.0f, 1160.0f, 780.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1012.0f, 515.0f, 1110.0f, 780.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1110.0f, 252.0f, 1427.0f, 300.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1110.0f, 300.0f, 1152.0f, 540.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1152.0f, 540.0f, 1297.0f, 580.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1320.0f, 640.0f, 1427.0f, 690.0f));

    m_SolidBlocks.push_back(ImageRectToWorldRect(28.0f, 140.0f, 190.0f, 180.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(292.0f, 180.0f, 470.0f, 218.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(690.0f, 532.0f, 826.0f, 570.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(747.0f, 740.0f, 876.0f, 780.0f));

    m_Slopes = {
        {ImagePointToWorldPoint(250.0f, 1040.0f), ImagePointToWorldPoint(402.0f, 1041.0f)},
        {ImagePointToWorldPoint(1010.0f, 1000.0f), ImagePointToWorldPoint(1320.0f, 860.0f)},
        {ImagePointToWorldPoint(1110.0f, 946.0f), ImagePointToWorldPoint(1310.0f, 882.0f)},
        {ImagePointToWorldPoint(1320.0f, 640.0f), ImagePointToWorldPoint(1427.0f, 640.0f)},
        {ImagePointToWorldPoint(1110.0f, 252.0f), ImagePointToWorldPoint(1427.0f, 252.0f)},
    };

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

    // Level 4 diamonds mapped from the original-game reference screenshot.
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
