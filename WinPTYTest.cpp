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

void outputLogMessage(const TCHAR* message) {
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

    std::wcout << cmdPath << std::endl;
}
