#include <windows.h> // detours requires windows.h, this is stupid
#include <detours/detours.h>
#include <stdexcept>
#include <fmt/format.h>

#include "hook.h"

void hook_function_internal(void **address, void *replacement)
{
    const void *original_address = address;

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    LONG result = DetourAttach(address, replacement);
    if (result != NO_ERROR)
    {
        // Leak the message so we can format it
        std::string *msg = new std::string(fmt::format("Error attaching to address {:016x}: {}", reinterpret_cast<uintptr_t>(original_address), result));
        throw std::runtime_error(msg->c_str());
    }

    DetourTransactionCommit();
};

void unhook_function_internal(void **address, void *replacement)
{
    const void *original_address = address;

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    LONG result = DetourDetach(address, replacement);
    if (result != NO_ERROR)
    {
        // Leak the message so we can format it
        std::string *msg = new std::string(fmt::format("Error detaching from address {:016x}: {}", reinterpret_cast<uintptr_t>(original_address), result));
        throw std::runtime_error(msg->c_str());
    }

    DetourTransactionCommit();
};