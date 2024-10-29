#include <windows.h>
#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#include <tchar.h>

#define SERVICE_NAME L"ClipboardAutoService"

SERVICE_STATUS ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE hServiceStatusHandle = NULL;
HANDLE hServiceStopEvent = INVALID_HANDLE_VALUE;

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
VOID WINAPI ServiceCtrlHandler(DWORD ctrlCode);

int main() {
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        { (LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    if (!StartServiceCtrlDispatcher(ServiceTable)) 
    {
        DWORD error = GetLastError();
    }
    return 0;
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {
    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    hServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
    if (hServiceStatusHandle == NULL) {
        return;
    }

    hServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hServiceStopEvent == NULL) {
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = GetLastError();
        SetServiceStatus(hServiceStatusHandle, &ServiceStatus);
        return;
    }

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hServiceStatusHandle, &ServiceStatus);

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);
    si.wShowWindow = SW_SHOW;
    si.dwFlags = STARTF_USESHOWWINDOW;

    if (CreateProcess(L"D:\\Work\\Play Projects\\Clipboard Auto Capture\\x64\\Debug\\Clipboard Auto Capture.exe", NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE); // Wait for the process to exit
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    while (WaitForSingleObject(hServiceStopEvent, 0) != WAIT_OBJECT_0) {
        Sleep(1000); // Check for stop event every second
    }

    CloseHandle(hServiceStopEvent);
    ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatusHandle, &ServiceStatus);
}


VOID WINAPI ServiceCtrlHandler(DWORD ctrlCode) {
    switch (ctrlCode) {
    case SERVICE_CONTROL_STOP:
        ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(hServiceStatusHandle, &ServiceStatus);
        SetEvent(hServiceStopEvent);
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hServiceStatusHandle, &ServiceStatus);
        break;
    default:
        break;
    }
}