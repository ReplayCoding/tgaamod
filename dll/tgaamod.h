#pragma once
#include "hook.h"

class TGAAMod {
    TGAAMod();
    ~TGAAMod();

public:
    static void create();

private:
    static void* sSkeletonMain_constructor_replacement(void* original_constructor(void*), void* _this);

    FunctionHook<0x140057530, void*, void*> m_sSkeletonMain_constructor_hook;
};