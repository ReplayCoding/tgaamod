#include <imgui.h>
#include <fmt/format.h>

#include "mt/mtproperty.h"
#include "widgets/proplist.hpp"

void draw_prop_list(MtPropertyList *object)
{
    MtProperty *current_prop = object->elements();
    while (current_prop != nullptr)
    {
        std::string formatted_text = fmt::format("{}: {}", current_prop->name(), current_prop->formatted_value());
        if (current_prop->type() == static_cast<uint32_t>(MtProperty::PropType::COLOR))
        {
            MtColor color = current_prop->get_color();
            auto converted_color = ImVec4(
                static_cast<float>(color.m_r) / 255.f,
                static_cast<float>(color.m_g) / 255.f,
                static_cast<float>(color.m_b) / 255.f, 1.0);
            ImGui::TextColored(converted_color, formatted_text.c_str());
        }
        else
        {
            ImGui::Text(formatted_text.c_str());
        };

        if (current_prop->type() == static_cast<uint32_t>(MtProperty::PropType::CLASSREF))
        {
            MtObject *classref = current_prop->get_classref();
            if (classref != nullptr)
            {
                std::unique_ptr<MtPropertyList> prop_list = classref->create_property();
                if (ImGui::TreeNode(current_prop->name()))
                {
                    draw_prop_list(prop_list.get());
                    ImGui::TreePop();
                }
            }
        }

        current_prop = current_prop->prev();
    }
}
