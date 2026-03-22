#include "Character.hpp"

#include "Util/Image.hpp"

Character::Character(const std::string& imagePath, bool skipSetImage) {
    if (!skipSetImage && !imagePath.empty()) {
        SetImage(imagePath);
    }
    ResetPosition();
}

void Character::SetImage(const std::string& imagePath) {
    m_ImagePath = imagePath;
    m_Drawable = std::make_shared<Util::Image>(m_ImagePath);

    if (auto image = std::dynamic_pointer_cast<Util::Image>(m_Drawable)) {
        image->SetSize(m_Size);
    }
}

void Character::SetSize(const glm::vec2& size) {
    m_Size = size;

    if (auto image = std::dynamic_pointer_cast<Util::Image>(m_Drawable)) {
        image->SetSize(size);
    }
}
