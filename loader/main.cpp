#include <windows.h>
#include <cstdio>

// TODO: don't hardcode paths
const char *GAME_CWD = "Z:\\TGAAC-for-research\\";
const char *GAME_PATH = "Z:\\TGAAC-for-research\\TGAAC.exe.unpacked-for-renderdoc.exe";
const char *DLL_NAME = "C:\\Users\\User\\Desktop\\tgaamod\\build\\dll\\Debug\\tgaamod.dll";

int main()
{
    STARTUPINFOA startup_info = {0};
    PROCESS_INFORMATION process_info = {0};

    if (!CreateProcessA(GAME_PATH, nullptr, 0, 0, FALSE, DEBUG_ONLY_THIS_PROCESS | DETACHED_PROCESS, nullptr, GAME_CWD, &startup_info, &process_info))
    {
        printf("Failed to create process!\n");
        return 1;
    };

    printf("Created process: %d\n", process_info.dwProcessId);

    while (true)
    {
        DEBUG_EVENT debug_event = {0};
        WaitForDebugEvent(&debug_event, INFINITE);

        switch (debug_event.dwDebugEventCode)
        {
        case EXIT_PROCESS_DEBUG_EVENT:
        {
            printf("Exiting process, goodbye!");

            return 0;
            break;
        }
        case CREATE_PROCESS_DEBUG_EVENT:
        {
            // TODO
            break;
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