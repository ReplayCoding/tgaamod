#include <string>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <fmt/format.h>

#include "mt/mtdti.h"
#include "mt/mtobject.h"
#include "mt/mtproperty.h"
#include "module.h"

class FpsModule : public Module
{
public:
    FpsModule(MtObject *skeleton_main) : m_skeleton_main(skeleton_main) {};
    virtual ~FpsModule() {};

    virtual void draw() override
    {
        ImGui::Begin("FPS Counter", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        const std::unique_ptr<MtPropertyList> prop_list = m_skeleton_main->create_property();

        std::string actual_fps = fmt::format("FPS: {}", prop_list->find_property("mActualFps")->get_f32());
        std::string build_version = fmt::format("Build Version: {}", prop_list->find_property("mBuildVersion")->get_cstring());
        ImGui::Text(actual_fps.c_str());
        ImGui::Text(build_version.c_str());
        ImGui::End();
    }

private:
    MtObject *m_skeleton_main;
};

REGISTER_MODULE(FpsModule);