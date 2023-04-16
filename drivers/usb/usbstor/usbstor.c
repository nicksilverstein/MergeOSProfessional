/*
 * PROJECT:     MergeOS Universal Serial Bus Bulk Storage Driver
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     USB block storage device driver.
 * COPYRIGHT:   2005-2006 James Tabor
 *              2011-2012 Michael Martin (michael.martin@reactos.org)
 *              2011-2013 Johannes Anderwald (johannes.anderwald@reactos.org)
 */

#include "usbstor.h"

#define NDEBUG
#include <debug.h>


NTSTATUS
NTAPI
USBSTOR_AddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject)
{
    NTSTATUS Status;
    PDEVICE_OBJECT DeviceObject;
    PFDO_DEVICE_EXTENSION DeviceExtension;

    Status = IoCreateDevice(DriverObject, sizeof(FDO_DEVICE_EXTENSION), 0, FILE_DEVICE_BUS_EXTENDER, FILE_AUTOGENERATED_DEVICE_NAME | FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
    if (!NT_SUCCESS(Status))
    {
        DPRINT1("USBSTOR_AddDevice: Failed to create FDO Status %x\n", Status);
        return Status;
    }

    DeviceExtension = (PFDO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
    ASSERT(DeviceExtension);
    RtlZeroMemory(DeviceExtension, sizeof(FDO_DEVICE_EXTENSION));

    // initialize device extension
    DeviceExtension->Common.IsFDO = TRUE;
    DeviceExtension->FunctionalDeviceObject = DeviceObject;
    DeviceExtension->PhysicalDeviceObject = PhysicalDeviceObject;
    DeviceExtension->LowerDeviceObject = IoAttachDeviceToDeviceStack(DeviceObject, PhysicalDeviceObject);

    KeInitializeSpinLock(&DeviceExtension->CommonLock);

    IoInitializeTimer(DeviceObject, USBSTOR_TimerRoutine, (PVOID)DeviceExtension);

    // did attaching fail
    if (!DeviceExtension->LowerDeviceObject)
    {
        IoDeleteDevice(DeviceObject);

        return STATUS_DEVICE_REMOVED;
    }

    DeviceObject->Flags |= DO_BUFFERED_IO | DO_POWER_PAGABLE;

    // device is initialized
    DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;
}

VOID
NTAPI
USBSTOR_Unload(
    PDRIVER_OBJECT DriverObject)
{
    // no-op
}

NTSTATUS
NTAPI
USBSTOR_DispatchClose(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp)
{
    // function always succeeds ;)
    DPRINT("USBSTOR_DispatchClose\n");
    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS
NTAPI
USBSTOR_DispatchDeviceControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp)
{
    NTSTATUS Status;

    Status = USBSTOR_HandleDeviceControl(DeviceObject, Irp);
    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Status;
}

NTSTATUS
NTAPI
USBSTOR_DispatchScsi(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp)
{
    return USBSTOR_HandleInternalDeviceControl(DeviceObject, Irp);
}

NTSTATUS
NTAPI
USBSTOR_DispatchReadWrite(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp)
{
    // read write ioctl is not supported
    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_INVALID_PARAMETER;
}

NTSTATUS
NTAPI
USBSTOR_DispatchPnp(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp)
{
    PUSBSTOR_COMMON_DEVICE_EXTENSION DeviceExtension;

    DeviceExtension = (PUSBSTOR_COMMON_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

    if (DeviceExtension->IsFDO)
    {
        return USBSTOR_FdoHandlePnp(DeviceObject, Irp);
    }
    else
    {
        return USBSTOR_PdoHandlePnp(DeviceObject, Irp);
    }
}

NTSTATUS
NTAPI
USBSTOR_DispatchSystemControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp)
{
    PUSBSTOR_COMMON_DEVICE_EXTENSION DeviceExtension = (PUSBSTOR_COMMON_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
    IoSkipCurrentIrpStackLocation(Irp);

    if (DeviceExtension->IsFDO)
    {
        return IoCallDriver(((PFDO_DEVICE_EXTENSION)DeviceExtension)->LowerDeviceObject, Irp);
    }
    else
    {
        return IoCallDriver(((PPDO_DEVICE_EXTENSION)DeviceExtension)->LowerDeviceObject, Irp);
    }
}

NTSTATUS
NTAPI
USBSTOR_DispatchPower(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp)
{
    PFDO_DEVICE_EXTENSION DeviceExtension;

    // get common device extension
    DeviceExtension = (PFDO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

    if (DeviceExtension->Common.IsFDO)
    {
        PoStartNextPowerIrp(Irp);
        IoSkipCurrentIrpStackLocation(Irp);
        return PoCallDriver(DeviceExtension->LowerDeviceObject, Irp);
    }
    else
    {
        PoStartNextPowerIrp(Irp);
        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }
}

NTSTATUS
NTAPI
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegPath)
{

    DPRINT("********* USB Storage *********\n");

    DriverObject->DriverUnload = USBSTOR_Unload;
    DriverObject->DriverExtension->AddDevice = USBSTOR_AddDevice;
    DriverObject->DriverStartIo = USBSTOR_StartIo;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = USBSTOR_DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = USBSTOR_DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = USBSTOR_DispatchDeviceControl; // scsi pass through requests
    DriverObject->MajorFunction[IRP_MJ_READ] = USBSTOR_DispatchReadWrite;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = USBSTOR_DispatchReadWrite;
    DriverObject->MajorFunction[IRP_MJ_SCSI] = USBSTOR_DispatchScsi;
    DriverObject->MajorFunction[IRP_MJ_PNP] = USBSTOR_DispatchPnp;
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = USBSTOR_DispatchSystemControl;
    DriverObject->MajorFunction[IRP_MJ_POWER] = USBSTOR_DispatchPower;

    return STATUS_SUCCESS;
}
