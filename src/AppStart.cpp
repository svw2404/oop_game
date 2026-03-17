#include "App.hpp"

#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");

    m_Background = std::make_shared<BackgroundImage>(
        GA_RESOURCE_DIR "/Image/Background/background0.png"
    );
    m_Background->SetPosition({ 0.0f, 0.0f });
    m_Root.AddChild(m_Background);

    m_SolidBlocks.clear();

    // World borders from Gemini image:
    // floor, roof, left wall, right wall are all 45 px thick in ORIGINAL image coordinates

    // Roof: y = 0 to 45
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 2380.0f, 45.0f));

    // Floor: y = 1715 to 1760
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 1715.0f, 2380.0f, 1760.0f));

    // Left wall: x = 0 to 45
    m_SolidBlocks.push_back(ImageRectToWorldRect(0.0f, 0.0f, 45.0f, 1760.0f));

    // Right wall: x = 2335 to 2380
    m_SolidBlocks.push_back(ImageRectToWorldRect(2335.0f, 0.0f, 2380.0f, 1760.0f));

    // Main uneven ground mass from your image-map polygon.
    // We keep runtime collision rectangular by decomposing the perimeter
    // into a handful of solid blocks that approximate the traced shape.
    m_TestBlock = ImageRectToWorldRect(46.0f, 1466.0f, 784.0f, 1519.0f);
    m_SolidBlocks.push_back(m_TestBlock);

    const std::vector<RectObject> perimeterBlocks = {
        ImageRectToWorldRect(46.0f, 1216.0f, 426.0f, 1273.0f),
        ImageRectToWorldRect(481.0f, 1250.0f, 670.0f, 1273.0f),
        ImageRectToWorldRect(721.0f, 1223.0f, 1029.0f, 1273.0f),
        ImageRectToWorldRect(901.0f, 1273.0f, 1160.0f, 1395.0f),
        ImageRectToWorldRect(1036.0f, 1340.0f, 1475.0f, 1395.0f),
        ImageRectToWorldRect(1475.0f, 1342.0f, 1519.0f, 1395.0f),
        ImageRectToWorldRect(1519.0f, 1372.0f, 1704.0f, 1395.0f),
        ImageRectToWorldRect(1704.0f, 1342.0f, 1758.0f, 1395.0f),
        ImageRectToWorldRect(1758.0f, 1342.0f, 2063.0f, 1393.0f),
        ImageRectToWorldRect(2149.0f, 1578.0f, 2325.0f, 1704.0f),
        ImageRectToWorldRect(2208.0f, 1525.0f, 2321.0f, 1578.0f),
    };

    m_SolidBlocks.insert(m_SolidBlocks.end(), perimeterBlocks.begin(), perimeterBlocks.end());

    m_Fireboy = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Character/Fireboy/Idle/fireboy_idle_00.png"
    );
    m_Fireboy->SetZIndex(10);

    // Spawn Fireboy on the floor, a bit away from the left wall
    const RectObject& floorBlock = m_SolidBlocks[1];
    float floorTop = floorBlock.center.y + floorBlock.size.y * 0.5f;
    float spawnX = -450.0f;
    float spawnY = floorTop + m_FireboyHitboxSize.y * 0.5f;
    
    m_Fireboy->SetSize({ 45.0f, 45.0f });
    m_Fireboy->SetPosition({ spawnX, spawnY });
    m_Root.AddChild(m_Fireboy);

    m_Diamond = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Character/RedDiamonds.png"
    );
    m_Diamond->SetZIndex(8);
    m_Diamond->SetSize({ 18.0f, 18.0f });

    // Place diamond on top of the invisible platform block
    float blockTop = m_TestBlock.center.y + m_TestBlock.size.y * 0.5f;
    float diamondY = blockTop + m_DiamondHitboxSize.y * 0.5f + 5.0f;

    m_Diamond->SetPosition({ m_TestBlock.center.x, diamondY });
    m_Root.AddChild(m_Diamond);

    m_FireboyVelocity = { 0.0f, 0.0f };
    m_FireboyOnGround = true;

    m_CurrentState = State::UPDATE;
}
