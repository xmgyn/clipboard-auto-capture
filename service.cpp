#include <windows.h>
#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#include <tchar.h>

#define SERVICE_NAME _T("ClipboardAutoService")

SERVICE_STATUS ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE hServiceStatusHandle = NULL;
HANDLE hServiceStopEvent = INVALID_HANDLE_VALUE;

void WriteLog(const char* msg) {
    HANDLE hEventSource = RegisterEventSource(NULL, SERVICE_NAME);
    if (hEventSource != NULL) {
        const char* messages[1];
        messages[0] = msg;
        /*ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, messages, NULL);*/
        DeregisterEventSource(hEventSource);
    }
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
VOID WINAPI ServiceCtrlHandler(DWORD ctrlCode);

int lol() {
    /*SERVICE_TABLE_ENTRY ServiceTable[] = {
        { SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    if (!StartServiceCtrlDispatcher(ServiceTable)) {
        WriteLog("StartServiceCtrlDispatcher failed.");
    }
*/
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
        WriteLog("RegisterServiceCtrlHandler failed.");
        return;
    }

    hServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hServiceStopEvent == NULL) {
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = GetLastError();
        SetServiceStatus(hServiceStatusHandle, &ServiceStatus);
        WriteLog("CreateEvent failed.");
        return;
    }

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hServiceStatusHandle, &ServiceStatus);

    while (ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
        /*CaptureClipboard();*/
        Sleep(5000); // Capture clipboard data every 5 seconds
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