#include <spdlog/spdlog.h>
#include <memory>

#include "tgaamod.h"
#include "hook.h"
#include "mt/mtdti.h"
#include "mt/mtobject.h"
#include "mt/mtproperty.h"

TGAAMod *TGAAMod::s_tgaa_mod = nullptr;

TGAAMod::TGAAMod() : m_sSkeletonMain_constructor_hook(&sSkeletonMain_constructor_replacement)

{

    spdlog::info("TGAAMod has been setup!");
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

    spdlog::info("sSkeletonMain constructor called: name {}", _this->get_dti()->name());
    const std::unique_ptr<MtPropertyList> prop_list = MtDTI::from_hash(0x2a031018)->new_instance_owned<MtPropertyList>();
    _this->create_property(prop_list.get());

    MtProperty *prop = prop_list->elements();
    while (prop != nullptr)
    {
        std::string_view prop_name = {prop->name()};
        if (prop_name == "mBuildVersion")
        {
            spdlog::info("Build Version is {}", prop->get_cstring());
        }

        prop = prop->prev();
    }

    return return_value;
}