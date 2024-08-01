#pragma once
#include <memory>

#include "hook.h"
#include "overlay.h"
#include "module.h"
#include "mt/mtobject.h"

class TGAAMod
{
    TGAAMod();
    ~TGAAMod();

public:
    static void create();

private:
    static void *sSkeletonMain_constructor_replacement(void *original_constructor(MtObject *), MtObject *_this);
    StaticFunctionHook<0x140057530, void *, MtObject *> m_sSkeletonMain_constructor_hook;

    std::vector<std::unique_ptr<Module>> m_modules;
    std::unique_ptr<Overlay> m_overlay;

    static TGAAMod *s_tgaa_mod;
    friend class Overlay; // Overlay needs the list of modules to call draw()
};