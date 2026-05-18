set(_ptsd_root "${CMAKE_CURRENT_SOURCE_DIR}/PTSD")

function(_patch_file_replace _path _from _to)
    if(NOT EXISTS "${_path}")
        return()
    endif()

    file(READ "${_path}" _content)
    string(FIND "${_content}" "${_to}" _already_patched)
    if(NOT _already_patched EQUAL -1)
        return()
    endif()

    string(FIND "${_content}" "${_from}" _found_at)
    if(_found_at EQUAL -1)
        message(FATAL_ERROR "Patch anchor not found in ${_path}")
    endif()

    string(REPLACE "${_from}" "${_to}" _patched "${_content}")
    file(WRITE "${_path}" "${_patched}")
endfunction()

function(_patch_file_insert_after _path _anchor _snippet)
    if(NOT EXISTS "${_path}")
        return()
    endif()

    file(READ "${_path}" _content)
    if("${_snippet}" MATCHES "SetSize" AND _content MATCHES "void SetSize\\(const glm::vec2")
        return()
    endif()
    string(FIND "${_content}" "${_snippet}" _already_present)
    if(NOT _already_present EQUAL -1)
        return()
    endif()

    string(FIND "${_content}" "${_anchor}" _anchor_pos)
    if(_anchor_pos EQUAL -1)
        message(FATAL_ERROR "Patch anchor not found in ${_path}")
    endif()

    string(REPLACE "${_anchor}" "${_anchor}${_snippet}" _patched "${_content}")
    file(WRITE "${_path}" "${_patched}")
endfunction()

_patch_file_replace(
    "${_ptsd_root}/lib/spdlog/include/spdlog/fmt/bundled/format.h"
    "#if defined(_SECURE_SCL) && _SECURE_SCL"
    "#if defined(_SECURE_SCL) && _SECURE_SCL && (!defined(FMT_MSC_VER) || FMT_MSC_VER < 1950)"
)

_patch_file_insert_after(
    "${_ptsd_root}/include/Util/Image.hpp"
    "    glm::vec2 GetSize() const override { return m_Size; };\n"
    "\n    /**\n     * @brief Overrides the drawable size used for rendering.\n     *\n     * This keeps the image API compatible with gameplay code that authors\n     * sprites in world-space dimensions rather than raw pixel size.\n     *\n     * @param size Desired drawable size.\n     */\n    void SetSize(const glm::vec2& size);\n"
)

_patch_file_insert_after(
    "${_ptsd_root}/include/Util/Animation.hpp"
    "    glm::vec2 GetSize() const override { return m_Frames[m_Index]->GetSize(); }\n"
    "\n    /**\n     * @brief Overrides the size of every animation frame.\n     *\n     * @param size Desired drawable size.\n     */\n    void SetSize(const glm::vec2& size);\n"
)
