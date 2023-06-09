/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         File Management IFS Utility functions
 * FILE:            mergeos/dll/win32/fmifs/format.c
 * PURPOSE:         Volume format
 *
 * PROGRAMMERS:     Emanuele Aliberti
 *                  Herv� Poussineau (hpoussin@reactos.org)
 */

#include "precomp.h"

#define NDEBUG
#include <debug.h>

/* FMIFS.6 */
VOID NTAPI
Format(
    IN PWCHAR DriveRoot,
    IN FMIFS_MEDIA_FLAG MediaFlag,
    IN PWCHAR Format,
    IN PWCHAR Label,
    IN BOOLEAN QuickFormat,
    IN PFMIFSCALLBACK Callback)
{
    FormatEx(DriveRoot,
             MediaFlag,
             Format,
             Label,
             QuickFormat,
             0,
             Callback);
}

/* FMIFS.7 */
VOID
NTAPI
FormatEx(
    IN PWCHAR DriveRoot,
    IN FMIFS_MEDIA_FLAG MediaFlag,
    IN PWCHAR Format,
    IN PWCHAR Label,
    IN BOOLEAN QuickFormat,
    IN ULONG ClusterSize,
    IN PFMIFSCALLBACK Callback)
{
    PIFS_PROVIDER Provider;
    UNICODE_STRING usDriveRoot;
    UNICODE_STRING usLabel;
    BOOLEAN Success = FALSE;
    BOOLEAN BackwardCompatible = FALSE; // Default to latest FS versions.
    MEDIA_TYPE MediaType;
    WCHAR VolumeName[MAX_PATH];
    //CURDIR CurDir;

//
// TODO: Convert filesystem Format into ULIB format string.
//

    Provider = GetProvider(Format);
    if (!Provider)
    {
        /* Unknown file system */
        Callback(DONE, 0, &Success);
        return;
    }

#if 1
    DPRINT1("Warning: use GetVolumeNameForVolumeMountPointW() instead!\n");
    swprintf(VolumeName, L"\\??\\%c:", towupper(DriveRoot[0]));
    RtlCreateUnicodeString(&usDriveRoot, VolumeName);
    /* Code disabled as long as our storage stack doesn't understand IOCTL_MOUNTDEV_QUERY_DEVICE_NAME */
#else
    if (!GetVolumeNameForVolumeMountPointW(DriveRoot, VolumeName, RTL_NUMBER_OF(VolumeName)) ||
        !RtlDosPathNameToNtPathName_U(VolumeName, &usDriveRoot, NULL, &CurDir))
    {
        /* Report an error */
        Callback(DONE, 0, &Success);
        return;
    }
#endif

    RtlInitUnicodeString(&usLabel, Label);

    /* Set the BackwardCompatible flag in case we format with older FAT12/16 */
    if (wcsicmp(Format, L"FAT") == 0)
        BackwardCompatible = TRUE;
    // else if (wcsicmp(Format, L"FAT32") == 0)
        // BackwardCompatible = FALSE;

    /* Convert the FMIFS MediaFlag to a NT MediaType */
    // FIXME: Actually covert all the possible flags.
    switch (MediaFlag)
    {
    case FMIFS_FLOPPY:
        MediaType = F5_320_1024; // FIXME: This is hardfixed!
        break;
    case FMIFS_REMOVABLE:
        MediaType = RemovableMedia;
        break;
    case FMIFS_HARDDISK:
        MediaType = FixedMedia;
        break;
    default:
        DPRINT1("Unknown FMIFS MediaFlag %d, converting 1-to-1 to NT MediaType\n",
                MediaFlag);
        MediaType = (MEDIA_TYPE)MediaFlag;
        break;
    }

    DPRINT("Format() - %S\n", Format);
    Success = Provider->Format(&usDriveRoot,
                               Callback,
                               QuickFormat,
                               BackwardCompatible,
                               MediaType,
                               &usLabel,
                               ClusterSize);
    if (!Success)
        DPRINT1("Format() failed\n");

    /* Report success */
    Callback(DONE, 0, &Success);

    RtlFreeUnicodeString(&usDriveRoot);
}

/* EOF */
