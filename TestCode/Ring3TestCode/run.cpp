#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>


int _tmain(_In_ int argc, _In_reads_(argc) _Pre_z_ _TCHAR** argv, _In_z_ _TCHAR** envp)
{
    unsigned char ShellCode[] =
        "\x64,\xa1,\x30,\x00,\x00,\x00"
        "\x8B,\x40,\x10"
        "\x8B,\x40,\x10";

}