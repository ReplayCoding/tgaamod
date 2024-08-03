#include <fmt/format.h>
#include <memory>
#include <string>
#include <imgui.h>

#include "mt/mtproperty.h"
#include "mt/mtobject.h"
#include "mt/mtdti.h"
#include "widgets/proplist.hpp"
#include "module.h"

class ScriptInspectorModule : public Module
{
public:
    ScriptInspectorModule(MtObject *skeleton_main)
    {
        m_scenario = skeleton_main->create_property()->find_property("mpScenario")->get_classref();
    }

    virtual void draw() override
    {
        ImGui::Begin("Scenario");

        std::unique_ptr<MtPropertyList> sscenario_prop_list = m_scenario->create_property();
        draw_prop_list(sscenario_prop_list.get());

        ImGui::End();

        const ptrdiff_t SCRIPT_PLAYERS_OFFSET = 0x80;
        const ptrdiff_t SCRIPT_PLAYERS_CONTAINER_SIZE = 0x20;
        for (int player_index = 0; player_index < 5; player_index++)
        {
            MtObject *script_player =
                *reinterpret_cast<MtObject **>(
                    reinterpret_cast<uintptr_t>(m_scenario) + SCRIPT_PLAYERS_OFFSET + (player_index * SCRIPT_PLAYERS_CONTAINER_SIZE));

            if (script_player == nullptr)
            {
                continue;
            }

            std::string window_name = fmt::format("{} ({})", script_player->get_dti()->name(), player_index);
            std::unique_ptr<MtPropertyList> player_prop_list = script_player->create_property();

            // Is active flag
            if ((player_prop_list->find_property("mFlag[0]")->get_u32() & 1) == 0)
            {
                continue;
            }

            ImGui::Begin(window_name.c_str());
            draw_prop_list(player_prop_list.get());
            ImGui::End();
        };
    };

private:
    MtObject *m_scenario;
};

REGISTER_MODULE(ScriptInspectorModule)