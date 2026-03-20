#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Util/Renderer.hpp"
#include "Character.hpp"
#include "BackgroundImage.hpp"
#include "HeadBodyCharacter.hpp"

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End();

private:
    void ValidTask();

    void HandleFireboyInput();
    void UpdateFireboyPhysics();
    void CheckDiamondCollection();

    struct RectObject {
        glm::vec2 center = { 0.0f, 0.0f };
        glm::vec2 size = { 0.0f, 0.0f };
    };

    RectObject ImageRectToWorldRect(float x1, float y1, float x2, float y2) const;
    bool CheckAABB(
        const glm::vec2& centerA, const glm::vec2& sizeA,
        const glm::vec2& centerB, const glm::vec2& sizeB
    ) const;

    void ResolveHorizontalCollisions(const glm::vec2& oldPos, glm::vec2& newPos);
    void ResolveVerticalCollisions(const glm::vec2& oldPos, glm::vec2& newPos);

private:
    State m_CurrentState = State::START;

    Util::Renderer m_Root;

    std::shared_ptr<BackgroundImage> m_Background;
    std::shared_ptr<HeadBodyCharacter> m_Fireboy;
    std::shared_ptr<Character> m_Diamond;

    glm::vec2 m_FireboyVelocity = { 0.0f, 0.0f };
    bool m_FireboyOnGround = false;

    float m_MoveSpeed = 4.0f;
    float m_JumpSpeed = 14.0f;
    float m_Gravity = 0.7f;

    // Gameplay hitboxes (not image size)
    glm::vec2 m_FireboyHitboxSize = { 25.0f, 30.0f };
    glm::vec2 m_DiamondHitboxSize = { 16.0f, 16.0f };

    // Background image sizes
    glm::vec2 m_BackgroundOriginalSize = { 2380.0f, 1760.0f };
    glm::vec2 m_BackgroundDisplayedSize = { 1244.16f, 699.84f };

    // Solid collision rectangles
    std::vector<RectObject> m_SolidBlocks;

    // One mapped platform block
    RectObject m_TestBlock;
};

#endif
