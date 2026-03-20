#include "Character.hpp"
#include "Util/Image.hpp"
#include "Util/Animation.hpp"

void Character::SetSize(const glm::vec2& size) {
    m_Size = size;

    if (auto img = std::dynamic_pointer_cast<Util::Image>(m_Drawable)) {
        img->SetSize(size);
        return;
    }

    if (auto anim = std::dynamic_pointer_cast<Util::Animation>(m_Drawable)) {
        anim->SetSize(size);
        return;
    }
}