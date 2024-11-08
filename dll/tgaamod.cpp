#include <spdlog/spdlog.h>
#include <memory>

#include "tgaamod.h"
#include "hook.h"
#include "module.h"
#include "mt/mtdti.h"
#include "mt/mtobject.h"
#include "mt/mtproperty.h"

ModuleDesc *g_module_desc = nullptr;
TGAAMod *TGAAMod::s_tgaa_mod = nullptr;

TGAAMod::TGAAMod() : m_sSkeletonMain_constructor_hook(&sSkeletonMain_constructor_replacement), m_overlay(nullptr)

{
}

TGAAMod::~TGAAMod()
{
}

void TGAAMod::create()
{
    if (!s_tgaa_mod)
    {
        s_tgaa_mod = new TGAAMod();
    }
    else
    {
        spdlog::error("TGAAMod instance has already been created, not recreating");
    }
}

void *TGAAMod::sSkeletonMain_constructor_replacement(void *original_constructor(MtObject *), MtObject *_this)
{
    void *return_value = original_constructor(_this);

    const std::unique_ptr<MtPropertyList> prop_list = _this->create_property();

    spdlog::info("Build Version is {}", prop_list->find_property("mBuildVersion")->get_cstring());

    ModuleDesc *module_desc = g_module_desc;
    while (module_desc != nullptr)
    {
        s_tgaa_mod->m_modules.emplace_back(module_desc->create(_this));

        module_desc = module_desc->m_next;
    }

    s_tgaa_mod->m_overlay = std::make_unique<Overlay>(_this);

    return return_value;
}