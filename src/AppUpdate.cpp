#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

void App::HandleFireboyInput() {
    glm::vec2 pos = m_Fireboy->GetPosition();

    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        pos.x -= m_MoveSpeed;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        pos.x += m_MoveSpeed;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::W) && m_FireboyOnGround) {
        m_FireboyVelocity.y = m_JumpSpeed;
        m_FireboyOnGround = false;
    }

    m_Fireboy->SetPosition(pos);
}

void App::UpdateFireboyPhysics() {
    glm::vec2 pos = m_Fireboy->GetPosition();

    m_FireboyVelocity.y -= m_Gravity;
    pos.y += m_FireboyVelocity.y;

    if (pos.y <= m_GroundY) {
        pos.y = m_GroundY;
        m_FireboyVelocity.y = 0.0f;
        m_FireboyOnGround = true;
    }

    m_Fireboy->SetPosition(pos);
}

void App::CheckDiamondCollection() {
    if (m_Diamond && m_Diamond->GetVisibility() && m_Fireboy->IfCollides(m_Diamond)) {
        m_Diamond->SetVisible(false);
    }
}

void App::Update() {
    HandleFireboyInput();
    UpdateFireboyPhysics();
    CheckDiamondCollection();

    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update();
}