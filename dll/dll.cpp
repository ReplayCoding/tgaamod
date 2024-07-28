#include <windows.h>
#include <string>
#include <iostream>

void setup_console() {
    AllocConsole();

    // Stupid
    FILE *fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);

    std::cout.clear();
    std::cin.clear();
    std::cerr.clear();
}

void on_dll_attach()
{
    setup_console();

    PWSTR thread_desc_ptr = NULL;
    HRESULT hr = GetThreadDescription(GetCurrentThread(), &thread_desc_ptr);
    if (FAILED(hr))
    {
        std::string msg = "Couldn't get thread description" + std::to_string(hr);
        MessageBoxA(NULL, msg.c_str(), NULL, MB_OK);

        ExitProcess(1);
    }

    std::wstring thread_desc = std::wstring(thread_desc_ptr);
    LocalFree(thread_desc_ptr);

    int main_thread_id = std::stoi(thread_desc);
    printf("Main thread is: %d\n", main_thread_id);

    HANDLE main_thread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, main_thread_id);
    ResumeThread(main_thread);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        on_dll_attach();
        break;
    }
    default:
    {
        break;
    }
    }

    return true;
}