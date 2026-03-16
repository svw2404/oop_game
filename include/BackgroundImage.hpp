#ifndef BACKGROUND_IMAGE_HPP
#define BACKGROUND_IMAGE_HPP

#include <string>

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class BackgroundImage : public Util::GameObject {
public:
    explicit BackgroundImage(
        const std::string& imagePath = GA_RESOURCE_DIR "/Image/Background/background0.png"
    )
        : Util::GameObject(std::make_unique<Util::Image>(imagePath), -10) {
        auto* image = dynamic_cast<Util::Image*>(m_Drawable.get());
        if (image) {
            image->SetSize(glm::vec2(1244.16f, 699.84f));
        }
    }

    void NextPhase(int phase) {
        auto* image = dynamic_cast<Util::Image*>(m_Drawable.get());
        if (image) {
            image->SetImage(ImagePath(phase));
        }
    }

    void SetImage(const std::string& imagePath) {
        auto* image = dynamic_cast<Util::Image*>(m_Drawable.get());
        if (image) {
            image->SetImage(imagePath);
        }
    }

    void SetImageSize(const glm::vec2& size) {
        auto* image = dynamic_cast<Util::Image*>(m_Drawable.get());
        if (image) {
            image->SetSize(size);
        }
    }

    void SetPosition(const glm::vec2& pos) {
        m_Transform.translation = pos;
    }

private:
    std::string ImagePath(int phase) {
        return GA_RESOURCE_DIR "/Image/Background/phase" + std::to_string(phase) + ".png";
    }
};

#endif // BACKGROUND_IMAGE_HPP