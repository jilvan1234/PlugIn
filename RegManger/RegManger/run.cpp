

#include <windows.h>
#include <stdlib.h>
#include "CRegManger.h"
int main(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ char** envp)

{
    CRegManger reg;
    reg.RegGetSpecifiedKeyAllKey(HKEY_LOCAL_MACHINE,TEXT("SYSTEM\\CurrentControlSet\\Control"));
    return 0;
}