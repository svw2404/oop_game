#include "App.hpp"

void App::BuildLevel5() {
    const std::vector<std::string> lavaPaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_04.png",
    };
    const std::vector<std::string> lavaLargePaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_04.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/lava_large_05.png",
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
        rect.isSlopeFill = true;
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
    m_SolidBlocks.push_back(ImageRectToWorldRect(217.0f, 143.0f, 274.0f, 191.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(309.0f, 176.0f, 456.0f, 191.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(491.0f, 143.0f, 560.0f, 191.0f));
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
    m_SolidBlocks.push_back(ImageRectToWorldRect(44.0f, 1013.0f, 104.0f, 1082.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(104.0f, 1032.0f, 343.0f, 1082.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(343.0f, 1013.0f, 405.0f, 1082.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(448.0f, 1040.0f, 702.0f, 1082.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(750.0f, 1037.0f, 997.0f, 1082.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1067.0f, 1025.0f, 1114.0f, 1082.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1114.0f, 1043.0f, 1331.0f, 1082.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1331.0f, 1025.0f, 1406.0f, 1082.0f));

    // Right tower with top lava basin.
    m_SolidBlocks.push_back(ImageRectToWorldRect(886.0f, 148.0f, 950.0f, 194.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(988.0f, 179.0f, 1126.0f, 194.0f));
    m_SolidBlocks.push_back(ImageRectToWorldRect(1164.0f, 153.0f, 1222.0f, 194.0f));
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
        {ImagePointToWorldPoint(274.0f, 149.0f), ImagePointToWorldPoint(309.0f, 176.0f)},
        {ImagePointToWorldPoint(456.0f, 176.0f), ImagePointToWorldPoint(491.0f, 149.0f)},
        {ImagePointToWorldPoint(450.0f, 226.0f), ImagePointToWorldPoint(493.0f, 191.0f)},
        {ImagePointToWorldPoint(560.0f, 372.0f), ImagePointToWorldPoint(594.0f, 410.0f)},
        {ImagePointToWorldPoint(559.0f, 590.0f), ImagePointToWorldPoint(596.0f, 634.0f)},
        {ImagePointToWorldPoint(559.0f, 816.0f), ImagePointToWorldPoint(598.0f, 858.0f)},
        {ImagePointToWorldPoint(851.0f, 412.0f), ImagePointToWorldPoint(884.0f, 372.0f)},
        {ImagePointToWorldPoint(847.0f, 637.0f), ImagePointToWorldPoint(902.0f, 591.0f)},
        {ImagePointToWorldPoint(848.0f, 859.0f), ImagePointToWorldPoint(893.0f, 814.0f)},
        {ImagePointToWorldPoint(950.0f, 148.0f), ImagePointToWorldPoint(988.0f, 179.0f)},
        {ImagePointToWorldPoint(1126.0f, 179.0f), ImagePointToWorldPoint(1164.0f, 153.0f)},
        {ImagePointToWorldPoint(1043.0f, 337.0f), ImagePointToWorldPoint(1186.0f, 189.0f)},
        {ImagePointToWorldPoint(44.0f, 1004.0f), ImagePointToWorldPoint(73.0f, 1023.0f)},
        {ImagePointToWorldPoint(67.0f, 1013.0f), ImagePointToWorldPoint(104.0f, 1032.0f)},
        {ImagePointToWorldPoint(343.0f, 1032.0f), ImagePointToWorldPoint(380.0f, 1013.0f)},
        {ImagePointToWorldPoint(371.0f, 1023.0f), ImagePointToWorldPoint(405.0f, 1001.0f)},
        {ImagePointToWorldPoint(1037.0f, 1005.0f), ImagePointToWorldPoint(1067.0f, 1025.0f)},
        {ImagePointToWorldPoint(1076.0f, 1025.0f), ImagePointToWorldPoint(1114.0f, 1043.0f)},
        {ImagePointToWorldPoint(1331.0f, 1043.0f), ImagePointToWorldPoint(1369.0f, 1026.0f)},
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

    auto addHazardInBasin = [this](
        const std::vector<std::string>& paths,
        HazardRect::Type type,
        float assetWidthImage,
        float assetHeightImage,
        float hazardTopLeftXImage,
        float hazardTopLeftYImage,
        float hazardTopRightXImage,
        float hazardTopRightYImage,
        float hazardBottomLeftXImage,
        float hazardBottomLeftYImage,
        float hazardBottomRightXImage,
        float hazardBottomRightYImage,
        float visualTopLeftXImage,
        float visualTopLeftYImage,
        float visualTopRightXImage,
        float visualTopRightYImage,
        float visualBottomLeftXImage,
        float visualBottomLeftYImage,
        float visualBottomRightXImage,
        float visualBottomRightYImage,
        float zIndex
    ) {
        auto hazardAnimation = std::make_shared<Util::Animation>(paths, true, 120, true, 0);
        const float visualTopWidthImage = std::abs(visualTopRightXImage - visualTopLeftXImage);
        const float visualTopYImage = (visualTopLeftYImage + visualTopRightYImage) * 0.5f;
        const float visualBottomYImage = (visualBottomLeftYImage + visualBottomRightYImage) * 0.5f;
        const float visualHeightImage = std::abs(visualBottomYImage - visualTopYImage);
        const float overlayWidthImage =
            visualTopWidthImage + std::min(36.0f, visualTopWidthImage * 0.13f);
        const float overlayHeightImage =
            std::max(visualHeightImage, overlayWidthImage * (assetHeightImage / assetWidthImage) * 0.39f);
        hazardAnimation->SetSize(ImageSizeToWorldSize(overlayWidthImage, overlayHeightImage));

        const float visualCenterXImage =
            (visualTopLeftXImage + visualTopRightXImage + visualBottomLeftXImage + visualBottomRightXImage) * 0.25f;
        glm::vec2 pos = ImagePointToWorldPoint(visualCenterXImage, visualBottomYImage);
        pos.y += ImageSizeToWorldSize(overlayWidthImage, overlayHeightImage).y * 0.5f;

        auto hazardObject = std::make_shared<Util::GameObject>(hazardAnimation, zIndex);
        hazardObject->m_Transform.translation = pos;
        m_Root.AddChild(hazardObject);
        m_AnimatedLevelProps.push_back(hazardObject);

        const float hazardTopWidthImage = std::abs(hazardTopRightXImage - hazardTopLeftXImage);
        const float hazardBottomWidthImage = std::abs(hazardBottomRightXImage - hazardBottomLeftXImage);
        const float hazardTopYImage = (hazardTopLeftYImage + hazardTopRightYImage) * 0.5f;
        const float hazardBottomYImage = (hazardBottomLeftYImage + hazardBottomRightYImage) * 0.5f;
        const float hazardHeightImage = std::abs(hazardBottomYImage - hazardTopYImage);
        const float surfaceInset = std::min(6.0f, hazardTopWidthImage * 0.03f);
        const float bodyInset = std::min(6.0f, hazardBottomWidthImage * 0.04f);
        const float upperBandBottomY = hazardTopYImage + hazardHeightImage * 0.42f;

        const SolidRect surfaceRect = ImageRectToWorldRect(
            std::min(hazardTopLeftXImage, hazardTopRightXImage) + surfaceInset,
            hazardTopYImage,
            std::max(hazardTopLeftXImage, hazardTopRightXImage) - surfaceInset,
            upperBandBottomY,
            false
        );
        m_Hazards.push_back({surfaceRect.center, surfaceRect.size, type});

        const SolidRect bodyRect = ImageRectToWorldRect(
            std::min(hazardBottomLeftXImage, hazardBottomRightXImage) + bodyInset,
            upperBandBottomY - 1.0f,
            std::max(hazardBottomLeftXImage, hazardBottomRightXImage) - bodyInset,
            hazardBottomYImage,
            false
        );
        m_Hazards.push_back({bodyRect.center, bodyRect.size, type});
    };

    // Visible liquids in the source art.
    addHazardInBasin(
        waterPaths,
        HazardRect::Type::Water,
        178.0f, 60.0f,
        274.0f, 148.0f,
        491.0f, 149.0f,
        309.0f, 176.0f,
        456.0f, 176.0f,
        250.0f, 148.0f,
        515.0f, 149.0f,
        288.0f, 176.0f,
        477.0f, 176.0f,
        10.2f
    );
    addHazardInBasin(
        lavaPaths,
        HazardRect::Type::Lava,
        177.0f, 61.0f,
        950.0f, 148.0f,
        1164.0f, 153.0f,
        988.0f, 179.0f,
        1126.0f, 179.0f,
        922.0f, 148.0f,
        1192.0f, 153.0f,
        966.0f, 179.0f,
        1148.0f, 179.0f,
        10.2f
    );
    addHazardInBasin(
        lavaLargePaths,
        HazardRect::Type::Lava,
        360.0f, 80.0f,
        67.0f, 1016.0f,
        380.0f, 1016.0f,
        104.0f, 1032.0f,
        343.0f, 1032.0f,
        35.0f, 1016.0f,
        412.0f, 1016.0f,
        76.0f, 1034.0f,
        371.0f, 1034.0f,
        10.2f
    );
    addHazardInBasin(
        waterPaths,
        HazardRect::Type::Water,
        178.0f, 60.0f,
        1076.0f, 1025.0f,
        1369.0f, 1026.0f,
        1114.0f, 1043.0f,
        1331.0f, 1043.0f,
        1054.0f, 1025.0f,
        1391.0f, 1026.0f,
        1096.0f, 1043.0f,
        1349.0f, 1043.0f,
        10.2f
    );

    // Level 5 chain platforms use the Level 4 chain art, but are weighted by
    // either character standing on the platform instead of by a cube.
    constexpr float chainPlatformScale = 1.0f;
    constexpr float chainPlatformWidthImage = 90.0f;
    constexpr float chainPlatformHeightImage = 20.0f;
    const glm::vec2 chainPlatformSize = ImageSizeToWorldSize(103.0f, 17.0f, chainPlatformScale);
    constexpr float lowerChainPlatformCenterXImage = 185.0f;
    constexpr float upperChainPlatformCenterXImage = 1265.0f;
    constexpr float lowerChainPlatformTopYImage = 940.0f;
    constexpr float upperChainPlatformTopYImage = 245.0f;

    const SolidRect lowerChainPlatformRect = ImageRectToWorldRect(
        lowerChainPlatformCenterXImage - chainPlatformWidthImage * 0.5f,
        lowerChainPlatformTopYImage,
        lowerChainPlatformCenterXImage + chainPlatformWidthImage * 0.5f,
        lowerChainPlatformTopYImage + chainPlatformHeightImage
    );
    m_Level5ChainPlatformBottomRestRect = lowerChainPlatformRect;
    m_Level5ChainPlatformBottomCurrentRect = lowerChainPlatformRect;
    m_Level5ChainPlatformBottomBlockIndex = m_SolidBlocks.size();
    m_HasLevel5ChainPlatformBottom = true;
    m_SolidBlocks.push_back(lowerChainPlatformRect);

    const SolidRect upperChainPlatformRect = ImageRectToWorldRect(
        upperChainPlatformCenterXImage - chainPlatformWidthImage * 0.5f,
        upperChainPlatformTopYImage,
        upperChainPlatformCenterXImage + chainPlatformWidthImage * 0.5f,
        upperChainPlatformTopYImage + chainPlatformHeightImage
    );
    m_Level5ChainPlatformTopRestRect = upperChainPlatformRect;
    m_Level5ChainPlatformTopCurrentRect = upperChainPlatformRect;
    m_Level5ChainPlatformTopBlockIndex = m_SolidBlocks.size();
    m_HasLevel5ChainPlatforms = true;
    m_HasLevel5ChainPlatformTop = true;
    m_Level5ChainPlatformMaxOffset = ImageSizeToWorldSize(0.0f, 260.0f).y;
    m_Level5ChainPlatformSpeed = 1.4f;
    m_Level5ChainPlatformAcceleration = 0.075f;
    m_Level5ChainPlatformBottomTravelScale = 1.0f;
    m_SolidBlocks.push_back(upperChainPlatformRect);

    auto addLevel5HorizontalChainLinks = [&](float leftXImage, float rightXImage, float centerYImage) {
        constexpr float chainLinkScale = 0.064f;
        const glm::vec2 chainLinkSize = ImageSizeToWorldSize(124.0f, 1024.0f, chainLinkScale);
        const float leftX = ImagePointToWorldPoint(leftXImage, centerYImage).x;
        const float rightX = ImagePointToWorldPoint(rightXImage, centerYImage).x;
        const float chainWidth = std::max(1.0f, rightX - leftX);
        const float desiredSpacing = ImageSizeToWorldSize(44.0f, 0.0f).x;
        const int linkCount = std::max(
            3,
            static_cast<int>(std::ceil(chainWidth / std::max(1.0f, desiredSpacing)))
        );

        m_Level5HorizontalChainMinX = leftX;
        m_Level5HorizontalChainWidth = chainWidth;
        m_Level5HorizontalChainSpacing = chainWidth / static_cast<float>(linkCount);

        for (int i = 0; i < linkCount; ++i) {
            auto link = std::make_shared<Character>(
                GA_RESOURCE_DIR "/Image/Assets/chain2.png"
            );
            link->SetZIndex(8.12f);
            link->SetSize(chainLinkSize);
            const glm::vec2 linkPosition = {
                leftX + (static_cast<float>(i) + 0.5f) * m_Level5HorizontalChainSpacing,
                ImagePointToWorldPoint(0.0f, centerYImage).y,
            };
            link->SetPosition(linkPosition);
            link->m_Transform.rotation = 1.57079632679f;
            m_Root.AddChild(link);
            m_LevelProps.push_back(link);
            m_Level5HorizontalChainLinks.push_back(link);
            m_Level5HorizontalChainBasePositions.push_back(linkPosition);
        }
    };

    constexpr float level5ChainYImage = 58.0f;
    constexpr float upperChainConnectorOffsetXImage = 8.0f;
    const float upperChainCenterXImage = upperChainPlatformCenterXImage + upperChainConnectorOffsetXImage;
    addLevel5HorizontalChainLinks(lowerChainPlatformCenterXImage, upperChainCenterXImage, level5ChainYImage);

    auto addLevel5ChainPoint = [&](float centerXImage, float centerYImage) {
        auto point = std::make_shared<Character>(
            GA_RESOURCE_DIR "/Image/Assets/chain_point.png"
        );
        point->SetZIndex(8.25f);
        point->SetSize(ImageSizeToWorldSize(641.0f, 646.0f, 0.06f));
        point->SetPosition(ImagePointToWorldPoint(centerXImage, centerYImage));
        m_Root.AddChild(point);
        m_LevelProps.push_back(point);
        m_Level5ChainPulleys.push_back(point);
    };
    addLevel5ChainPoint(lowerChainPlatformCenterXImage + 5.0f, level5ChainYImage + 15.0f);
    addLevel5ChainPoint(upperChainCenterXImage - 5.0f, level5ChainYImage + 15.0f);

    const glm::vec2 verticalChainSize = ImageSizeToWorldSize(124.0f, 1024.0f, 0.11f);
    auto lowerVerticalChain = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/chain2.png"
    );
    m_Level5ChainBottom = lowerVerticalChain;
    m_Level5ChainBottomAnchorY = ImagePointToWorldPoint(lowerChainPlatformCenterXImage, level5ChainYImage).y;
    m_HasLevel5ChainBottomAnchor = true;
    m_Level5ChainBottom->SetZIndex(8.14f);
    m_Level5ChainBottom->SetSize(verticalChainSize);
    m_Level5ChainBottom->SetPosition(ImagePointToWorldPoint(
        lowerChainPlatformCenterXImage,
        (level5ChainYImage + lowerChainPlatformTopYImage) * 0.5f
    ));
    m_Root.AddChild(m_Level5ChainBottom);
    m_LevelProps.push_back(m_Level5ChainBottom);

    auto lowerChainConnector = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/chain_connect.png"
    );
    lowerChainConnector->SetZIndex(8.2f);
    lowerChainConnector->SetSize(ImageSizeToWorldSize(774.0f, 621.0f, 0.105f));
    lowerChainConnector->SetPosition(ImagePointToWorldPoint(
        lowerChainPlatformCenterXImage,
        lowerChainPlatformTopYImage - 31.0f
    ));
    m_Root.AddChild(lowerChainConnector);
    m_LevelProps.push_back(lowerChainConnector);
    m_Level5ChainConnectBottom = lowerChainConnector;
    m_Level5ChainBottomConnectorYOffset =
        lowerChainConnector->GetPosition().y - m_Level5ChainPlatformBottomRestRect.center.y;

    m_Level5ChainPlatformBottom = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/platform_wood.png"
    );
    m_Level5ChainPlatformBottom->SetZIndex(8.0f);
    m_Level5ChainPlatformBottom->SetSize(chainPlatformSize);
    m_Level5ChainPlatformBottom->SetPosition(lowerChainPlatformRect.center);
    m_Root.AddChild(m_Level5ChainPlatformBottom);
    m_LevelProps.push_back(m_Level5ChainPlatformBottom);

    m_Level5ChainTopXOffset = ImagePointToWorldPoint(upperChainCenterXImage, 0.0f).x -
        ImagePointToWorldPoint(upperChainPlatformCenterXImage, 0.0f).x;
    m_Level5ChainTopAnchorY = ImagePointToWorldPoint(upperChainCenterXImage, level5ChainYImage).y;
    m_HasLevel5ChainTopAnchor = true;
    m_Level5ChainTop = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/chain2.png"
    );
    m_Level5ChainTop->SetZIndex(8.14f);
    m_Level5ChainTop->SetSize(verticalChainSize);
    m_Level5ChainTop->SetPosition(ImagePointToWorldPoint(
        upperChainCenterXImage,
        (level5ChainYImage + upperChainPlatformTopYImage) * 0.5f
    ));
    m_Root.AddChild(m_Level5ChainTop);
    m_LevelProps.push_back(m_Level5ChainTop);

    auto upperChainConnector = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/chain_connect.png"
    );
    upperChainConnector->SetZIndex(8.2f);
    upperChainConnector->SetSize(ImageSizeToWorldSize(774.0f, 621.0f, 0.105f));
    upperChainConnector->SetPosition(ImagePointToWorldPoint(
        upperChainCenterXImage,
        upperChainPlatformTopYImage - 31.0f
    ));
    m_Root.AddChild(upperChainConnector);
    m_LevelProps.push_back(upperChainConnector);
    m_Level5ChainConnectTop = upperChainConnector;
    m_Level5ChainTopConnectorYOffset =
        upperChainConnector->GetPosition().y - m_Level5ChainPlatformTopRestRect.center.y;

    m_Level5ChainPlatformTop = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/platform_wood.png"
    );
    m_Level5ChainPlatformTop->SetZIndex(8.0f);
    m_Level5ChainPlatformTop->SetSize(chainPlatformSize);
    m_Level5ChainPlatformTop->SetPosition(upperChainPlatformRect.center);
    m_Root.AddChild(m_Level5ChainPlatformTop);
    m_LevelProps.push_back(m_Level5ChainPlatformTop);

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
    const float doorScale = propScale * 1.25f * level5ToStandardScale;
    const float diamondScale = propScale * 0.90f * level5ToStandardScale * 1.25f;
    const float switchScale = propScale * 0.72f;
    constexpr float switchPlatformHeightImage = 26.0f;

    auto addSwitchPlatform = [&](float platformLeft,
                                 float platformTop,
                                 float platformRight,
                                 float platformBottom,
                                 float pressedLeft,
                                 float pressedTop,
                                 float pressedRight,
                                 float pressedBottom,
                                 float switchCenterX,
                                 float switchBottomY) {
        Level5SwitchPlatform switchPlatform;
        switchPlatform.restRect = ImageRectToWorldRect(
            platformLeft, platformTop, platformRight, platformBottom
        );
        switchPlatform.pressedRect = ImageRectToWorldRect(
            pressedLeft, pressedTop, pressedRight, pressedBottom
        );
        switchPlatform.currentRect = switchPlatform.restRect;
        switchPlatform.blockIndex = m_SolidBlocks.size();
        m_SolidBlocks.push_back(switchPlatform.currentRect);

        switchPlatform.platform = std::make_shared<Character>(
            GA_RESOURCE_DIR "/Image/Assets/platform_green.png"
        );
        switchPlatform.platform->SetZIndex(7.0f);
        switchPlatform.platform->SetSize({
            switchPlatform.currentRect.size.x,
            switchPlatform.currentRect.size.y,
        });
        switchPlatform.platform->SetPosition(switchPlatform.currentRect.center);
        m_Root.AddChild(switchPlatform.platform);

        switchPlatform.switchSprite = AddPropAtBottom(
            std::string(GA_RESOURCE_DIR) + "/Image/Assets/switch_green_off.png",
            switchCenterX, switchBottomY, 148.0f, 124.0f, 7.6f, switchScale
        );
        const glm::vec2 switchSpriteSize = ImageSizeToWorldSize(148.0f, 124.0f, switchScale);
        switchPlatform.switchHitbox.center = switchPlatform.switchSprite->GetPosition() + glm::vec2{
            -switchSpriteSize.x * 0.01f,
            -switchSpriteSize.y * 0.10f,
        };
        switchPlatform.switchHitbox.size = {
            switchSpriteSize.x * 0.72f,
            switchSpriteSize.y * 0.62f,
        };

        m_Level5SwitchPlatforms.push_back(switchPlatform);
    };

    addSwitchPlatform(
        553.0f, 486.0f, 703.0f, 486.0f + switchPlatformHeightImage,
        750.0f, 486.0f, 900.0f, 486.0f + switchPlatformHeightImage,
        500.0f, 392.0f
    );
    addSwitchPlatform(
        750.0f, 706.0f, 900.0f, 706.0f + switchPlatformHeightImage,
        553.0f, 706.0f, 703.0f, 706.0f + switchPlatformHeightImage,
        928.0f, 615.0f
    );
    addSwitchPlatform(
        553.0f, 925.0f, 703.0f, 925.0f + switchPlatformHeightImage,
        750.0f, 925.0f, 900.0f, 925.0f + switchPlatformHeightImage,
        500.0f, 838.0f
    );

    auto addDoorFrame = [&](float cx, float bottomY) {
        AddPropAtBottom(
            std::string(GA_RESOURCE_DIR) + "/Image/Character/door frame.png",
            cx, bottomY, 138.0f, 147.0f, 7.1f, doorScale
        );
    };
    constexpr float leftDoorX = 363.0f;
    constexpr float rightDoorX = 1076.0f;
    constexpr float doorBottomY = 632.0f;

    addDoorFrame(leftDoorX, doorBottomY);
    addDoorFrame(rightDoorX, doorBottomY);

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
        rightDoorX, doorBottomY, 160.0f, 205.0f, 7.2f, doorScale
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
        leftDoorX, doorBottomY, 162.0f, 205.0f, 7.2f, doorScale
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

    addRedDiamond(1059.0f, 91.0f);
    addRedDiamond(1293.0f, 124.0f);
    addRedDiamond(648.0f, 201.0f);
    addRedDiamond(1135.0f, 423.0f);
    addRedDiamond(501.0f, 536.0f);
    addRedDiamond(1286.0f, 572.0f);
    addRedDiamond(1286.0f, 870.0f);

    addBlueDiamond(388.0f, 91.0f);
    addBlueDiamond(164.0f, 124.0f);
    addBlueDiamond(800.0f, 201.0f);
    addBlueDiamond(314.0f, 423.0f);
    addBlueDiamond(164.0f, 568.0f);
    addBlueDiamond(948.0f, 760.0f);
    addBlueDiamond(164.0f, 865.0f);
}
