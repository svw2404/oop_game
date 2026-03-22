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
    SolidRect ImageRectToWorldRect(float x1, float y1, float x2, float y2) const;
    glm::vec2 ImagePointToWorldPoint(float x, float y) const;

    bool CheckAABB(
        const glm::vec2& centerA, const glm::vec2& sizeA,
        const glm::vec2& centerB, const glm::vec2& sizeB
    ) const;

    void ResolveHorizontalCollisions(const glm::vec2& oldPos, glm::vec2& newPos);
    void ResolveVerticalCollisions(const glm::vec2& oldPos, glm::vec2& newPos);
    bool ResolveSlopeGrounding(const glm::vec2& oldPos, glm::vec2& newPos);
    bool TrySnapToSlopeTopTransition(
        const glm::vec2& oldPos,
        glm::vec2& newPos,
        const SolidRect& blockingBlock
    ) const;
    bool FindBestSlopeYAtX(const glm::vec2& oldPos, float desiredX, float& outSlopeY) const;
    void ApplySlopeFollow(const glm::vec2& oldPos, glm::vec2& newPos) const;
    bool FindNearbyGroundY(const glm::vec2& pos, float maxDistance, float& outGroundY) const;

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

    float m_MoveSpeed = 4.0f;
    float m_JumpSpeed = 14.0f;
    float m_Gravity = 0.7f;
    float m_GroundSnapTolerance = 4.0f;
    float m_GroundStickTolerance = 10.0f;
    float m_SlopeSnapTolerance = 8.0f;
    float m_StepUpHeight = 12.0f;
    float m_SlopeTopTransitionHeight = 28.0f;
    float m_SlopeFollowTolerance = 20.0f;
    float m_SlopeTopTransitionWidth = 18.0f;

    // ------------------------------------------------------------------------
    // Gameplay sizes
    // ------------------------------------------------------------------------
    glm::vec2 m_FireboyHitboxSize = {25.0f, 30.0f};
    glm::vec2 m_DiamondHitboxSize = {16.0f, 16.0f};

    glm::vec2 m_BackgroundOriginalSize = {2380.0f, 1760.0f};
    glm::vec2 m_BackgroundDisplayedSize = {1244.16f, 699.84f};

    // ------------------------------------------------------------------------
    // Collision geometry
    // ------------------------------------------------------------------------
    std::vector<SolidRect> m_SolidBlocks;
    std::vector<SlopeSurface> m_Slopes;
    std::vector<HazardRect> m_Hazards;
    SolidRect m_TestBlock;
};

#endif // APP_HPP
