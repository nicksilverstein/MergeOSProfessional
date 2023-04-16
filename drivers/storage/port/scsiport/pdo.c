/*
 * PROJECT:     MergeOS Storage Stack / SCSIPORT storage port library
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Logical Unit (PDO) functions
 * COPYRIGHT:   Eric Kohl (eric.kohl@reactos.org)
 *              Aleksey Bragin (aleksey@reactos.org)
 *              2020 Victor Perevertkin (victor.perevertkin@reactos.org)
 */

#include "scsiport.h"
#include "scsitypes.h"

#define NDEBUG
#include <debug.h>


PDEVICE_OBJECT
PdoCreateLunDevice(
    _In_ PSCSI_PORT_DEVICE_EXTENSION DeviceExtension)
{
    PSCSI_PORT_LUN_EXTENSION LunExtension;
    PDEVICE_OBJECT LunPDO;

    ULONG LunExtensionSize = DeviceExtension->LunExtensionSize + sizeof(SCSI_PORT_LUN_EXTENSION);

    NTSTATUS Status = IoCreateDevice(DeviceExtension->Common.DeviceObject->DriverObject,
                                     LunExtensionSize,
                                     NULL,
                                     FILE_DEVICE_DISK,
                                     FILE_AUTOGENERATED_DEVICE_NAME | FILE_DEVICE_SECURE_OPEN,
                                     FALSE,
                                     &LunPDO);

    if (!NT_SUCCESS(Status))
    {
        DPRINT1("Failed to create a Lun PDO, status: %x\n", Status);
        return NULL;
    }

    LunExtension = LunPDO->DeviceExtension;

    /* Zero everything */
    RtlZeroMemory(LunExtension, LunExtensionSize);

    LunExtension->Common.IsFDO = FALSE;
    LunExtension->Common.DeviceObject = LunPDO;
    LunExtension->Common.LowerDevice = DeviceExtension->Common.DeviceObject;

    /* Initialize a list of requests */
    InitializeListHead(&LunExtension->SrbInfo.Requests);

    /* Initialize timeout counter */
    LunExtension->RequestTimeout = -1;

    /* Set maximum queue size */
    LunExtension->MaxQueueCount = 256;

    /* Initialize request queue */
    KeInitializeDeviceQueue(&LunExtension->DeviceQueue);

    LunPDO->Flags |= DO_DIRECT_IO;
    LunPDO->Flags &= ~DO_DEVICE_INITIALIZING;

    return LunPDO;
}

PSCSI_PORT_LUN_EXTENSION
GetLunByPath(
    _In_ PSCSI_PORT_DEVICE_EXTENSION DeviceExtension,
    _In_ UCHAR PathId,
    _In_ UCHAR TargetId,
    _In_ UCHAR Lun)
{
    if (PathId >= DeviceExtension->NumberOfBuses)
    {
        DPRINT1("Invalid PathId: %u\n", PathId);
        return NULL;
    }

    PSCSI_BUS_INFO bus = &DeviceExtension->Buses[PathId];

    for (PLIST_ENTRY lunEntry = bus->LunsListHead.Flink;
         lunEntry != &bus->LunsListHead;
         lunEntry = lunEntry->Flink)
    {
        PSCSI_PORT_LUN_EXTENSION lunExt =
            CONTAINING_RECORD(lunEntry, SCSI_PORT_LUN_EXTENSION, LunEntry);

        if (lunExt->PathId == PathId &&
            lunExt->TargetId == TargetId &&
            lunExt->Lun == Lun)
        {
            return lunExt;
        }
    }

    DPRINT("SCSI LUN (%u, %u, %u) was not found\n", PathId, TargetId, Lun);
    return NULL;
}

PSCSI_REQUEST_BLOCK_INFO
SpiGetSrbData(
    _In_ PSCSI_PORT_DEVICE_EXTENSION DeviceExtension,
    _In_ PSCSI_PORT_LUN_EXTENSION LunExtension,
    _In_ UCHAR QueueTag)
{
    if (QueueTag == SP_UNTAGGED)
    {
        /* Return the pointer to SrbInfo */
        return &LunExtension->SrbInfo;
    }
    else
    {
        /* Make sure the tag is valid, if it is - return the data */
        if (QueueTag > DeviceExtension->SrbDataCount || QueueTag < 1)
            return NULL;
        else
            return &DeviceExtension->SrbInfo[QueueTag -1];
    }
}

static
ULONG
CopyField(
    IN PUCHAR Name,
    IN PCHAR Buffer,
    IN ULONG MaxLength,
    IN CHAR DefaultCharacter,
    IN BOOLEAN Trim)
{
    ULONG Index;

    for (Index = 0; Index < MaxLength; Index++)
    {
        if (Name[Index] <= ' ' || Name[Index] >= 0x7F /* last printable ascii character */ ||  Name[Index] == ',')
        {
            // convert to underscore
            Buffer[Index] = DefaultCharacter;
        }
        else
        {
            // just copy character
            Buffer[Index] = Name[Index];
        }
    }

    /* Trim trailing default characters */
    if (Trim)
    {
        Index = MaxLength - 1;
        for (;;)
        {
            if (Buffer[Index] != DefaultCharacter)
            {
                Index++;
                break;
            }

            Index--;
        }
    }

    return Index;
}

static
NTSTATUS
PdoHandleQueryDeviceText(
    _In_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp)
{
    PSCSI_PORT_LUN_EXTENSION DeviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION IoStack;
    UINT32 Offset = 0;
    PINQUIRYDATA InquiryData;
    CHAR LocalBuffer[64];
    ANSI_STRING AnsiString;
    UNICODE_STRING DeviceDescription;

    DPRINT("PdoHandleQueryDeviceText\n");

    IoStack = IoGetCurrentIrpStackLocation(Irp);

    InquiryData = &DeviceExtension->InquiryData;

    switch (IoStack->Parameters.QueryDeviceText.DeviceTextType)
    {
        case DeviceTextDescription:
        {
            DPRINT("DeviceTextDescription\n");

            Offset += CopyField(InquiryData->VendorId,
                                &LocalBuffer[Offset],
                                sizeof(InquiryData->VendorId),
                                ' ',
                                TRUE);
            LocalBuffer[Offset++] = ' ';
            Offset += CopyField(InquiryData->ProductId,
                                &LocalBuffer[Offset],
                                sizeof(InquiryData->ProductId),
                                ' ',
                                TRUE);
            Offset += sprintf(&LocalBuffer[Offset],
                              " SCSI %s Device",
                              GetDeviceType(InquiryData));
            LocalBuffer[Offset++] = '\0';

            RtlInitAnsiString(&AnsiString, (PCSZ)&LocalBuffer);

            DeviceDescription.Length = 0;
            DeviceDescription.MaximumLength = (USHORT)(Offset * sizeof(WCHAR));
            DeviceDescription.Buffer = ExAllocatePoolWithTag(PagedPool,
                                                             DeviceDescription.MaximumLength,
                                                             TAG_SCSIPORT);
            if (!DeviceDescription.Buffer)
            {
                Irp->IoStatus.Information = 0;
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            RtlAnsiStringToUnicodeString(&DeviceDescription, &AnsiString, FALSE);

            Irp->IoStatus.Information = (ULONG_PTR)DeviceDescription.Buffer;
            return STATUS_SUCCESS;
        }

        case DeviceTextLocationInformation:
        {
            DPRINT("DeviceTextLocationInformation\n");

            sprintf(LocalBuffer, "Bus Number %d, Target ID %d, LUN %d",
                    DeviceExtension->PathId, DeviceExtension->TargetId, DeviceExtension->Lun);

            RtlInitAnsiString(&AnsiString, (PCSZ)&LocalBuffer);

            DeviceDescription.Length = 0;
            DeviceDescription.MaximumLength = (USHORT)((strlen(LocalBuffer) + 1) * sizeof(WCHAR));
            DeviceDescription.Buffer = ExAllocatePoolWithTag(PagedPool,
                                                             DeviceDescription.MaximumLength,
                                                             TAG_SCSIPORT);
            if (!DeviceDescription.Buffer)
            {
                Irp->IoStatus.Information = 0;
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            RtlAnsiStringToUnicodeString(&DeviceDescription, &AnsiString, FALSE);

            Irp->IoStatus.Information = (ULONG_PTR)DeviceDescription.Buffer;
            return STATUS_SUCCESS;
        }

        default:
        {
            Irp->IoStatus.Information = 0;
            return Irp->IoStatus.Status;
        }
    }
}

static
NTSTATUS
PdoHandleQueryDeviceId(
    _In_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp)
{
    PSCSI_PORT_LUN_EXTENSION DeviceExtension;
    NTSTATUS Status;
    CHAR Buffer[100] = {0};
    LPCSTR DeviceType;
    ULONG Offset = 0;
    PINQUIRYDATA InquiryData;
    ANSI_STRING AnsiString;
    UNICODE_STRING DeviceId;

    DeviceExtension = DeviceObject->DeviceExtension;
    InquiryData = &DeviceExtension->InquiryData;

    DeviceType = GetDeviceType(InquiryData);

    // lets create device string
    Offset = sprintf(&Buffer[Offset], "SCSI\\");
    Offset += sprintf(&Buffer[Offset], DeviceType);
    Offset += sprintf(&Buffer[Offset], "&Ven_");
    Offset += CopyField(InquiryData->VendorId, &Buffer[Offset], 8, '_', TRUE);
    Offset += sprintf(&Buffer[Offset], "&Prod_");
    Offset += CopyField(InquiryData->ProductId, &Buffer[Offset], 16, '_', TRUE);
    Offset += sprintf(&Buffer[Offset], "&Rev_");
    Offset += CopyField(InquiryData->ProductRevisionLevel, &Buffer[Offset], 4, '_', TRUE);
    Buffer[Offset] = '\0';

    RtlInitAnsiString(&AnsiString, (PCSZ)Buffer);

    // allocate DeviceId string
    Status = RtlAnsiStringToUnicodeString(&DeviceId, &AnsiString, TRUE);

    if (NT_SUCCESS(Status))
    {
        Irp->IoStatus.Information = (ULONG_PTR)DeviceId.Buffer;
    }

    DPRINT("DeviceId %wZ Status %x\n", &DeviceId, Status);

    return Status;
}

static
VOID
ConvertToUnicodeString(
    IN CHAR * Buffer,
    IN ULONG ResultBufferLength,
    IN ULONG ResultBufferOffset,
    OUT LPWSTR ResultBuffer,
    OUT PULONG NewResultBufferOffset)
{
    UNICODE_STRING DeviceString;
    ANSI_STRING AnsiString;
    NTSTATUS Status;

    ASSERT(ResultBufferLength);
    ASSERT(ResultBufferLength > ResultBufferOffset);

    DPRINT("ResultBufferOffset %lu ResultBufferLength %lu Buffer %s Length %lu\n",
        ResultBufferOffset, ResultBufferLength, Buffer, strlen(Buffer));

    // construct destination string
    DeviceString.Buffer = &ResultBuffer[ResultBufferOffset];
    DeviceString.Length = 0;
    DeviceString.MaximumLength = (ResultBufferLength - ResultBufferOffset) * sizeof(WCHAR);

    // initialize source string
    RtlInitAnsiString(&AnsiString, Buffer);

    Status = RtlAnsiStringToUnicodeString(&DeviceString, &AnsiString, FALSE);
    ASSERT(Status == STATUS_SUCCESS);

    // subtract consumed bytes
    ResultBufferLength -= (DeviceString.Length + sizeof(WCHAR)) / sizeof(WCHAR);
    ResultBufferOffset += (DeviceString.Length + sizeof(WCHAR)) / sizeof(WCHAR);

    *NewResultBufferOffset = ResultBufferOffset;
}

static
NTSTATUS
PdoHandleQueryHardwareId(
    _In_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp)
{
    PSCSI_PORT_LUN_EXTENSION PDODeviceExtension = DeviceObject->DeviceExtension;
    LPCSTR GenericType, DeviceType;
    LPWSTR Buffer;
    CHAR Id1[50], Id2[50], Id3[50], Id4[50], Id5[50], Id6[50];
    ULONG Id1Length, Id2Length, Id3Length, Id4Length, Id5Length, Id6Length;
    ULONG Offset, TotalLength, Length;
    PINQUIRYDATA InquiryData;

    InquiryData = &PDODeviceExtension->InquiryData;

    DeviceType = GetDeviceType(InquiryData);
    GenericType = GetGenericType(InquiryData);

    ASSERT(GenericType);

    // generate id 1
    // SCSI\SCSIType_VendorId(8)_ProductId(16)_Revision(4)
    RtlZeroMemory(Id1, sizeof(Id1));
    Offset = 0;
    Offset = sprintf(&Id1[Offset], "SCSI\\");
    Offset += sprintf(&Id1[Offset], DeviceType);
    Offset += CopyField(InquiryData->VendorId, &Id1[Offset], 8, '_', FALSE);
    Offset += CopyField(InquiryData->ProductId, &Id1[Offset], 16, '_', FALSE);
    Offset += CopyField(InquiryData->ProductRevisionLevel, &Id1[Offset], 4, '_', FALSE);
    Id1Length = strlen(Id1) + 1;
    DPRINT("PdoHandleQueryHardwareId HardwareId1 %s\n", Id1);

    // generate id 2
    // SCSI\SCSIType_VendorId(8)_ProductId(16)
    RtlZeroMemory(Id2, sizeof(Id2));
    Offset = 0;
    Offset = sprintf(&Id2[Offset], "SCSI\\");
    Offset += sprintf(&Id2[Offset], DeviceType);
    Offset += CopyField(InquiryData->VendorId, &Id2[Offset], 8, '_', FALSE);
    Offset += CopyField(InquiryData->ProductId, &Id2[Offset], 16, '_', FALSE);
    Id2Length = strlen(Id2) + 1;
    DPRINT("PdoHandleQueryHardwareId HardwareId2 %s\n", Id2);

    // generate id 3
    // SCSI\SCSIType_VendorId(8)
    RtlZeroMemory(Id3, sizeof(Id3));
    Offset = 0;
    Offset = sprintf(&Id3[Offset], "SCSI\\");
    Offset += sprintf(&Id3[Offset], DeviceType);
    Offset += CopyField(InquiryData->VendorId, &Id3[Offset], 8, '_', FALSE);
    Id3Length = strlen(Id3) + 1;
    DPRINT("PdoHandleQueryHardwareId HardwareId3 %s\n", Id3);

    // generate id 4
    // SCSI\VendorId(8)_ProductId(16)_Revision(1)
    RtlZeroMemory(Id4, sizeof(Id4));
    Offset = 0;
    Offset = sprintf(&Id4[Offset], "SCSI\\");
    Offset += CopyField(InquiryData->VendorId, &Id4[Offset], 8, '_', FALSE);
    Offset += CopyField(InquiryData->ProductId, &Id4[Offset], 16, '_', FALSE);
    Offset += CopyField(InquiryData->ProductRevisionLevel, &Id4[Offset], 1, '_', FALSE);
    Id4Length = strlen(Id4) + 1;
    DPRINT("PdoHandleQueryHardwareId HardwareId4 %s\n", Id4);

    // generate id 5
    // VendorId(8)_ProductId(16)_Revision(1)
    RtlZeroMemory(Id5, sizeof(Id5));
    Offset = 0;
    Offset = CopyField(InquiryData->VendorId, &Id5[Offset], 8, '_', FALSE);
    Offset += CopyField(InquiryData->ProductId, &Id5[Offset], 16, '_', FALSE);
    Offset += CopyField(InquiryData->ProductRevisionLevel, &Id5[Offset], 1, '_', FALSE);
    Id5Length = strlen(Id5) + 1;
    DPRINT("PdoHandleQueryHardwareId HardwareId5 %s\n", Id5);

    // generate id 6
    // SCSIType
    RtlZeroMemory(Id6, sizeof(Id6));
    Offset = 0;
    Offset = sprintf(&Id6[Offset], GenericType);
    Id6Length = strlen(Id6) + 1;
    DPRINT("PdoHandleQueryHardwareId HardwareId6 %s\n", Id6);

    TotalLength = Id1Length + Id2Length + Id3Length + Id4Length + Id5Length + Id6Length + 1;

    Buffer = ExAllocatePoolWithTag(PagedPool, TotalLength * sizeof(WCHAR), TAG_SCSIPORT);
    if (!Buffer)
    {
        Irp->IoStatus.Information = 0;
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // reset offset
    Offset = 0;
    Length = TotalLength;

    ConvertToUnicodeString(Id1, Length, Offset, Buffer, &Offset);
    ConvertToUnicodeString(Id2, Length, Offset, Buffer, &Offset);
    ConvertToUnicodeString(Id3, Length, Offset, Buffer, &Offset);
    ConvertToUnicodeString(Id4, Length, Offset, Buffer, &Offset);
    ConvertToUnicodeString(Id5, Length, Offset, Buffer, &Offset);
    ConvertToUnicodeString(Id6, Length, Offset, Buffer, &Offset);

    Buffer[Offset] = UNICODE_NULL;

    ASSERT(Offset + 1 == Length);

    Irp->IoStatus.Information = (ULONG_PTR)Buffer;
    return STATUS_SUCCESS;
}

static
NTSTATUS
PdoHandleQueryCompatibleId(
    _In_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp)
{
    PSCSI_PORT_LUN_EXTENSION PDODeviceExtension = DeviceObject->DeviceExtension;
    CHAR Buffer[100] = {0};
    ULONG Length, Offset;
    LPWSTR InstanceId;
    LPCSTR DeviceType;

    DeviceType = GetDeviceType(&PDODeviceExtension->InquiryData);

    // format instance id
    Length = sprintf(Buffer, "SCSI\\%s", DeviceType) + 1;
    Length += sprintf(&Buffer[Length], "SCSI\\%s", "RAW") + 2;

    InstanceId = ExAllocatePoolWithTag(PagedPool, Length * sizeof(WCHAR), TAG_SCSIPORT);
    if (!InstanceId)
    {
        Irp->IoStatus.Information = 0;
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    ConvertToUnicodeString(Buffer, Length, 0, InstanceId, &Offset);
    ConvertToUnicodeString(&Buffer[Offset], Length, Offset, InstanceId, &Offset);

    InstanceId[Offset] = UNICODE_NULL;

    DPRINT("PdoHandleQueryCompatibleId %S\n", InstanceId);

    Irp->IoStatus.Information = (ULONG_PTR)InstanceId;
    return STATUS_SUCCESS;
}

static
NTSTATUS
PdoHandleQueryInstanceId(
    _In_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp)
{
    PSCSI_PORT_LUN_EXTENSION lunExt = DeviceObject->DeviceExtension;
    WCHAR Buffer[26];
    ULONG Length;
    LPWSTR InstanceId;

    // use instance count and LUN
    swprintf(Buffer, L"%x%x%x", lunExt->PathId, lunExt->TargetId, lunExt->Lun);

    Length = wcslen(Buffer) + 1;

    InstanceId = ExAllocatePoolWithTag(PagedPool, Length * sizeof(WCHAR), TAG_SCSIPORT);
    if (!InstanceId)
    {
        Irp->IoStatus.Information = 0;
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    wcscpy(InstanceId, Buffer);

    DPRINT("PdoHandleQueryInstanceId %S\n", InstanceId);

    Irp->IoStatus.Information = (ULONG_PTR)InstanceId;
    return STATUS_SUCCESS;
}

static
NTSTATUS
PdoHandleDeviceRelations(
    _In_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp)
{
    PDEVICE_RELATIONS deviceRelations;
    PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(Irp);

    // check if relation type is BusRelations
    if (ioStack->Parameters.QueryDeviceRelations.Type != TargetDeviceRelation)
    {
        // PDO handles only target device relation
        return Irp->IoStatus.Status;
    }

    deviceRelations = ExAllocatePoolWithTag(PagedPool, sizeof(DEVICE_RELATIONS), TAG_SCSIPORT);
    if (!deviceRelations)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // initialize device relations
    deviceRelations->Count = 1;
    deviceRelations->Objects[0] = DeviceObject;
    ObReferenceObject(DeviceObject);

    Irp->IoStatus.Information = (ULONG_PTR)deviceRelations;
    return STATUS_SUCCESS;
}

NTSTATUS
PdoDispatchPnp(
    _In_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp)
{
    PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(Irp);
    PSCSI_PORT_LUN_EXTENSION lunExt = DeviceObject->DeviceExtension;
    NTSTATUS status;

    DPRINT("PDO PnP request %s\n", GetIRPMinorFunctionString(ioStack->MinorFunction));

    ASSERT(!lunExt->Common.IsFDO);

    switch (ioStack->MinorFunction)
    {
        case IRP_MN_START_DEVICE:
        {
            RegistryInitLunKey(lunExt);
            status = STATUS_SUCCESS;
            break;
        }
        case IRP_MN_REMOVE_DEVICE:
        case IRP_MN_QUERY_CAPABILITIES:
        case IRP_MN_QUERY_REMOVE_DEVICE:
        case IRP_MN_QUERY_STOP_DEVICE:
        case IRP_MN_SURPRISE_REMOVAL:
        {
            status = STATUS_SUCCESS;
            break;
        }
        case IRP_MN_QUERY_DEVICE_RELATIONS:
        {
            status = PdoHandleDeviceRelations(DeviceObject, Irp);
            break;
        }
        case IRP_MN_QUERY_DEVICE_TEXT:
        {
            status = PdoHandleQueryDeviceText(DeviceObject, Irp);
            break;
        }
        case IRP_MN_QUERY_ID:
        {
            DPRINT("IRP_MN_QUERY_ID IdType %s\n",
                DbgGetDeviceIDString(ioStack->Parameters.QueryId.IdType));

           if (ioStack->Parameters.QueryId.IdType == BusQueryDeviceID)
           {
               status = PdoHandleQueryDeviceId(DeviceObject, Irp);
               break;
           }
           else if (ioStack->Parameters.QueryId.IdType == BusQueryHardwareIDs)
           {
               status = PdoHandleQueryHardwareId(DeviceObject, Irp);
               break;
           }
           else if (ioStack->Parameters.QueryId.IdType == BusQueryInstanceID)
           {
               status = PdoHandleQueryInstanceId(DeviceObject, Irp);
               break;
           }
           else if (ioStack->Parameters.QueryId.IdType == BusQueryCompatibleIDs)
           {
               status = PdoHandleQueryCompatibleId(DeviceObject, Irp);
               break;
           }

           // fallthrough
        }
        default:
        {
            // do nothing
            status = Irp->IoStatus.Status;
        }
    }

    if (status != STATUS_PENDING)
    {
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }

    return status;
}