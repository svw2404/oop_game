#include "App.hpp"

#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");

    m_Background = std::make_shared<BackgroundImage>(
        GA_RESOURCE_DIR "/Image/Background/background0.png"
    );
    m_Background->SetPosition({ 0.0f, 0.0f });
    m_Root.AddChild(m_Background);



    m_Fireboy = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Character/Fireboy/Idle/fireboy_idle_00.png"
    );
    m_Fireboy->SetSize({ 120.0f, 120.0f });
    m_Fireboy->SetPosition({ -300.0f, -220.0f });
    m_Fireboy->SetZIndex(10);
    m_Root.AddChild(m_Fireboy);

    m_FireboyVelocity = { 0.0f, 0.0f };
    m_FireboyOnGround = true;



    m_Diamond = std::make_shared<Character>(
        GA_RESOURCE_DIR "/Image/Character/RedDiamonds.png"
    );

    m_Diamond->SetPosition({ 100.0f, -220.0f });
    m_Diamond->SetZIndex(5);

    m_Root.AddChild(m_Diamond);

    m_CurrentState = State::UPDATE;
}