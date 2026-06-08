#include "App.hpp"

#include <cmath>

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
    const std::vector<std::string> venomLargePaths = {
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_large_00.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_large_01.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_large_02.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_large_03.png",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/venom_large_04.png",
    };

    const float level4ToStandardScale = 1455.0f / 2380.0f;
    const float propScale = 1.0f / 1.5f;
    const float doorScale = propScale * 1.6f * level4ToStandardScale;
    const float diamondScale = propScale * 0.90f * level4ToStandardScale * 1.25f;

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

    // Far-left chain and the wood platform hanging under it.
    constexpr float chainPlatformScale = 1.0f;
    constexpr float chainPlatformWidthImage = 90.0f;
    constexpr float chainPlatformHeightImage = 20.0f;
    const glm::vec2 chainPlatformSize = ImageSizeToWorldSize(103.0f, 17.0f, chainPlatformScale);
    constexpr float leftChainPlatformShiftXImage = 24.0f;
    constexpr float leftChainPlatformShiftYImage = 30.0f;
    constexpr float leftChainPlatformCenterXImage = 65.0f + leftChainPlatformShiftXImage;
    const SolidRect leftChainPlatformRect = ImageRectToWorldRect(
        leftChainPlatformCenterXImage - chainPlatformWidthImage * 0.5f,
        987.0f + leftChainPlatformShiftYImage,
        leftChainPlatformCenterXImage + chainPlatformWidthImage * 0.5f,
        987.0f + leftChainPlatformShiftYImage + chainPlatformHeightImage
    );
    m_Level4ChainPlatformBottomRestRect = leftChainPlatformRect;
    m_Level4ChainPlatformBottomCurrentRect = m_Level4ChainPlatformBottomRestRect;
    m_Level4ChainPlatformBottomBlockIndex = m_SolidBlocks.size();
    m_HasLevel4ChainPlatformBottom = true;
    m_SolidBlocks.push_back(leftChainPlatformRect);

    auto addHorizontalChainLinks = [&](float leftXImage, float rightXImage, float centerYImage) {
        constexpr float chainLinkScale = 0.064f;
        const glm::vec2 chainLinkSize = ImageSizeToWorldSize(124.0f, 1024.0f, chainLinkScale);
        const float chainLinkLengthImage = 1024.0f * chainLinkScale;
        const float halfChainLinkLengthImage = chainLinkLengthImage * 0.5f;
        const float stepXImage = 46.0f;
        const float firstCenterXImage = leftXImage + halfChainLinkLengthImage;
        const float lastCenterXImage = rightXImage - halfChainLinkLengthImage;
        float previousCenterXImage = firstCenterXImage - stepXImage;

        auto addHorizontalChainLink = [&](float centerXImage) {
            auto link = std::make_shared<Character>(
                GA_RESOURCE_DIR "/Image/Assets/chain2.png"
            );
            link->SetZIndex(8.12f);
            link->SetSize(chainLinkSize);
            const glm::vec2 linkPosition = ImagePointToWorldPoint(centerXImage, centerYImage);
            link->SetPosition(linkPosition);
            link->m_Transform.rotation = 1.57079632679f;
            m_Root.AddChild(link);
            m_LevelProps.push_back(link);
            m_Level4HorizontalChainLinks.push_back(link);
            m_Level4HorizontalChainBasePositions.push_back(linkPosition);
        };

        for (float centerXImage = firstCenterXImage;
             centerXImage <= lastCenterXImage;
             centerXImage += stepXImage) {
            addHorizontalChainLink(centerXImage);
            previousCenterXImage = centerXImage;
        }

        if (lastCenterXImage - previousCenterXImage > stepXImage * 0.4f) {
            addHorizontalChainLink(lastCenterXImage);
        }
    };

    constexpr float upperChainPlatformCenterXImage = 1060.0f;
    addHorizontalChainLinks(leftChainPlatformCenterXImage, upperChainPlatformCenterXImage, 33.0f);
    constexpr float upperChainConnectorOffsetXImage = 8.0f;
    const float upperChainCenterXImage = upperChainPlatformCenterXImage + upperChainConnectorOffsetXImage;
    constexpr float chainPointInsetXImage = 5.0f;
    constexpr float chainPointDropYImage = 15.0f;

    auto addChainPoint = [&](float centerXImage, float centerYImage) {
        auto point = std::make_shared<Character>(
            GA_RESOURCE_DIR "/Image/Assets/chain_point.png"
        );
        point->SetZIndex(8.25f);
        point->SetSize(ImageSizeToWorldSize(641.0f, 646.0f, 0.06f));
        point->SetPosition(ImagePointToWorldPoint(centerXImage, centerYImage));
        m_Root.AddChild(point);
        m_LevelProps.push_back(point);
    };
    addChainPoint(leftChainPlatformCenterXImage + chainPointInsetXImage, 33.0f + chainPointDropYImage);
    addChainPoint(upperChainCenterXImage - chainPointInsetXImage, 33.0f + chainPointDropYImage);

    const float leftChainTopYImage = 33.0f;
    const float leftChainConnectorYImage = 956.0f + leftChainPlatformShiftYImage;
    const glm::vec2 verticalChainSize = ImageSizeToWorldSize(124.0f, 1024.0f, 0.11f);
    auto leftVerticalChain = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/chain2.png"
    );
    m_Level4ChainBottom = leftVerticalChain;
    m_Level4ChainBottomAnchorY = ImagePointToWorldPoint(leftChainPlatformCenterXImage, leftChainTopYImage).y;
    m_HasLevel4ChainBottomAnchor = true;
    m_Level4ChainBottom->SetZIndex(8.14f);
    m_Level4ChainBottom->SetSize({
        verticalChainSize.x,
        std::abs(
            ImagePointToWorldPoint(leftChainPlatformCenterXImage, leftChainTopYImage).y -
            ImagePointToWorldPoint(leftChainPlatformCenterXImage, leftChainConnectorYImage).y
        )
    });
    m_Level4ChainBottom->SetPosition(ImagePointToWorldPoint(
        leftChainPlatformCenterXImage,
        (leftChainTopYImage + leftChainConnectorYImage) * 0.5f
    ));
    m_Root.AddChild(m_Level4ChainBottom);
    m_LevelProps.push_back(m_Level4ChainBottom);

    auto chainConnector = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/chain_connect.png"
    );
    chainConnector->SetZIndex(8.2f);
    chainConnector->SetSize(ImageSizeToWorldSize(774.0f, 621.0f, 0.105f));
    chainConnector->SetPosition(ImagePointToWorldPoint(
        leftChainPlatformCenterXImage,
        956.0f + leftChainPlatformShiftYImage
    ));
    m_Root.AddChild(chainConnector);
    m_LevelProps.push_back(chainConnector);
    m_Level4ChainConnectBottom = chainConnector;

    auto leftChainPlatform = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/platform_wood.png"
    );
    leftChainPlatform->SetZIndex(8.0f);
    leftChainPlatform->SetSize(chainPlatformSize);
    leftChainPlatform->SetPosition(leftChainPlatformRect.center);
    m_Root.AddChild(leftChainPlatform);
    m_LevelProps.push_back(leftChainPlatform);
    m_Level4ChainPlatformBottom = leftChainPlatform;

    constexpr float upperChainPlatformDropImage = 24.0f;
    const glm::vec2 upperChainPlatformSize = chainPlatformSize;
    const SolidRect upperChainPlatformRect = ImageRectToWorldRect(
        upperChainPlatformCenterXImage - chainPlatformWidthImage * 0.5f,
        150.0f + upperChainPlatformDropImage,
        upperChainPlatformCenterXImage + chainPlatformWidthImage * 0.5f,
        150.0f + upperChainPlatformDropImage + chainPlatformHeightImage
    );
    m_Level4ChainPlatformTopRestRect = upperChainPlatformRect;
    m_Level4ChainPlatformTopCurrentRect = m_Level4ChainPlatformTopRestRect;
    m_Level4ChainPlatformTopBlockIndex = m_SolidBlocks.size();
    m_HasLevel4ChainPlatforms = true;
    m_HasLevel4ChainPlatformTop = true;
    m_Level4ChainPlatformMaxOffset = ImageSizeToWorldSize(
        0.0f,
        735.0f - (150.0f + upperChainPlatformDropImage + chainPlatformHeightImage)
    ).y;
    m_Level4ChainPlatformSpeed = 1.4f;
    m_Level4ChainPlatformBottomTravelScale = 1.0f;
    m_SolidBlocks.push_back(upperChainPlatformRect);

    const float upperChainAnchorYImage = 33.0f;
    const float upperChainConnectorYImage = 119.0f + upperChainPlatformDropImage;
    const glm::vec2 upperChainSize = verticalChainSize;
    m_Level4ChainTopXOffset = ImagePointToWorldPoint(upperChainCenterXImage, 0.0f).x -
        ImagePointToWorldPoint(upperChainPlatformCenterXImage, 0.0f).x;
    m_Level4ChainTopAnchorY = ImagePointToWorldPoint(upperChainCenterXImage, upperChainAnchorYImage).y;
    m_HasLevel4ChainTopAnchor = true;
    m_Level4ChainTop = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/chain2.png"
    );
    m_Level4ChainTop->SetZIndex(8.14f);
    m_Level4ChainTop->SetSize({
        upperChainSize.x,
        std::abs(
            ImagePointToWorldPoint(upperChainCenterXImage, upperChainAnchorYImage).y -
            ImagePointToWorldPoint(upperChainCenterXImage, upperChainConnectorYImage).y
        )
    });
    m_Level4ChainTop->SetPosition(ImagePointToWorldPoint(
        upperChainCenterXImage,
        (upperChainAnchorYImage + upperChainConnectorYImage) * 0.5f
    ));
    m_Root.AddChild(m_Level4ChainTop);
    m_LevelProps.push_back(m_Level4ChainTop);

    auto upperChainConnector = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/chain_connect.png"
    );
    upperChainConnector->SetZIndex(8.2f);
    upperChainConnector->SetSize(ImageSizeToWorldSize(774.0f, 621.0f, 0.105f));
    upperChainConnector->SetPosition(ImagePointToWorldPoint(upperChainCenterXImage, upperChainConnectorYImage));
    m_Root.AddChild(upperChainConnector);
    m_LevelProps.push_back(upperChainConnector);
    m_Level4ChainConnectTop = upperChainConnector;

    auto upperChainPlatform = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/platform_wood.png"
    );
    upperChainPlatform->SetZIndex(8.0f);
    upperChainPlatform->SetSize(upperChainPlatformSize);
    upperChainPlatform->SetPosition(upperChainPlatformRect.center);
    m_Root.AddChild(upperChainPlatform);
    m_LevelProps.push_back(upperChainPlatform);
    m_Level4ChainPlatformTop = upperChainPlatform;

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
        venomLargePaths, HazardRect::Type::Venom,
        360.0f, 36.0f,
        430.0f, 1019.0f,
        1015.0f, 1018.0f,
        467.0f, 1036.0f,
        978.0f, 1033.0f,
        10.2f
    );

    const float switchScale = propScale * 0.72f;
    const float buttonScale = propScale * 0.82f * level4ToStandardScale * 0.90f;
    const glm::vec2 greenButtonSize = ImageSizeToWorldSize(189.0f, 120.0f, buttonScale);

    m_GreenPlatformRestRect = ImageRectToWorldRect(260.0f, 62.0f, 306.0f, 218.0f);
    m_GreenPlatformPressedRect = ImageRectToWorldRect(260.0f, 210.0f, 306.0f, 366.0f);
    m_GreenPlatformCurrentRect = m_GreenPlatformRestRect;
    m_GreenPlatformBlockIndex = m_SolidBlocks.size();
    m_HasGreenPlatformBlock = true;
    m_SolidBlocks.push_back(m_GreenPlatformCurrentRect);

    m_GreenPlatform = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/platform_green.png"
    );
    m_GreenPlatform->SetZIndex(7.0f);
    m_GreenPlatform->SetSize({
        m_GreenPlatformCurrentRect.size.y,
        m_GreenPlatformCurrentRect.size.x,
    });
    m_GreenPlatform->SetPosition(m_GreenPlatformCurrentRect.center);
    m_GreenPlatform->m_Transform.rotation = 1.57079632679f;
    m_Root.AddChild(m_GreenPlatform);

    m_GreenSwitch = AddPropAtBottom(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/switch_green_off.png",
        145.0f, 146.0f, 148.0f, 124.0f, 7.6f, switchScale
    );
    const glm::vec2 switchSpriteSize = ImageSizeToWorldSize(148.0f, 124.0f, switchScale);
    m_GreenSwitchHitbox.center = m_GreenSwitch->GetPosition() + glm::vec2{
        -switchSpriteSize.x * 0.01f,
        -switchSpriteSize.y * 0.10f,
    };
    m_GreenSwitchHitbox.size = {
        switchSpriteSize.x * 0.72f,
        switchSpriteSize.y * 0.62f,
    };

    m_GreenPlatformRestRect2 = ImageRectToWorldRect(400.0f, 725.0f, 440.0f, 895.0f);
    m_GreenPlatformPressedRect2 = ImageRectToWorldRect(400.0f, 895.0f, 440.0f, 1065.0f);
    m_GreenPlatformCurrentRect2 = m_GreenPlatformRestRect2;
    m_GreenPlatformBlockIndex2 = m_SolidBlocks.size();
    m_HasGreenPlatformBlock2 = true;
    m_SolidBlocks.push_back(m_GreenPlatformCurrentRect2);

    m_GreenPlatform2 = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/platform_green.png"
    );
    m_GreenPlatform2->SetZIndex(7.0f);
    m_GreenPlatform2->SetSize({
        m_GreenPlatformCurrentRect2.size.y,
        m_GreenPlatformCurrentRect2.size.x,
    });
    m_GreenPlatform2->SetPosition(m_GreenPlatformCurrentRect2.center);
    m_GreenPlatform2->m_Transform.rotation = 1.57079632679f;
    m_Root.AddChild(m_GreenPlatform2);

    m_GreenButtonAfterHitbox = ImageRectToWorldRect(1216.0f, 500.0f, 1288.0f, 521.0f);
    m_GreenButtonAfter = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/button_green.png"
    );
    m_GreenButtonAfter->SetZIndex(7.6f);
    m_GreenButtonAfter->SetSize(greenButtonSize);
    m_GreenButtonAfterBasePosition = {
        m_GreenButtonAfterHitbox.center.x,
        ImagePointToWorldPoint(0.0f, 521.0f).y + greenButtonSize.y * 0.5f,
    };
    m_GreenButtonAfterBaseSize = greenButtonSize;
    m_GreenButtonAfter->SetPosition(m_GreenButtonAfterBasePosition);
    m_Root.AddChild(m_GreenButtonAfter);
    m_LevelProps.push_back(m_GreenButtonAfter);

    m_GreenButtonAfterHitbox.center = {
        m_GreenButtonAfterBasePosition.x,
        ImagePointToWorldPoint(0.0f, 521.0f).y + 4.0f,
    };
    m_GreenButtonAfterHitbox.size = {
        greenButtonSize.x * 0.72f,
        8.0f,
    };

    const float cubeScale = level4ToStandardScale * 0.88f;
    const glm::vec2 cubeSpriteSize = ImageSizeToWorldSize(123.0f, 124.0f, cubeScale);
    const glm::vec2 cubeHitboxSize = {
        cubeSpriteSize.x * m_CubeHitboxScale.x,
        cubeSpriteSize.y * m_CubeHitboxScale.y,
    };
    const glm::vec2 cubeFloorPoint = ImagePointToWorldPoint(1238.0f, 178.0f);
    m_CubeRect.center = cubeFloorPoint + glm::vec2{0.0f, cubeHitboxSize.y * 0.5f};
    m_CubeRect.size = cubeHitboxSize;
    m_CubeSpawnRect = m_CubeRect;
    m_CubeBlockIndex = m_SolidBlocks.size();
    m_HasCubeBlock = true;
    m_SolidBlocks.push_back(m_CubeRect);

    m_Cube = std::make_shared<Character>(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/cube.png"
    );
    m_Cube->SetZIndex(8.1f);
    m_Cube->SetSize(cubeSpriteSize);
    m_Cube->SetPosition(cubeFloorPoint + glm::vec2{0.0f, cubeSpriteSize.y * 0.5f});
    m_CubeSpawnPosition = m_Cube->GetPosition();
    m_Root.AddChild(m_Cube);

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
