/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         MergeOS GUI/console setup
 * FILE:            base/setup/setup/setup.c
 * PURPOSE:         Second stage setup
 * PROGRAMMER:      Eric Kohl
 */

#include <stdarg.h>
#include <windef.h>
#include <winbase.h>

#define NDEBUG
#include <debug.h>

typedef INT (WINAPI *PINSTALL_MERGEOS)(INT argc, WCHAR** argv);

/* FUNCTIONS ****************************************************************/

static
INT
RunInstallMergeOS(INT argc, WCHAR* argv[])
{
    INT RetVal;
    HMODULE hDll;
    PINSTALL_MERGEOS InstallMergeOS;

    hDll = LoadLibraryW(L"syssetup.dll");
    if (hDll == NULL)
    {
        DPRINT("Failed to load 'syssetup.dll'!\n");
        return GetLastError();
    }
    DPRINT("Loaded 'syssetup.dll'!\n");

    /* Call the standard Windows-compatible export */
    InstallMergeOS = (PINSTALL_MERGEOS)GetProcAddress(hDll, "InstallWindowsNt");
    if (InstallMergeOS == NULL)
    {
        RetVal = GetLastError();
        DPRINT("Failed to get address for 'InstallWindowsNt()'!\n");
    }
    else
    {
        RetVal = InstallMergeOS(argc, argv);
    }

    return RetVal;
}


/* Called from wmainCRTStartup */
INT wmain(INT argc, WCHAR* argv[])
{
    LPWSTR CmdLine, p;

    // NOTE: Temporary, until we correctly use argc/argv.
    CmdLine = GetCommandLineW();
    DPRINT("CmdLine: <%S>\n", CmdLine);

    p = wcschr(CmdLine, L'-');
    if (p == NULL)
        return ERROR_INVALID_PARAMETER;
    p++;

    // NOTE: On Windows, "mini" means "minimal UI", and can be used
    // in addition to "newsetup"; these options are not exclusive.
    if (_wcsicmp(p, L"newsetup") == 0 || _wcsicmp(p, L"mini") == 0)
    {
        RunInstallMergeOS(argc, argv);
    }

#if 0
    /* Add new setup types here */
    else if (...)
    {

    }
#endif

    return 0;
}

/* EOF */
