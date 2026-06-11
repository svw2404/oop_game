#include "Util/Animation.hpp"

namespace Util {
void Animation::SetSize(const glm::vec2& size) {
    for (auto& frame : m_Frames) {
        if (frame) {
            frame->SetSize(size);
        }
    }
}
} // namespace Util
