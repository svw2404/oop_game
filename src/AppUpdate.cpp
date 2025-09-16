#include "App.hpp"
#include "AnimatedCharacter.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

void App::Update() {

    // TODO: Add your own logics to finish the tasks in README.md
    
    /* = = = 長頸鹿移動 = = = */
    const float speed = 2.5f;
    glm::vec2 pos = m_Giraffe->GetPosition();

    if (Util::Input::IsKeyPressed(Util::Keycode::W)) {
        pos.y += speed;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::S)) {
        pos.y -= speed;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        pos.x -= speed;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        pos.x += speed;
    }

    m_Giraffe->SetPosition(pos);

    /* = = = 長頸鹿碰撞 = = = */
    if (m_Giraffe->IfCollides(m_Chest)) {
        m_Chest->SetVisible(false);
    }

    /* = = = 蜜蜂動畫 = = = */
    if (m_Bee->GetVisibility()) {
        m_Bee->Update();
    }

    /* = = = 開門 = = = */
    for (auto& door : m_Doors) {
        if (door->GetVisibility() && m_Giraffe->IfCollides(door)) {
            door->SetImage(GA_RESOURCE_DIR "/Image/Character/door_open.png");
        }
    }

	/* = = = 計時球 = = = */
    if (m_Phase == Phase::COUNTDOWN) {
        m_Ball->SetVisible(true);
        if (m_CountFrame == 0) {
            m_Ball->Play();
        }
        m_Ball->Update();
        m_CountFrame++;
    }




    /*
     *  Do not touch the code below as they serve the purpose for validating the tasks,
     *  rendering the frame, and exiting the game.
    */

    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    if (m_EnterDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::RETURN)) {
            ValidTask();
        }
    }

    

    m_EnterDown = Util::Input::IsKeyPressed(Util::Keycode::RETURN);

    m_Root.Update();
}
