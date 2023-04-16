/*
 *  MergeOS kernel
 *  Copyright (C) 2002, 2003, 2004 MergeOS Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         MergeOS text-mode setup
 * FILE:            base/setup/usetup/usetup.c
 * PURPOSE:         Text-mode setup
 * PROGRAMMERS:     Casper S. Hornstrup (chorns@users.sourceforge.net)
 *                  Hervé Poussineau (hpoussin@reactos.org)
 */

#include <usetup.h>
#include <math.h>
#include <ntstrsafe.h>

#include "bootsup.h"
#include "chkdsk.h"
#include "cmdcons.h"
#include "devinst.h"
#include "format.h"

#define NDEBUG
#include <debug.h>


/* GLOBALS & LOCALS *********************************************************/

HANDLE ProcessHeap;
BOOLEAN IsUnattendedSetup = FALSE;

static USETUP_DATA USetupData;

/* The partition where to perform the installation */
static PPARTENTRY InstallPartition = NULL;
/*
 * The system partition we will actually use. It can be different from
 * PartitionList->SystemPartition in case we don't support it, or we install
 * on a removable disk.
 * We may indeed not support the original system partition in case we do not
 * have write support on it. Please note that this situation is partly a HACK
 * and MUST NEVER happen on architectures where real system partitions are
 * mandatory (because then they are formatted in FAT FS and we support write
 * operation on them).
 */
static PPARTENTRY SystemPartition = NULL;


/* OTHER Stuff *****/

PCWSTR SelectedLanguageId;
static WCHAR DefaultLanguage[20];   // Copy of string inside LanguageList
static WCHAR DefaultKBLayout[20];   // Copy of string inside KeyboardList

static BOOLEAN RepairUpdateFlag = FALSE;

/* Global partition list on the system */
static PPARTLIST PartitionList = NULL;

/* Currently selected partition entry in the list */
static PPARTENTRY CurrentPartition = NULL;

/* List of supported file systems for the partition to be formatted */
static PFILE_SYSTEM_LIST FileSystemList = NULL;

/* Machine state for the formatter */
static PPARTENTRY TempPartition = NULL;
static FORMATMACHINESTATE FormatState = Start;

/*****************************************************/

static PNTOS_INSTALLATION CurrentInstallation = NULL;
static PGENERIC_LIST NtOsInstallsList = NULL;

#ifdef __MERGEOS__ /* HACK */

/* FONT SUBSTITUTION WORKAROUND *************************************************/

/* For font file check */
FONTSUBSTSETTINGS s_SubstSettings = { FALSE };

static void
DoWatchDestFileName(LPCWSTR FileName)
{
    if (FileName[0] == 'm' || FileName[0] == 'M')
    {
        if (wcsicmp(FileName, L"mingliu.ttc") == 0)
        {
            DPRINT("mingliu.ttc found\n");
            s_SubstSettings.bFoundFontMINGLIU = TRUE;
        }
        else if (wcsicmp(FileName, L"msgothic.ttc") == 0)
        {
            DPRINT("msgothic.ttc found\n");
            s_SubstSettings.bFoundFontMSGOTHIC = TRUE;
        }
        else if (wcsicmp(FileName, L"msmincho.ttc") == 0)
        {
            DPRINT("msmincho.ttc found\n");
            s_SubstSettings.bFoundFontMSMINCHO = TRUE;
        }
        else if (wcsicmp(FileName, L"mssong.ttf") == 0)
        {
            DPRINT("mssong.ttf found\n");
            s_SubstSettings.bFoundFontMSSONG = TRUE;
        }
    }
    else
    {
        if (wcsicmp(FileName, L"simsun.ttc") == 0)
        {
            DPRINT("simsun.ttc found\n");
            s_SubstSettings.bFoundFontSIMSUN = TRUE;
        }
        else if (wcsicmp(FileName, L"gulim.ttc") == 0)
        {
            DPRINT("gulim.ttc found\n");
            s_SubstSettings.bFoundFontGULIM = TRUE;
        }
        else if (wcsicmp(FileName, L"batang.ttc") == 0)
        {
            DPRINT("batang.ttc found\n");
            s_SubstSettings.bFoundFontBATANG = TRUE;
        }
    }
}
#endif  /* HACK */

/* FUNCTIONS ****************************************************************/

static VOID
PrintString(IN PCSTR fmt,...)
{
    CHAR buffer[512];
    va_list ap;
    UNICODE_STRING UnicodeString;
    ANSI_STRING AnsiString;

    va_start(ap, fmt);
    vsprintf(buffer, fmt, ap);
    va_end(ap);

    RtlInitAnsiString(&AnsiString, buffer);
    RtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, TRUE);
    NtDisplayString(&UnicodeString);
    RtlFreeUnicodeString(&UnicodeString);
}


static VOID
DrawBox(IN SHORT xLeft,
        IN SHORT yTop,
        IN SHORT Width,
        IN SHORT Height)
{
    COORD coPos;
    DWORD Written;

    /* Draw upper left corner */
    coPos.X = xLeft;
    coPos.Y = yTop;
    FillConsoleOutputCharacterA(StdOutput,
                                CharUpperLeftCorner, // '+',
                                1,
                                coPos,
                                &Written);

    /* Draw upper edge */
    coPos.X = xLeft + 1;
    coPos.Y = yTop;
    FillConsoleOutputCharacterA(StdOutput,
                                CharHorizontalLine, // '-',
                                Width - 2,
                                coPos,
                                &Written);

    /* Draw upper right corner */
    coPos.X = xLeft + Width - 1;
    coPos.Y = yTop;
    FillConsoleOutputCharacterA(StdOutput,
                                CharUpperRightCorner, // '+',
                                1,
                                coPos,
                                &Written);

    /* Draw right edge, inner space and left edge */
    for (coPos.Y = yTop + 1; coPos.Y < yTop + Height - 1; coPos.Y++)
    {
        coPos.X = xLeft;
        FillConsoleOutputCharacterA(StdOutput,
                                    CharVerticalLine, // '|',
                                    1,
                                    coPos,
                                    &Written);

        coPos.X = xLeft + 1;
        FillConsoleOutputCharacterA(StdOutput,
                                    ' ',
                                    Width - 2,
                                    coPos,
                                    &Written);

        coPos.X = xLeft + Width - 1;
        FillConsoleOutputCharacterA(StdOutput,
                                    CharVerticalLine, // '|',
                                    1,
                                    coPos,
                                    &Written);
    }

    /* Draw lower left corner */
    coPos.X = xLeft;
    coPos.Y = yTop + Height - 1;
    FillConsoleOutputCharacterA(StdOutput,
                                CharLowerLeftCorner, // '+',
                                1,
                                coPos,
                                &Written);

    /* Draw lower edge */
    coPos.X = xLeft + 1;
    coPos.Y = yTop + Height - 1;
    FillConsoleOutputCharacterA(StdOutput,
                                CharHorizontalLine, // '-',
                                Width - 2,
                                coPos,
                                &Written);

    /* Draw lower right corner */
    coPos.X = xLeft + Width - 1;
    coPos.Y = yTop + Height - 1;
    FillConsoleOutputCharacterA(StdOutput,
                                CharLowerRightCorner, // '+',
                                1,
                                coPos,
                                &Written);
}


VOID
PopupError(PCCH Text,
           PCCH Status,
           PINPUT_RECORD Ir,
           ULONG WaitEvent)
{
    SHORT yTop;
    SHORT xLeft;
    COORD coPos;
    DWORD Written;
    ULONG Length;
    ULONG MaxLength;
    ULONG Lines;
    PCHAR p;
    PCCH pnext;
    BOOLEAN LastLine;
    SHORT Width;
    SHORT Height;

    /* Count text lines and longest line */
    MaxLength = 0;
    Lines = 0;
    pnext = Text;

    while (TRUE)
    {
        p = strchr(pnext, '\n');

        if (p == NULL)
        {
            Length = strlen(pnext);
            LastLine = TRUE;
        }
        else
        {
            Length = (ULONG)(p - pnext);
            LastLine = FALSE;
        }

        Lines++;

        if (Length > MaxLength)
            MaxLength = Length;

        if (LastLine)
            break;

        pnext = p + 1;
    }

    /* Check length of status line */
    if (Status != NULL)
    {
        Length = strlen(Status);

        if (Length > MaxLength)
            MaxLength = Length;
    }

    Width = MaxLength + 4;
    Height = Lines + 2;

    if (Status != NULL)
        Height += 2;

    yTop = (yScreen - Height) / 2;
    xLeft = (xScreen - Width) / 2;


    /* Set screen attributes */
    coPos.X = xLeft;
    for (coPos.Y = yTop; coPos.Y < yTop + Height; coPos.Y++)
    {
        FillConsoleOutputAttribute(StdOutput,
                                   FOREGROUND_RED | BACKGROUND_WHITE,
                                   Width,
                                   coPos,
                                   &Written);
    }

    DrawBox(xLeft, yTop, Width, Height);

    /* Print message text */
    coPos.Y = yTop + 1;
    pnext = Text;
    while (TRUE)
    {
        p = strchr(pnext, '\n');

        if (p == NULL)
        {
            Length = strlen(pnext);
            LastLine = TRUE;
        }
        else
        {
            Length = (ULONG)(p - pnext);
            LastLine = FALSE;
        }

        if (Length != 0)
        {
            coPos.X = xLeft + 2;
            WriteConsoleOutputCharacterA(StdOutput,
                                         pnext,
                                         Length,
                                         coPos,
                                         &Written);
        }

        if (LastLine)
            break;

        coPos.Y++;
        pnext = p + 1;
    }

    /* Print separator line and status text */
    if (Status != NULL)
    {
        coPos.Y = yTop + Height - 3;
        coPos.X = xLeft;
        FillConsoleOutputCharacterA(StdOutput,
                                    CharVertLineAndRightHorizLine, // '+',
                                    1,
                                    coPos,
                                    &Written);

        coPos.X = xLeft + 1;
        FillConsoleOutputCharacterA(StdOutput,
                                    CharHorizontalLine, // '-',
                                    Width - 2,
                                    coPos,
                                    &Written);

        coPos.X = xLeft + Width - 1;
        FillConsoleOutputCharacterA(StdOutput,
                                    CharLeftHorizLineAndVertLine, // '+',
                                    1,
                                    coPos,
                                    &Written);

        coPos.Y++;
        coPos.X = xLeft + 2;
        WriteConsoleOutputCharacterA(StdOutput,
                                     Status,
                                     min(strlen(Status), (SIZE_T)Width - 4),
                                     coPos,
                                     &Written);
    }

    if (WaitEvent == POPUP_WAIT_NONE)
        return;

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if (WaitEvent == POPUP_WAIT_ANY_KEY ||
            Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D)
        {
            return;
        }
    }
}


/*
 * Confirm quit setup
 * RETURNS
 *   TRUE: Quit setup.
 *   FALSE: Don't quit setup.
 */
static BOOL
ConfirmQuit(PINPUT_RECORD Ir)
{
    BOOL Result = FALSE;
    MUIDisplayError(ERROR_NOT_INSTALLED, NULL, POPUP_WAIT_NONE);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            Result = TRUE;
            break;
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D)  /* ENTER */
        {
            Result = FALSE;
            break;
        }
    }

    return Result;
}


static VOID
UpdateKBLayout(VOID)
{
    PGENERIC_LIST_ENTRY ListEntry;
    PCWSTR pszNewLayout;

    pszNewLayout = MUIDefaultKeyboardLayout(SelectedLanguageId);

    if (USetupData.LayoutList == NULL)
    {
        USetupData.LayoutList = CreateKeyboardLayoutList(USetupData.SetupInf, SelectedLanguageId, DefaultKBLayout);
        if (USetupData.LayoutList == NULL)
        {
            /* FIXME: Handle error! */
            return;
        }
    }

    /* Search for default layout (if provided) */
    if (pszNewLayout != NULL)
    {
        for (ListEntry = GetFirstListEntry(USetupData.LayoutList); ListEntry;
             ListEntry = GetNextListEntry(ListEntry))
        {
            if (!wcscmp(pszNewLayout, ((PGENENTRY)GetListEntryData(ListEntry))->Id))
            {
                SetCurrentListEntry(USetupData.LayoutList, ListEntry);
                break;
            }
        }
    }
}


static NTSTATUS
NTAPI
GetSettingDescription(
    IN PGENERIC_LIST_ENTRY Entry,
    OUT PSTR Buffer,
    IN SIZE_T cchBufferSize)
{
    return RtlStringCchPrintfA(Buffer, cchBufferSize, "%S",
                               ((PGENENTRY)GetListEntryData(Entry))->Value);
}

static NTSTATUS
NTAPI
GetNTOSInstallationName(
    IN PGENERIC_LIST_ENTRY Entry,
    OUT PSTR Buffer,
    IN SIZE_T cchBufferSize)
{
    PNTOS_INSTALLATION NtOsInstall = (PNTOS_INSTALLATION)GetListEntryData(Entry);
    PPARTENTRY PartEntry = NtOsInstall->PartEntry;

    if (PartEntry && PartEntry->DriveLetter)
    {
        /* We have retrieved a partition that is mounted */
        return RtlStringCchPrintfA(Buffer, cchBufferSize,
                                   "%C:%S  \"%S\"",
                                   PartEntry->DriveLetter,
                                   NtOsInstall->PathComponent,
                                   NtOsInstall->InstallationName);
    }
    else
    {
        /* We failed somewhere, just show the NT path */
        return RtlStringCchPrintfA(Buffer, cchBufferSize,
                                   "%wZ  \"%S\"",
                                   &NtOsInstall->SystemNtPath,
                                   NtOsInstall->InstallationName);
    }
}


/*
 * Displays the LanguagePage.
 *
 * Next pages: WelcomePage, QuitPage
 *
 * SIDEEFFECTS
 *  Init SelectedLanguageId
 *  Init USetupData.LanguageId
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
LanguagePage(PINPUT_RECORD Ir)
{
    GENERIC_LIST_UI ListUi;
    PCWSTR NewLanguageId;
    BOOL RefreshPage = FALSE;

    /* Initialize the computer settings list */
    if (USetupData.LanguageList == NULL)
    {
        USetupData.LanguageList = CreateLanguageList(USetupData.SetupInf, DefaultLanguage);
        if (USetupData.LanguageList == NULL)
        {
           PopupError("Setup failed to initialize available translations", NULL, NULL, POPUP_WAIT_NONE);
           return WELCOME_PAGE;
        }
    }

    SelectedLanguageId = DefaultLanguage;
    USetupData.LanguageId = 0;

    /* Load the font */
    SetConsoleCodePage();
    UpdateKBLayout();

    /*
     * If there is no language or just a single one in the list,
     * skip the language selection process altogether.
     */
    if (GetNumberOfListEntries(USetupData.LanguageList) <= 1)
    {
        USetupData.LanguageId = (LANGID)(wcstol(SelectedLanguageId, NULL, 16) & 0xFFFF);
        return WELCOME_PAGE;
    }

    InitGenericListUi(&ListUi, USetupData.LanguageList, GetSettingDescription);
    DrawGenericList(&ListUi,
                    2, 18,
                    xScreen - 3,
                    yScreen - 3);

    ScrollToPositionGenericList(&ListUi, GetDefaultLanguageIndex());

    MUIDisplayPage(LANGUAGE_PAGE);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_DOWN))  /* DOWN */
        {
            ScrollDownGenericList(&ListUi);
            RefreshPage = TRUE;
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_UP))  /* UP */
        {
            ScrollUpGenericList(&ListUi);
            RefreshPage = TRUE;
        }
        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_NEXT))  /* PAGE DOWN */
        {
            ScrollPageDownGenericList(&ListUi);
            RefreshPage = TRUE;
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_PRIOR))  /* PAGE UP */
        {
            ScrollPageUpGenericList(&ListUi);
            RefreshPage = TRUE;
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;
            else
                RedrawGenericList(&ListUi);
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D)  /* ENTER */
        {
            ASSERT(GetNumberOfListEntries(USetupData.LanguageList) >= 1);

            SelectedLanguageId =
                ((PGENENTRY)GetListEntryData(GetCurrentListEntry(USetupData.LanguageList)))->Id;

            USetupData.LanguageId = (LANGID)(wcstol(SelectedLanguageId, NULL, 16) & 0xFFFF);

            if (wcscmp(SelectedLanguageId, DefaultLanguage))
            {
                UpdateKBLayout();
            }

            /* Load the font */
            SetConsoleCodePage();

            return WELCOME_PAGE;
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar > 0x60) && (Ir->Event.KeyEvent.uChar.AsciiChar < 0x7b))
        {
            /* a-z */
            GenericListKeyPress(&ListUi, Ir->Event.KeyEvent.uChar.AsciiChar);
            RefreshPage = TRUE;
        }

        if (RefreshPage)
        {
            ASSERT(GetNumberOfListEntries(USetupData.LanguageList) >= 1);

            NewLanguageId =
                ((PGENENTRY)GetListEntryData(GetCurrentListEntry(USetupData.LanguageList)))->Id;

            if (wcscmp(SelectedLanguageId, NewLanguageId))
            {
                /* Clear the language page */
                MUIClearPage(LANGUAGE_PAGE);

                SelectedLanguageId = NewLanguageId;

                /* Load the font */
                SetConsoleCodePage();

                /* Redraw the list */
                DrawGenericList(&ListUi,
                                2, 18,
                                xScreen - 3,
                                yScreen - 3);

                /* Redraw language selection page in native language */
                MUIDisplayPage(LANGUAGE_PAGE);
            }

            RefreshPage = FALSE;
        }
    }

    return WELCOME_PAGE;
}


/*
 * Start page
 *
 * Next pages:
 *  LanguagePage (at once, default)
 *  InstallIntroPage (at once, if unattended)
 *  QuitPage
 *
 * SIDEEFFECTS
 *  Init Sdi
 *  Init USetupData.SourcePath
 *  Init USetupData.SourceRootPath
 *  Init USetupData.SourceRootDir
 *  Init USetupData.SetupInf
 *  Init USetupData.RequiredPartitionDiskSpace
 *  Init IsUnattendedSetup
 *  If unattended, init *List and sets the Codepage
 *  If unattended, init SelectedLanguageId
 *  If unattended, init USetupData.LanguageId
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
SetupStartPage(PINPUT_RECORD Ir)
{
    ULONG Error;
    PGENERIC_LIST_ENTRY ListEntry;
    PCWSTR LocaleId;

    MUIDisplayPage(SETUP_INIT_PAGE);

    /* Initialize Setup, phase 1 */
    Error = InitializeSetup(&USetupData, 1);
    if (Error != ERROR_SUCCESS)
    {
        MUIDisplayError(Error, Ir, POPUP_WAIT_ENTER);
        return QUIT_PAGE;
    }

    /* Initialize the user-mode PnP manager */
    if (!EnableUserModePnpManager())
        DPRINT1("The user-mode PnP manager could not initialize, expect unavailable devices!\n");

    /* Wait for any immediate pending installations to finish */
    if (WaitNoPendingInstallEvents(NULL) != STATUS_WAIT_0)
        DPRINT1("WaitNoPendingInstallEvents() failed to wait!\n");

    CheckUnattendedSetup(&USetupData);

    if (IsUnattendedSetup)
    {
        // TODO: Read options from inf
        /* Load the hardware, language and keyboard layout lists */

        USetupData.ComputerList = CreateComputerTypeList(USetupData.SetupInf);
        USetupData.DisplayList = CreateDisplayDriverList(USetupData.SetupInf);
        USetupData.KeyboardList = CreateKeyboardDriverList(USetupData.SetupInf);

        USetupData.LanguageList = CreateLanguageList(USetupData.SetupInf, DefaultLanguage);

        /* new part */
        SelectedLanguageId = DefaultLanguage;
        wcscpy(DefaultLanguage, USetupData.LocaleID);
        USetupData.LanguageId = (LANGID)(wcstol(SelectedLanguageId, NULL, 16) & 0xFFFF);

        USetupData.LayoutList = CreateKeyboardLayoutList(USetupData.SetupInf, SelectedLanguageId, DefaultKBLayout);

        /* first we hack LanguageList */
        for (ListEntry = GetFirstListEntry(USetupData.LanguageList); ListEntry;
             ListEntry = GetNextListEntry(ListEntry))
        {
            LocaleId = ((PGENENTRY)GetListEntryData(ListEntry))->Id;
            if (!wcsicmp(USetupData.LocaleID, LocaleId))
            {
                DPRINT("found %S in LanguageList\n", LocaleId);
                SetCurrentListEntry(USetupData.LanguageList, ListEntry);
                break;
            }
        }

        /* now LayoutList */
        for (ListEntry = GetFirstListEntry(USetupData.LayoutList); ListEntry;
             ListEntry = GetNextListEntry(ListEntry))
        {
            LocaleId = ((PGENENTRY)GetListEntryData(ListEntry))->Id;
            if (!wcsicmp(USetupData.LocaleID, LocaleId))
            {
                DPRINT("found %S in LayoutList\n", LocaleId);
                SetCurrentListEntry(USetupData.LayoutList, ListEntry);
                break;
            }
        }

        SetConsoleCodePage();

        return INSTALL_INTRO_PAGE;
    }

    return LANGUAGE_PAGE;
}


/*
 * Displays the WelcomePage.
 *
 * Next pages:
 *  InstallIntroPage (default)
 *  RepairIntroPage
 *  RecoveryPage
 *  LicensePage
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
WelcomePage(PINPUT_RECORD Ir)
{
    MUIDisplayPage(WELCOME_PAGE);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D) /* ENTER */
        {
            return INSTALL_INTRO_PAGE;
        }
        else if (toupper(Ir->Event.KeyEvent.uChar.AsciiChar) == 'R') /* R */
        {
            return RECOVERY_PAGE; // REPAIR_INTRO_PAGE;
        }
        else if (toupper(Ir->Event.KeyEvent.uChar.AsciiChar) == 'L') /* L */
        {
            return LICENSE_PAGE;
        }
    }

    return WELCOME_PAGE;
}


/*
 * Displays the License page.
 *
 * Next page:
 *  WelcomePage (default)
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
LicensePage(PINPUT_RECORD Ir)
{
    MUIDisplayPage(LICENSE_PAGE);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D)  /* ENTER */
        {
            return WELCOME_PAGE;
        }
    }

    return LICENSE_PAGE;
}


/*
 * Displays the RepairIntroPage.
 *
 * Next pages:
 *  RebootPage (default)
 *  InstallIntroPage
 *  RecoveryPage
 *  IntroPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
RepairIntroPage(PINPUT_RECORD Ir)
{
    MUIDisplayPage(REPAIR_INTRO_PAGE);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D)  /* ENTER */
        {
            return REBOOT_PAGE;
        }
        else if (toupper(Ir->Event.KeyEvent.uChar.AsciiChar) == 'U')  /* U */
        {
            RepairUpdateFlag = TRUE;
            return INSTALL_INTRO_PAGE;
        }
        else if (toupper(Ir->Event.KeyEvent.uChar.AsciiChar) == 'R')  /* R */
        {
            return RECOVERY_PAGE;
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)  /* ESC */
        {
            return WELCOME_PAGE;
        }
    }

    return REPAIR_INTRO_PAGE;
}

/*
 * Displays the UpgradeRepairPage.
 *
 * Next pages:
 *  RebootPage (default)
 *  InstallIntroPage
 *  RecoveryPage
 *  WelcomePage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
UpgradeRepairPage(PINPUT_RECORD Ir)
{
    GENERIC_LIST_UI ListUi;

/*** HACK!! ***/
    if (PartitionList == NULL)
    {
        PartitionList = CreatePartitionList();
        if (PartitionList == NULL)
        {
            /* FIXME: show an error dialog */
            MUIDisplayError(ERROR_DRIVE_INFORMATION, Ir, POPUP_WAIT_ENTER);
            return QUIT_PAGE;
        }
        else if (IsListEmpty(&PartitionList->DiskListHead))
        {
            MUIDisplayError(ERROR_NO_HDD, Ir, POPUP_WAIT_ENTER);
            return QUIT_PAGE;
        }

        /* Reset the formatter machine state */
        TempPartition = NULL;
        FormatState = Start;
    }
/**************/

    NtOsInstallsList = CreateNTOSInstallationsList(PartitionList);
    if (!NtOsInstallsList)
        DPRINT1("Failed to get a list of NTOS installations; continue installation...\n");

    /*
     * If there is no available installation (or just a single one??) that can
     * be updated in the list, just continue with the regular installation.
     */
    if (!NtOsInstallsList || GetNumberOfListEntries(NtOsInstallsList) == 0)
    {
        RepairUpdateFlag = FALSE;

        // return INSTALL_INTRO_PAGE;
        return DEVICE_SETTINGS_PAGE;
        // return SCSI_CONTROLLER_PAGE;
    }

    MUIDisplayPage(UPGRADE_REPAIR_PAGE);

    InitGenericListUi(&ListUi, NtOsInstallsList, GetNTOSInstallationName);
    DrawGenericList(&ListUi,
                    2, 23,
                    xScreen - 3,
                    yScreen - 3);

    // return HandleGenericList(&ListUi, DEVICE_SETTINGS_PAGE, Ir);
    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x00)
        {
            switch (Ir->Event.KeyEvent.wVirtualKeyCode)
            {
            case VK_DOWN:   /* DOWN */
                ScrollDownGenericList(&ListUi);
                break;
            case VK_UP:     /* UP */
                ScrollUpGenericList(&ListUi);
                break;
            case VK_NEXT:   /* PAGE DOWN */
                ScrollPageDownGenericList(&ListUi);
                break;
            case VK_PRIOR:  /* PAGE UP */
                ScrollPageUpGenericList(&ListUi);
                break;
            case VK_F3:     /* F3 */
            {
                if (ConfirmQuit(Ir))
                    return QUIT_PAGE;
                else
                    RedrawGenericList(&ListUi);
                break;
            }
#if 1
/* TODO: Temporarily kept until correct keyboard layout is in place.
 * (Actual AsciiChar of ESCAPE should be 0x1B instead of 0.)
 * Addendum to commit 8b94515b.
 */
            case VK_ESCAPE: /* ESC */
            {
                RestoreGenericListUiState(&ListUi);
                // return nextPage;    // prevPage;

                // return INSTALL_INTRO_PAGE;
                return DEVICE_SETTINGS_PAGE;
                // return SCSI_CONTROLLER_PAGE;
            }

#endif
            }
        }
#if 0
/* TODO: Restore this once correct keyboard layout is in place. */
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) /* ESC */
        {
            RestoreGenericListUiState(&ListUi);
            // return nextPage;    // prevPage;

            // return INSTALL_INTRO_PAGE;
            return DEVICE_SETTINGS_PAGE;
            // return SCSI_CONTROLLER_PAGE;
        }
#endif
        else
        {
            // switch (toupper(Ir->Event.KeyEvent.uChar.AsciiChar))
            // if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D) /* ENTER */
            if (toupper(Ir->Event.KeyEvent.uChar.AsciiChar) == 'U')  /* U */
            {
                /* Retrieve the current installation */
                ASSERT(GetNumberOfListEntries(NtOsInstallsList) >= 1);

                CurrentInstallation =
                    (PNTOS_INSTALLATION)GetListEntryData(GetCurrentListEntry(NtOsInstallsList));

                DPRINT1("Selected installation for repair: \"%S\" ; DiskNumber = %d , PartitionNumber = %d\n",
                        CurrentInstallation->InstallationName, CurrentInstallation->DiskNumber, CurrentInstallation->PartitionNumber);

                RepairUpdateFlag = TRUE;

                // return nextPage;
                /***/return INSTALL_INTRO_PAGE;/***/
            }
            else if ((Ir->Event.KeyEvent.uChar.AsciiChar > 0x60) &&
                     (Ir->Event.KeyEvent.uChar.AsciiChar < 0x7b))   /* a-z */
            {
                GenericListKeyPress(&ListUi, Ir->Event.KeyEvent.uChar.AsciiChar);
            }
        }
    }

    return UPGRADE_REPAIR_PAGE;
}


/*
 * Displays the InstallIntroPage.
 *
 * Next pages:
 *  DeviceSettingsPage  (At once if repair or update is selected)
 *  SelectPartitionPage (At once if unattended setup)
 *  DeviceSettingsPage  (default)
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
InstallIntroPage(PINPUT_RECORD Ir)
{
    if (RepairUpdateFlag)
    {
#if 1 /* Old code that looks good */

        // return SELECT_PARTITION_PAGE;
        return DEVICE_SETTINGS_PAGE;

#else /* Possible new code? */

        return DEVICE_SETTINGS_PAGE;
        // return SCSI_CONTROLLER_PAGE;

#endif
    }

    if (IsUnattendedSetup)
        return SELECT_PARTITION_PAGE;

    MUIDisplayPage(INSTALL_INTRO_PAGE);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3)) /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D) /* ENTER */
        {
            return UPGRADE_REPAIR_PAGE;
        }
    }

    return INSTALL_INTRO_PAGE;
}


#if 0
static PAGE_NUMBER
ScsiControllerPage(PINPUT_RECORD Ir)
{
    // MUIDisplayPage(SCSI_CONTROLLER_PAGE);

    CONSOLE_SetTextXY(6, 8, "Setup detected the following mass storage devices:");

    /* FIXME: print loaded mass storage driver descriptions */
#if 0
    CONSOLE_SetTextXY(8, 10, "TEST device");
#endif

    CONSOLE_SetStatusText("   ENTER = Continue   F3 = Quit");

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3)) /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D) /* ENTER */
        {
            return DEVICE_SETTINGS_PAGE;
        }
    }

    return SCSI_CONTROLLER_PAGE;
}

static PAGE_NUMBER
OemDriverPage(PINPUT_RECORD Ir)
{
    // MUIDisplayPage(OEM_DRIVER_PAGE);

    CONSOLE_SetTextXY(6, 8, "This is the OEM driver page!");

    /* FIXME: Implement!! */

    CONSOLE_SetStatusText("   ENTER = Continue   F3 = Quit");

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3)) /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D) /* ENTER */
        {
            return DEVICE_SETTINGS_PAGE;
        }
    }

    return OEM_DRIVER_PAGE;
}
#endif


/*
 * Displays the DeviceSettingsPage.
 *
 * Next pages:
 *  SelectPartitionPage (At once if repair or update is selected)
 *  ComputerSettingsPage
 *  DisplaySettingsPage
 *  KeyboardSettingsPage
 *  LayoutsettingsPage
 *  SelectPartitionPage
 *  QuitPage
 *
 * SIDEEFFECTS
 *  Init USetupData.ComputerList
 *  Init USetupData.DisplayList
 *  Init USetupData.KeyboardList
 *  Init USetupData.LayoutList
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
DeviceSettingsPage(PINPUT_RECORD Ir)
{
    static ULONG Line = 16;

    /* Initialize the computer settings list */
    if (USetupData.ComputerList == NULL)
    {
        USetupData.ComputerList = CreateComputerTypeList(USetupData.SetupInf);
        if (USetupData.ComputerList == NULL)
        {
            MUIDisplayError(ERROR_LOAD_COMPUTER, Ir, POPUP_WAIT_ENTER);
            return QUIT_PAGE;
        }
    }

    /* Initialize the display settings list */
    if (USetupData.DisplayList == NULL)
    {
        USetupData.DisplayList = CreateDisplayDriverList(USetupData.SetupInf);
        if (USetupData.DisplayList == NULL)
        {
            MUIDisplayError(ERROR_LOAD_DISPLAY, Ir, POPUP_WAIT_ENTER);
            return QUIT_PAGE;
        }
    }

    /* Initialize the keyboard settings list */
    if (USetupData.KeyboardList == NULL)
    {
        USetupData.KeyboardList = CreateKeyboardDriverList(USetupData.SetupInf);
        if (USetupData.KeyboardList == NULL)
        {
            MUIDisplayError(ERROR_LOAD_KEYBOARD, Ir, POPUP_WAIT_ENTER);
            return QUIT_PAGE;
        }
    }

    /* Initialize the keyboard layout list */
    if (USetupData.LayoutList == NULL)
    {
        USetupData.LayoutList = CreateKeyboardLayoutList(USetupData.SetupInf, SelectedLanguageId, DefaultKBLayout);
        if (USetupData.LayoutList == NULL)
        {
            /* FIXME: report error */
            MUIDisplayError(ERROR_LOAD_KBLAYOUT, Ir, POPUP_WAIT_ENTER);
            return QUIT_PAGE;
        }
    }

    if (RepairUpdateFlag)
        return SELECT_PARTITION_PAGE;

    // if (IsUnattendedSetup)
        // return SELECT_PARTITION_PAGE;

    MUIDisplayPage(DEVICE_SETTINGS_PAGE);

    DrawGenericListCurrentItem(USetupData.ComputerList, GetSettingDescription, 25, 11);
    DrawGenericListCurrentItem(USetupData.DisplayList , GetSettingDescription, 25, 12);
    DrawGenericListCurrentItem(USetupData.KeyboardList, GetSettingDescription, 25, 13);
    DrawGenericListCurrentItem(USetupData.LayoutList  , GetSettingDescription, 25, 14);

    CONSOLE_InvertTextXY(24, Line, 48, 1);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_DOWN))  /* DOWN */
        {
            CONSOLE_NormalTextXY(24, Line, 48, 1);

            if (Line == 14)
                Line = 16;
            else if (Line == 16)
                Line = 11;
            else
                Line++;

            CONSOLE_InvertTextXY(24, Line, 48, 1);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_UP))  /* UP */
        {
            CONSOLE_NormalTextXY(24, Line, 48, 1);

            if (Line == 11)
                Line = 16;
            else if (Line == 16)
                Line = 14;
            else
                Line--;

            CONSOLE_InvertTextXY(24, Line, 48, 1);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D) /* ENTER */
        {
            if (Line == 11)
                return COMPUTER_SETTINGS_PAGE;
            else if (Line == 12)
                return DISPLAY_SETTINGS_PAGE;
            else if (Line == 13)
                return KEYBOARD_SETTINGS_PAGE;
            else if (Line == 14)
                return LAYOUT_SETTINGS_PAGE;
            else if (Line == 16)
                return SELECT_PARTITION_PAGE;
        }
    }

    return DEVICE_SETTINGS_PAGE;
}


/*
 * Handles generic selection lists.
 *
 * PARAMS
 * GenericList: The list to handle.
 * nextPage: The page it needs to jump to after this page.
 * Ir: The PINPUT_RECORD
 */
static PAGE_NUMBER
HandleGenericList(PGENERIC_LIST_UI ListUi,
                  PAGE_NUMBER nextPage,
                  PINPUT_RECORD Ir)
{
    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_DOWN))  /* DOWN */
        {
            ScrollDownGenericList(ListUi);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_UP))  /* UP */
        {
            ScrollUpGenericList(ListUi);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_NEXT))  /* PAGE DOWN */
        {
            ScrollPageDownGenericList(ListUi);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_PRIOR))  /* PAGE UP */
        {
            ScrollPageUpGenericList(ListUi);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;
            else
                RedrawGenericList(ListUi);
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)  /* ESC */
        {
            RestoreGenericListUiState(ListUi);
            return nextPage;    // Use some "prevPage;" instead?
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D) /* ENTER */
        {
            return nextPage;
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar > 0x60) && (Ir->Event.KeyEvent.uChar.AsciiChar < 0x7b))
        {
            /* a-z */
            GenericListKeyPress(ListUi, Ir->Event.KeyEvent.uChar.AsciiChar);
        }
    }
}


/*
 * Displays the ComputerSettingsPage.
 *
 * Next pages:
 *  DeviceSettingsPage
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
ComputerSettingsPage(PINPUT_RECORD Ir)
{
    GENERIC_LIST_UI ListUi;
    MUIDisplayPage(COMPUTER_SETTINGS_PAGE);

    InitGenericListUi(&ListUi, USetupData.ComputerList, GetSettingDescription);
    DrawGenericList(&ListUi,
                    2, 18,
                    xScreen - 3,
                    yScreen - 3);

    return HandleGenericList(&ListUi, DEVICE_SETTINGS_PAGE, Ir);
}


/*
 * Displays the DisplaySettingsPage.
 *
 * Next pages:
 *  DeviceSettingsPage
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
DisplaySettingsPage(PINPUT_RECORD Ir)
{
    GENERIC_LIST_UI ListUi;
    MUIDisplayPage(DISPLAY_SETTINGS_PAGE);

    InitGenericListUi(&ListUi, USetupData.DisplayList, GetSettingDescription);
    DrawGenericList(&ListUi,
                    2, 18,
                    xScreen - 3,
                    yScreen - 3);

    return HandleGenericList(&ListUi, DEVICE_SETTINGS_PAGE, Ir);
}


/*
 * Displays the KeyboardSettingsPage.
 *
 * Next pages:
 *  DeviceSettingsPage
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
KeyboardSettingsPage(PINPUT_RECORD Ir)
{
    GENERIC_LIST_UI ListUi;
    MUIDisplayPage(KEYBOARD_SETTINGS_PAGE);

    InitGenericListUi(&ListUi, USetupData.KeyboardList, GetSettingDescription);
    DrawGenericList(&ListUi,
                    2, 18,
                    xScreen - 3,
                    yScreen - 3);

    return HandleGenericList(&ListUi, DEVICE_SETTINGS_PAGE, Ir);
}


/*
 * Displays the LayoutSettingsPage.
 *
 * Next pages:
 *  DeviceSettingsPage
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
LayoutSettingsPage(PINPUT_RECORD Ir)
{
    GENERIC_LIST_UI ListUi;
    MUIDisplayPage(LAYOUT_SETTINGS_PAGE);

    InitGenericListUi(&ListUi, USetupData.LayoutList, GetSettingDescription);
    DrawGenericList(&ListUi,
                    2, 18,
                    xScreen - 3,
                    yScreen - 3);

    return HandleGenericList(&ListUi, DEVICE_SETTINGS_PAGE, Ir);
}


static BOOL
IsDiskSizeValid(PPARTENTRY PartEntry)
{
    ULONGLONG size;

    size = PartEntry->SectorCount.QuadPart * PartEntry->DiskEntry->BytesPerSector;
    size = (size + (512 * KB)) / MB;  /* in MBytes */

    if (size < USetupData.RequiredPartitionDiskSpace)
    {
        /* Partition is too small so ask for another one */
        DPRINT1("Partition is too small (size: %I64u MB), required disk space is %lu MB\n", size, USetupData.RequiredPartitionDiskSpace);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


/*
 * Displays the SelectPartitionPage.
 *
 * Next pages:
 *  SelectFileSystemPage (At once if unattended)
 *  SelectFileSystemPage (Default if free space is selected)
 *  CreatePrimaryPartitionPage
 *  CreateExtendedPartitionPage
 *  CreateLogicalPartitionPage
 *  ConfirmDeleteSystemPartitionPage (if the selected partition is the system partition, aka with the boot flag set)
 *  DeletePartitionPage
 *  QuitPage
 *
 * SIDEEFFECTS
 *  Set InstallShortcut (only if not unattended + free space is selected)
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
SelectPartitionPage(PINPUT_RECORD Ir)
{
    PARTLIST_UI ListUi;
    ULONG Error;

    if (PartitionList == NULL)
    {
        PartitionList = CreatePartitionList();
        if (PartitionList == NULL)
        {
            MUIDisplayError(ERROR_DRIVE_INFORMATION, Ir, POPUP_WAIT_ENTER);
            return QUIT_PAGE;
        }
        else if (IsListEmpty(&PartitionList->DiskListHead))
        {
            MUIDisplayError(ERROR_NO_HDD, Ir, POPUP_WAIT_ENTER);
            return QUIT_PAGE;
        }

        /* Reset the formatter machine state */
        TempPartition = NULL;
        FormatState = Start;
    }

    if (RepairUpdateFlag)
    {
        ASSERT(CurrentInstallation);

        /* Determine the selected installation disk & partition */
        InstallPartition = SelectPartition(PartitionList,
                                           CurrentInstallation->DiskNumber,
                                           CurrentInstallation->PartitionNumber);
        if (!InstallPartition)
        {
            DPRINT1("RepairUpdateFlag == TRUE, SelectPartition() returned FALSE, assert!\n");
            ASSERT(FALSE);
        }
        ASSERT(!IsContainerPartition(InstallPartition->PartitionType));

        return SELECT_FILE_SYSTEM_PAGE;
    }

    MUIDisplayPage(SELECT_PARTITION_PAGE);

    InitPartitionListUi(&ListUi, PartitionList,
                        CurrentPartition,
                        2, 23,
                        xScreen - 3,
                        yScreen - 3);
    DrawPartitionList(&ListUi);

    if (IsUnattendedSetup)
    {
        /* Determine the selected installation disk & partition */
        InstallPartition = SelectPartition(PartitionList,
                                           USetupData.DestinationDiskNumber,
                                           USetupData.DestinationPartitionNumber);
        if (!InstallPartition)
        {
            CurrentPartition = ListUi.CurrentPartition;

            if (USetupData.AutoPartition)
            {
                ASSERT(CurrentPartition != NULL);
                ASSERT(!IsContainerPartition(CurrentPartition->PartitionType));

                if (CurrentPartition->LogicalPartition)
                {
                    CreateLogicalPartition(PartitionList,
                                           CurrentPartition,
                                           CurrentPartition->SectorCount.QuadPart,
                                           TRUE);
                }
                else
                {
                    CreatePrimaryPartition(PartitionList,
                                           CurrentPartition,
                                           CurrentPartition->SectorCount.QuadPart,
                                           TRUE);
                }

// FIXME?? Aren't we going to enter an infinite loop, if this test fails??
                if (!IsDiskSizeValid(CurrentPartition))
                {
                    MUIDisplayError(ERROR_INSUFFICIENT_PARTITION_SIZE, Ir, POPUP_WAIT_ANY_KEY,
                                    USetupData.RequiredPartitionDiskSpace);
                    return SELECT_PARTITION_PAGE; /* let the user select another partition */
                }

                InstallPartition = CurrentPartition;
                return SELECT_FILE_SYSTEM_PAGE;
            }
        }
        else
        {
            ASSERT(!IsContainerPartition(InstallPartition->PartitionType));

            DrawPartitionList(&ListUi); // FIXME: Doesn't make much sense...

// FIXME?? Aren't we going to enter an infinite loop, if this test fails??
            if (!IsDiskSizeValid(InstallPartition))
            {
                MUIDisplayError(ERROR_INSUFFICIENT_PARTITION_SIZE, Ir, POPUP_WAIT_ANY_KEY,
                                USetupData.RequiredPartitionDiskSpace);
                return SELECT_PARTITION_PAGE; /* let the user select another partition */
            }

            return SELECT_FILE_SYSTEM_PAGE;
        }
    }

    while (TRUE)
    {
        CurrentPartition = ListUi.CurrentPartition;

        /* Update status text */
        if (CurrentPartition == NULL)
        {
            CONSOLE_SetStatusText(MUIGetString(STRING_INSTALLCREATEPARTITION));
        }
        else if (CurrentPartition->LogicalPartition)
        {
            if (CurrentPartition->IsPartitioned)
            {
                CONSOLE_SetStatusText(MUIGetString(STRING_DELETEPARTITION));
            }
            else
            {
                CONSOLE_SetStatusText(MUIGetString(STRING_INSTALLCREATELOGICAL));
            }
        }
        else
        {
            if (CurrentPartition->IsPartitioned)
            {
                if (IsContainerPartition(CurrentPartition->PartitionType))
                {
                    CONSOLE_SetStatusText(MUIGetString(STRING_DELETEPARTITION));
                }
                else
                {
                    CONSOLE_SetStatusText(MUIGetString(STRING_INSTALLDELETEPARTITION));
                }
            }
            else
            {
                CONSOLE_SetStatusText(MUIGetString(STRING_INSTALLCREATEPARTITION));
            }
        }

        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
            {
                DestroyPartitionList(PartitionList);
                PartitionList = NULL;
                return QUIT_PAGE;
            }

            break;
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_DOWN))  /* DOWN */
        {
            ScrollDownPartitionList(&ListUi);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_UP))  /* UP */
        {
            ScrollUpPartitionList(&ListUi);
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_RETURN)  /* ENTER */
        {
            ASSERT(CurrentPartition != NULL);

            if (IsContainerPartition(CurrentPartition->PartitionType))
                continue; // return SELECT_PARTITION_PAGE;

            /*
             * Check whether the user wants to install MergeOS on a disk that
             * is not recognized by the computer's firmware and if so, display
             * a warning since such disks may not be bootable.
             */
            if (CurrentPartition->DiskEntry->MediaType == FixedMedia &&
                !CurrentPartition->DiskEntry->BiosFound)
            {
                PopupError("The disk you have selected for installing MergeOS\n"
                           "is not visible by the firmware of your computer,\n"
                           "and so may not be bootable.\n"
                           "Press ENTER to continue nonetheless.",
                           MUIGetString(STRING_CONTINUE),
                           Ir, POPUP_WAIT_ENTER);
                // return SELECT_PARTITION_PAGE;
            }

            if (CurrentPartition->IsPartitioned == FALSE)
            {
                if (CurrentPartition->LogicalPartition)
                {
                    Error = LogicalPartitionCreationChecks(CurrentPartition);
                    if (Error != NOT_AN_ERROR)
                    {
                        MUIDisplayError(Error, Ir, POPUP_WAIT_ANY_KEY);
                        return SELECT_PARTITION_PAGE;
                    }

                    CreateLogicalPartition(PartitionList,
                                           CurrentPartition,
                                           0ULL,
                                           TRUE);
                }
                else
                {
                    Error = PrimaryPartitionCreationChecks(CurrentPartition);
                    if (Error != NOT_AN_ERROR)
                    {
                        MUIDisplayError(Error, Ir, POPUP_WAIT_ANY_KEY);
                        return SELECT_PARTITION_PAGE;
                    }

                    CreatePrimaryPartition(PartitionList,
                                           CurrentPartition,
                                           0ULL,
                                           TRUE);
                }
            }

            if (!IsDiskSizeValid(CurrentPartition))
            {
                MUIDisplayError(ERROR_INSUFFICIENT_PARTITION_SIZE, Ir, POPUP_WAIT_ANY_KEY,
                                USetupData.RequiredPartitionDiskSpace);
                return SELECT_PARTITION_PAGE; /* let the user select another partition */
            }

            InstallPartition = CurrentPartition;
            return SELECT_FILE_SYSTEM_PAGE;
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == 'P')  /* P */
        {
            ASSERT(CurrentPartition != NULL);

            if (CurrentPartition->LogicalPartition == FALSE)
            {
                Error = PrimaryPartitionCreationChecks(CurrentPartition);
                if (Error != NOT_AN_ERROR)
                {
                    MUIDisplayError(Error, Ir, POPUP_WAIT_ANY_KEY);
                    return SELECT_PARTITION_PAGE;
                }

                return CREATE_PRIMARY_PARTITION_PAGE;
            }
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == 'E')  /* E */
        {
            ASSERT(CurrentPartition != NULL);

            if (CurrentPartition->LogicalPartition == FALSE)
            {
                Error = ExtendedPartitionCreationChecks(CurrentPartition);
                if (Error != NOT_AN_ERROR)
                {
                    MUIDisplayError(Error, Ir, POPUP_WAIT_ANY_KEY);
                    return SELECT_PARTITION_PAGE;
                }

                return CREATE_EXTENDED_PARTITION_PAGE;
            }
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == 'L')  /* L */
        {
            ASSERT(CurrentPartition != NULL);

            if (CurrentPartition->LogicalPartition)
            {
                Error = LogicalPartitionCreationChecks(CurrentPartition);
                if (Error != NOT_AN_ERROR)
                {
                    MUIDisplayError(Error, Ir, POPUP_WAIT_ANY_KEY);
                    return SELECT_PARTITION_PAGE;
                }

                return CREATE_LOGICAL_PARTITION_PAGE;
            }
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == 'D')  /* D */
        {
            UNICODE_STRING CurrentPartitionU;
            WCHAR PathBuffer[MAX_PATH];

            ASSERT(CurrentPartition != NULL);

            if (CurrentPartition->IsPartitioned == FALSE)
            {
                MUIDisplayError(ERROR_DELETE_SPACE, Ir, POPUP_WAIT_ANY_KEY);
                return SELECT_PARTITION_PAGE;
            }

// TODO: Do something similar before trying to format the partition?
            if (!CurrentPartition->New &&
                !IsContainerPartition(CurrentPartition->PartitionType) &&
                CurrentPartition->FormatState != Unformatted)
            {
                ASSERT(CurrentPartition->PartitionNumber != 0);

                RtlStringCchPrintfW(PathBuffer, ARRAYSIZE(PathBuffer),
                        L"\\Device\\Harddisk%lu\\Partition%lu\\",
                        CurrentPartition->DiskEntry->DiskNumber,
                        CurrentPartition->PartitionNumber);
                RtlInitUnicodeString(&CurrentPartitionU, PathBuffer);

                /*
                 * Check whether the user attempts to delete the partition on which
                 * the installation source is present. If so, fail with an error.
                 */
                // &USetupData.SourceRootPath
                if (RtlPrefixUnicodeString(&CurrentPartitionU, &USetupData.SourcePath, TRUE))
                {
                    MUIDisplayError(ERROR_SOURCE_PATH, Ir, POPUP_WAIT_ENTER);
                    return SELECT_PARTITION_PAGE;
                }
            }

            if (CurrentPartition == PartitionList->SystemPartition ||
                IsPartitionActive(CurrentPartition))
            {
                return CONFIRM_DELETE_SYSTEM_PARTITION_PAGE;
            }

            return DELETE_PARTITION_PAGE;
        }
    }

    return SELECT_PARTITION_PAGE;
}


#define PARTITION_SIZE_INPUT_FIELD_LENGTH 9
/* Restriction for MaxSize */
#define PARTITION_MAXSIZE (pow(10, (PARTITION_SIZE_INPUT_FIELD_LENGTH - 1)) - 1)

static VOID
ShowPartitionSizeInputBox(SHORT Left,
                          SHORT Top,
                          SHORT Right,
                          SHORT Bottom,
                          ULONG MaxSize,
                          PWSTR InputBuffer,
                          PBOOLEAN Quit,
                          PBOOLEAN Cancel)
{
    INPUT_RECORD Ir;
    COORD coPos;
    DWORD Written;
    CHAR Buffer[128];
    INT Length, Pos;
    WCHAR ch;
    SHORT iLeft;
    SHORT iTop;

    if (Quit != NULL)
        *Quit = FALSE;

    if (Cancel != NULL)
        *Cancel = FALSE;

    DrawBox(Left, Top, Right - Left + 1, Bottom - Top + 1);

    /* Print message */
    coPos.X = Left + 2;
    coPos.Y = Top + 2;
    strcpy(Buffer, MUIGetString(STRING_PARTITIONSIZE));
    iLeft = coPos.X + (USHORT)strlen(Buffer) + 1;
    iTop = coPos.Y;

    WriteConsoleOutputCharacterA(StdOutput,
                                 Buffer,
                                 strlen(Buffer),
                                 coPos,
                                 &Written);

    sprintf(Buffer, MUIGetString(STRING_MAXSIZE), MaxSize);
    coPos.X = iLeft + PARTITION_SIZE_INPUT_FIELD_LENGTH + 1;
    coPos.Y = iTop;
    WriteConsoleOutputCharacterA(StdOutput,
                                 Buffer,
                                 strlen(Buffer),
                                 coPos,
                                 &Written);

    swprintf(InputBuffer, L"%lu", MaxSize);
    Length = wcslen(InputBuffer);
    Pos = Length;
    CONSOLE_SetInputTextXY(iLeft,
                           iTop,
                           PARTITION_SIZE_INPUT_FIELD_LENGTH,
                           InputBuffer);
    CONSOLE_SetCursorXY(iLeft + Length, iTop);
    CONSOLE_SetCursorType(TRUE, TRUE);

    while (TRUE)
    {
        CONSOLE_ConInKey(&Ir);

        if ((Ir.Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir.Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (Quit != NULL)
                *Quit = TRUE;

            InputBuffer[0] = UNICODE_NULL;
            CONSOLE_SetCursorType(TRUE, FALSE);
            break;
        }
        else if (Ir.Event.KeyEvent.wVirtualKeyCode == VK_RETURN)    /* ENTER */
        {
            CONSOLE_SetCursorType(TRUE, FALSE);
            break;
        }
        else if (Ir.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)    /* ESC */
        {
            if (Cancel != NULL)
                *Cancel = TRUE;

            InputBuffer[0] = UNICODE_NULL;
            CONSOLE_SetCursorType(TRUE, FALSE);
            break;
        }
        else if ((Ir.Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir.Event.KeyEvent.wVirtualKeyCode == VK_HOME))  /* HOME */
        {
            Pos = 0;
            CONSOLE_SetCursorXY(iLeft + Pos, iTop);
        }
        else if ((Ir.Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir.Event.KeyEvent.wVirtualKeyCode == VK_END))  /* END */
        {
            Pos = Length;
            CONSOLE_SetCursorXY(iLeft + Pos, iTop);
        }
        else if ((Ir.Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir.Event.KeyEvent.wVirtualKeyCode == VK_LEFT))  /* LEFT */
        {
            if (Pos > 0)
            {
                Pos--;
                CONSOLE_SetCursorXY(iLeft + Pos, iTop);
            }
        }
        else if ((Ir.Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT))  /* RIGHT */
        {
            if (Pos < Length)
            {
                Pos++;
                CONSOLE_SetCursorXY(iLeft + Pos, iTop);
            }
        }
        else if ((Ir.Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir.Event.KeyEvent.wVirtualKeyCode == VK_DELETE))  /* DEL */
        {
            if (Pos < Length)
            {
                memmove(&InputBuffer[Pos],
                        &InputBuffer[Pos + 1],
                        (Length - Pos - 1) * sizeof(WCHAR));
                InputBuffer[Length - 1] = UNICODE_NULL;

                Length--;
                CONSOLE_SetInputTextXY(iLeft,
                                       iTop,
                                       PARTITION_SIZE_INPUT_FIELD_LENGTH,
                                       InputBuffer);
                CONSOLE_SetCursorXY(iLeft + Pos, iTop);
            }
        }
        else if (Ir.Event.KeyEvent.wVirtualKeyCode == VK_BACK)  /* BACKSPACE */
        {
            if (Pos > 0)
            {
                if (Pos < Length)
                    memmove(&InputBuffer[Pos - 1],
                            &InputBuffer[Pos],
                            (Length - Pos) * sizeof(WCHAR));
                InputBuffer[Length - 1] = UNICODE_NULL;

                Pos--;
                Length--;
                CONSOLE_SetInputTextXY(iLeft,
                                       iTop,
                                       PARTITION_SIZE_INPUT_FIELD_LENGTH,
                                       InputBuffer);
                CONSOLE_SetCursorXY(iLeft + Pos, iTop);
            }
        }
        else if (Ir.Event.KeyEvent.uChar.AsciiChar != 0x00)
        {
            if (Length < PARTITION_SIZE_INPUT_FIELD_LENGTH - 1)
            {
                ch = (WCHAR)Ir.Event.KeyEvent.uChar.AsciiChar;

                if ((ch >= L'0') && (ch <= L'9'))
                {
                    if (Pos < Length)
                        memmove(&InputBuffer[Pos + 1],
                                &InputBuffer[Pos],
                                (Length - Pos) * sizeof(WCHAR));
                    InputBuffer[Length + 1] = UNICODE_NULL;
                    InputBuffer[Pos] = ch;

                    Pos++;
                    Length++;
                    CONSOLE_SetInputTextXY(iLeft,
                                           iTop,
                                           PARTITION_SIZE_INPUT_FIELD_LENGTH,
                                           InputBuffer);
                    CONSOLE_SetCursorXY(iLeft + Pos, iTop);
                }
            }
        }
    }
}


/*
 * Displays the CreatePrimaryPartitionPage.
 *
 * Next pages:
 *  SelectPartitionPage
 *  SelectFileSystemPage (default)
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
CreatePrimaryPartitionPage(PINPUT_RECORD Ir)
{
    PPARTENTRY PartEntry;
    PDISKENTRY DiskEntry;
    BOOLEAN Quit;
    BOOLEAN Cancel;
    ULONG MaxSize;
    ULONGLONG PartSize;
    ULONGLONG SectorCount;
    WCHAR InputBuffer[50];
    CHAR LineBuffer[100];

    if (PartitionList == NULL || CurrentPartition == NULL)
    {
        /* FIXME: show an error dialog */
        return QUIT_PAGE;
    }

    PartEntry = CurrentPartition;
    DiskEntry = CurrentPartition->DiskEntry;

    CONSOLE_SetStatusText(MUIGetString(STRING_PLEASEWAIT));

    CONSOLE_SetTextXY(6, 8, MUIGetString(STRING_CHOOSENEWPARTITION));

    DiskDescription(DiskEntry, LineBuffer, ARRAYSIZE(LineBuffer));
    CONSOLE_PrintTextXY(6, 10, MUIGetString(STRING_HDINFOPARTCREATE_1),
                        LineBuffer);

    CONSOLE_SetTextXY(6, 12, MUIGetString(STRING_HDDSIZE));

#if 0
    CONSOLE_PrintTextXY(8, 10, "Maximum size of the new partition is %I64u MB",
                        CurrentPartition->SectorCount * DiskEntry->BytesPerSector / MB);
#endif

    CONSOLE_SetStatusText(MUIGetString(STRING_CREATEPARTITION));

    PartEntry = CurrentPartition;
    while (TRUE)
    {
        MaxSize = (PartEntry->SectorCount.QuadPart * DiskEntry->BytesPerSector) / MB;  /* in MBytes (rounded) */

        if (MaxSize > PARTITION_MAXSIZE)
            MaxSize = PARTITION_MAXSIZE;

        ShowPartitionSizeInputBox(12, 14, xScreen - 12, 17, /* left, top, right, bottom */
                                  MaxSize, InputBuffer, &Quit, &Cancel);

        if (Quit)
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Cancel)
        {
            return SELECT_PARTITION_PAGE;
        }
        else
        {
            PartSize = _wcstoui64(InputBuffer, NULL, 10);

            if (PartSize < 1)
            {
                /* Too small */
                continue;
            }

            if (PartSize > MaxSize)
            {
                /* Too large */
                continue;
            }

            /* Convert to bytes */
            if (PartSize == MaxSize)
            {
                /* Use all of the unpartitioned disk space */
                SectorCount = PartEntry->SectorCount.QuadPart;
            }
            else
            {
                /* Calculate the sector count from the size in MB */
                SectorCount = PartSize * MB / DiskEntry->BytesPerSector;

                /* But never get larger than the unpartitioned disk space */
                if (SectorCount > PartEntry->SectorCount.QuadPart)
                    SectorCount = PartEntry->SectorCount.QuadPart;
            }

            DPRINT ("Partition size: %I64u bytes\n", PartSize);

            CreatePrimaryPartition(PartitionList,
                                   CurrentPartition,
                                   SectorCount,
                                   FALSE);

            return SELECT_PARTITION_PAGE;
        }
    }

    return CREATE_PRIMARY_PARTITION_PAGE;
}


/*
 * Displays the CreateExtendedPartitionPage.
 *
 * Next pages:
 *  SelectPartitionPage (default)
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
CreateExtendedPartitionPage(PINPUT_RECORD Ir)
{
    PPARTENTRY PartEntry;
    PDISKENTRY DiskEntry;
    BOOLEAN Quit;
    BOOLEAN Cancel;
    ULONG MaxSize;
    ULONGLONG PartSize;
    ULONGLONG SectorCount;
    WCHAR InputBuffer[50];
    CHAR LineBuffer[100];

    if (PartitionList == NULL || CurrentPartition == NULL)
    {
        /* FIXME: show an error dialog */
        return QUIT_PAGE;
    }

    PartEntry = CurrentPartition;
    DiskEntry = CurrentPartition->DiskEntry;

    CONSOLE_SetStatusText(MUIGetString(STRING_PLEASEWAIT));

    CONSOLE_SetTextXY(6, 8, MUIGetString(STRING_CHOOSE_NEW_EXTENDED_PARTITION));

    DiskDescription(DiskEntry, LineBuffer, ARRAYSIZE(LineBuffer));
    CONSOLE_PrintTextXY(6, 10, MUIGetString(STRING_HDINFOPARTCREATE_1),
                        LineBuffer);

    CONSOLE_SetTextXY(6, 12, MUIGetString(STRING_HDDSIZE));

#if 0
    CONSOLE_PrintTextXY(8, 10, "Maximum size of the new partition is %I64u MB",
                        CurrentPartition->SectorCount * DiskEntry->BytesPerSector / MB);
#endif

    CONSOLE_SetStatusText(MUIGetString(STRING_CREATEPARTITION));

    PartEntry = CurrentPartition;
    while (TRUE)
    {
        MaxSize = (PartEntry->SectorCount.QuadPart * DiskEntry->BytesPerSector) / MB;  /* in MBytes (rounded) */

        if (MaxSize > PARTITION_MAXSIZE)
            MaxSize = PARTITION_MAXSIZE;

        ShowPartitionSizeInputBox(12, 14, xScreen - 12, 17, /* left, top, right, bottom */
                                  MaxSize, InputBuffer, &Quit, &Cancel);

        if (Quit)
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Cancel)
        {
            return SELECT_PARTITION_PAGE;
        }
        else
        {
            PartSize = _wcstoui64(InputBuffer, NULL, 10);

            if (PartSize < 1)
            {
                /* Too small */
                continue;
            }

            if (PartSize > MaxSize)
            {
                /* Too large */
                continue;
            }

            /* Convert to bytes */
            if (PartSize == MaxSize)
            {
                /* Use all of the unpartitioned disk space */
                SectorCount = PartEntry->SectorCount.QuadPart;
            }
            else
            {
                /* Calculate the sector count from the size in MB */
                SectorCount = PartSize * MB / DiskEntry->BytesPerSector;

                /* But never get larger than the unpartitioned disk space */
                if (SectorCount > PartEntry->SectorCount.QuadPart)
                    SectorCount = PartEntry->SectorCount.QuadPart;
            }

            DPRINT ("Partition size: %I64u bytes\n", PartSize);

            CreateExtendedPartition(PartitionList,
                                    CurrentPartition,
                                    SectorCount);

            return SELECT_PARTITION_PAGE;
        }
    }

    return CREATE_EXTENDED_PARTITION_PAGE;
}


/*
 * Displays the CreateLogicalPartitionPage.
 *
 * Next pages:
 *  SelectFileSystemPage (default)
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
CreateLogicalPartitionPage(PINPUT_RECORD Ir)
{
    PPARTENTRY PartEntry;
    PDISKENTRY DiskEntry;
    BOOLEAN Quit;
    BOOLEAN Cancel;
    ULONG MaxSize;
    ULONGLONG PartSize;
    ULONGLONG SectorCount;
    WCHAR InputBuffer[50];
    CHAR LineBuffer[100];

    if (PartitionList == NULL || CurrentPartition == NULL)
    {
        /* FIXME: show an error dialog */
        return QUIT_PAGE;
    }

    PartEntry = CurrentPartition;
    DiskEntry = CurrentPartition->DiskEntry;

    CONSOLE_SetStatusText(MUIGetString(STRING_PLEASEWAIT));

    CONSOLE_SetTextXY(6, 8, MUIGetString(STRING_CHOOSE_NEW_LOGICAL_PARTITION));

    DiskDescription(DiskEntry, LineBuffer, ARRAYSIZE(LineBuffer));
    CONSOLE_PrintTextXY(6, 10, MUIGetString(STRING_HDINFOPARTCREATE_1),
                        LineBuffer);

    CONSOLE_SetTextXY(6, 12, MUIGetString(STRING_HDDSIZE));

#if 0
    CONSOLE_PrintTextXY(8, 10, "Maximum size of the new partition is %I64u MB",
                        CurrentPartition->SectorCount * DiskEntry->BytesPerSector / MB);
#endif

    CONSOLE_SetStatusText(MUIGetString(STRING_CREATEPARTITION));

    PartEntry = CurrentPartition;
    while (TRUE)
    {
        MaxSize = (PartEntry->SectorCount.QuadPart * DiskEntry->BytesPerSector) / MB;  /* in MBytes (rounded) */

        if (MaxSize > PARTITION_MAXSIZE)
            MaxSize = PARTITION_MAXSIZE;

        ShowPartitionSizeInputBox(12, 14, xScreen - 12, 17, /* left, top, right, bottom */
                                  MaxSize, InputBuffer, &Quit, &Cancel);

        if (Quit)
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Cancel)
        {
            return SELECT_PARTITION_PAGE;
        }
        else
        {
            PartSize = _wcstoui64(InputBuffer, NULL, 10);

            if (PartSize < 1)
            {
                /* Too small */
                continue;
            }

            if (PartSize > MaxSize)
            {
                /* Too large */
                continue;
            }

            /* Convert to bytes */
            if (PartSize == MaxSize)
            {
                /* Use all of the unpartitioned disk space */
                SectorCount = PartEntry->SectorCount.QuadPart;
            }
            else
            {
                /* Calculate the sector count from the size in MB */
                SectorCount = PartSize * MB / DiskEntry->BytesPerSector;

                /* But never get larger than the unpartitioned disk space */
                if (SectorCount > PartEntry->SectorCount.QuadPart)
                    SectorCount = PartEntry->SectorCount.QuadPart;
            }

            DPRINT("Partition size: %I64u bytes\n", PartSize);

            CreateLogicalPartition(PartitionList,
                                   CurrentPartition,
                                   SectorCount,
                                   FALSE);

            return SELECT_PARTITION_PAGE;
        }
    }

    return CREATE_LOGICAL_PARTITION_PAGE;
}


/*
 * Displays the ConfirmDeleteSystemPartitionPage.
 *
 * Next pages:
 *  DeletePartitionPage (default)
 *  SelectPartitionPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
ConfirmDeleteSystemPartitionPage(PINPUT_RECORD Ir)
{
    MUIDisplayPage(CONFIRM_DELETE_SYSTEM_PARTITION_PAGE);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_RETURN) /* ENTER */
        {
            return DELETE_PARTITION_PAGE;
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)  /* ESC */
        {
            return SELECT_PARTITION_PAGE;
        }
    }

    return CONFIRM_DELETE_SYSTEM_PARTITION_PAGE;
}


/*
 * Displays the DeletePartitionPage.
 *
 * Next pages:
 *  SelectPartitionPage (default)
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
DeletePartitionPage(PINPUT_RECORD Ir)
{
    PPARTENTRY PartEntry;
    PDISKENTRY DiskEntry;
    CHAR LineBuffer[100];

    if (PartitionList == NULL || CurrentPartition == NULL)
    {
        /* FIXME: show an error dialog */
        return QUIT_PAGE;
    }

    PartEntry = CurrentPartition;
    DiskEntry = CurrentPartition->DiskEntry;

    MUIDisplayPage(DELETE_PARTITION_PAGE);

    PartitionDescription(PartEntry, LineBuffer, ARRAYSIZE(LineBuffer));
    CONSOLE_PrintTextXY(6, 10, "   %s", LineBuffer);

    DiskDescription(DiskEntry, LineBuffer, ARRAYSIZE(LineBuffer));
    CONSOLE_PrintTextXY(6, 12, MUIGetString(STRING_HDINFOPARTDELETE_1),
                        LineBuffer);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)  /* ESC */
        {
            return SELECT_PARTITION_PAGE;
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == 'L') /* L */
        {
            DeletePartition(PartitionList,
                            CurrentPartition,
                            &CurrentPartition);
            return SELECT_PARTITION_PAGE;
        }
    }

    return DELETE_PARTITION_PAGE;
}


static VOID
ResetFileSystemList(VOID)
{
    if (!FileSystemList)
        return;

    DestroyFileSystemList(FileSystemList);
    FileSystemList = NULL;
}

/*
 * Displays the SelectFileSystemPage.
 *
 * Next pages:
 *  CheckFileSystemPage (At once if RepairUpdate is selected)
 *  CheckFileSystemPage (At once if Unattended and not USetupData.FormatPartition)
 *  FormatPartitionPage (At once if Unattended and USetupData.FormatPartition)
 *  SelectPartitionPage (If the user aborts)
 *  FormatPartitionPage (Default)
 *  QuitPage
 *
 * SIDEEFFECTS
 *  Calls UpdatePartitionType()
 *  Calls FindSupportedSystemPartition()
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
SelectFileSystemPage(PINPUT_RECORD Ir)
{
    PPARTENTRY PartEntry;
    PDISKENTRY DiskEntry;
    FORMATMACHINESTATE PreviousFormatState;
    PCWSTR DefaultFs;
    CHAR LineBuffer[100];

    DPRINT("SelectFileSystemPage()\n");

    if (PartitionList == NULL || InstallPartition == NULL)
    {
        /* FIXME: show an error dialog */
        return QUIT_PAGE;
    }

    /* Find or set the active system partition when starting formatting */
    if (FormatState == Start)
    {
        /*
         * If we install on a fixed disk, try to find a supported system
         * partition on the system. Otherwise if we install on a removable disk
         * use the install partition as the system partition.
         */
        if (InstallPartition->DiskEntry->MediaType == FixedMedia)
        {
            SystemPartition = FindSupportedSystemPartition(PartitionList,
                                                           FALSE,
                                                           InstallPartition->DiskEntry,
                                                           InstallPartition);
            /* Use the original system partition as the old active partition hint */
            PartEntry = PartitionList->SystemPartition;

            if ( SystemPartition && PartitionList->SystemPartition &&
                (SystemPartition != PartitionList->SystemPartition) )
            {
                DPRINT1("We are using a different system partition!!!!\n");

                MUIDisplayPage(CHANGE_SYSTEM_PARTITION);

                {
                PPARTENTRY PartEntry; // Shadow variable

                PartEntry = PartitionList->SystemPartition;
                DiskEntry = PartEntry->DiskEntry;

                PartitionDescription(PartEntry, LineBuffer, ARRAYSIZE(LineBuffer));
                CONSOLE_SetTextXY(8, 10, LineBuffer);

                DiskDescription(DiskEntry, LineBuffer, ARRAYSIZE(LineBuffer));
                CONSOLE_PrintTextXY(8, 14, MUIGetString(STRING_HDINFOPARTCREATE_1),
                                    LineBuffer);


                PartEntry = SystemPartition;
                DiskEntry = PartEntry->DiskEntry;

                PartitionDescription(PartEntry, LineBuffer, ARRAYSIZE(LineBuffer));
                CONSOLE_SetTextXY(8, 23, LineBuffer);
                }

                while (TRUE)
                {
                    CONSOLE_ConInKey(Ir);

                    if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_RETURN) /* ENTER */
                    {
                        break;
                    }
                    else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)  /* ESC */
                    {
                        return SELECT_PARTITION_PAGE;
                    }
                }

                CONSOLE_ClearScreen();
                CONSOLE_Flush();
            }
        }
        else // if (InstallPartition->DiskEntry->MediaType == RemovableMedia)
        {
            SystemPartition = InstallPartition;
            /* Don't specify any old active partition hint */
            PartEntry = NULL;
        }

        if (!SystemPartition)
        {
            /* FIXME: improve the error dialog */
            //
            // Error dialog should say that we cannot find a suitable
            // system partition and create one on the system. At this point,
            // it may be nice to ask the user whether he wants to continue,
            // or use an external drive as the system drive/partition
            // (e.g. floppy, USB drive, etc...)
            //
            PopupError("The MergeOS Setup could not find a supported system partition\n"
                       "on your system or could not create a new one. Without such partition\n"
                       "the Setup program cannot install MergeOS.\n"
                       "Press ENTER to return to the partition selection list.",
                       MUIGetString(STRING_CONTINUE),
                       Ir, POPUP_WAIT_ENTER);
            return SELECT_PARTITION_PAGE;
        }

        /*
         * If the system partition can be created in some
         * non-partitioned space, create it now.
         */
        if (!SystemPartition->IsPartitioned)
        {
            CreatePrimaryPartition(PartitionList,
                                   SystemPartition,
                                   0LL, // SystemPartition->SectorCount.QuadPart,
                                   TRUE);
            ASSERT(SystemPartition->IsPartitioned);
        }

        /* Set it as such */
        if (!SetActivePartition(PartitionList, SystemPartition, PartEntry))
        {
            DPRINT1("SetActivePartition(0x%p) failed?!\n", SystemPartition);
            ASSERT(FALSE);
        }

        /* Commit all partition changes to all the disks */
        if (!WritePartitionsToDisk(PartitionList))
        {
            DPRINT("WritePartitionsToDisk() failed\n");
            MUIDisplayError(ERROR_WRITE_PTABLE, Ir, POPUP_WAIT_ENTER);
            return QUIT_PAGE;
        }

        /*
         * In all cases, whether or not we are going to perform a formatting,
         * we must perform a filesystem check of both the system and the
         * installation partitions.
         */
        InstallPartition->NeedsCheck = TRUE;
        if (SystemPartition != InstallPartition)
            SystemPartition->NeedsCheck = TRUE;

        /*
         * In case we just repair an existing installation, or make
         * an unattended setup without formatting, just go to the
         * filesystem check step.
         */
        if (RepairUpdateFlag)
            return CHECK_FILE_SYSTEM_PAGE;

        if (IsUnattendedSetup && !USetupData.FormatPartition)
            return CHECK_FILE_SYSTEM_PAGE;
    }

    // ASSERT(SystemPartition->IsPartitioned);

    /* Reset the filesystem list for each partition that is to be formatted */
    ResetFileSystemList();

    PreviousFormatState = FormatState;
    switch (FormatState)
    {
        case Start:
        {
            /*
             * We start by formatting the system partition in case it is new
             * (it didn't exist before) and is not the same as the installation
             * partition. Otherwise we just require a filesystem check on it,
             * and start by formatting the installation partition instead.
             */

            ASSERT(SystemPartition->IsPartitioned);

            if ((SystemPartition != InstallPartition) &&
                (SystemPartition->FormatState == Unformatted))
            {
                TempPartition = SystemPartition;
                TempPartition->NeedsCheck = TRUE;

                // TODO: Should we let the user using a custom file-system,
                // or should we always use FAT(32) for it?
                // For "compatibility", FAT(32) would be best indeed.

                FormatState = FormatSystemPartition;
                DPRINT1("FormatState: Start --> FormatSystemPartition\n");
            }
            else
            {
                TempPartition = InstallPartition;
                TempPartition->NeedsCheck = TRUE;

                if (SystemPartition != InstallPartition)
                {
                    /* The system partition is separate, so it had better be formatted! */
                    ASSERT((SystemPartition->FormatState == Preformatted) ||
                           (SystemPartition->FormatState == Formatted));

                    /* Require a filesystem check on the system partition too */
                    SystemPartition->NeedsCheck = TRUE;
                }

                FormatState = FormatInstallPartition;
                DPRINT1("FormatState: Start --> FormatInstallPartition\n");
            }
            break;
        }

        case FormatSystemPartition:
        {
            TempPartition = InstallPartition;
            TempPartition->NeedsCheck = TRUE;

            FormatState = FormatInstallPartition;
            DPRINT1("FormatState: FormatSystemPartition --> FormatInstallPartition\n");
            break;
        }

        case FormatInstallPartition:
        case FormatOtherPartition:
        {
            if (GetNextUnformattedPartition(PartitionList,
                                            NULL,
                                            &TempPartition))
            {
                FormatState = FormatOtherPartition;
                TempPartition->NeedsCheck = TRUE;

                if (FormatState == FormatInstallPartition)
                    DPRINT1("FormatState: FormatInstallPartition --> FormatOtherPartition\n");
                else
                    DPRINT1("FormatState: FormatOtherPartition --> FormatOtherPartition\n");
            }
            else
            {
                FormatState = FormatDone;

                if (FormatState == FormatInstallPartition)
                    DPRINT1("FormatState: FormatInstallPartition --> FormatDone\n");
                else
                    DPRINT1("FormatState: FormatOtherPartition --> FormatDone\n");

                return CHECK_FILE_SYSTEM_PAGE;
            }
            break;
        }

        case FormatDone:
        {
            DPRINT1("FormatState: FormatDone\n");
            return CHECK_FILE_SYSTEM_PAGE;
        }

        default:
        {
            DPRINT1("FormatState: Invalid value %ld\n", FormatState);
            ASSERT(FALSE);
            return QUIT_PAGE;
        }
    }

    PartEntry = TempPartition;
    DiskEntry = TempPartition->DiskEntry;

    ASSERT(PartEntry->IsPartitioned && PartEntry->PartitionNumber != 0);

    MUIDisplayPage(SELECT_FILE_SYSTEM_PAGE);

    if (PartEntry->AutoCreate)
    {
        CONSOLE_SetTextXY(6, 8, MUIGetString(STRING_NEWPARTITION));

#if 0
        PartitionDescription(PartEntry, LineBuffer, ARRAYSIZE(LineBuffer));
        CONSOLE_SetTextXY(8, 10, LineBuffer);
#endif

        DiskDescription(DiskEntry, LineBuffer, ARRAYSIZE(LineBuffer));
        CONSOLE_PrintTextXY(8, 10, MUIGetString(STRING_HDINFOPARTCREATE_1),
                            LineBuffer);

        CONSOLE_SetTextXY(6, 12, MUIGetString(STRING_PARTFORMAT));

        PartEntry->AutoCreate = FALSE;
    }
    else if (PartEntry->New)
    {
        switch (FormatState)
        {
            case FormatSystemPartition:
                CONSOLE_SetTextXY(6, 8, MUIGetString(STRING_NONFORMATTEDSYSTEMPART));
                break;

            case FormatInstallPartition:
                CONSOLE_SetTextXY(6, 8, MUIGetString(STRING_NONFORMATTEDPART));
                break;

            case FormatOtherPartition:
                CONSOLE_SetTextXY(6, 8, MUIGetString(STRING_NONFORMATTEDOTHERPART));
                break;

            default:
                ASSERT(FALSE);
                break;
        }

        DiskDescription(DiskEntry, LineBuffer, ARRAYSIZE(LineBuffer));
        CONSOLE_PrintTextXY(8, 10, MUIGetString(STRING_HDINFOPARTCREATE_1),
                            LineBuffer);

        CONSOLE_SetTextXY(6, 12, MUIGetString(STRING_PARTFORMAT));
    }
    else
    {
        CONSOLE_SetTextXY(6, 8, MUIGetString(STRING_INSTALLONPART));

        PartitionDescription(PartEntry, LineBuffer, ARRAYSIZE(LineBuffer));
        CONSOLE_SetTextXY(8, 10, LineBuffer);

        DiskDescription(DiskEntry, LineBuffer, ARRAYSIZE(LineBuffer));
        CONSOLE_PrintTextXY(6, 12, MUIGetString(STRING_HDINFOPARTDELETE_1),
                            LineBuffer);
    }

    ASSERT(FileSystemList == NULL);

    if (IsUnattendedSetup)
    {
        ASSERT(USetupData.FormatPartition);

        switch (USetupData.FsType)
        {
            /* 1 is for BtrFS */
            case 1:
                DefaultFs = L"BTRFS";
                break;

            /* If we don't understand input, default to FAT */
            default:
                DefaultFs = L"FAT";
                break;
        }
    }
    else
    {
        /* By default select the "FAT" file system */
        DefaultFs = L"FAT";
    }

    /* Create the file system list */
    // TODO: Display only the FSes compatible with the selected partition!
    FileSystemList = CreateFileSystemList(6, 26,
                                          PartEntry->New ||
                                          PartEntry->FormatState == Unformatted,
                                          DefaultFs);
    if (FileSystemList == NULL)
    {
        /* FIXME: show an error dialog */
        return QUIT_PAGE;
    }

    if (IsUnattendedSetup)
    {
        ASSERT(USetupData.FormatPartition);
        return FORMAT_PARTITION_PAGE;
    }

    DrawFileSystemList(FileSystemList);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
            {
                /* Reset the filesystem list */
                ResetFileSystemList();
                return QUIT_PAGE;
            }

            break;
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)  /* ESC */
        {
            /* Reset the formatter machine state */
            TempPartition = NULL;
            FormatState = Start;

            /* Reset the filesystem list */
            ResetFileSystemList();

            return SELECT_PARTITION_PAGE;
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_DOWN))  /* DOWN */
        {
            ScrollDownFileSystemList(FileSystemList);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_UP))  /* UP */
        {
            ScrollUpFileSystemList(FileSystemList);
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_RETURN) /* ENTER */
        {
            if (!FileSystemList->Selected->FileSystem)
            {
                ASSERT(!TempPartition->New && TempPartition->FormatState != Unformatted);

                /*
                 * Skip formatting this partition. We will also ignore
                 * filesystem checks on it, unless it is either the system
                 * or the installation partition.
                 */
                if (TempPartition != SystemPartition &&
                    TempPartition != InstallPartition)
                {
                    PartEntry->NeedsCheck = FALSE;
                }

                return SELECT_FILE_SYSTEM_PAGE;
            }
            else
            {
                /* Format this partition */
                return FORMAT_PARTITION_PAGE;
            }
        }
    }

    FormatState = PreviousFormatState;

    return SELECT_FILE_SYSTEM_PAGE;
}


/*
 * Displays the FormatPartitionPage.
 *
 * Next pages:
 *  InstallDirectoryPage (At once if IsUnattendedSetup or InstallShortcut)
 *  SelectPartitionPage  (At once)
 *  QuitPage
 *
 * SIDEEFFECTS
 *  Sets InstallPartition->FormatState
 *  Sets USetupData.DestinationRootPath
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
FormatPartitionPage(PINPUT_RECORD Ir)
{
    NTSTATUS Status;
    PPARTENTRY PartEntry;
    PDISKENTRY DiskEntry;
    PFILE_SYSTEM_ITEM SelectedFileSystem;
    WCHAR PathBuffer[MAX_PATH];
    CHAR Buffer[MAX_PATH];

    DPRINT("FormatPartitionPage()\n");

    MUIDisplayPage(FORMAT_PARTITION_PAGE);

    if (PartitionList == NULL || TempPartition == NULL)
    {
        /* FIXME: show an error dialog */
        return QUIT_PAGE;
    }

    PartEntry = TempPartition;
    DiskEntry = TempPartition->DiskEntry;

    ASSERT(PartEntry->IsPartitioned && PartEntry->PartitionNumber != 0);

    SelectedFileSystem = FileSystemList->Selected;
    ASSERT(SelectedFileSystem && SelectedFileSystem->FileSystem);

    while (TRUE)
    {
        if (!IsUnattendedSetup)
            CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
            {
                /* Reset the filesystem list */
                ResetFileSystemList();
                return QUIT_PAGE;
            }

            break;
        }
        else if (Ir->Event.KeyEvent.wVirtualKeyCode == VK_RETURN || IsUnattendedSetup) /* ENTER */
        {
            /*
             * Remove the "Press ENTER to continue" message prompt when the ENTER
             * key is pressed as the user wants to begin the partition formatting.
             */
            MUIClearStyledText(FORMAT_PARTITION_PAGE, TEXT_ID_FORMAT_PROMPT, TEXT_TYPE_REGULAR);
            CONSOLE_SetStatusText(MUIGetString(STRING_PLEASEWAIT));

            /* Format the partition */
            Status = DoFormat(PartEntry,
                              SelectedFileSystem->FileSystem,
                              SelectedFileSystem->QuickFormat);
            if (Status == STATUS_PARTITION_FAILURE)
            {
                MUIDisplayError(ERROR_WRITE_PTABLE, Ir, POPUP_WAIT_ENTER);

                /* Reset the filesystem list */
                ResetFileSystemList();
                return QUIT_PAGE;
            }
            else if (Status == STATUS_UNRECOGNIZED_VOLUME)
            {
                /* FIXME: show an error dialog */
                // MUIDisplayError(ERROR_FORMATTING_PARTITION, Ir, POPUP_WAIT_ANY_KEY, PathBuffer);

                /* Reset the filesystem list */
                ResetFileSystemList();
                return QUIT_PAGE;
            }
            else if (Status == STATUS_NOT_SUPPORTED)
            {
                RtlStringCbPrintfA(Buffer,
                                   sizeof(Buffer),
                                   "Setup is currently unable to format a partition in %S.\n"
                                   "\n"
                                   "  \x07  Press ENTER to continue Setup.\n"
                                   "  \x07  Press F3 to quit Setup.",
                                   SelectedFileSystem->FileSystem);

                PopupError(Buffer,
                           MUIGetString(STRING_QUITCONTINUE),
                           NULL, POPUP_WAIT_NONE);

                while (TRUE)
                {
                    CONSOLE_ConInKey(Ir);

                    if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x00 &&
                        Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3)  /* F3 */
                    {
                        if (ConfirmQuit(Ir))
                        {
                            /* Reset the filesystem list */
                            ResetFileSystemList();
                            return QUIT_PAGE;
                        }
                        else
                        {
                            return SELECT_FILE_SYSTEM_PAGE;
                        }
                    }
                    else if (Ir->Event.KeyEvent.uChar.AsciiChar == VK_RETURN) /* ENTER */
                    {
                        return SELECT_FILE_SYSTEM_PAGE;
                    }
                }
            }
            else if (!NT_SUCCESS(Status))
            {
                /** HACK!! **/
                RtlStringCchPrintfW(PathBuffer, ARRAYSIZE(PathBuffer),
                                    L"\\Device\\Harddisk%lu\\Partition%lu",
                                    DiskEntry->DiskNumber,
                                    PartEntry->PartitionNumber);

                DPRINT1("FormatPartition() failed with status 0x%08lx\n", Status);
                MUIDisplayError(ERROR_FORMATTING_PARTITION, Ir, POPUP_WAIT_ANY_KEY, PathBuffer);

                /* Reset the filesystem list */
                ResetFileSystemList();
                return QUIT_PAGE;
            }

            return SELECT_FILE_SYSTEM_PAGE;
        }
    }

    return FORMAT_PARTITION_PAGE;
}


/*
 * Displays the CheckFileSystemPage.
 *
 * Next pages:
 *  InstallDirectoryPage (At once)
 *  QuitPage
 *
 * SIDEEFFECTS
 *  Inits or reloads FileSystemList
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
CheckFileSystemPage(PINPUT_RECORD Ir)
{
    NTSTATUS Status;
    PPARTENTRY PartEntry;
    CHAR Buffer[MAX_PATH];

    MUIDisplayPage(CHECK_FILE_SYSTEM_PAGE);

    if (PartitionList == NULL)
    {
        /* FIXME: show an error dialog */
        return QUIT_PAGE;
    }

    if (!GetNextUncheckedPartition(PartitionList, NULL, &PartEntry))
    {
        return INSTALL_DIRECTORY_PAGE;
    }

    ASSERT(PartEntry->IsPartitioned && PartEntry->PartitionNumber != 0);

    DPRINT1("CheckFileSystemPage -- PartitionType: 0x%02X ; FileSystem: %S\n",
            PartEntry->PartitionType, (*PartEntry->FileSystem ? PartEntry->FileSystem : L"n/a"));

    /* Check the partition */
    Status = DoChkdsk(PartEntry);
    if (Status == STATUS_NOT_SUPPORTED)
    {
        /*
         * Partition checking is not supported with the current filesystem,
         * so disable FS checks on it.
         */
        PartEntry->NeedsCheck = FALSE;

        RtlStringCbPrintfA(Buffer,
                           sizeof(Buffer),
                           "Setup is currently unable to check a partition formatted in %S.\n"
                           "\n"
                           "  \x07  Press ENTER to continue Setup.\n"
                           "  \x07  Press F3 to quit Setup.",
                           PartEntry->FileSystem);

        PopupError(Buffer,
                   MUIGetString(STRING_QUITCONTINUE),
                   NULL, POPUP_WAIT_NONE);

        while (TRUE)
        {
            CONSOLE_ConInKey(Ir);

            if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x00 &&
                Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3)  /* F3 */
            {
                if (ConfirmQuit(Ir))
                    return QUIT_PAGE;
                else
                    return CHECK_FILE_SYSTEM_PAGE;
            }
            else if (Ir->Event.KeyEvent.uChar.AsciiChar == VK_RETURN) /* ENTER */
            {
                return CHECK_FILE_SYSTEM_PAGE;
            }
        }
    }
    else if (!NT_SUCCESS(Status))
    {
        DPRINT1("ChkdskPartition() failed with status 0x%08lx\n", Status);

        RtlStringCbPrintfA(Buffer,
                           sizeof(Buffer),
                           "ChkDsk detected some disk errors.\n(Status 0x%08lx).\n",
                           Status);

        PopupError(Buffer,
                   MUIGetString(STRING_CONTINUE),
                   Ir, POPUP_WAIT_ENTER);
    }

    PartEntry->NeedsCheck = FALSE;
    return CHECK_FILE_SYSTEM_PAGE;
}


static BOOLEAN
IsValidPath(
    IN PCWSTR InstallDir)
{
    UINT i, Length;

    Length = wcslen(InstallDir);

    // TODO: Add check for 8.3 too.

    /* Path must be at least 2 characters long */
//    if (Length < 2)
//        return FALSE;

    /* Path must start with a backslash */
//    if (InstallDir[0] != L'\\')
//        return FALSE;

    /* Path must not end with a backslash */
    if (InstallDir[Length - 1] == L'\\')
        return FALSE;

    /* Path must not contain whitespace characters */
    for (i = 0; i < Length; i++)
    {
        if (iswspace(InstallDir[i]))
            return FALSE;
    }

    /* Path component must not end with a dot */
    for (i = 0; i < Length; i++)
    {
        if (InstallDir[i] == L'\\' && i > 0)
        {
            if (InstallDir[i - 1] == L'.')
                return FALSE;
        }
    }

    if (InstallDir[Length - 1] == L'.')
        return FALSE;

    return TRUE;
}


/*
 * Displays the InstallDirectoryPage.
 *
 * Next pages:
 *  PrepareCopyPage
 *  QuitPage
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
InstallDirectoryPage(PINPUT_RECORD Ir)
{
    NTSTATUS Status;
    ULONG Length, Pos;
    WCHAR c;
    WCHAR InstallDir[MAX_PATH];

    /* We do not need the filesystem list anymore */
    ResetFileSystemList();

    if (PartitionList == NULL || InstallPartition == NULL)
    {
        /* FIXME: show an error dialog */
        return QUIT_PAGE;
    }

    // if (IsUnattendedSetup)
    if (RepairUpdateFlag)
        wcscpy(InstallDir, CurrentInstallation->PathComponent); // SystemNtPath
    else if (USetupData.InstallationDirectory[0])
        wcscpy(InstallDir, USetupData.InstallationDirectory);
    else
        wcscpy(InstallDir, L"\\MergeOS");

    /*
     * Check the validity of the predefined 'InstallDir'. If we are either
     * in unattended setup or in update/repair mode, and the installation path
     * is valid, just perform the installation. Otherwise (either in the case
     * of an invalid path, or we are in regular setup), display the UI and allow
     * the user to specify a new installation path.
     */
    if ((RepairUpdateFlag || IsUnattendedSetup) && IsValidPath(InstallDir))
    {
        Status = InitDestinationPaths(&USetupData, InstallDir, InstallPartition);
        if (!NT_SUCCESS(Status))
        {
            DPRINT1("InitDestinationPaths() failed. Status code: 0x%lx", Status);
            MUIDisplayError(ERROR_NO_BUILD_PATH, Ir, POPUP_WAIT_ENTER);
            return QUIT_PAGE;
        }

        /*
         * Check whether the user attempts to install MergeOS within the
         * installation source directory, or in a subdirectory thereof.
         * If so, fail with an error.
         */
        if (RtlPrefixUnicodeString(&USetupData.SourcePath, &USetupData.DestinationPath, TRUE))
        {
            MUIDisplayError(ERROR_SOURCE_DIR, Ir, POPUP_WAIT_ENTER);
            return INSTALL_DIRECTORY_PAGE;
        }

        return PREPARE_COPY_PAGE;
    }

    Length = wcslen(InstallDir);
    Pos = Length;

    MUIDisplayPage(INSTALL_DIRECTORY_PAGE);
    CONSOLE_SetInputTextXY(8, 11, 51, InstallDir);
    CONSOLE_SetCursorXY(8 + Pos, 11);
    CONSOLE_SetCursorType(TRUE, TRUE);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            CONSOLE_SetCursorType(TRUE, FALSE);

            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            CONSOLE_SetCursorType(TRUE, TRUE);
            break;
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_DELETE))  /* DEL */
        {
            if (Pos < Length)
            {
                memmove(&InstallDir[Pos],
                        &InstallDir[Pos + 1],
                        (Length - Pos - 1) * sizeof(WCHAR));
                InstallDir[Length - 1] = UNICODE_NULL;

                Length--;
                CONSOLE_SetInputTextXY(8, 11, 51, InstallDir);
                CONSOLE_SetCursorXY(8 + Pos, 11);
            }
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_HOME))  /* HOME */
        {
            Pos = 0;
            CONSOLE_SetCursorXY(8 + Pos, 11);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_END))  /* END */
        {
            Pos = Length;
            CONSOLE_SetCursorXY(8 + Pos, 11);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_LEFT))  /* LEFT */
        {
            if (Pos > 0)
            {
                Pos--;
                CONSOLE_SetCursorXY(8 + Pos, 11);
            }
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_RIGHT))  /* RIGHT */
        {
            if (Pos < Length)
            {
                Pos++;
                CONSOLE_SetCursorXY(8 + Pos, 11);
            }
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D) /* ENTER */
        {
            CONSOLE_SetCursorType(TRUE, FALSE);

            /*
             * Check for the validity of the installation directory and pop up
             * an error if it is not the case. Then the user can fix its input.
             */
            if (!IsValidPath(InstallDir))
            {
                MUIDisplayError(ERROR_DIRECTORY_NAME, Ir, POPUP_WAIT_ENTER);
                return INSTALL_DIRECTORY_PAGE;
            }

            Status = InitDestinationPaths(&USetupData, InstallDir, InstallPartition);
            if (!NT_SUCCESS(Status))
            {
                DPRINT1("InitDestinationPaths() failed. Status code: 0x%lx", Status);
                MUIDisplayError(ERROR_NO_BUILD_PATH, Ir, POPUP_WAIT_ENTER);
                return QUIT_PAGE;
            }

            /*
             * Check whether the user attempts to install MergeOS within the
             * installation source directory, or in a subdirectory thereof.
             * If so, fail with an error.
             */
            if (RtlPrefixUnicodeString(&USetupData.SourcePath, &USetupData.DestinationPath, TRUE))
            {
                MUIDisplayError(ERROR_SOURCE_DIR, Ir, POPUP_WAIT_ENTER);
                return INSTALL_DIRECTORY_PAGE;
            }

            return PREPARE_COPY_PAGE;
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x08) /* BACKSPACE */
        {
            if (Pos > 0)
            {
                if (Pos < Length)
                    memmove(&InstallDir[Pos - 1],
                            &InstallDir[Pos],
                            (Length - Pos) * sizeof(WCHAR));
                InstallDir[Length - 1] = UNICODE_NULL;

                Pos--;
                Length--;
                CONSOLE_SetInputTextXY(8, 11, 51, InstallDir);
                CONSOLE_SetCursorXY(8 + Pos, 11);
            }
        }
        else if (isprint(Ir->Event.KeyEvent.uChar.AsciiChar))
        {
            if (Length < 50)
            {
                c = (WCHAR)Ir->Event.KeyEvent.uChar.AsciiChar;
                if (iswalpha(c) || iswdigit(c) || c == '.' || c == '\\' || c == '-' || c == '_')
                {
                    if (Pos < Length)
                        memmove(&InstallDir[Pos + 1],
                                &InstallDir[Pos],
                                (Length - Pos) * sizeof(WCHAR));
                    InstallDir[Length + 1] = UNICODE_NULL;
                    InstallDir[Pos] = c;

                    Pos++;
                    Length++;
                    CONSOLE_SetInputTextXY(8, 11, 51, InstallDir);
                    CONSOLE_SetCursorXY(8 + Pos, 11);
                }
            }
        }
    }

    return INSTALL_DIRECTORY_PAGE;
}


// PSETUP_ERROR_ROUTINE
static VOID
__cdecl
USetupErrorRoutine(
    IN PUSETUP_DATA pSetupData,
    ...)
{
    INPUT_RECORD Ir;
    va_list arg_ptr;

    va_start(arg_ptr, pSetupData);

    if (pSetupData->LastErrorNumber >= ERROR_SUCCESS &&
        pSetupData->LastErrorNumber <  ERROR_LAST_ERROR_CODE)
    {
        // Note: the "POPUP_WAIT_ENTER" actually depends on the LastErrorNumber...
        MUIDisplayErrorV(pSetupData->LastErrorNumber, &Ir, POPUP_WAIT_ENTER, arg_ptr);
    }

    va_end(arg_ptr);
}

/*
 * Displays the PrepareCopyPage.
 *
 * Next pages:
 *  FileCopyPage(At once)
 *  QuitPage
 *
 * SIDEEFFECTS
 * Calls PrepareFileCopy
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
PrepareCopyPage(PINPUT_RECORD Ir)
{
    // ERROR_NUMBER ErrorNumber;
    BOOLEAN Success;

    MUIDisplayPage(PREPARE_COPY_PAGE);

    /* ErrorNumber = */ Success = PrepareFileCopy(&USetupData, NULL);
    if (/*ErrorNumber != ERROR_SUCCESS*/ !Success)
    {
        // MUIDisplayError(ErrorNumber, Ir, POPUP_WAIT_ENTER);
        return QUIT_PAGE;
    }

    return FILE_COPY_PAGE;
}

typedef struct _COPYCONTEXT
{
    ULONG TotalOperations;
    ULONG CompletedOperations;
    PPROGRESSBAR ProgressBar;
    PPROGRESSBAR MemoryBars[4];
} COPYCONTEXT, *PCOPYCONTEXT;

static VOID
SetupUpdateMemoryInfo(IN PCOPYCONTEXT CopyContext,
                      IN BOOLEAN First)
{
    SYSTEM_PERFORMANCE_INFORMATION PerfInfo;

    /* Get the memory information from the system */
    NtQuerySystemInformation(SystemPerformanceInformation,
                             &PerfInfo,
                             sizeof(PerfInfo),
                             NULL);

    /* Check if this is initial setup */
    if (First)
    {
        /* Set maximum limits to be total RAM pages */
        ProgressSetStepCount(CopyContext->MemoryBars[0], PerfInfo.CommitLimit);
        ProgressSetStepCount(CopyContext->MemoryBars[1], PerfInfo.CommitLimit);
        ProgressSetStepCount(CopyContext->MemoryBars[2], PerfInfo.CommitLimit);
    }

    /* Set current values */
    ProgressSetStep(CopyContext->MemoryBars[0], PerfInfo.PagedPoolPages + PerfInfo.NonPagedPoolPages);
    ProgressSetStep(CopyContext->MemoryBars[1], PerfInfo.ResidentSystemCachePage);
    ProgressSetStep(CopyContext->MemoryBars[2], PerfInfo.AvailablePages);
}

static UINT
CALLBACK
FileCopyCallback(PVOID Context,
                 UINT Notification,
                 UINT_PTR Param1,
                 UINT_PTR Param2)
{
    PCOPYCONTEXT CopyContext = (PCOPYCONTEXT)Context;
    PFILEPATHS_W FilePathInfo;
    PCWSTR SrcFileName, DstFileName;

    switch (Notification)
    {
        case SPFILENOTIFY_STARTSUBQUEUE:
        {
            CopyContext->TotalOperations = (ULONG)Param2;
            CopyContext->CompletedOperations = 0;
            ProgressSetStepCount(CopyContext->ProgressBar,
                                 CopyContext->TotalOperations);
            SetupUpdateMemoryInfo(CopyContext, TRUE);
            break;
        }

        case SPFILENOTIFY_STARTDELETE:
        case SPFILENOTIFY_STARTRENAME:
        case SPFILENOTIFY_STARTCOPY:
        {
            FilePathInfo = (PFILEPATHS_W)Param1;

            if (Notification == SPFILENOTIFY_STARTDELETE)
            {
                /* Display delete message */
                ASSERT(Param2 == FILEOP_DELETE);

                DstFileName = wcsrchr(FilePathInfo->Target, L'\\');
                if (DstFileName) ++DstFileName;
                else DstFileName = FilePathInfo->Target;

                CONSOLE_SetStatusText(MUIGetString(STRING_DELETING),
                                      DstFileName);
            }
            else if (Notification == SPFILENOTIFY_STARTRENAME)
            {
                /* Display move/rename message */
                ASSERT(Param2 == FILEOP_RENAME);

                SrcFileName = wcsrchr(FilePathInfo->Source, L'\\');
                if (SrcFileName) ++SrcFileName;
                else SrcFileName = FilePathInfo->Source;

                DstFileName = wcsrchr(FilePathInfo->Target, L'\\');
                if (DstFileName) ++DstFileName;
                else DstFileName = FilePathInfo->Target;

                if (!wcsicmp(SrcFileName, DstFileName))
                    Param2 = STRING_MOVING;
                else
                    Param2 = STRING_RENAMING;

                CONSOLE_SetStatusText(MUIGetString(Param2),
                                      SrcFileName, DstFileName);
            }
            else if (Notification == SPFILENOTIFY_STARTCOPY)
            {
                static PCSTR s_pszCopying = NULL; /* Cached for speed */

                /* Display copy message */
                ASSERT(Param2 == FILEOP_COPY);

                /* NOTE: When extracting from CABs the Source is the CAB name */
                DstFileName = wcsrchr(FilePathInfo->Target, L'\\');
                if (DstFileName) ++DstFileName;
                else DstFileName = FilePathInfo->Target;

                if (!s_pszCopying)
                    s_pszCopying = MUIGetString(STRING_COPYING);
                CONSOLE_SetStatusText(s_pszCopying, DstFileName);
#ifdef __MERGEOS__ /* HACK */
                DoWatchDestFileName(DstFileName);
#endif
            }

            SetupUpdateMemoryInfo(CopyContext, FALSE);
            break;
        }

        case SPFILENOTIFY_COPYERROR:
        {
            FilePathInfo = (PFILEPATHS_W)Param1;

            DPRINT1("An error happened while trying to copy file '%S' (error 0x%08lx), skipping it...\n",
                    FilePathInfo->Target, FilePathInfo->Win32Error);
            return FILEOP_SKIP;
        }

        case SPFILENOTIFY_ENDDELETE:
        case SPFILENOTIFY_ENDRENAME:
        case SPFILENOTIFY_ENDCOPY:
        {
            CopyContext->CompletedOperations++;

            /* SYSREG checkpoint */
            if (CopyContext->TotalOperations >> 1 == CopyContext->CompletedOperations)
                DPRINT1("CHECKPOINT:HALF_COPIED\n");

            ProgressNextStep(CopyContext->ProgressBar);
            SetupUpdateMemoryInfo(CopyContext, FALSE);
            break;
        }
    }

    return FILEOP_DOIT;
}


/*
 * Displays the FileCopyPage.
 *
 * Next pages:
 *  RegistryPage(At once)
 *
 * SIDEEFFECTS
 *  Calls DoFileCopy
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
FileCopyPage(PINPUT_RECORD Ir)
{
    COPYCONTEXT CopyContext;
    UINT MemBarWidth;

    MUIDisplayPage(FILE_COPY_PAGE);

    /* Create context for the copy process */
    CopyContext.TotalOperations = 0;
    CopyContext.CompletedOperations = 0;

    /* Create the progress bar as well */
    CopyContext.ProgressBar = CreateProgressBar(13,
                                                26,
                                                xScreen - 13,
                                                yScreen - 20,
                                                10,
                                                24,
                                                TRUE,
                                                MUIGetString(STRING_SETUPCOPYINGFILES));

    // fit memory bars to screen width, distribute them uniform
    MemBarWidth = (xScreen - 26) / 5;
    MemBarWidth -= MemBarWidth % 2;  // make even
    /* ATTENTION: The following progress bars are debug stuff, which should not be translated!! */
    /* Create the paged pool progress bar */
    CopyContext.MemoryBars[0] = CreateProgressBar(13,
                                                  40,
                                                  13 + MemBarWidth,
                                                  43,
                                                  13,
                                                  44,
                                                  FALSE,
                                                  "Kernel Pool");

    /* Create the non paged pool progress bar */
    CopyContext.MemoryBars[1] = CreateProgressBar((xScreen / 2)- (MemBarWidth / 2),
                                                  40,
                                                  (xScreen / 2) + (MemBarWidth / 2),
                                                  43,
                                                  (xScreen / 2)- (MemBarWidth / 2),
                                                  44,
                                                  FALSE,
                                                  "Kernel Cache");

    /* Create the global memory progress bar */
    CopyContext.MemoryBars[2] = CreateProgressBar(xScreen - 13 - MemBarWidth,
                                                  40,
                                                  xScreen - 13,
                                                  43,
                                                  xScreen - 13 - MemBarWidth,
                                                  44,
                                                  FALSE,
                                                  "Free Memory");

    /* Do the file copying */
    DoFileCopy(&USetupData, FileCopyCallback, &CopyContext);

    /* If we get here, we're done, so cleanup the progress bar */
    DestroyProgressBar(CopyContext.ProgressBar);
    DestroyProgressBar(CopyContext.MemoryBars[0]);
    DestroyProgressBar(CopyContext.MemoryBars[1]);
    DestroyProgressBar(CopyContext.MemoryBars[2]);

    /* Create the $winnt$.inf file */
    InstallSetupInfFile(&USetupData);

    /* Go display the next page */
    return REGISTRY_PAGE;
}


static VOID
__cdecl
RegistryStatus(IN REGISTRY_STATUS RegStatus, ...)
{
    /* WARNING: Please keep this lookup table in sync with the resources! */
    static const UINT StringIDs[] =
    {
        STRING_DONE,                    /* Success */
        STRING_REGHIVEUPDATE,           /* RegHiveUpdate */
        STRING_IMPORTFILE,              /* ImportRegHive */
        STRING_DISPLAYSETTINGSUPDATE,   /* DisplaySettingsUpdate */
        STRING_LOCALESETTINGSUPDATE,    /* LocaleSettingsUpdate */
        STRING_ADDKBLAYOUTS,            /* KeybLayouts */
        STRING_KEYBOARDSETTINGSUPDATE,  /* KeybSettingsUpdate */
        STRING_CODEPAGEINFOUPDATE,      /* CodePageInfoUpdate */
    };

    va_list args;

    if (RegStatus < ARRAYSIZE(StringIDs))
    {
        va_start(args, RegStatus);
        CONSOLE_SetStatusTextV(MUIGetString(StringIDs[RegStatus]), args);
        va_end(args);
    }
    else
    {
        CONSOLE_SetStatusText("Unknown status %d", RegStatus);
    }
}

/*
 * Displays the RegistryPage.
 *
 * Next pages:
 *  SuccessPage (if RepairUpdate)
 *  BootLoaderPage (default)
 *  QuitPage
 *
 * SIDEEFFECTS
 *  Calls UpdateRegistry
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
RegistryPage(PINPUT_RECORD Ir)
{
    ULONG Error;

    MUIDisplayPage(REGISTRY_PAGE);

    Error = UpdateRegistry(&USetupData,
                           RepairUpdateFlag,
                           PartitionList,
                           InstallPartition->DriveLetter,
                           SelectedLanguageId,
                           RegistryStatus,
                           &s_SubstSettings);
    if (Error != ERROR_SUCCESS)
    {
        MUIDisplayError(Error, Ir, POPUP_WAIT_ENTER);
        return QUIT_PAGE;
    }
    else
    {
        CONSOLE_SetStatusText(MUIGetString(STRING_DONE));
        return BOOT_LOADER_PAGE;
    }
}


/*
 * Displays the BootLoaderPage.
 *
 * Next pages:
 *  SuccessPage (if RepairUpdate)
 *  BootLoaderHarddiskMbrPage
 *  BootLoaderHarddiskVbrPage
 *  BootLoaderFloppyPage
 *  SuccessPage
 *  QuitPage
 *
 * SIDEEFFECTS
 *  Calls RegInitializeRegistry
 *  Calls ImportRegistryFile
 *  Calls SetDefaultPagefile
 *  Calls SetMountedDeviceValues
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
BootLoaderPage(PINPUT_RECORD Ir)
{
    USHORT Line = 12;
    WCHAR PathBuffer[MAX_PATH];

    CONSOLE_SetStatusText(MUIGetString(STRING_PLEASEWAIT));

    /* We must have a supported system partition by now */
    ASSERT(SystemPartition && SystemPartition->IsPartitioned && SystemPartition->PartitionNumber != 0);

    RtlFreeUnicodeString(&USetupData.SystemRootPath);
    RtlStringCchPrintfW(PathBuffer, ARRAYSIZE(PathBuffer),
            L"\\Device\\Harddisk%lu\\Partition%lu\\",
            SystemPartition->DiskEntry->DiskNumber,
            SystemPartition->PartitionNumber);
    RtlCreateUnicodeString(&USetupData.SystemRootPath, PathBuffer);
    DPRINT1("SystemRootPath: %wZ\n", &USetupData.SystemRootPath);

    /* For unattended setup, skip MBR installation or install on floppy if needed */
    if (IsUnattendedSetup)
    {
        if ((USetupData.MBRInstallType == 0) ||
            (USetupData.MBRInstallType == 1))
        {
            goto Quit;
        }
    }

    /*
     * We may install an MBR or VBR, but before that, check whether
     * we need to actually install the VBR on floppy/removable media
     * if the system partition is not recognized.
     */
    if ((SystemPartition->DiskEntry->DiskStyle != PARTITION_STYLE_MBR) ||
        !IsRecognizedPartition(SystemPartition->PartitionType))
    {
        USetupData.MBRInstallType = 1;
        goto Quit;
    }

    /* Is it an unattended install on hdd? */
    if (IsUnattendedSetup)
    {
        if ((USetupData.MBRInstallType == 2) ||
            (USetupData.MBRInstallType == 3))
        {
            goto Quit;
        }
    }

    MUIDisplayPage(BOOT_LOADER_PAGE);
    CONSOLE_InvertTextXY(8, Line, 60, 1);

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_DOWN))  /* DOWN */
        {
            CONSOLE_NormalTextXY(8, Line, 60, 1);

            Line++;
            if (Line < 12)
                Line = 15;

            if (Line > 15)
                Line = 12;

            CONSOLE_InvertTextXY(8, Line, 60, 1);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_UP))  /* UP */
        {
            CONSOLE_NormalTextXY(8, Line, 60, 1);

            Line--;
            if (Line < 12)
                Line = 15;

            if (Line > 15)
                Line = 12;

            CONSOLE_InvertTextXY(8, Line, 60, 1);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_HOME))  /* HOME */
        {
            CONSOLE_NormalTextXY(8, Line, 60, 1);

            Line = 12;

            CONSOLE_InvertTextXY(8, Line, 60, 1);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_END))  /* END */
        {
            CONSOLE_NormalTextXY(8, Line, 60, 1);

            Line = 15;

            CONSOLE_InvertTextXY(8, Line, 60, 1);
        }
        else if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
                 (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D)    /* ENTER */
        {
            if (Line == 12)
            {
                /* Install on both MBR and VBR */
                USetupData.MBRInstallType = 2;
                break;
            }
            else if (Line == 13)
            {
                /* Install on VBR only */
                USetupData.MBRInstallType = 3;
                break;
            }
            else if (Line == 14)
            {
                /* Install on floppy */
                USetupData.MBRInstallType = 1;
                break;
            }
            else if (Line == 15)
            {
                /* Skip MBR installation */
                USetupData.MBRInstallType = 0;
                break;
            }

            return BOOT_LOADER_PAGE;
        }
    }

Quit:
    switch (USetupData.MBRInstallType)
    {
        /* Skip MBR installation */
        case 0:
            return SUCCESS_PAGE;

        /* Install on floppy */
        case 1:
            return BOOT_LOADER_FLOPPY_PAGE;

        /* Install on both MBR and VBR or VBR only */
        case 2:
        case 3:
            return BOOT_LOADER_INSTALLATION_PAGE;
    }

    return BOOT_LOADER_PAGE;
}


/*
 * Displays the BootLoaderFloppyPage.
 *
 * Next pages:
 *  SuccessPage (At once)
 *  QuitPage
 *
 * SIDEEFFECTS
 *  Calls InstallFatBootcodeToFloppy()
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
BootLoaderFloppyPage(PINPUT_RECORD Ir)
{
    NTSTATUS Status;

    MUIDisplayPage(BOOT_LOADER_FLOPPY_PAGE);

//  CONSOLE_SetStatusText(MUIGetString(STRING_PLEASEWAIT));

    while (TRUE)
    {
        CONSOLE_ConInKey(Ir);

        if ((Ir->Event.KeyEvent.uChar.AsciiChar == 0x00) &&
            (Ir->Event.KeyEvent.wVirtualKeyCode == VK_F3))  /* F3 */
        {
            if (ConfirmQuit(Ir))
                return QUIT_PAGE;

            break;
        }
        else if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D)    /* ENTER */
        {
            Status = InstallFatBootcodeToFloppy(&USetupData.SourceRootPath,
                                                &USetupData.DestinationArcPath);
            if (!NT_SUCCESS(Status))
            {
                if (Status == STATUS_DEVICE_NOT_READY)
                    MUIDisplayError(ERROR_NO_FLOPPY, Ir, POPUP_WAIT_ENTER);

                /* TODO: Print error message */
                return BOOT_LOADER_FLOPPY_PAGE;
            }

            return SUCCESS_PAGE;
        }
    }

    return BOOT_LOADER_FLOPPY_PAGE;
}


/*
 * Displays the BootLoaderInstallationPage.
 *
 * Next pages:
 *  SuccessPage (At once)
 *  QuitPage
 *
 * SIDEEFFECTS
 *  Calls InstallVBRToPartition() if VBR installation is chosen.
 *  Otherwise both InstallVBRToPartition() and InstallMbrBootCodeToDisk()
 *  are called if both MBR and VBR installation is chosen.
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
BootLoaderInstallationPage(PINPUT_RECORD Ir)
{
    NTSTATUS Status;
    WCHAR DestinationDevicePathBuffer[MAX_PATH];

    MUIDisplayPage(BOOT_LOADER_INSTALLATION_PAGE);

    if (USetupData.MBRInstallType == 2)
    {
        /* Step 1: Write the VBR */
        Status = InstallVBRToPartition(&USetupData.SystemRootPath,
                                       &USetupData.SourceRootPath,
                                       &USetupData.DestinationArcPath,
                                       SystemPartition->FileSystem);
        if (!NT_SUCCESS(Status))
        {
            MUIDisplayError(ERROR_WRITE_BOOT, Ir, POPUP_WAIT_ENTER,
                            SystemPartition->FileSystem);
            return QUIT_PAGE;
        }

        /* Step 2: Write the MBR if the disk containing the system partition is not a super-floppy */
        if (!IsSuperFloppy(SystemPartition->DiskEntry))
        {
            RtlStringCchPrintfW(DestinationDevicePathBuffer, ARRAYSIZE(DestinationDevicePathBuffer),
                                L"\\Device\\Harddisk%d\\Partition0",
                                SystemPartition->DiskEntry->DiskNumber);
            Status = InstallMbrBootCodeToDisk(&USetupData.SystemRootPath,
                                              &USetupData.SourceRootPath,
                                              DestinationDevicePathBuffer);
            if (!NT_SUCCESS(Status))
            {
                DPRINT1("InstallMbrBootCodeToDisk() failed (Status %lx)\n", Status);
                MUIDisplayError(ERROR_INSTALL_BOOTCODE, Ir, POPUP_WAIT_ENTER, L"MBR");
                return QUIT_PAGE;
            }
        }
    }
    else
    {
        Status = InstallVBRToPartition(&USetupData.SystemRootPath,
                                       &USetupData.SourceRootPath,
                                       &USetupData.DestinationArcPath,
                                       SystemPartition->FileSystem);
        if (!NT_SUCCESS(Status))
        {
            MUIDisplayError(ERROR_WRITE_BOOT, Ir, POPUP_WAIT_ENTER,
                            SystemPartition->FileSystem);
            return QUIT_PAGE;
        }
    }

    return SUCCESS_PAGE;
}


/**
 * @name ProgressTimeOutStringHandler
 *
 * Handles the generation (displaying) of the timeout
 * countdown to the screen dynamically.
 *
 * @param   Bar
 *     A pointer to a progress bar.
 *
 * @param   AlwaysUpdate
 *     Constantly update the progress bar (boolean type).
 *
 * @param   Buffer
 *     A pointer to a string buffer.
 *
 * @param   cchBufferSize
 *     The buffer's size in number of characters.
 *
 * @return
 *     TRUE or FALSE on function termination.
 *
 */
static
BOOLEAN NTAPI
ProgressTimeOutStringHandler(
    IN PPROGRESSBAR Bar,
    IN BOOLEAN AlwaysUpdate,
    OUT PSTR Buffer,
    IN SIZE_T cchBufferSize)
{
    ULONG OldProgress = Bar->Progress;

    if (Bar->StepCount == 0)
    {
        Bar->Progress = 0;
    }
    else
    {
        Bar->Progress = Bar->StepCount - Bar->CurrentStep;
    }

    /* Build the progress string if it has changed */
    if (Bar->ProgressFormatText &&
        (AlwaysUpdate || (Bar->Progress != OldProgress)))
    {
        RtlStringCchPrintfA(Buffer, cchBufferSize,
                            Bar->ProgressFormatText, Bar->Progress / max(1, Bar->Width) + 1);

        return TRUE;
    }

    return FALSE;
}

/**
 * @name ProgressCountdown
 *
 * Displays and draws a red-coloured progress bar with a countdown.
 * When the timeout is reached, the flush page is displayed for reboot.
 *
 * @param   Ir
 *     A pointer to an input keyboard record.
 *
 * @param   TimeOut
 *     Initial countdown value in seconds.
 *
 * @return
 *     Nothing.
 *
 */
static VOID
ProgressCountdown(
    IN PINPUT_RECORD Ir,
    IN LONG TimeOut)
{
    NTSTATUS Status;
    ULONG StartTime, BarWidth, TimerDiv;
    LONG TimeElapsed;
    LONG TimerValue, OldTimerValue;
    LARGE_INTEGER Timeout;
    PPROGRESSBAR ProgressBar;
    BOOLEAN RefreshProgress = TRUE;

    /* Bail out if the timeout is already zero */
    if (TimeOut <= 0)
        return;

    /* Create the timeout progress bar and set it up */
    ProgressBar = CreateProgressBarEx(13,
                                      26,
                                      xScreen - 13,
                                      yScreen - 20,
                                      10,
                                      24,
                                      TRUE,
                                      FOREGROUND_RED | BACKGROUND_BLUE,
                                      0,
                                      NULL,
                                      MUIGetString(STRING_REBOOTPROGRESSBAR),
                                      ProgressTimeOutStringHandler);

    BarWidth = max(1, ProgressBar->Width);
    TimerValue = TimeOut * BarWidth;
    ProgressSetStepCount(ProgressBar, TimerValue);

    StartTime = NtGetTickCount();
    CONSOLE_Flush();

    TimerDiv = 1000 / BarWidth;
    TimerDiv = max(1, TimerDiv);
    OldTimerValue = TimerValue;
    while (TRUE)
    {
        /* Decrease the timer */

        /*
         * Compute how much time the previous operations took.
         * This allows us in particular to take account for any time
         * elapsed if something slowed down.
         */
        TimeElapsed = NtGetTickCount() - StartTime;
        if (TimeElapsed >= TimerDiv)
        {
            /* Increase StartTime by steps of 1 / ProgressBar->Width seconds */
            TimeElapsed /= TimerDiv;
            StartTime += (TimerDiv * TimeElapsed);

            if (TimeElapsed <= TimerValue)
                TimerValue -= TimeElapsed;
            else
                TimerValue = 0;

            RefreshProgress = TRUE;
        }

        if (RefreshProgress)
        {
            ProgressSetStep(ProgressBar, OldTimerValue - TimerValue);
            RefreshProgress = FALSE;
        }

        /* Stop when the timer reaches zero */
        if (TimerValue <= 0)
            break;

        /* Check for user key presses */

        /*
         * If the timer is used, use a passive wait of maximum 1 second
         * while monitoring for incoming console input events, so that
         * we are still able to display the timing count.
         */

        /* Wait a maximum of 1 second for input events */
        TimeElapsed = NtGetTickCount() - StartTime;
        if (TimeElapsed < TimerDiv)
        {
            /* Convert the time to NT format */
            Timeout.QuadPart = (TimerDiv - TimeElapsed) * -10000LL;
            Status = NtWaitForSingleObject(StdInput, FALSE, &Timeout);
        }
        else
        {
            Status = STATUS_TIMEOUT;
        }

        /* Check whether the input event has been signaled, or a timeout happened */
        if (Status == STATUS_TIMEOUT)
        {
            continue;
        }
        if (Status != STATUS_WAIT_0)
        {
            /* An error happened, bail out */
            DPRINT1("NtWaitForSingleObject() failed, Status 0x%08lx\n", Status);
            break;
        }

        /* Check for an ENTER key press */
        while (CONSOLE_ConInKeyPeek(Ir))
        {
            if (Ir->Event.KeyEvent.uChar.AsciiChar == 0x0D) /* ENTER */
            {
                /* Found it, stop waiting */
                goto Exit;
            }
        }
    }

Exit:
    /* Destroy the progress bar and quit */
    DestroyProgressBar(ProgressBar);
}


/*
 * Displays the QuitPage.
 *
 * Next pages:
 *  FlushPage (At once)
 *
 * SIDEEFFECTS
 *  Destroy the Lists
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
QuitPage(PINPUT_RECORD Ir)
{
    MUIDisplayPage(QUIT_PAGE);

    /* Destroy the NTOS installations list */
    if (NtOsInstallsList != NULL)
    {
        DestroyGenericList(NtOsInstallsList, TRUE);
        NtOsInstallsList = NULL;
    }

    /* Destroy the partition list */
    if (PartitionList != NULL)
    {
        DestroyPartitionList(PartitionList);
        PartitionList = NULL;
    }

    /* Reset the formatter machine state */
    TempPartition = NULL;
    FormatState = Start;

    /* Destroy the filesystem list */
    ResetFileSystemList();

    CONSOLE_SetStatusText(MUIGetString(STRING_REBOOTCOMPUTER2));

    /* Wait for maximum 15 seconds or an ENTER key before quitting */
    ProgressCountdown(Ir, 15);
    return FLUSH_PAGE;
}


/*
 * Displays the SuccessPage.
 *
 * Next pages:
 *  FlushPage (At once)
 *
 * SIDEEFFECTS
 *  Destroy the Lists
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
SuccessPage(PINPUT_RECORD Ir)
{
    MUIDisplayPage(SUCCESS_PAGE);

    if (IsUnattendedSetup)
        return FLUSH_PAGE;

    /* Wait for maximum 15 seconds or an ENTER key before quitting */
    ProgressCountdown(Ir, 15);
    return FLUSH_PAGE;
}


/*
 * Displays the FlushPage.
 *
 * Next pages:
 *  RebootPage (At once)
 *
 * RETURNS
 *   Number of the next page.
 */
static PAGE_NUMBER
FlushPage(PINPUT_RECORD Ir)
{
    MUIDisplayPage(FLUSH_PAGE);
    return REBOOT_PAGE;
}


/*
 * The start routine and page management
 */
NTSTATUS
RunUSetup(VOID)
{
    NTSTATUS Status;
    INPUT_RECORD Ir;
    PAGE_NUMBER Page;
    BOOLEAN Old;

    InfSetHeap(ProcessHeap);

    /* Tell the Cm this is a setup boot, and it has to behave accordingly */
    Status = NtInitializeRegistry(CM_BOOT_FLAG_SETUP);
    if (!NT_SUCCESS(Status))
        DPRINT1("NtInitializeRegistry() failed (Status 0x%08lx)\n", Status);

    /* Initialize the user-mode PnP manager */
    Status = InitializeUserModePnpManager(&USetupData.SetupInf);
    if (!NT_SUCCESS(Status))
    {
        // PrintString(??);
        DPRINT1("The user-mode PnP manager could not initialize (Status 0x%08lx), expect unavailable devices!\n", Status);
    }

    if (!CONSOLE_Init())
    {
        PrintString(MUIGetString(STRING_CONSOLEFAIL1));
        PrintString(MUIGetString(STRING_CONSOLEFAIL2));
        PrintString(MUIGetString(STRING_CONSOLEFAIL3));

        /* We failed to initialize the video, just quit the installer */
        return STATUS_APP_INIT_FAILURE;
    }

    /* Initialize Setup, phase 0 */
    InitializeSetup(&USetupData, 0);
    USetupData.ErrorRoutine = USetupErrorRoutine;

    /* Hide the cursor and clear the screen and keyboard buffer */
    CONSOLE_SetCursorType(TRUE, FALSE);
    CONSOLE_ClearScreen();
    CONSOLE_Flush();

    /* Global Initialization page */
    Page = SetupStartPage(&Ir);

    while (Page != REBOOT_PAGE && Page != RECOVERY_PAGE)
    {
        CONSOLE_ClearScreen();
        CONSOLE_Flush();

        // CONSOLE_SetUnderlinedTextXY(4, 3, " MergeOS " KERNEL_VERSION_STR " Setup ");

        switch (Page)
        {
            /* Language page */
            case LANGUAGE_PAGE:
                Page = LanguagePage(&Ir);
                break;

            /* Welcome page */
            case WELCOME_PAGE:
                Page = WelcomePage(&Ir);
                break;

            /* License page */
            case LICENSE_PAGE:
                Page = LicensePage(&Ir);
                break;

            /* Install pages */
            case INSTALL_INTRO_PAGE:
                Page = InstallIntroPage(&Ir);
                break;

#if 0
            case SCSI_CONTROLLER_PAGE:
                Page = ScsiControllerPage(&Ir);
                break;

            case OEM_DRIVER_PAGE:
                Page = OemDriverPage(&Ir);
                break;
#endif

            case DEVICE_SETTINGS_PAGE:
                Page = DeviceSettingsPage(&Ir);
                break;

            case COMPUTER_SETTINGS_PAGE:
                Page = ComputerSettingsPage(&Ir);
                break;

            case DISPLAY_SETTINGS_PAGE:
                Page = DisplaySettingsPage(&Ir);
                break;

            case KEYBOARD_SETTINGS_PAGE:
                Page = KeyboardSettingsPage(&Ir);
                break;

            case LAYOUT_SETTINGS_PAGE:
                Page = LayoutSettingsPage(&Ir);
                break;

            /* Partitioning pages */
            case SELECT_PARTITION_PAGE:
                Page = SelectPartitionPage(&Ir);
                break;

            case CREATE_PRIMARY_PARTITION_PAGE:
                Page = CreatePrimaryPartitionPage(&Ir);
                break;

            case CREATE_EXTENDED_PARTITION_PAGE:
                Page = CreateExtendedPartitionPage(&Ir);
                break;

            case CREATE_LOGICAL_PARTITION_PAGE:
                Page = CreateLogicalPartitionPage(&Ir);
                break;

            case CONFIRM_DELETE_SYSTEM_PARTITION_PAGE:
                Page = ConfirmDeleteSystemPartitionPage(&Ir);
                break;

            case DELETE_PARTITION_PAGE:
                Page = DeletePartitionPage(&Ir);
                break;

            /* Filesystem partition operations pages */
            case SELECT_FILE_SYSTEM_PAGE:
                Page = SelectFileSystemPage(&Ir);
                break;

            case FORMAT_PARTITION_PAGE:
                Page = FormatPartitionPage(&Ir);
                break;

            case CHECK_FILE_SYSTEM_PAGE:
                Page = CheckFileSystemPage(&Ir);
                break;

            /* Installation pages */
            case INSTALL_DIRECTORY_PAGE:
                Page = InstallDirectoryPage(&Ir);
                break;

            case PREPARE_COPY_PAGE:
                Page = PrepareCopyPage(&Ir);
                break;

            case FILE_COPY_PAGE:
                Page = FileCopyPage(&Ir);
                break;

            case REGISTRY_PAGE:
                Page = RegistryPage(&Ir);
                break;

            /* Bootloader installation pages */
            case BOOT_LOADER_PAGE:
                Page = BootLoaderPage(&Ir);
                break;

            case BOOT_LOADER_FLOPPY_PAGE:
                Page = BootLoaderFloppyPage(&Ir);
                break;

            case BOOT_LOADER_INSTALLATION_PAGE:
                Page = BootLoaderInstallationPage(&Ir);
                break;

            /* Repair pages */
            case REPAIR_INTRO_PAGE:
                Page = RepairIntroPage(&Ir);
                break;

            case UPGRADE_REPAIR_PAGE:
                Page = UpgradeRepairPage(&Ir);
                break;

            case SUCCESS_PAGE:
                Page = SuccessPage(&Ir);
                break;

            case FLUSH_PAGE:
                Page = FlushPage(&Ir);
                break;

            case QUIT_PAGE:
                Page = QuitPage(&Ir);
                break;

            /* Virtual pages */
            case SETUP_INIT_PAGE:
            case REBOOT_PAGE:
            case RECOVERY_PAGE:
                break;

            default:
                break;
        }
    }

    /* Terminate the user-mode PnP manager */
    TerminateUserModePnpManager();

    /* Setup has finished */
    FinishSetup(&USetupData);

    if (Page == RECOVERY_PAGE)
        RecoveryConsole();

    FreeConsole();

    /* Reboot */
    RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &Old);
    NtShutdownSystem(ShutdownReboot);
    RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, Old, FALSE, &Old);

    return STATUS_SUCCESS;
}


VOID NTAPI
NtProcessStartup(PPEB Peb)
{
    NTSTATUS Status;
    LARGE_INTEGER Time;

    RtlNormalizeProcessParams(Peb->ProcessParameters);

    ProcessHeap = Peb->ProcessHeap;

    NtQuerySystemTime(&Time);

    Status = RunUSetup();

    if (NT_SUCCESS(Status))
    {
        /*
         * Avoid a bugcheck if RunUSetup() finishes too quickly by implementing
         * a protective waiting.
         * This wait is needed because, since we are started as SMSS.EXE,
         * the NT kernel explicitly waits 5 seconds for the initial process
         * SMSS.EXE to initialize (as a protective measure), and otherwise
         * bugchecks with the code SESSION5_INITIALIZATION_FAILED.
         */
        Time.QuadPart += 50000000;
        NtDelayExecution(FALSE, &Time);
    }
    else
    {
        /* The installer failed to start: raise a hard error (crash the system/BSOD) */
        Status = NtRaiseHardError(STATUS_SYSTEM_PROCESS_TERMINATED,
                                  0, 0, NULL, 0, NULL);
    }

    NtTerminateProcess(NtCurrentProcess(), Status);
}

/* EOF */
