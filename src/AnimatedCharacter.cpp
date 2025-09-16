#include "AnimatedCharacter.hpp"
#include <iostream>

AnimatedCharacter::AnimatedCharacter(const std::vector<std::string>& AnimationPaths)
    : Character(AnimationPaths[0], true)
{
    m_Drawable = std::make_shared<Util::Animation>(AnimationPaths, false, 500, false, 0);
    std::cout << "[Construct] this: " << this
        << ", m_Drawable: " << m_Drawable.get() << std::endl;
}


#include <iostream>
#include <typeinfo>

bool AnimatedCharacter::IfAnimationEnds() const {
    auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
    if (!animation) {
        return false;
    }

    return animation->GetCurrentFrameIndex() == animation->GetFrameCount() - 1;
}



void AnimatedCharacter::Update() {

}

void AnimatedCharacter::Play() {
    auto anim = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
    if (anim) anim->Play();
}

