#include <Windows.h>
#include <stdio.h>
#include <psapi.h>
#include <libgen.h>
#include <io.h>
#include <stdint-gcc.h>
#include <time.h>
#include <tchar.h>

unsigned long long count = 0;
FILE *f;

HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        // the action is valid: HC_ACTION.
        if (wParam == WM_KEYDOWN) {
            kbdStruct = *((KBDLLHOOKSTRUCT *) lParam);

            // time is in ms
            HWND foreground = GetForegroundWindow();
            DWORD processId;
            GetWindowThreadProcessId(foreground, &processId);
            HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
            if (process == NULL) {
                printf("[err] OpenProcess Error: %lu\n", GetLastError());
            }

            TCHAR executablePath[512];
            DWORD fail = GetModuleFileNameEx(process, NULL, executablePath, 512);
            CloseHandle(process);
            if (fail == 0) {
                printf("[err] GetModuleFileNameEx Error: %lu\n", GetLastError());
            }
            TCHAR executableName[256];
            TCHAR executableExtension[32];
            _tsplitpath(executablePath, NULL, NULL, executableName, executableExtension);
            strcat(executableName, executableExtension);

            size_t size = _tcslen(executableName);
            uint8_t len = (uint8_t) size;

            uint8_t vkCode = (uint8_t) kbdStruct.vkCode;

            // printf("strs: %llu, app: %s, vk: %lu, t: %lu\n", count++, executableName, kbdStruct.vkCode, kbdStruct.time);
            uint32_t timestamp = (unsigned) time(NULL);
            fwrite(&timestamp, sizeof(DWORD), 1, f);
            fwrite(&vkCode, sizeof(uint8_t), 1, f);
            fwrite(&len, sizeof(uint8_t), 1, f);
            fwrite(executableName, sizeof(char), len, f);

            count++;
            if ((count % 256) == 0) {
                printf("Flushing file...");
                fflush(f);
            }
        }
    }

    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook() {
    printf("Setting up hook...\n");
    // Set the hook and set it to use the callback function above
    // WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
    // The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
    // function that sets and releases the hook. If you create a hack you will not need the callback function
    // in another place then your own code file anyway. Read more about it at MSDN.
    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0))) {
        MessageBox(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
    }
}

void ReleaseHook() {
    UnhookWindowsHookEx(_hook);
}

void openNextFile() {
    char *filename = "\\log.bin";
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, filename);
    printf("Logging to: %s\n", cwd);
    f = fopen(cwd, "ab");
    setvbuf(f, NULL, _IOFBF, 4096 * 4);
    if (f == NULL) {
        MessageBox(NULL, "Failed to open logging file for writing!", "Error", MB_ICONERROR);
    }
}

int main() {
    printf("keystroke_counter | v. 1.0\n");
    printf("--------------------------\n");
    // Open next file.
    openNextFile();

    // Set the hook
    SetHook();

    // Don't mind this, it is a meaningless loop to keep a console application running.
    // I used this to test the keyboard hook functionality. If you want to test it, keep it in ;)
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {

    }
    return 0;
}
