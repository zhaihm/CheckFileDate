#include "getopt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Windows.h>

int find_all_files(const char *path, const FILETIME& date);

int main(int argc, char *argv[])
{
    int opt;
    char *date = NULL;

    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
        case 'n':
            date = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-n date]\n",
                argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    SYSTEMTIME st;
    sscanf(date, "%d-%d-%d %d:%d:%d", &st.wYear, &st.wMonth, &st.wDay, &st.wHour, &st.wMinute, &st.wSecond);

    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);

    if (date != NULL) {
        find_all_files(argv[optind], ft);
    }
}

int find_all_files(const char *path, const FILETIME& date)
{
    std::string strFind = path;
    strFind += "\\*.*";

    WIN32_FIND_DATA findData;
    HANDLE hFind = ::FindFirstFile(strFind.c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFind)
        return -1;

    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                strFind = path;
                strFind += "\\";
                strFind += findData.cFileName;
                find_all_files(strFind.c_str(), date);
            }
        }
        else {
            FILETIME ftFileLastWrite;
            FileTimeToLocalFileTime(&findData.ftLastWriteTime, &ftFileLastWrite); // findData.ftLastWriteTime is UTC time
            
            if (CompareFileTime(&ftFileLastWrite, &date) == 1) {
                printf("%s\\%s\n", path, findData.cFileName);

                std::string fullPath = path;
                fullPath += "\\";
                fullPath += findData.cFileName;
                //printf("fullPath=%s\n", fullPath.c_str());
            }
        }
    } while (::FindNextFile(hFind, &findData));

    ::FindClose(hFind);
    return 0;
}
