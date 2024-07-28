#include <windows.h>
#include <string>
#include <iostream>

void setup_console() {
    AllocConsole();

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

    PWSTR thread_desc = NULL;
    HRESULT hr = GetThreadDescription(GetCurrentThread(), &thread_desc);
    if (FAILED(hr))
    {
        std::string msg = "Couldn't get thread description" + std::to_string(hr);
        MessageBoxA(NULL, msg.c_str(), NULL, MB_OK);

        ExitProcess(1);
    }

    printf("Main thread is: %ls\n", thread_desc);

    LocalFree(thread_desc);
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