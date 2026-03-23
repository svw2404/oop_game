#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "BackgroundImage.hpp"
#include "Character.hpp"
#include "HeadBodyCharacter.hpp"
#include "LevelGeometry.hpp"
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
    // ------------------------------------------------------------------------
    // Core gameplay steps
    // ------------------------------------------------------------------------
    void HandleFireboyInput();
    void UpdateFireboyPhysics();
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

    void ResolveHorizontalCollisions(const glm::vec2& oldPos, glm::vec2& newPos);
    void ResolveVerticalCollisions(const glm::vec2& oldPos, glm::vec2& newPos);
    bool ResolveSlopeGrounding(const glm::vec2& oldPos, glm::vec2& newPos);
    bool ResolveCeilingSlopeCollision(const glm::vec2& oldPos, glm::vec2& newPos);
    bool TrySnapToSlopeTopTransition(
        const glm::vec2& oldPos,
        glm::vec2& newPos,
        const SolidRect& blockingBlock
    ) const;
    bool FindBestSlopeYAtX(const glm::vec2& oldPos, float desiredX, float& outSlopeY) const;
    void ApplySlopeFollow(const glm::vec2& oldPos, glm::vec2& newPos) const;
    bool FindNearbyGroundY(const glm::vec2& pos, float maxDistance, float& outGroundY) const;
    bool FindNearbyCeilingY(const glm::vec2& pos, float maxDistance, float& outCeilingY) const;
    void RecalculateFireboyCollisionBoxes();
    void GetFireboyBodyBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const;
    void GetFireboyHeadBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const;
    bool CheckFireboyCollision(const glm::vec2& bodyPos, const SolidRect& block) const;
    float GetFireboyLeftEdge(const glm::vec2& bodyPos) const;
    float GetFireboyRightEdge(const glm::vec2& bodyPos) const;
    float GetFireboyHeadTop(const glm::vec2& bodyPos) const;
    float GetFireboyBodyBottom(const glm::vec2& bodyPos) const;
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
    std::shared_ptr<Character> m_Diamond;

    // ------------------------------------------------------------------------
    // Physics parameters
    // ------------------------------------------------------------------------
    glm::vec2 m_FireboyVelocity = {0.0f, 0.0f};
    bool m_FireboyOnGround = false;

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
    glm::vec2 m_FireboyBodyHitboxSize = {30.0f, 50.0f};
    glm::vec2 m_FireboyBodyHitboxOffset = {0.0f, -2.0f};
    glm::vec2 m_FireboyHeadHitboxSize = {28.0f, 34.0f};
    glm::vec2 m_FireboyHeadHitboxOffset = {0.0f, 40.0f};
    glm::vec2 m_DiamondHitboxSize = {16.0f, 16.0f};
    glm::vec2 m_FireboyBodyHitboxScale = {0.72f, 0.90f};
    glm::vec2 m_FireboyHeadHitboxScale = {0.60f, 0.60f};
    glm::vec2 m_FireboyBodyHitboxPadding = {0.0f, 0.0f};
    glm::vec2 m_FireboyHeadHitboxPadding = {0.0f, 0.0f};

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
};

#endif // APP_HPP
