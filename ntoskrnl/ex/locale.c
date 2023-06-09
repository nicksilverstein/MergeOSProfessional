/*
 * PROJECT:         MergeOS Kernel
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            ntoskrnl/ex/locale.c
 * PURPOSE:         Locale (Language) Support for the Executive
 * PROGRAMMERS:     Alex Ionescu (alex.ionescu@reactos.org)
 *                  Eric Kohl
 *                  Thomas Weidenmueller (w3seek@reactos.org
 */

/* INCLUDES ******************************************************************/

#include <ntoskrnl.h>
#define NDEBUG
#include <debug.h>

/* GLOBALS *******************************************************************/

/* System IDs: EN_US */
LCID PsDefaultSystemLocaleId = 0x00000409;
LANGID PsInstallUILanguageId = LANGIDFROMLCID(0x00000409);

/* UI/Thread IDs: Same as system */
LCID PsDefaultThreadLocaleId = 0x00000409;
LANGID PsDefaultUILanguageId = LANGIDFROMLCID(0x00000409);

/* PRIVATE FUNCTIONS *********************************************************/

NTSTATUS
NTAPI
ExpGetCurrentUserUILanguage(IN PCWSTR MuiName,
                            OUT LANGID* LanguageId)
{
    UCHAR ValueBuffer[256];
    PKEY_VALUE_PARTIAL_INFORMATION ValueInfo;
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING KeyName =
        RTL_CONSTANT_STRING(L"Control Panel\\Desktop");
    UNICODE_STRING ValueName;
    UNICODE_STRING ValueString;
    ULONG ValueLength;
    ULONG Value;
    HANDLE UserKey;
    HANDLE KeyHandle;
    NTSTATUS Status;
    PAGED_CODE();

    /* Setup the key name */
    RtlInitUnicodeString(&ValueName, MuiName);

    /* Open the use key */
    Status = RtlOpenCurrentUser(KEY_READ, &UserKey);
    if (!NT_SUCCESS(Status)) return Status;

    /* Initialize the attributes and open the key */
    InitializeObjectAttributes(&ObjectAttributes,
                               &KeyName,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               UserKey,
                               NULL);
    Status = ZwOpenKey(&KeyHandle, KEY_QUERY_VALUE,&ObjectAttributes);
    if (NT_SUCCESS(Status))
    {
        /* Set buffer and query the current value */
        ValueInfo = (PKEY_VALUE_PARTIAL_INFORMATION)ValueBuffer;
        Status = ZwQueryValueKey(KeyHandle,
                                 &ValueName,
                                 KeyValuePartialInformation,
                                 ValueBuffer,
                                 sizeof(ValueBuffer),
                                 &ValueLength);
        if (NT_SUCCESS(Status))
        {
            /* Success, is the value the right type? */
            if (ValueInfo->Type == REG_SZ)
            {
                /* It is. Initialize the data and convert it */
                RtlInitUnicodeString(&ValueString, (PWSTR)ValueInfo->Data);
                Status = RtlUnicodeStringToInteger(&ValueString, 16, &Value);
                if (NT_SUCCESS(Status))
                {
                    /* Return the language */
                    *LanguageId = (USHORT)Value;
                }
            }
            else
            {
                /* Fail */
                Status = STATUS_UNSUCCESSFUL;
            }
        }

        /* Close the key */
        ZwClose(KeyHandle);
    }

    /* Close the user key and return */
    ZwClose(UserKey);
    return Status;
}

NTSTATUS
NTAPI
ExpSetCurrentUserUILanguage(IN PCWSTR MuiName,
                            IN LANGID LanguageId)
{
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING KeyName = RTL_CONSTANT_STRING(L"Control Panel\\Desktop");
    UNICODE_STRING ValueName;
    WCHAR ValueBuffer[8];
    ULONG ValueLength;
    HANDLE UserHandle;
    HANDLE KeyHandle;
    NTSTATUS Status;
    PAGED_CODE();

    /* Setup the key name */
    RtlInitUnicodeString(&ValueName, MuiName);

    /* Open the use key */
    Status = RtlOpenCurrentUser(KEY_WRITE, &UserHandle);
    if (!NT_SUCCESS(Status)) return Status;

    /* Initialize the attributes */
    InitializeObjectAttributes(&ObjectAttributes,
                               &KeyName,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               UserHandle,
                               NULL);

    /* Open the key */
    Status = ZwOpenKey(&KeyHandle, KEY_SET_VALUE, &ObjectAttributes);
    if (NT_SUCCESS(Status))
    {
        /* Setup the value name */
        ValueLength = swprintf(ValueBuffer,
                               L"%04lX",
                               (ULONG)LanguageId);

        /* Set the length for the call and set the value */
        ValueLength = (ValueLength + 1) * sizeof(WCHAR);
        Status = ZwSetValueKey(KeyHandle,
                               &ValueName,
                               0,
                               REG_SZ,
                               ValueBuffer,
                               ValueLength);

        /* Close the handle for this key */
        ZwClose(KeyHandle);
    }

    /* Close the user key and return status */
    ZwClose(UserHandle);
    return Status;
}

/* PUBLIC FUNCTIONS **********************************************************/

NTSTATUS
NTAPI
NtQueryDefaultLocale(IN BOOLEAN UserProfile,
                     OUT PLCID DefaultLocaleId)
{
    NTSTATUS Status = STATUS_SUCCESS;
    PAGED_CODE();

    /* Enter SEH for probing */
    _SEH2_TRY
    {
        /* Check if we came from user mode */
        if (KeGetPreviousMode() != KernelMode)
        {
            /* Probe the language ID */
            ProbeForWriteLangId(DefaultLocaleId);
        }

        /* Check if we have a user profile */
        if (UserProfile)
        {
            /* Return session wide thread locale */
            *DefaultLocaleId = MmGetSessionLocaleId();
        }
        else
        {
            /* Return system locale */
            *DefaultLocaleId = PsDefaultSystemLocaleId;
        }
    }
    _SEH2_EXCEPT(ExSystemExceptionFilter())
    {
        /* Get exception code */
        Status = _SEH2_GetExceptionCode();
    }
    _SEH2_END;

    /* Return status */
    return Status;
}

NTSTATUS
NTAPI
NtSetDefaultLocale(IN BOOLEAN UserProfile,
                   IN LCID DefaultLocaleId)
{
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING KeyName;
    UNICODE_STRING ValueName;
    UNICODE_STRING LocaleString;
    HANDLE KeyHandle;
    ULONG ValueLength;
    WCHAR ValueBuffer[20];
    HANDLE UserKey;
    NTSTATUS Status;
    UCHAR KeyValueBuffer[256];
    PKEY_VALUE_PARTIAL_INFORMATION KeyValueInformation;
    PAGED_CODE();

    /* Check if we have a profile */
    if (UserProfile)
    {
        /* Open the user's key */
        Status = RtlOpenCurrentUser(KEY_WRITE, &UserKey);
        if (!NT_SUCCESS(Status)) return Status;

        /* Initialize the registry location */
        RtlInitUnicodeString(&KeyName, L"Control Panel\\International");
        RtlInitUnicodeString(&ValueName, L"Locale");
    }
    else
    {
        /* Initialize the system registry location */
        RtlInitUnicodeString(&KeyName,
                             L"\\Registry\\Machine\\System\\CurrentControlSet"
                             L"\\Control\\Nls\\Language");
        RtlInitUnicodeString(&ValueName, L"Default");
        UserKey = NULL;
    }

    /* Initialize the object attributes */
    InitializeObjectAttributes(&ObjectAttributes,
                               &KeyName,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               UserKey,
                               NULL);

    /* Check if we don't have a default locale yet */
    if (!DefaultLocaleId)
    {
        /* Open the key for reading */
        Status = ZwOpenKey(&KeyHandle, KEY_QUERY_VALUE, &ObjectAttributes);
        if (!NT_SUCCESS(Status))
        {
            KeyHandle = NULL;
            goto Cleanup;
        }

        /* Query the key value */
        KeyValueInformation = (PKEY_VALUE_PARTIAL_INFORMATION)KeyValueBuffer;
        Status = ZwQueryValueKey(KeyHandle,
                                 &ValueName,
                                 KeyValuePartialInformation,
                                 KeyValueInformation,
                                 sizeof(KeyValueBuffer),
                                 &ValueLength);
        if (!NT_SUCCESS(Status))
        {
            goto Cleanup;
        }

        /* Check if this is a REG_DWORD */
        if ((KeyValueInformation->Type == REG_DWORD) &&
            (KeyValueInformation->DataLength == sizeof(ULONG)))
        {
            /* It contains the LCID as a DWORD */
            DefaultLocaleId = *((ULONG*)KeyValueInformation->Data);
        }
        /* Otherwise check for a REG_SZ */
        else if (KeyValueInformation->Type == REG_SZ)
        {
            /* Initialize a unicode string from the value data */
            LocaleString.Buffer = (PWCHAR)KeyValueInformation->Data;
            LocaleString.Length = (USHORT)KeyValueInformation->DataLength;
            LocaleString.MaximumLength = LocaleString.Length;

            /* Convert the hex string to a number */
            RtlUnicodeStringToInteger(&LocaleString, 16, &DefaultLocaleId);
        }
        else
        {
            Status = STATUS_UNSUCCESSFUL;
        }
    }
    else
    {
        /* Otherwise, open the key */
        Status = ZwOpenKey(&KeyHandle, KEY_SET_VALUE, &ObjectAttributes);
        if (NT_SUCCESS(Status))
        {
            /* Check if we had a profile */
            if (UserProfile)
            {
                /* Fill in the buffer */
                ValueLength = swprintf(ValueBuffer,
                                       L"%08lx",
                                       (ULONG)DefaultLocaleId);
            }
            else
            {
                /* Fill in the buffer */
                ValueLength = swprintf(ValueBuffer,
                                       L"%04lx",
                                       (ULONG)DefaultLocaleId & 0xFFFF);
            }

            /* Set the length for the registry call */
            ValueLength = (ValueLength + 1) * sizeof(WCHAR);

            /* Now write the actual value */
            Status = ZwSetValueKey(KeyHandle,
                                   &ValueName,
                                   0,
                                   REG_SZ,
                                   ValueBuffer,
                                   ValueLength);
        }
    }

Cleanup:

    /* Close the locale key */
    if (KeyHandle)
    {
        ObCloseHandle(KeyHandle, KernelMode);
    }

    /* Close the user key */
    if (UserKey)
    {
        ObCloseHandle(UserKey, KernelMode);
    }

    /* Check for success */
    if (NT_SUCCESS(Status))
    {
        /* Check if it was for a user */
        if (UserProfile)
        {
            /* Set the session wide thread locale */
            MmSetSessionLocaleId(DefaultLocaleId);
        }
        else
        {
            /* Set system locale */
            PsDefaultSystemLocaleId = DefaultLocaleId;
        }
    }

    /* Return status */
    return Status;
}

/*
 * @implemented
 */
NTSTATUS
NTAPI
NtQueryInstallUILanguage(OUT LANGID* LanguageId)
{
    NTSTATUS Status = STATUS_SUCCESS;
    PAGED_CODE();

    /* Enter SEH for probing */
    _SEH2_TRY
    {
        /* Check if we came from user mode */
        if (KeGetPreviousMode() != KernelMode)
        {
            /* Probe the Language ID */
            ProbeForWriteLangId(LanguageId);
        }

        /* Return it */
        *LanguageId = PsInstallUILanguageId;
    }
    _SEH2_EXCEPT(ExSystemExceptionFilter())
    {
        /* Get exception code */
        Status = _SEH2_GetExceptionCode();
    }
    _SEH2_END;

    /* Return status */
    return Status;
}

/*
 * @implemented
 */
NTSTATUS
NTAPI
NtQueryDefaultUILanguage(OUT LANGID* LanguageId)
{
    NTSTATUS Status;
    LANGID SafeLanguageId;
    PAGED_CODE();

    /* Call the executive helper routine */
    Status = ExpGetCurrentUserUILanguage(L"MultiUILanguageId", &SafeLanguageId);

    /* Enter SEH for probing */
    _SEH2_TRY
    {
        /* Check if we came from user mode */
        if (KeGetPreviousMode() != KernelMode)
        {
            /* Probe the Language ID */
            ProbeForWriteLangId(LanguageId);
        }

        if (NT_SUCCESS(Status))
        {
            /* Success, return the language */
            *LanguageId = SafeLanguageId;
        }
        else
        {
            /* Failed, use fallback value */
            // NOTE: Windows doesn't use PsDefaultUILanguageId.
            *LanguageId = PsInstallUILanguageId;
        }
    }
    _SEH2_EXCEPT(ExSystemExceptionFilter())
    {
        /* Return exception code */
        _SEH2_YIELD(return _SEH2_GetExceptionCode());
    }
    _SEH2_END;

    /* Return success */
    return STATUS_SUCCESS;
}

/*
 * @implemented
 */
NTSTATUS
NTAPI
NtSetDefaultUILanguage(IN LANGID LanguageId)
{
    NTSTATUS Status;
    PAGED_CODE();

    /* Check if the caller specified a language id */
    if (LanguageId)
    {
        /* Set the pending MUI language id */
        Status = ExpSetCurrentUserUILanguage(L"MUILanguagePending", LanguageId);
    }
    else
    {
        /* Otherwise get the pending MUI language id */
        Status = ExpGetCurrentUserUILanguage(L"MUILanguagePending", &LanguageId);
        if (!NT_SUCCESS(Status))
        {
            return Status;
        }

        /* And apply it as actual */
        Status = ExpSetCurrentUserUILanguage(L"MultiUILanguageId", LanguageId);
    }

    return Status;
}

/* EOF */
