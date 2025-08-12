#include <windows.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(" Please provide the folder path as an argument.\n");
        printf("Usage: OS2.exe \"C:\\\\Path\\\\To\\\\Folder\"\n");
        return 1;
    }

    char *folderPath = argv[1];
    FILE *logFile = fopen("events.log", "a");
    if (!logFile) {
        printf("❌ Cannot open log file.\n");
        return 1;
    }

    HANDLE hDir = CreateFileA(
        folderPath,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        fprintf(logFile, " Failed to open directory: %s\n", folderPath);
        fclose(logFile);
        return 1;
    }

    fprintf(logFile, " Started monitoring folder: %s\n", folderPath);
    fflush(logFile);

    char buffer[1024];
    DWORD bytesReturned;

    while (1) {
        if (ReadDirectoryChangesW(
                hDir,
                &buffer,
                sizeof(buffer),
                TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_LAST_WRITE |
                FILE_NOTIFY_CHANGE_CREATION,
                &bytesReturned,
                NULL,
                NULL
            )) {
            FILE_NOTIFY_INFORMATION *info = (FILE_NOTIFY_INFORMATION *)buffer;
            char filename[MAX_PATH];
            int count = WideCharToMultiByte(CP_UTF8, 0, info->FileName, info->FileNameLength / 2, filename, MAX_PATH, NULL, NULL);
            filename[count] = '\0';

            const char *eventType = "";
            switch (info->Action) {
                case FILE_ACTION_ADDED: eventType = " File Created"; break;
                case FILE_ACTION_REMOVED: eventType = " File Deleted"; break;
                case FILE_ACTION_MODIFIED: eventType = " File Modified"; break;
                case FILE_ACTION_RENAMED_OLD_NAME: eventType = " Renamed (Old Name)"; break;
                case FILE_ACTION_RENAMED_NEW_NAME: eventType = " Renamed (New Name)"; break;
                default: eventType = "❓ Unknown Action";
            }

            SYSTEMTIME st;
            GetLocalTime(&st);
            fprintf(logFile, "[%02d:%02d:%02d] %s → %s\n", st.wHour, st.wMinute, st.wSecond, eventType, filename);
            fflush(logFile);
        }
    }

    CloseHandle(hDir);
    fclose(logFile);
    return 0;
}
