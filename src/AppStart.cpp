#include "App.hpp"

#include "HeadBodyCharacter.hpp"
#include "Util/Animation.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

#include <algorithm>
#include <filesystem>
#include <unordered_map>

static std::unordered_map<HeadBodyCharacter*, glm::vec2> s_LastPos;

// TODO(Roadmap):
// - Build the real front-end flow: intro screen, home/menu, options, sound,
//   credits, and pause UI, based on the original game's structure.
// - Add a level-select world map with connected-path progression and 5 total
//   levels represented by emerald/gem nodes.
// - Prepare 5 clean level backgrounds without baked gameplay props, then place
//   doors, liquids, diamonds, switches, cubes, and platforms in code.
// - Reposition and recenter our diamonds to match the original Level 1 layout
//   more closely after comparing against gameplay footage.
// - Clean the liquid assets by trimming the side edges so no thin line floats
//   above the floor.
// - Clean the door assets by trimming the screenshot borders on both sides.
// - Add full game-feel polish: music, SFX, pause/home/restart/mute controls,
//   stored progression, stored rank, and stored diamond totals.
// - Keep polishing gameplay edge cases: remaining slope seams, cube-on-slope
//   behavior, door trigger feel, liquid death feel, and z-order/hitbox cleanup.
// - Decide whether to preserve the current controls or switch to the original
//   default mapping (Fireboy arrows, Watergirl A/W/D).

void App::Start() {
    LOG_TRACE("Start");
    // Rebuild the whole scene graph on every Start() so restarting the level
    // creates a fresh world instead of stacking duplicate drawables.
    m_Root = Util::Renderer();

    // 載入並播放背景音樂
    if (!m_BackgroundMusic) {
        m_BackgroundMusic = std::make_shared<Util::BGM>(GA_RESOURCE_DIR "/Music/Adventure.mp3");
        m_BackgroundMusic->SetVolume(64); // 設定音量 (範圍 0~128)
        m_BackgroundMusic->Play();        // 預設 -1 表示無限循環播放
    }

    // 載入死亡音效
    if (!m_DeathSound) {
        m_DeathSound = std::make_shared<Util::SFX>(GA_RESOURCE_DIR "/Music/Death.mp3");
        m_DeathSound->SetVolume(64); // 設定音量
    }

    // 載入過關音效
    if (!m_FinishSound) {
        m_FinishSound = std::make_shared<Util::SFX>(GA_RESOURCE_DIR "/Music/Finish.mp3");
        m_FinishSound->SetVolume(64); // 設定音量
    }

    // -------------------------------------------------------------------------
    // 1) 基本場景參數
    // -------------------------------------------------------------------------
    m_BackgroundOriginalSize = {2380.0f, 1760.0f};
    m_BackgroundDisplayedSize = {1244.16f, 699.84f};

    // -------------------------------------------------------------------------
    // 2) 背景
    // -------------------------------------------------------------------------
    std::string backgroundPath = std::string(GA_RESOURCE_DIR) + "/Image/Background/background0.png";
    if (m_ActiveLevelIndex == 2) {
        backgroundPath = std::string(GA_RESOURCE_DIR) + "/Image/Background/background1.png";
    }
    else if (m_ActiveLevelIndex == 3) {
        backgroundPath = std::string(GA_RESOURCE_DIR) + "/Image/Background/background2.png";
    }

    m_Background = std::make_shared<BackgroundImage>(backgroundPath);
    m_Background->SetPosition({0.0f, 0.0f});
    m_Root.AddChild(m_Background);

    // -------------------------------------------------------------------------
    // 3) 碰撞區塊（世界邊界 + 地形）
    // -------------------------------------------------------------------------
    m_SolidBlocks.clear();
    m_Slopes.clear();
    m_CeilingSlopes.clear();
    m_Hazards.clear();
    m_LevelProps.clear();
    m_AnimatedLevelProps.clear();
    m_FailOverlayObjects.clear();
    m_Diamonds.clear();
    m_GreenButton.reset();
    m_GreenButtonAfter.reset();
    m_GreenButtonAfter2.reset();
    m_GreenSwitch.reset();
    m_Level2HiddenPlatform.reset();
    m_Level2TopButtonLeft.reset();
    m_Level2TopButtonRight.reset();
    m_Cube.reset();
    m_HasGreenPlatformBlock = false;
    m_HasGreenPlatformBlock2 = false;
    m_HasCubeBlock = false;
    m_GreenButtonPressed = false;
    m_GreenSwitchOn = false;
    m_GreenSwitchTouchLatch = false;
    m_GreenButtonAfterPressVisual = 0.0f;
    m_GreenButtonAfterPressVisual2 = 0.0f;
    m_FireDiamondsCollected = 0;
    m_WaterDiamondsCollected = 0;
    m_GreenDiamondsCollected = 0;
    m_FireDiamondsTotal = 0;
    m_WaterDiamondsTotal = 0;
    m_GreenDiamondsTotal = 0;
    m_CubeVelocity = {0.0f, 0.0f};
    m_FireboyDeathStartScale = {1.0f, 1.0f};
    m_WatergirlDeathStartScale = {1.0f, 1.0f};
    m_FireboyVictoryStartPosition = {0.0f, 0.0f};
    m_WatergirlVictoryStartPosition = {0.0f, 0.0f};
    m_FireboyVictoryStartScale = {1.0f, 1.0f};
    m_WatergirlVictoryStartScale = {1.0f, 1.0f};
    m_CubeOnGround = false;
    m_FireboyDeathTimer = 0.0f;
    m_WatergirlDeathTimer = 0.0f;
    m_FireboyCeilingCarryTimer = 0.0f;
    m_WatergirlCeilingCarryTimer = 0.0f;
    m_FireboyCeilingCarrySpeed = 0.0f;
    m_WatergirlCeilingCarrySpeed = 0.0f;
    m_FireboyDoor = {};
    m_WatergirlDoor = {};
    m_VictoryPhase = VictoryPhase::None;
    m_VictoryTimer = 0.0f;
    m_VictoryCelebrateDuration = 0.0f;
    m_FailOverlayVisible = false;
    m_FailOverlayMouseLatch = false;
    m_FailOverlayPanel.reset();
    m_FailOverlayTitle.reset();
    m_FailRestartButton = {};
    m_FailHomeButton = {};
    m_FailExitButton = {};
    m_VictoryOverlayVisible = false;
    m_VictoryOverlayPanel.reset();
    m_VictoryOverlayTitle.reset();
    m_VictoryTimeText.reset();
    m_VictoryBlueDiamondIcon.reset();
    m_VictoryRedDiamondIcon.reset();
    m_VictoryBlueDiamondText.reset();
    m_VictoryRedDiamondText.reset();
    m_VictoryRankText.reset();
    m_VictoryContinueButton = {};
    m_VictoryOverlayObjects.clear();
    m_LevelStartTimeMs = 0.0f;
    m_LevelCompleteTimeMs = 0.0f;
    m_GreenButtonAfterBaseSize = {0.0f, 0.0f};
    m_GreenButtonAfterBaseSize2 = { 0.0f, 0.0f };
    m_GreenPlatform2VisualCoverY = 0.0f;
    m_GreenPlatform2VisualSize = {0.0f, 0.0f};
    m_Level2HiddenPlatformVisualSize = {0.0f, 0.0f};
    m_Level2TopButtonLeftBasePosition = {0.0f, 0.0f};
    m_Level2TopButtonRightBasePosition = {0.0f, 0.0f};
    m_Level2TopButtonBaseSize = {0.0f, 0.0f};
    m_DeathMusicPlayed = false;
    m_FinishMusicPlayed = false;
    m_GreenPlatform2UseVerticalVisualClip = false;
    m_HasLevel2HiddenPlatformBlock = false;
    m_Level2HiddenPlatformBlockIndex = 0;
    m_Level2TopButtonLeftPressVisual = 0.0f;
    m_Level2TopButtonRightPressVisual = 0.0f;

    if (!m_GameplayMusic) {
        m_GameplayMusic = std::make_unique<Util::BGM>(
            std::string(GA_RESOURCE_DIR) + "/Music/Adventure.mp3"
        );
    }
    if (!m_DeathMusic) {
        m_DeathMusic = std::make_unique<Util::BGM>(
            std::string(GA_RESOURCE_DIR) + "/Music/Death.mp3"
        );
    }
    if (!m_FinishMusic) {
        m_FinishMusic = std::make_unique<Util::BGM>(
            std::string(GA_RESOURCE_DIR) + "/Music/Finish.mp3"
        );
    }

    if (m_ActiveLevelIndex == 2) {
        BuildLevel2();
    } else {
        BuildLevel1();
    }

    // -------------------------------------------------------------------------
    // 4) Fireboy
    // -------------------------------------------------------------------------
    // Fireboy/Watergirl share the same body+head controller architecture.
    // The difference is mainly asset folders, controls, and element rules.
    {
        const std::string bodyDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Body";
        const std::string idleDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Idle";
        const std::string runDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Run";
        const std::string jumpDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Jump";
        const std::string fallDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Fall";
        const std::string winDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Win";

        std::vector<std::string> bodyPaths;
        std::vector<std::string> idleHeadPaths;
        std::vector<std::string> runHeadPaths;
        std::vector<std::string> jumpHeadPaths;
        std::vector<std::string> fallHeadPaths;
        std::vector<std::string> winHeadPaths;

        try {
            for (const auto& entry : std::filesystem::directory_iterator(bodyDir)) {
                if (entry.is_regular_file()) {
                    bodyPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(idleDir)) {
                if (entry.is_regular_file()) {
                    idleHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(runDir)) {
                if (entry.is_regular_file()) {
                    runHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(jumpDir)) {
                if (entry.is_regular_file()) {
                    jumpHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(fallDir)) {
                if (entry.is_regular_file()) {
                    fallHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(winDir)) {
                if (entry.is_regular_file()) {
                    winHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        std::sort(bodyPaths.begin(), bodyPaths.end());
        std::sort(idleHeadPaths.begin(), idleHeadPaths.end());
        std::sort(runHeadPaths.begin(), runHeadPaths.end());
        std::sort(jumpHeadPaths.begin(), jumpHeadPaths.end());
        std::sort(fallHeadPaths.begin(), fallHeadPaths.end());
        std::sort(winHeadPaths.begin(), winHeadPaths.end());

        if (bodyPaths.empty()) {
            bodyPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Body/fireboy_body_00.png");
        }
        if (idleHeadPaths.empty()) {
            idleHeadPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Fireboy/Idle/fireboy_idle_00.png");
        }
        if (runHeadPaths.empty()) {
            runHeadPaths = idleHeadPaths;
        }
        if (jumpHeadPaths.empty()) {
            jumpHeadPaths = runHeadPaths;
        }
        if (fallHeadPaths.empty()) {
            fallHeadPaths = runHeadPaths;
        }
        if (winHeadPaths.empty()) {
            winHeadPaths = idleHeadPaths;
        }

        std::vector<std::string> moveBodyPaths;
        if (bodyPaths.size() > 1) {
            moveBodyPaths.assign(bodyPaths.begin() + 1, bodyPaths.end());
        } else {
            moveBodyPaths.push_back(bodyPaths.front());
        }

        m_Fireboy = std::make_shared<HeadBodyCharacter>(
            moveBodyPaths, runHeadPaths, 10.0f, 120, true, 120, true
        );
        m_Fireboy->SetSize({36.0f, 40.0f});
        m_Fireboy->SetHeadScale(1.25f / 1.4f);
        m_Fireboy->SetBodySize({30.0f / 1.4f, 31.0f / 1.4f});
        m_Fireboy->SetHeadOffset({0.0f, -4.0f});
        RecalculateFireboyCollisionBoxes();

        m_Fireboy->SetIdleBodyImage(bodyPaths.front());
        m_Fireboy->SetIdleHeadImage(idleHeadPaths, 120, true);
        m_Fireboy->SetJumpHeadImage(jumpHeadPaths, 120, true);
        m_Fireboy->SetFallHeadImage(fallHeadPaths, 120, true);
        m_Fireboy->SetWinHeadImage(winHeadPaths, 360, false);
        m_VictoryCelebrateDuration = std::max(
            m_VictoryCelebrateDuration,
            static_cast<float>(std::max<std::size_t>(1, winHeadPaths.size()) - 1) * 0.36f
        );
        m_Fireboy->SetLifeState(HeadBodyCharacter::LifeState::Alive);
        m_Fireboy->SetMotionState(HeadBodyCharacter::MotionState::Idle);

        const float floorTop = m_LeftFloorSpawnRect.center.y + (m_LeftFloorSpawnRect.size.y * 0.5f);
        const float wallRight = m_LeftWallSpawnRect.center.x + (m_LeftWallSpawnRect.size.x * 0.5f);

        glm::vec2 spawnPos;
        const float leftExtent = -GetFireboyLeftEdge({0.0f, 0.0f});
        const float bodyBottom = GetFireboyBodyBottom({0.0f, 0.0f});
        spawnPos.x = wallRight + leftExtent + 5.0f;
        spawnPos.y = floorTop - bodyBottom;

        m_Fireboy->SetPosition(spawnPos);
        m_FireboySpawnPosition = spawnPos;
        s_LastPos[m_Fireboy.get()] = spawnPos;

        m_Root.AddChild(m_Fireboy);
    }

    // -------------------------------------------------------------------------
    // 5) Watergirl
    // -------------------------------------------------------------------------
    {
        const std::string bodyDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Body";
        const std::string idleDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Idle";
        const std::string runDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Run";
        const std::string jumpDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Jump";
        const std::string fallDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Fall";
        const std::string winDir = std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Win";

        std::vector<std::string> bodyPaths;
        std::vector<std::string> idleHeadPaths;
        std::vector<std::string> runHeadPaths;
        std::vector<std::string> jumpHeadPaths;
        std::vector<std::string> fallHeadPaths;
        std::vector<std::string> winHeadPaths;

        try {
            for (const auto& entry : std::filesystem::directory_iterator(bodyDir)) {
                if (entry.is_regular_file()) {
                    bodyPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(idleDir)) {
                if (entry.is_regular_file()) {
                    idleHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(runDir)) {
                if (entry.is_regular_file()) {
                    runHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(jumpDir)) {
                if (entry.is_regular_file()) {
                    jumpHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(fallDir)) {
                if (entry.is_regular_file()) {
                    fallHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        try {
            for (const auto& entry : std::filesystem::directory_iterator(winDir)) {
                if (entry.is_regular_file()) {
                    winHeadPaths.push_back(entry.path().string());
                }
            }
        } catch (const std::exception&) {
        }

        std::sort(bodyPaths.begin(), bodyPaths.end());
        std::sort(idleHeadPaths.begin(), idleHeadPaths.end());
        std::sort(runHeadPaths.begin(), runHeadPaths.end());
        std::sort(jumpHeadPaths.begin(), jumpHeadPaths.end());
        std::sort(fallHeadPaths.begin(), fallHeadPaths.end());
        std::sort(winHeadPaths.begin(), winHeadPaths.end());

        if (bodyPaths.empty()) {
            bodyPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Body/watergirl_body_00.png");
        }
        if (idleHeadPaths.empty()) {
            idleHeadPaths.push_back(std::string(GA_RESOURCE_DIR) + "/Image/Character/Watergirl/Idle/watergirl_idle_00.png");
        }
        if (runHeadPaths.empty()) {
            runHeadPaths = idleHeadPaths;
        }
        if (jumpHeadPaths.empty()) {
            jumpHeadPaths = runHeadPaths;
        }
        if (fallHeadPaths.empty()) {
            fallHeadPaths = runHeadPaths;
        }
        if (winHeadPaths.empty()) {
            winHeadPaths = idleHeadPaths;
        }

        std::vector<std::string> moveBodyPaths;
        if (bodyPaths.size() > 1) {
            moveBodyPaths.assign(bodyPaths.begin() + 1, bodyPaths.end());
        } else {
            moveBodyPaths.push_back(bodyPaths.front());
        }

        m_Watergirl = std::make_shared<HeadBodyCharacter>(
            moveBodyPaths, runHeadPaths, 10.0f, 120, true, 120, true
        );
        m_Watergirl->SetSize({36.0f, 40.0f});
        m_Watergirl->SetHeadScale(1.25f / 1.4f);
        m_Watergirl->SetMoveHeadWidthScale(1.18f);
        m_Watergirl->SetBodySize({30.0f / 1.4f, 31.0f / 1.4f});
        m_Watergirl->SetHeadOffset({0.0f, -4.0f});
        RecalculateWatergirlCollisionBoxes();

        m_Watergirl->SetIdleBodyImage(bodyPaths.front());
        m_Watergirl->SetIdleHeadImage(idleHeadPaths, 120, true);
        m_Watergirl->SetJumpHeadImage(jumpHeadPaths, 120, true);
        m_Watergirl->SetFallHeadImage(fallHeadPaths, 120, true);
        m_Watergirl->SetWinHeadImage(winHeadPaths, 360, false);
        m_VictoryCelebrateDuration = std::max(
            m_VictoryCelebrateDuration,
            static_cast<float>(std::max<std::size_t>(1, winHeadPaths.size()) - 1) * 0.36f
        );
        m_Watergirl->SetLifeState(HeadBodyCharacter::LifeState::Alive);
        m_Watergirl->SetMotionState(HeadBodyCharacter::MotionState::Idle);

        const float floorTop = m_LeftFloorSpawnRect.center.y + (m_LeftFloorSpawnRect.size.y * 0.5f);
        const float wallRight = m_LeftWallSpawnRect.center.x + (m_LeftWallSpawnRect.size.x * 0.5f);

        glm::vec2 spawnPos;
        const float leftExtent = -GetWatergirlLeftEdge({0.0f, 0.0f});
        const float bodyBottom = GetWatergirlBodyBottom({0.0f, 0.0f});
        spawnPos.x = wallRight + leftExtent + 45.0f;
        spawnPos.y = floorTop - bodyBottom;

        m_Watergirl->SetPosition(spawnPos);
        m_WatergirlSpawnPosition = spawnPos;
        s_LastPos[m_Watergirl.get()] = spawnPos;

        m_Root.AddChild(m_Watergirl);
    }

    // -------------------------------------------------------------------------
    // 6) 初始狀態
    // -------------------------------------------------------------------------
    m_FireboyVelocity = {0.0f, 0.0f};
    m_WatergirlVelocity = {0.0f, 0.0f};
    m_FireboyOnGround = true;
    m_WatergirlOnGround = true;

    LOG_DEBUG("Background Size: {}x{}", m_BackgroundDisplayedSize.x, m_BackgroundDisplayedSize.y);
    LOG_DEBUG(
        "Fireboy Body Box: {}x{} Head Box: {}x{}",
        m_FireboyCollision.bodyHitboxSize.x,
        m_FireboyCollision.bodyHitboxSize.y,
        m_FireboyCollision.headHitboxSize.x,
        m_FireboyCollision.headHitboxSize.y
    );
    LOG_DEBUG(
        "Watergirl Body Box: {}x{} Head Box: {}x{}",
        m_WatergirlCollision.bodyHitboxSize.x,
        m_WatergirlCollision.bodyHitboxSize.y,
        m_WatergirlCollision.headHitboxSize.x,
        m_WatergirlCollision.headHitboxSize.y
    );

    // -------------------------------------------------------------------------
    // 7) Fail overlay
    // -------------------------------------------------------------------------
    // This is intentionally structured like future UI buttons:
    // - current branch uses text labels
    // - each button already stores its future asset path so swapping to real
    //   sprites later should not require rewriting the fail-state logic.
    m_FailOverlayPanel = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/ui_fail_overlay_panel.png"
    );
    m_FailOverlayPanel->SetZIndex(50.0f);
    m_FailOverlayPanel->SetSize({760.0f, 430.0f});
    m_FailOverlayPanel->SetPosition({0.0f, 20.0f});
    m_FailOverlayPanel->SetVisible(false);
    m_Root.AddChild(m_FailOverlayPanel);
    m_FailOverlayObjects.push_back(m_FailOverlayPanel);

    auto makeOverlayText = [&](const std::string& text,
                               const glm::vec2& position,
                               int fontSize,
                               const Util::Color& color,
                               float zIndex) {
        auto object = std::make_shared<OverlayText>(text, fontSize, color, zIndex);
        object->m_Transform.translation = position;
        object->SetVisible(false);
        m_Root.AddChild(object);
        m_FailOverlayObjects.push_back(object);
        return object;
    };

    m_FailOverlayTitle = makeOverlayText(
        "Both heroes fell",
        {0.0f, 120.0f},
        34,
        Util::Color(255, 245, 220, 255),
        51.0f
    );

    auto makeFailButton = [&](const std::string& label,
                              const std::string& futureAssetPath,
                              const glm::vec2& center,
                              const glm::vec2& size) {
        FailOverlayButton button;
        button.label = label;
        button.futureAssetPath = futureAssetPath;
        button.rect.center = center;
        button.rect.size = size;
        button.labelObject = makeOverlayText(
            label,
            center,
            26,
            Util::Color(255, 255, 255, 255),
            51.0f
        );
        return button;
    };

    m_FailRestartButton = makeFailButton(
        "RESTART",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/button_restart.png",
        {0.0f, 25.0f},
        {240.0f, 64.0f}
    );
    m_FailHomeButton = makeFailButton(
        "HOME",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/button_home.png",
        {0.0f, -55.0f},
        {240.0f, 64.0f}
    );
    m_FailExitButton = makeFailButton(
        "EXIT",
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/button_exit.png",
        {0.0f, -135.0f},
        {240.0f, 64.0f}
    );

    UpdateFailOverlayVisuals();

    // -------------------------------------------------------------------------
    // 8) Victory overlay
    // -------------------------------------------------------------------------
    // Mirrors the fail overlay structure so swapping text placeholders for
    // real UI button art later will be straightforward.
    m_VictoryOverlayPanel = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Assets/ui_fail_overlay_panel.png"
    );
    m_VictoryOverlayPanel->SetZIndex(50.0f);
    m_VictoryOverlayPanel->SetSize({760.0f, 460.0f});
    m_VictoryOverlayPanel->SetPosition({0.0f, 20.0f});
    m_VictoryOverlayPanel->SetVisible(false);
    m_Root.AddChild(m_VictoryOverlayPanel);
    m_VictoryOverlayObjects.push_back(m_VictoryOverlayPanel);

    auto makeVictoryOverlayText = [&](const std::string& text,
                                      const glm::vec2& position,
                                      int fontSize,
                                      const Util::Color& color,
                                      float zIndex) {
        auto object = std::make_shared<OverlayText>(text, fontSize, color, zIndex);
        object->m_Transform.translation = position;
        object->SetVisible(false);
        m_Root.AddChild(object);
        m_VictoryOverlayObjects.push_back(object);
        return object;
    };

    auto makeVictoryOverlayIcon = [&](const std::string& path,
                                      const glm::vec2& position,
                                      const glm::vec2& size,
                                      float zIndex) {
        auto object = std::make_shared<Character>(path);
        object->SetZIndex(zIndex);
        object->SetSize(size);
        object->SetPosition(position);
        object->SetVisible(false);
        m_Root.AddChild(object);
        m_VictoryOverlayObjects.push_back(object);
        return object;
    };

    m_VictoryOverlayTitle = makeVictoryOverlayText(
        "LEVEL COMPLETE",
        {0.0f, 128.0f},
        34,
        Util::Color(255, 245, 220, 255),
        51.0f
    );
    m_VictoryTimeText = makeVictoryOverlayText(
        "00:00",
        {0.0f, 72.0f},
        24,
        Util::Color(255, 255, 255, 255),
        51.0f
    );
    m_VictoryBlueDiamondIcon = makeVictoryOverlayIcon(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        {-102.0f, 18.0f},
        {42.0f, 38.0f},
        51.0f
    );
    m_VictoryBlueDiamondText = makeVictoryOverlayText(
        "x 0",
        {12.0f, 18.0f},
        24,
        Util::Color(240, 248, 255, 255),
        51.0f
    );
    m_VictoryRedDiamondIcon = makeVictoryOverlayIcon(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        {-102.0f, -38.0f},
        {42.0f, 38.0f},
        51.0f
    );
    m_VictoryRedDiamondText = makeVictoryOverlayText(
        "x 0",
        {12.0f, -38.0f},
        24,
        Util::Color(255, 235, 230, 255),
        51.0f
    );
    m_VictoryRankText = makeVictoryOverlayText(
        "Rank: C",
        {0.0f, -108.0f},
        26,
        Util::Color(255, 230, 170, 255),
        51.0f
    );

    m_VictoryContinueButton.label = "CONTINUE";
    m_VictoryContinueButton.futureAssetPath =
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/button_continue.png";
    m_VictoryContinueButton.rect.center = {0.0f, -190.0f};
    m_VictoryContinueButton.rect.size = {260.0f, 64.0f};
    m_VictoryContinueButton.labelObject = makeVictoryOverlayText(
        m_VictoryContinueButton.label,
        m_VictoryContinueButton.rect.center,
        26,
        Util::Color(255, 255, 255, 255),
        51.0f
    );

    UpdateVictoryOverlayVisuals();
    // Rebuild the switch latch from the new scene so restart does not inherit
    // an accidental pressed/not-pressed edge from the previous run.
    m_GreenSwitchTouchLatch = IsGreenSwitchTouched();
    m_LevelStartTimeMs = Util::Time::GetElapsedTimeMs();
    if (m_GameplayMusic) {
        m_GameplayMusic->Play(-1);
    }

    m_CurrentState = State::UPDATE;
}

glm::vec2 App::ImageSizeToWorldSize(float imageWidth, float imageHeight, float scale) const {
    const float scaleX = m_BackgroundDisplayedSize.x / m_BackgroundOriginalSize.x;
    const float scaleY = m_BackgroundDisplayedSize.y / m_BackgroundOriginalSize.y;
    return {imageWidth * scaleX * scale, imageHeight * scaleY * scale};
}

std::shared_ptr<Character> App::AddPropAtBottom(
    const std::string& path,
    float centerXImage,
    float bottomYImage,
    float imageWidth,
    float imageHeight,
    float zIndex,
    float scale
) {
    auto prop = std::make_shared<Character>(path);
    const glm::vec2 size = ImageSizeToWorldSize(imageWidth, imageHeight, scale);
    glm::vec2 pos = ImagePointToWorldPoint(centerXImage, bottomYImage);
    pos.y += size.y * 0.5f;
    prop->SetZIndex(zIndex);
    prop->SetSize(size);
    prop->SetPosition(pos);
    m_Root.AddChild(prop);
    m_LevelProps.push_back(prop);
    return prop;
}

void App::AddAnimatedHazardInImageTrap(
    const std::vector<std::string>& paths,
    HazardRect::Type type,
    float assetWidthImage,
    float assetHeightImage,
    float topLeftXImage,
    float topLeftYImage,
    float topRightXImage,
    float topRightYImage,
    float bottomLeftXImage,
    float bottomLeftYImage,
    float bottomRightXImage,
    float bottomRightYImage,
    float zIndex
) {
    auto hazardAnimation = std::make_shared<Util::Animation>(paths, true, 120, true, 0);
    const float topWidthImage = std::abs(topRightXImage - topLeftXImage);
    const float bottomWidthImage = std::abs(bottomRightXImage - bottomLeftXImage);
    const float topYImage = (topLeftYImage + topRightYImage) * 0.5f;
    const float bottomYImage = (bottomLeftYImage + bottomRightYImage) * 0.5f;
    const float imageHeight = std::abs(bottomYImage - topYImage);
    const float overlayWidthImage = topWidthImage + std::min(36.0f, topWidthImage * 0.13f);
    const float overlayHeightImage =
        overlayWidthImage * (assetHeightImage / assetWidthImage) * 0.39f;
    const glm::vec2 size = ImageSizeToWorldSize(overlayWidthImage, overlayHeightImage);
    hazardAnimation->SetSize(size);

    const float centerXImage =
        (topLeftXImage + topRightXImage + bottomLeftXImage + bottomRightXImage) * 0.25f;
    glm::vec2 pos = ImagePointToWorldPoint(centerXImage, bottomYImage);
    pos.y += size.y * 0.5f;

    auto hazardObject = std::make_shared<Util::GameObject>(hazardAnimation, zIndex);
    hazardObject->m_Transform.translation = pos;
    m_Root.AddChild(hazardObject);
    m_AnimatedLevelProps.push_back(hazardObject);

    const float surfaceInset = std::min(6.0f, topWidthImage * 0.03f);
    const float bodyInset = std::min(6.0f, bottomWidthImage * 0.04f);
    const float upperBandBottomY = topYImage + imageHeight * 0.42f;

    {
        const SolidRect rect = ImageRectToWorldRect(
            std::min(topLeftXImage, topRightXImage) + surfaceInset,
            topYImage,
            std::max(topLeftXImage, topRightXImage) - surfaceInset,
            upperBandBottomY,
            false
        );
        m_Hazards.push_back({rect.center, rect.size, type});
    }

    {
        const SolidRect rect = ImageRectToWorldRect(
            std::min(bottomLeftXImage, bottomRightXImage) + bodyInset,
            upperBandBottomY - 1.0f,
            std::max(bottomLeftXImage, bottomRightXImage) - bodyInset,
            bottomYImage,
            false
        );
        m_Hazards.push_back({rect.center, rect.size, type});
    }
}

void App::AddCollectibleDiamond(
    const std::string& path,
    DiamondType type,
    float centerXImage,
    float centerYImage,
    float imageWidth,
    float imageHeight,
    float zIndex,
    float scale,
    bool required
) {
    auto diamond = std::make_shared<Character>(path);
    const glm::vec2 size = ImageSizeToWorldSize(imageWidth, imageHeight, scale);
    diamond->SetZIndex(zIndex);
    diamond->SetSize(size);
    diamond->SetPosition(ImagePointToWorldPoint(centerXImage, centerYImage));
    m_Root.AddChild(diamond);
    m_Diamonds.push_back({diamond, type, required, false});

    switch (type) {
    case DiamondType::Fire:
        ++m_FireDiamondsTotal;
        break;
    case DiamondType::Water:
        ++m_WaterDiamondsTotal;
        break;
    case DiamondType::Neutral:
        ++m_GreenDiamondsTotal;
        break;
    }
}

void App::AddCurrentSlopeGuardBands() {
    auto addSlopeGuardBand = [&](const auto& slopes, float solidSideSign) {
        constexpr float guardHeight = 9.0f;
        constexpr float guardExtraOverlap = 7.0f;
        constexpr float endpointGuardSize = 12.0f;

        for (const auto& slope : slopes) {
            const glm::vec2 delta = slope.end - slope.start;
            const float slopeLength = glm::length(delta);
            if (slopeLength < 0.001f) {
                continue;
            }

            const int segments = std::max(3, static_cast<int>(std::ceil(slopeLength / 10.0f)));
            const float guardWidth = (slopeLength / static_cast<float>(segments)) + guardExtraOverlap;

            for (int i = 0; i <= segments; ++i) {
                const float t = static_cast<float>(i) / static_cast<float>(segments);
                const glm::vec2 point = slope.start + delta * t;

                SolidRect guard;
                guard.center = {
                    point.x,
                    point.y + solidSideSign * (guardHeight * 0.5f - 0.35f),
                };
                guard.size = {guardWidth, guardHeight};
                guard.blockBottom = true;
                m_SolidBlocks.push_back(guard);
            }

            for (const glm::vec2 endpoint : {slope.start, slope.end}) {
                SolidRect endpointGuard;
                endpointGuard.center = {
                    endpoint.x,
                    endpoint.y + solidSideSign * (endpointGuardSize * 0.5f - 0.35f),
                };
                endpointGuard.size = {endpointGuardSize, endpointGuardSize};
                endpointGuard.blockBottom = true;
                m_SolidBlocks.push_back(endpointGuard);
            }
        }
    };

    addSlopeGuardBand(m_Slopes, -1.0f);
    addSlopeGuardBand(m_CeilingSlopes, 1.0f);
}
