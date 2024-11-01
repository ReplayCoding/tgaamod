#pragma once
#include <memory>

#include "mt/mtobject.h"

// Don't include the whole header
class Overlay;

class Module
{
public:
    Module() = default;
    virtual ~Module() {};

    virtual void draw() = 0;

    bool m_should_draw = true;
    char *m_module_name = "If you see this, there's a bug";
};

class ModuleDesc;
extern ModuleDesc *g_module_desc; // initialized in tgaamod.cpp

class ModuleDesc
{
public:
    ModuleDesc()
    {
        m_next = g_module_desc;
        g_module_desc = this;
    };
    virtual std::unique_ptr<Module> create(MtObject *skeleton_main) = 0;

    ModuleDesc *m_next;
};

#define REGISTER_MODULE(class_)                                                  \
    class ModuleDesc##class_ : public ModuleDesc                                 \
    {                                                                            \
    public:                                                                      \
        virtual std::unique_ptr<Module> create(MtObject *skeleton_main) override \
        {                                                                        \
            auto module = std::make_unique<class_>(skeleton_main);               \
            module->m_module_name = #class_;                                     \
            return module;                                                       \
        };                                                                       \
    };                                                                           \
    ModuleDesc##class_ module_desc_##class_ = {};