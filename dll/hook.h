#pragma once
#include <cstdint>

void hook_function_internal(void **address, void *replacement);
void unhook_function_internal(void **address, void *replacement);

// Generic function hook
// TODO: We don't want to have multiple hooks on the same address, so keep track of if a hook is active
template <uintptr_t address, typename ReturnType, typename... Args>
class StaticFunctionHook
{
public:
    using OriginalFunctionType = ReturnType(Args...);
    using ReplacementFunctionType = ReturnType(OriginalFunctionType *original_func, Args...);

    inline StaticFunctionHook(ReplacementFunctionType *replacement)
    {
        s_original_func = reinterpret_cast<OriginalFunctionType*>(address);
        s_replacement = replacement;

        hook_function_internal(reinterpret_cast<void**>(&s_original_func), replacement_func);
    };
    inline ~StaticFunctionHook()
    {
        unhook_function_internal(reinterpret_cast<void**>(&s_original_func), replacement_func);
    };

private:
    static inline ReturnType replacement_func(Args... args)
    {
        return s_replacement(s_original_func, args...);
    }

    static inline OriginalFunctionType *s_original_func = nullptr;
    static inline ReplacementFunctionType *s_replacement = nullptr;
};

// index is needed to ensure that each hook gets different statics
template <uintptr_t index, typename ReturnType, typename... Args>
class RuntimeAddressFunctionHook
{
public:
    using OriginalFunctionType = ReturnType(Args...);
    using ReplacementFunctionType = ReturnType(OriginalFunctionType *original_func, Args...);

    inline RuntimeAddressFunctionHook(uintptr_t address, ReplacementFunctionType *replacement)
    {
        s_original_func = reinterpret_cast<OriginalFunctionType*>(address);
        s_replacement = replacement;

        hook_function_internal(reinterpret_cast<void**>(&s_original_func), replacement_func);
    };
    inline ~RuntimeAddressFunctionHook()
    {
        unhook_function_internal(reinterpret_cast<void**>(&s_original_func), replacement_func);
    };

private:
    static inline ReturnType replacement_func(Args... args)
    {
        return s_replacement(s_original_func, args...);
    }

    static inline OriginalFunctionType *s_original_func = nullptr;
    static inline ReplacementFunctionType *s_replacement = nullptr;
};