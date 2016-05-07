#include "getopt.h"
#include "StringUtils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Windows.h>
#include <assert.h>

bool g_recover;
int find_all_files(const char *path, const FILETIME& date);
void string_utils_unit_test();

int main(int argc, char *argv[])
{
    int opt;
    char *date = NULL;

    while ((opt = getopt(argc, argv, "n:r")) != -1) {
        switch (opt) {
        case 'n':
            date = optarg;
            break;
        case 'r':
            g_recover = true;
            break;
        default:
            fprintf(stderr, "Usage: %s [-n date] -r path\n",
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

            if (g_recover && StringUtils::EndWith(findData.cFileName, ".bak")) {
                std::string cmd = "copy /y ";
                cmd += path;
                cmd += "\\";
                cmd += findData.cFileName;
                cmd += " ";
                cmd += path;
                cmd += "\\";
                cmd += StringUtils::Left(findData.cFileName, strlen(findData.cFileName) - strlen(".bak"));
                system(cmd.c_str());
            }
        }
    } while (::FindNextFile(hFind, &findData));

    ::FindClose(hFind);
    return 0;
}

void string_utils_unit_test()
{
    // test StringUtils
    assert(StringUtils::EndWith("file.bak", ".bak"));
    assert(!StringUtils::EndWith("file.bak", ".bak1"));
    assert(!StringUtils::EndWith("file.bak1", ".bak"));

    assert(StringUtils::Left("file.bak", 4) == "file");
}
