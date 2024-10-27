#include <windows.h>
#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#include <tchar.h>

#define SERVICE_NAME L"ClipboardAutoService"
#include <fstream>

// Function to log messages to a file
void LogMessage(const std::wstring& message) {
    std::ofstream logFile("D:\\Work\\Play Projects\\Clipboard Auto Capture\\x64\\Debug\\service_log.txt", std::ios_base::app);
    logFile << std::string(message.begin(), message.end()) << std::endl;
}
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
    CreateProcess(L"colorcpl.exe", NULL, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

    if (CreateProcess(L"notepad.exe", NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        DWORD error = GetLastError();
        // Log the error code
        LogMessage(std::to_wstring(error));
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