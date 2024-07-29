#include <windows.h>
#include <string>
#include <cstdio>
#include "spdlog/spdlog.h"

// TODO: don't hardcode paths
const char *GAME_CWD = "Z:\\TGAAC-for-research\\";
const char *GAME_PATH = "Z:\\TGAAC-for-research\\TGAAC.exe.unpacked-for-renderdoc.exe";
const char *DLL_NAME = "C:\\Users\\User\\Desktop\\tgaamod\\build\\dll\\Debug\\tgaamod.dll";

bool inject_dll_to_process(const char *dll_to_inject, HANDLE process, HANDLE main_thread)
{
    // Suspend main thread so we our DLL can setup before the application runs
    SuspendThread(main_thread);

    HMODULE kernel32_lib = LoadLibraryA("kernel32.dll");
    // Apparently kernel32.dll should have the same base address across different processes, so this will work fine.
    LPVOID address_of_LoadLibraryA = GetProcAddress(kernel32_lib, "LoadLibraryA");

    LPVOID remote_dll_name = VirtualAllocEx(process, nullptr, strlen(dll_to_inject) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (remote_dll_name == NULL)
    {
        spdlog::error("Couldn't allocate DLL name string in remote process: {}", GetLastError());
        return false;
    }

    if (WriteProcessMemory(process, remote_dll_name, dll_to_inject, strlen(dll_to_inject) + 1, NULL) == 0)
    {
        spdlog::error("Couldn't write DLL name string to remote process: {}", GetLastError());
        return false;
    };

    HANDLE remote_thread = CreateRemoteThread(process, nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(address_of_LoadLibraryA), remote_dll_name, CREATE_SUSPENDED, nullptr);
    if (remote_thread == NULL)
    {
        spdlog::error("Couldn't create thread in remote process: {}", GetLastError());
        return false;
    }

    // Pass in the main thread id, so the DLL can resume it after setup
    DWORD remote_main_thread_id = GetThreadId(main_thread);
    std::wstring remote_thread_description = std::to_wstring(remote_main_thread_id);
    HRESULT hr = SetThreadDescription(remote_thread, remote_thread_description.c_str());
    if (FAILED(hr))
    {
        spdlog::error("Couldn't set remote thread description: {}", hr);
        return false;
    }

    if (ResumeThread(remote_thread) == -1)
    {
        spdlog::error("Couldn't resume remote thread: {}", GetLastError());
        return false;
    };

    return true;
}

int main()
{
    STARTUPINFOA startup_info = {0};
    PROCESS_INFORMATION process_info = {0};

    if (!CreateProcessA(GAME_PATH, nullptr, 0, 0, FALSE, DEBUG_ONLY_THIS_PROCESS | DETACHED_PROCESS, nullptr, GAME_CWD, &startup_info, &process_info))
    {
        spdlog::error("Failed to create process!\n");
        return 1;
    };

    spdlog::info("Created process: {}\n", process_info.dwProcessId);

    while (true)
    {
        DEBUG_EVENT debug_event = {0};
        WaitForDebugEvent(&debug_event, INFINITE);

        switch (debug_event.dwDebugEventCode)
        {
        case EXIT_PROCESS_DEBUG_EVENT:
        {
            spdlog::info("Exiting process, goodbye!");

            return 0;
            break;
        }

        case CREATE_PROCESS_DEBUG_EVENT:
        {
            CREATE_PROCESS_DEBUG_INFO *create_info = &debug_event.u.CreateProcessInfo;
            spdlog::info("Process launch event [baddr: {} thread: {}]\n", create_info->lpBaseOfImage, GetThreadId(create_info->hThread));

            if (!inject_dll_to_process(DLL_NAME, create_info->hProcess, create_info->hThread))
            {
                spdlog::error("Failed to inject DLL into process, killing\n");
                TerminateProcess(process_info.hProcess, 0);

                // Let us handle the exit process event so we get a nice exit message (stupid)
                break;
            };

            DebugActiveProcessStop(process_info.dwProcessId);
            return 0;
        }

        default:
        {
            break;
        }
        };

        ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, DBG_CONTINUE);
    }

    return 0;
}