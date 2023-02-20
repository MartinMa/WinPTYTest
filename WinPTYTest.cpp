#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <Shlobj_core.h>
#include <pathcch.h>
#include "winpty.h"

void printLastError(const wchar_t* format)
{
    wchar_t message[1024];
    _stprintf_s(message, 1024, format, GetLastError());
    OutputDebugString(message);
}

void outputLogMessage(const wchar_t* message) {
    // TODO Write log messages to file for Release build.
    OutputDebugString(message);
}

int main()
{
    // Determine full path of cmd.exe
    wchar_t cmdPath[MAX_PATH];
    if (FAILED(
        SHGetFolderPath(
            NULL,
            CSIDL_SYSTEM,
            NULL,
            0,
            cmdPath)
    )) {
        printLastError(_T("SHGetFolderPath failed with %d\r\n"));
        return 1;
    }
    else {
        if (FAILED(PathCchAppend(cmdPath, MAX_PATH, _T("cmd.exe")))) {
            printLastError(_T("PathCchAppend failed with %d\r\n"));
            return 1;
        }
    }

    std::wcout << "Path of cmd.exe: " << cmdPath << std::endl;

    winpty_error_ptr_t configError;
    // No specific flags.
    winpty_config_t* config = winpty_config_new(0, &configError);

    if (config == nullptr) {
        outputLogMessage(_T("winpty_config_new failed"));
        return 1;
    }

    winpty_config_set_initial_size(config, 80, 25);
    winpty_config_set_mouse_mode(config, WINPTY_MOUSE_MODE_AUTO);
    winpty_config_set_agent_timeout(config, 1000);

    winpty_error_ptr_t openError;
    winpty_t* winpty = winpty_open(config, &openError);
    winpty_config_free(config);

    if (winpty == nullptr) {
        outputLogMessage(_T("winpty_open failed"));
        return 1;
    }

    std::wcout << "winpty_open has been successfull" << std::endl;

    HANDLE agentProcess = winpty_agent_process(winpty);
    LPCWSTR coninPipeName = winpty_conin_name(winpty);
    LPCWSTR conoutPipeName = winpty_conout_name(winpty);
    LPCWSTR conerrPipeName = winpty_conerr_name(winpty);

    HANDLE conin = CreateFile(
        coninPipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0,
        NULL
    );

    HANDLE conout = CreateFile(
        conoutPipeName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0,
        NULL
    );

    HANDLE conerr = CreateFile(
        conerrPipeName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0,
        NULL
    );

    std::wcout << "Pipes created successfully" << std::endl;

    winpty_error_ptr_t spawnConfigError;
    winpty_spawn_config_t* spawnConfig = winpty_spawn_config_new(
        WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN,
        cmdPath,
        nullptr,
        nullptr,
        nullptr,
        &spawnConfigError
    );

    if (spawnConfig == nullptr) {
        outputLogMessage(_T("winpty_spawn_config_new failed"));
        return 1;
    }

    // Free handles and resources.
    CloseHandle(conout);
    CloseHandle(conerr);
    CloseHandle(conin);
    winpty_free(winpty);
}
