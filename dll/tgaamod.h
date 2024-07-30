#pragma once
#include "hook.h"
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

    static TGAAMod *s_tgaa_mod;
};