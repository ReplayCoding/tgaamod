#include <spdlog/spdlog.h>

#include "tgaamod.h"
#include "hook.h"

static TGAAMod *s_tgaa_mod = nullptr;

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

void *TGAAMod::sSkeletonMain_constructor_replacement(void *original_constructor(void *), void *_this)
{
    void* return_value = original_constructor(_this);

    spdlog::info("sSkeletonMain constructor called: vtable {}", *reinterpret_cast<void**>(_this));

    return return_value;
}