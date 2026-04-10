#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include <cstddef>

#include "BackgroundImage.hpp"
#include "Character.hpp"
#include "HeadBodyCharacter.hpp"
#include "LevelGeometry.hpp"
#include "Util/Keycode.hpp"
#include "Util/Renderer.hpp"

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    [[nodiscard]] State GetCurrentState() const { return m_CurrentState; }

    // 主流程
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
        Util::Keycode jumpKey
    );
    void UpdateCharacterPhysics(
        const std::shared_ptr<HeadBodyCharacter>& character,
        glm::vec2& velocity,
        bool& onGround,
        const CharacterCollisionProfile& profile
    );
    void UpdateCharacterMotionState(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const glm::vec2& velocity,
        bool onGround
    );
    void UpdateGreenPlatform();
    bool IsGreenButtonPressed() const;
    bool CharacterTouchesRect(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile,
        const SolidRect& rect
    ) const;
    void CarryCharacterWithPlatform(
        const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile,
        const SolidRect& oldPlatform,
        float platformDeltaY
    ) const;
    void HandleFireboyInput();
    void HandleWatergirlInput();
    void UpdateFireboyPhysics();
    void UpdateWatergirlPhysics();
    void CheckDiamondCollection();

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
        bool& onGround
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
    State m_CurrentState = State::START;
    Util::Renderer m_Root;

    // ------------------------------------------------------------------------
    // Scene objects
    // ------------------------------------------------------------------------
    std::shared_ptr<BackgroundImage> m_Background;
    std::shared_ptr<HeadBodyCharacter> m_Fireboy;
    std::shared_ptr<HeadBodyCharacter> m_Watergirl;
    std::shared_ptr<Character> m_Diamond;
    std::shared_ptr<Character> m_GreenPlatform;
    std::shared_ptr<Character> m_GreenButton;
    std::shared_ptr<Character> m_GreenButtonAfter;

    // ------------------------------------------------------------------------
    // Physics parameters
    // ------------------------------------------------------------------------
    glm::vec2 m_FireboyVelocity = {0.0f, 0.0f};
    glm::vec2 m_WatergirlVelocity = {0.0f, 0.0f};
    bool m_FireboyOnGround = false;
    bool m_WatergirlOnGround = false;

    float m_MoveSpeed = 1.8f;
    float m_GroundAcceleration = 0.10f;
    float m_AirAcceleration = 0.05f;
    float m_GroundDeceleration = 0.07f;
    float m_AirDeceleration = 0.010f;
    float m_JumpSpeed = 5.2f;
    float m_Gravity = 0.11f;
    float m_GroundSnapTolerance = 4.0f;
    float m_GroundStickTolerance = 14.0f;
    float m_CeilingStickTolerance = 10.0f;
    float m_SlopeSnapTolerance = 10.0f;
    float m_StepUpHeight = 14.0f;
    float m_SlopeTopTransitionHeight = 36.0f;
    float m_SlopeFollowTolerance = 20.0f;
    float m_SlopeTopTransitionWidth = 24.0f;
    float m_FootProbeInset = 2.0f;

    // ------------------------------------------------------------------------
    // Gameplay sizes
    // ------------------------------------------------------------------------
    glm::vec2 m_DiamondHitboxSize = {16.0f, 16.0f};
    CharacterCollisionProfile m_FireboyCollision;
    CharacterCollisionProfile m_WatergirlCollision;

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
    SolidRect m_GreenPlatformRestRect;
    SolidRect m_GreenPlatformPressedRect;
    SolidRect m_GreenPlatformCurrentRect;
    SolidRect m_GreenButtonHitbox;
    SolidRect m_GreenButtonAfterHitbox;
    std::size_t m_GreenPlatformBlockIndex = 0;
    bool m_HasGreenPlatformBlock = false;
    bool m_GreenButtonPressed = false;
    float m_GreenPlatformSpeed = 2.2f;
};

#endif // APP_HPP
