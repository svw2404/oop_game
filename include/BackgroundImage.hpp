#ifndef BACKGROUND_IMAGE_HPP
#define BACKGROUND_IMAGE_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class BackgroundImage : public Util::GameObject {

public:
    BackgroundImage() : GameObject(
        std::make_unique<Util::Image>(GA_RESOURCE_DIR"/Image/Background/background0.png"), -10) {

        Util::Image* image = dynamic_cast<Util::Image*>(m_Drawable.get());

        if (image) {
            image->SetSize(glm::vec2(192.0f, 108.0f));
        }
    }

    void NextPhase(const int phase) {
        auto temp = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
        temp->SetImage(ImagePath(phase));
    }

    void SetSize(float widthScale, float heightScale) {
        m_Transform.scale = glm::vec2(widthScale, heightScale);
    }

private:
    inline std::string ImagePath(const int phase) {
        return GA_RESOURCE_DIR"/Image/Background/phase" + std::to_string(phase) + ".png";
    }
};

#endif //BACKGROUND_IMAGE_HPP
