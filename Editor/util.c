#include <windows.h>
#include <Shlwapi.h>
#include <stdbool.h>

const char *levelFilesBaseDir = "data\\levels\\*";

void GetLevelFileList(char *levelFilePaths[], int *totalFilesFound) 
{
    (*totalFilesFound) = 0;

    WIN32_FIND_DATA findData = {0};

    HANDLE findHandle = findHandle = FindFirstFile((LPCSTR)levelFilesBaseDir, &findData);

    if(findHandle == INVALID_HANDLE_VALUE) return;

    while(true) 
    {
        if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) 
        {
            char *extension = PathFindExtension(findData.cFileName);
            char *fileName = PathFindFileName(findData.cFileName);
            if(strcmp(".tmap", extension) == 0) 
            {
                char *fileNameCopy = malloc(strlen(fileName));
                strcpy(fileNameCopy, fileName);
                levelFilePaths[(*totalFilesFound)] = fileNameCopy;                
                (*totalFilesFound)++;
            }
        }
        int hasNext = FindNextFile(findHandle, &findData);
        if(hasNext == 0) break;
    }
}

u32 Power(u32 x, u32 power)
{
    u32 result = 1;
    
    for(u32 n = 0; n < power; n++)
    {
        result *= x;
    }
    
    return result;
}

u32 StringToInteger(u8 *str, u32 size)
{
    u32 result = 0;
    
    for(u32 n = 0; n < size; n++)
    {
        result = (result * 10 ) + (str[n] - '0');
    }
    
    return result;
}
