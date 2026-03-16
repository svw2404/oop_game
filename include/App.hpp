#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Util/Renderer.hpp"
#include "Character.hpp"
#include "BackgroundImage.hpp"

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
    void CheckDiamondCollection();
    void HandleFireboyInput();
    void UpdateFireboyPhysics();

private:
    State m_CurrentState = State::START;

    Util::Renderer m_Root;
        
    std::shared_ptr<BackgroundImage> m_Background;
    std::shared_ptr<Character> m_Fireboy;
    std::shared_ptr<Character> m_Diamond;

    glm::vec2 m_FireboyVelocity = { 0.0f, 0.0f };
    bool m_FireboyOnGround = false;

    float m_MoveSpeed = 4.0f;
    float m_JumpSpeed = 14.0f;
    float m_Gravity = 0.7f;
    float m_GroundY = -220.0f;
};

#endif