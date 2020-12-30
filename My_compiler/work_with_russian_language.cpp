#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include <wchar.h>
#include <locale.h>
/*
int main()
{
    //setlocale (LC_CTYPE, "ru_RU.UTF-8");
    setlocale(LC_CTYPE, "Russian");
    FILE* input = fopen("text.txt", "r"); //rb?

    wchar_t* bufer = (wchar_t*)calloc(50, sizeof(wchar_t));

    fgetws(bufer, 50, input);
    wprintf(L"%ls", bufer);

    wchar_t word[6] = {};
    //wchar_t mraz[6] = L"לנאחü";
    swscanf(bufer, L"%ls", word);
    wprintf(L"%ls\n", word);

    if (!wcscmp(word, L"לנאחü"))
    {
        wprintf(L"בÿךא\n");
    }

    fclose(input);
    free(bufer); 

    return 0;
}
*/