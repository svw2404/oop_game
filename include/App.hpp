#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include <cstddef>

#include "BackgroundImage.hpp"
#include "Character.hpp"
#include "HeadBodyCharacter.hpp"
#include "LevelGeometry.hpp"
#include "Util/Color.hpp"
#include "Util/GameObject.hpp"
#include "Util/Keycode.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"
#include "Util/BGM.hpp"
#include "Util/SFX.hpp"

class OverlayText : public Util::GameObject {
public:
    OverlayText(const std::string& text,
                int fontSize,
                const Util::Color& color,
                float zIndex)
        : GameObject(
            std::make_shared<Util::Text>(
                GA_RESOURCE_DIR "/Font/Inkfree.ttf",
                fontSize,
                text,
                color
            ),
            zIndex
        ) {}

    void SetText(const std::string& text) {
        if (auto drawable = std::dynamic_pointer_cast<Util::Text>(m_Drawable)) {
            drawable->SetText(text);
        }
    }

    void SetColor(const Util::Color& color) {
        if (auto drawable = std::dynamic_pointer_cast<Util::Text>(m_Drawable)) {
            drawable->SetColor(color);
        }
    }
};

class App {
public:
    enum class State {
        LEVEL_SELECT,
        START,
        UPDATE,
        END,
    };

    [[nodiscard]] State GetCurrentState() const { return m_CurrentState; }

    // 主流程
    void UpdateLevelSelect();
    void Start();
    void Update();
    void End();

private:
    struct CharacterCollisionProfile {
        glm::vec2 bodyHitboxSize = {30.0f, 50.0f};
        glm::vec2 bodyHitboxOffset = {0.0f, -2.0f};
        glm::vec2 headHitboxSize = {28.0f, 34.0f};
        glm::vec2 headHitboxOffset = {0.0f, 40.0f};
        glm::vec2 bodyHitboxScale = {0.72f, 0.90f};
        glm::vec2 headHitboxScale = {0.60f, 0.60f};
        glm::vec2 bodyHitboxPadding = {0.0f, 0.0f};
        glm::vec2 headHitboxPadding = {0.0f, 0.0f};
    };

    enum class DiamondType {
        Fire,
        Water,
        Neutral,
    };

    struct CollectibleDiamond {
        std::shared_ptr<Character> sprite;
        DiamondType type = DiamondType::Fire;
        bool required = false;
        bool collected = false;
    };

    struct ExitDoor {
        std::shared_ptr<Character> sprite;
        SolidRect triggerRect;
        std::string closedImagePath;
        std::vector<std::string> openingImagePaths;
        float openProgress = 0.0f;
        bool occupied = false;
    };

    enum class VictoryPhase {
        None,
        RunIntoDoor,
        Celebrate,
    };

    enum class FailOverlayAction {
        None,
        Restart,
        Home,
        Exit,
    };

    struct FailOverlayButton {
        std::string label;
        std::string futureAssetPath;
        SolidRect rect;
        std::shared_ptr<OverlayText> labelObject;
    };

    struct VictoryOverlayButton {
        std::string label;
        std::string futureAssetPath;
        SolidRect rect;
        std::shared_ptr<OverlayText> labelObject;
    };

    struct LevelSelectButton {
        int levelIndex = 1;
        SolidRect rect;
        std::shared_ptr<Character> sprite;
    };

    struct FanProp {
        std::shared_ptr<Util::GameObject> sprite;
        SolidRect windZone;
        glm::vec2 windVelocityDelta = {0.0f, 0.0f};
    };

    // ------------------------------------------------------------------------
    // Core gameplay steps
    // ------------------------------------------------------------------------
    void HandleCharacterInput(
        const std::shared_ptr<HeadBodyCharacter>& character,
        glm::vec2& velocity,
        bool& onGround,
        const CharacterCollisionProfile& profile,
        Util::Keycode leftKey,
        Util::Keycode rightKey,
        Util::Keycode jumpKey,
        float& ceilingCarryTimer,
        float& ceilingCarrySpeed
    );
    void UpdateCharacterPhysics(
        const std::shared_ptr<HeadBodyCharacter>& character,
        glm::vec2& velocity,
        bool& onGround,
        const CharacterCollisionProfile& profile,
        float& ceilingCarryTimer,
        float& ceilingCarrySpeed
    );
    void UpdateCharacterMotionState(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const glm::vec2& velocity,
        bool onGround
    );
    void UpdateGreenSwitch();
    void UpdateGreenPlatform();
    void UpdateGreenPlatform2();
    void UpdateLevel2HiddenPlatform();
    void UpdateLevel2HangingPlatform();
    void UpdateFans();
    void UpdateCubePhysics();
    void ResolveCubeHorizontalCollisions(
        const SolidRect& oldCube,
        SolidRect& newCube,
        glm::vec2& velocity
    ) const;
    bool ResolveCubeSlopeGrounding(
        const SolidRect& oldCube,
        SolidRect& newCube,
        glm::vec2& velocity,
        bool& onGround
    ) const;
    bool FindBestCubeSlopeYAtX(
        const SolidRect& oldCube,
        float desiredCenterX,
        float& outSlopeY
    ) const;
    void ApplyCubeSlopeFollow(
        const SolidRect& oldCube,
        SolidRect& newCube
    ) const;
    bool FindNearbyCubeGroundY(
        const SolidRect& cube,
        float maxDistance,
        float& outGroundY
    ) const;
    void CheckHazards();
    void UpdateDeathSequence();
    void UpdateExitDoors();
    void UpdateVictorySequence();
    void UpdateVictoryOverlayVisuals();
    void UpdateFailOverlay();
    void UpdateFailOverlayVisuals();
    void ResetFailOverlayLatch();
    [[nodiscard]] bool AreBothCharactersDead() const;
    [[nodiscard]] bool IsFailOverlayVisible() const;
    [[nodiscard]] FailOverlayAction GetFailOverlayAction() const;
    bool IsGreenButtonPressed() const;
    bool IsGreenSwitchTouched() const;
    bool CubeTouchesRect(const SolidRect& rect) const;
    bool IsCharacterAtDoor(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile,
        const ExitDoor& door
    ) const;
    bool CharacterPressesRectFromAbove(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile,
        const SolidRect& rect
    ) const;
    bool CubePressesRectFromAbove(const SolidRect& rect) const;
    bool CharacterTouchesRect(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile,
        const SolidRect& rect
    ) const;
    bool CharacterTouchesHazard(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile,
        const HazardRect& hazard
    ) const;
    bool WouldCharacterHitTerrainAt(
        const glm::vec2& bodyPos,
        const CharacterCollisionProfile& profile,
        bool allowSupportContacts = true,
        std::size_t ignoredSolidBlockIndex = static_cast<std::size_t>(-1)
    ) const;
    bool IsCharacterInLiquid(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile
    ) const;
    void CarryCharacterWithPlatform(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile,
        const SolidRect& oldPlatform,
        float platformDeltaY
    ) const;
    float ClampDescendingPlatformDeltaAgainstCharacter(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile,
        const SolidRect& oldPlatform,
        float requestedDeltaY,
        std::size_t platformBlockIndex
    ) const;
    void CarryCharacterWithCube(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile,
        const SolidRect& oldCube,
        const glm::vec2& cubeDelta
    ) const;
    void CarryCubeWithPlatform(const SolidRect& oldPlatform, float platformDeltaY);
    void HandleFireboyInput();
    void HandleWatergirlInput();
    void UpdateFireboyPhysics();
    void UpdateWatergirlPhysics();
    void CheckDiamondCollection();

    // ------------------------------------------------------------------------
    // Level building
    // ------------------------------------------------------------------------
    void BuildLevel1();
    void BuildLevel2();
    void BuildLevel3();
    void BuildLevel4();
    void AddCurrentSlopeGuardBands();
    glm::vec2 ImageSizeToWorldSize(float imageWidth, float imageHeight, float scale = 1.0f) const;
    std::shared_ptr<Character> AddPropAtBottom(
        const std::string& path,
        float centerXImage,
        float bottomYImage,
        float imageWidth,
        float imageHeight,
        float zIndex,
        float scale = 1.0f
    );
    void AddAnimatedHazardInImageTrap(
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
    );
    void AddCollectibleDiamond(
        const std::string& path,
        DiamondType type,
        float centerXImage,
        float centerYImage,
        float imageWidth,
        float imageHeight,
        float zIndex,
        float scale = 1.0f,
        bool required = false
    );

    // ------------------------------------------------------------------------
    // Collision helpers
    // ------------------------------------------------------------------------
    SolidRect ImageRectToWorldRect(
        float x1,
        float y1,
        float x2,
        float y2,
        bool blockBottom = true
    ) const;
    glm::vec2 ImagePointToWorldPoint(float x, float y) const;

    bool CheckAABB(
        const glm::vec2& centerA, const glm::vec2& sizeA,
        const glm::vec2& centerB, const glm::vec2& sizeB
    ) const;

    void ResolveHorizontalCollisions(
        const glm::vec2& oldPos,
        glm::vec2& newPos,
        const CharacterCollisionProfile& profile
    );
    void ResolveVerticalCollisions(
        const glm::vec2& oldPos,
        glm::vec2& newPos,
        const CharacterCollisionProfile& profile,
        glm::vec2& velocity,
        bool& onGround,
        bool& hitCeiling
    );
    bool ResolveSlopeGrounding(
        const glm::vec2& oldPos,
        glm::vec2& newPos,
        const CharacterCollisionProfile& profile,
        glm::vec2& velocity,
        bool& onGround
    );
    bool ResolveCeilingSlopeCollision(
        const glm::vec2& oldPos,
        glm::vec2& newPos,
        const CharacterCollisionProfile& profile,
        glm::vec2& velocity
    );
    bool TrySnapToSlopeTopTransition(
        const glm::vec2& oldPos,
        glm::vec2& newPos,
        const SolidRect& blockingBlock,
        const CharacterCollisionProfile& profile
    ) const;
    bool FindBestSlopeYAtX(
        const glm::vec2& oldPos,
        float desiredX,
        float& outSlopeY,
        const CharacterCollisionProfile& profile
    ) const;
    void ApplySlopeFollow(
        const glm::vec2& oldPos,
        glm::vec2& newPos,
        const CharacterCollisionProfile& profile
    ) const;
    bool FindNearbyGroundY(
        const glm::vec2& pos,
        float maxDistance,
        float& outGroundY,
        const CharacterCollisionProfile& profile
    ) const;
    bool FindNearbyCeilingY(
        const glm::vec2& pos,
        float maxDistance,
        float& outCeilingY,
        const CharacterCollisionProfile& profile
    ) const;
    void RecalculateCharacterCollisionBoxes(
        const std::shared_ptr<HeadBodyCharacter>& character,
        CharacterCollisionProfile& profile
    );
    void RecalculateFireboyCollisionBoxes();
    void RecalculateWatergirlCollisionBoxes();
    void GetCharacterBodyBox(
        const glm::vec2& bodyPos,
        const CharacterCollisionProfile& profile,
        glm::vec2& outCenter,
        glm::vec2& outSize
    ) const;
    void GetCharacterHeadBox(
        const glm::vec2& bodyPos,
        const CharacterCollisionProfile& profile,
        glm::vec2& outCenter,
        glm::vec2& outSize
    ) const;
    bool CheckCharacterCollision(
        const glm::vec2& bodyPos,
        const SolidRect& block,
        const CharacterCollisionProfile& profile
    ) const;
    float GetCharacterLeftEdge(const glm::vec2& bodyPos, const CharacterCollisionProfile& profile) const;
    float GetCharacterRightEdge(const glm::vec2& bodyPos, const CharacterCollisionProfile& profile) const;
    float GetCharacterBodyBottom(const glm::vec2& bodyPos, const CharacterCollisionProfile& profile) const;
    void GetFireboyBodyBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const;
    void GetFireboyHeadBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const;
    bool CheckFireboyCollision(const glm::vec2& bodyPos, const SolidRect& block) const;
    float GetFireboyLeftEdge(const glm::vec2& bodyPos) const;
    float GetFireboyRightEdge(const glm::vec2& bodyPos) const;
    float GetFireboyHeadTop(const glm::vec2& bodyPos) const;
    float GetFireboyBodyBottom(const glm::vec2& bodyPos) const;
    void GetWatergirlBodyBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const;
    void GetWatergirlHeadBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const;
    bool CheckWatergirlCollision(const glm::vec2& bodyPos, const SolidRect& block) const;
    float GetWatergirlLeftEdge(const glm::vec2& bodyPos) const;
    float GetWatergirlRightEdge(const glm::vec2& bodyPos) const;
    float GetWatergirlBodyBottom(const glm::vec2& bodyPos) const;
    // 保留：舊任務檢查
    void ValidTask();

private:
    // ------------------------------------------------------------------------
    // App state
    // ------------------------------------------------------------------------
    State m_CurrentState = State::LEVEL_SELECT;
    Util::Renderer m_Root;

    // ------------------------------------------------------------------------
    // Scene objects
    // ------------------------------------------------------------------------
    std::shared_ptr<BackgroundImage> m_Background;
    std::shared_ptr<Util::BGM> m_BackgroundMusic;
    std::shared_ptr<Util::SFX> m_DeathSound;
    std::shared_ptr<Util::SFX> m_FinishSound;
    std::shared_ptr<HeadBodyCharacter> m_Fireboy;
    std::shared_ptr<HeadBodyCharacter> m_Watergirl;
    std::shared_ptr<Character> m_GreenPlatform;
    std::shared_ptr<Character> m_GreenPlatform2;
    std::shared_ptr<Character> m_GreenButton;
    std::shared_ptr<Character> m_GreenButtonAfter;
    std::shared_ptr<Character> m_GreenButtonAfter2;
    std::shared_ptr<Character> m_GreenSwitch;
    std::shared_ptr<Character> m_Level2HiddenPlatform;
    std::shared_ptr<Character> m_Level2HangingPlatform;
    std::shared_ptr<Character> m_Level2HangingPlatform2;
    std::shared_ptr<Character> m_Level2HangingChain;
    std::shared_ptr<Character> m_Level2HangingChain2;
    std::shared_ptr<Character> m_Level2TopButtonLeft;
    std::shared_ptr<Character> m_Level2TopButtonRight;
    std::shared_ptr<Character> m_Cube;
    ExitDoor m_FireboyDoor;
    ExitDoor m_WatergirlDoor;
    std::vector<std::shared_ptr<Character>> m_LevelProps;
    std::vector<std::shared_ptr<Util::GameObject>> m_AnimatedLevelProps;
    std::vector<std::shared_ptr<Util::GameObject>> m_FailOverlayObjects;
    std::vector<std::shared_ptr<Util::GameObject>> m_VictoryOverlayObjects;
    std::vector<CollectibleDiamond> m_Diamonds;
    std::vector<FanProp> m_Fans;
    std::shared_ptr<BackgroundImage> m_LevelSelectBackground;
    std::vector<LevelSelectButton> m_LevelSelectButtons;

    // ------------------------------------------------------------------------
    // Physics parameters
    // ------------------------------------------------------------------------
    glm::vec2 m_FireboyVelocity = {0.0f, 0.0f};
    glm::vec2 m_WatergirlVelocity = {0.0f, 0.0f};
    glm::vec2 m_CubeVelocity = {0.0f, 0.0f};
    glm::vec2 m_FireboySpawnPosition = {0.0f, 0.0f};
    glm::vec2 m_WatergirlSpawnPosition = {0.0f, 0.0f};
    glm::vec2 m_CubeSpawnPosition = {0.0f, 0.0f};
    glm::vec2 m_GreenButtonBasePosition = {0.0f, 0.0f};
    glm::vec2 m_GreenButtonAfterBasePosition = { 0.0f, 0.0f };
    glm::vec2 m_GreenButtonAfterBasePosition2 = { 0.0f, 0.0f };
    glm::vec2 m_GreenButtonAfterBaseSize = { 0.0f, 0.0f };
    glm::vec2 m_GreenButtonAfterBaseSize2 = { 0.0f, 0.0f };
    float m_GreenPlatform2VisualCoverY = 0.0f;
    glm::vec2 m_GreenPlatform2VisualSize = {0.0f, 0.0f};
    glm::vec2 m_Level2HiddenPlatformVisualSize = {0.0f, 0.0f};
    glm::vec2 m_Level2TopButtonLeftBasePosition = {0.0f, 0.0f};
    glm::vec2 m_Level2TopButtonRightBasePosition = {0.0f, 0.0f};
    glm::vec2 m_Level2TopButtonBaseSize = {0.0f, 0.0f};
    glm::vec2 m_FireboyDeathStartScale = {1.0f, 1.0f};
    glm::vec2 m_WatergirlDeathStartScale = {1.0f, 1.0f};
    glm::vec2 m_FireboyVictoryStartPosition = {0.0f, 0.0f};
    glm::vec2 m_WatergirlVictoryStartPosition = {0.0f, 0.0f};
    glm::vec2 m_FireboyVictoryStartScale = {1.0f, 1.0f};
    glm::vec2 m_WatergirlVictoryStartScale = {1.0f, 1.0f};
    bool m_FireboyOnGround = false;
    bool m_WatergirlOnGround = false;
    bool m_CubeOnGround = false;

    float m_MoveSpeed = 3.75f;
    float m_GroundAcceleration = 0.10f;
    float m_AirAcceleration = 0.23f;
    float m_GroundDeceleration = 0.07f;
    float m_AirDeceleration = 0.0005f;
    float m_JumpSpeed = 4.80f;
    float m_JumpHorizontalLaunchMin = 4.55f;
    float m_JumpHorizontalLaunchMax = 8.10f;
    float m_Gravity = 0.095f;
    float m_LiquidMoveSpeedScale = 0.72f;
    float m_LiquidAccelerationScale = 0.70f;
    float m_LiquidDecelerationScale = 0.80f;
    float m_LiquidJumpScale = 0.55f;
    float m_LiquidGravityScale = 0.55f;
    float m_LiquidVelocityDrag = 0.95f;
    float m_DeathAnimationDuration = 0.28f;
    float m_DeathSinkSpeed = 0.95f;
    float m_DeathEndScale = 0.52f;
    float m_CeilingMomentumCarryDuration = 0.18f;
    float m_CeilingMomentumCarryBoost = 1.08f;
    float m_CeilingMomentumCarryFloor = 0.74f;
    float m_GroundSnapTolerance = 4.0f;
    float m_GroundStickTolerance = 14.0f;
    float m_CeilingStickTolerance = 4.0f;
    float m_CeilingSideGlideThreshold = 1.25f;
    float m_SlopeSnapTolerance = 10.0f;
    float m_StepUpHeight = 14.0f;
    float m_SlopeTopTransitionHeight = 36.0f;
    float m_SlopeFollowTolerance = 20.0f;
    float m_SlopeTopTransitionWidth = 24.0f;
    float m_FootProbeInset = 2.0f;

    // ------------------------------------------------------------------------
    // Gameplay sizes
    // ------------------------------------------------------------------------
    glm::vec2 m_DiamondHitboxScale = {0.60f, 0.60f};
    glm::vec2 m_CubeHitboxScale = {0.90f, 1.00f};
    CharacterCollisionProfile m_FireboyCollision;
    CharacterCollisionProfile m_WatergirlCollision;
    int m_FireDiamondsCollected = 0;
    int m_WaterDiamondsCollected = 0;
    int m_GreenDiamondsCollected = 0;
    int m_FireDiamondsTotal = 0;
    int m_WaterDiamondsTotal = 0;
    int m_GreenDiamondsTotal = 0;
    int m_ActiveLevelIndex = 3;

    glm::vec2 m_BackgroundOriginalSize = {2380.0f, 1760.0f};
    glm::vec2 m_BackgroundDisplayedSize = {1244.16f, 699.84f};

    // ------------------------------------------------------------------------
    // Collision geometry
    // ------------------------------------------------------------------------
    std::vector<SolidRect> m_SolidBlocks;
    std::vector<SlopeSurface> m_Slopes;
    std::vector<CeilingSlopeSurface> m_CeilingSlopes;
    std::vector<HazardRect> m_Hazards;
    SolidRect m_TestBlock;
    SolidRect m_LeftFloorSpawnRect;
    SolidRect m_LeftWallSpawnRect;
    SolidRect m_GreenPlatformRestRect;
    SolidRect m_GreenPlatformPressedRect;
    SolidRect m_GreenPlatformCurrentRect;
    SolidRect m_GreenPlatformRestRect2;
    SolidRect m_GreenPlatformPressedRect2;
    SolidRect m_GreenPlatformCurrentRect2;
    SolidRect m_GreenButtonHitbox;
    SolidRect m_GreenButtonAfterHitbox;
    SolidRect m_GreenButtonAfterHitbox2;
    SolidRect m_GreenSwitchHitbox;
    SolidRect m_Level2HiddenPlatformRestRect;
    SolidRect m_Level2HiddenPlatformShownRect;
    SolidRect m_Level2HiddenPlatformCurrentRect;
    SolidRect m_Level2HangingPlatformRect;
    SolidRect m_Level2HangingPlatformRect2;
    SolidRect m_Level2TopButtonLeftHitbox;
    SolidRect m_Level2TopButtonRightHitbox;
    SolidRect m_CubeRect;
    SolidRect m_CubeSpawnRect;
    std::size_t m_GreenPlatformBlockIndex = 0;
    std::size_t m_GreenPlatformBlockIndex2 = 0;
    std::size_t m_Level2HiddenPlatformBlockIndex = 0;
    std::size_t m_Level2HangingPlatformBlockIndex = 0;
    std::size_t m_Level2HangingPlatformSlopeIndex = 0;
    std::size_t m_Level2HangingPlatformSlopeIndex2 = 0;
    std::size_t m_CubeBlockIndex = 0;
    bool m_HasGreenPlatformBlock = false;
    bool m_HasGreenPlatformBlock2 = false;
    bool m_HasLevel2HiddenPlatformBlock = false;
    bool m_HasLevel2HangingPlatformBlock = false;
    bool m_HasLevel2HangingPlatformSlope = false;
    bool m_HasCubeBlock = false;
    bool m_LevelSelectBuilt = false;
    bool m_LevelSelectMouseLatch = false;
    bool m_GreenPlatform2UseVerticalVisualClip = false;
    bool m_GreenButtonPressed = false;
    bool m_GreenSwitchOn = false;
    bool m_GreenSwitchTouchLatch = false;
    float m_FireboyDeathTimer = 0.0f;
    float m_WatergirlDeathTimer = 0.0f;
    float m_FireboyCeilingCarryTimer = 0.0f;
    float m_WatergirlCeilingCarryTimer = 0.0f;
    float m_FireboyCeilingCarrySpeed = 0.0f;
    float m_WatergirlCeilingCarrySpeed = 0.0f;
    float m_GreenPlatformSpeed = 2.2f;
    float m_CubePushSpeed = 1.15f;
    float m_CubePushAcceleration = 0.10f;
    float m_CubeDeceleration = 0.08f;
    float m_GreenButtonPressDepth = 7.5f;
    float m_GreenButtonAnimSpeed = 0.28f;
    float m_GreenButtonAfterPressVisual = 0.0f;
    float m_GreenButtonAfterPressVisual2 = 0.0f;
    float m_Level2TopButtonLeftPressVisual = 0.0f;
    float m_Level2TopButtonRightPressVisual = 0.0f;
    float m_Level2HangingPlatformAngle = 0.0f;
    float m_Level2HangingPlatformAngle2 = 0.0f;
    float m_Level2HangingPlatformMaxAngle = 0.45f;
    float m_Level2HangingPlatformTorqueScale = 0.015f;
    float m_Level2HangingPlatformAngularSpeed = 1.6f;
    VictoryPhase m_VictoryPhase = VictoryPhase::None;
    float m_VictoryTimer = 0.0f;
    float m_VictoryRunDuration = 0.30f;
    float m_VictoryCelebrateDuration = 0.0f;
    float m_VictoryAlignSpeed = 0.85f;
    float m_VictoryDoorCenterTolerance = 2.5f;
    float m_VictoryDoorSinkScale = 0.38f;
    float m_VictoryDoorSinkYOffset = 2.0f;
    float m_LevelStartTimeMs = 0.0f;
    float m_LevelCompleteTimeMs = 0.0f;
    float m_DoorFrameDuration = 0.06f;
    bool m_FailOverlayVisible = false;
    bool m_FailOverlayMouseLatch = false;
    std::shared_ptr<Character> m_FailOverlayPanel;
    std::shared_ptr<OverlayText> m_FailOverlayTitle;
    FailOverlayButton m_FailRestartButton;
    FailOverlayButton m_FailHomeButton;
    FailOverlayButton m_FailExitButton;
    bool m_VictoryOverlayVisible = false;
    std::shared_ptr<Character> m_VictoryOverlayPanel;
    std::shared_ptr<OverlayText> m_VictoryOverlayTitle;
    std::shared_ptr<OverlayText> m_VictoryTimeText;
    std::shared_ptr<Character> m_VictoryBlueDiamondIcon;
    std::shared_ptr<Character> m_VictoryRedDiamondIcon;
    std::shared_ptr<OverlayText> m_VictoryBlueDiamondText;
    std::shared_ptr<OverlayText> m_VictoryRedDiamondText;
    std::shared_ptr<OverlayText> m_VictoryRankText;
    VictoryOverlayButton m_VictoryContinueButton;

    std::unique_ptr<Util::BGM> m_GameplayMusic;
    std::unique_ptr<Util::BGM> m_DeathMusic;
    std::unique_ptr<Util::BGM> m_FinishMusic;
    bool m_DeathMusicPlayed = false;
    bool m_FinishMusicPlayed = false;
};

#endif // APP_HPP
