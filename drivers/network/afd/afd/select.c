/*
 * COPYRIGHT:        See COPYING in the top level directory
 * PROJECT:          MergeOS kernel
 * FILE:             drivers/net/afd/afd/select.c
 * PURPOSE:          Ancillary functions driver
 * PROGRAMMER:       Art Yerkes (ayerkes@speakeasy.net)
 * UPDATE HISTORY:
 * 20040708 Created
 */

#include "afd.h"

static VOID PrintEvents( ULONG Events ) {
#if DBG
    char *events_list[] = { "AFD_EVENT_RECEIVE",
                            "AFD_EVENT_OOB_RECEIVE",
                            "AFD_EVENT_SEND",
                            "AFD_EVENT_DISCONNECT",
                            "AFD_EVENT_ABORT",
                            "AFD_EVENT_CLOSE",
                            "AFD_EVENT_CONNECT",
                            "AFD_EVENT_ACCEPT",
                            "AFD_EVENT_CONNECT_FAIL",
                            "AFD_EVENT_QOS",
                            "AFD_EVENT_GROUP_QOS",
                            NULL };
    int i;

    for( i = 0; events_list[i]; i++ )
        if( Events & (1 << i) ) AFD_DbgPrint(MID_TRACE,("%s ", events_list[i] ));
#endif
}

static VOID CopyBackStatus( PAFD_HANDLE HandleArray,
                            UINT HandleCount ) {
    UINT i;

    for( i = 0; i < HandleCount; i++ ) {
        HandleArray[i].Events = HandleArray[i].Status;
        HandleArray[i].Status = 0;
    }
}

VOID ZeroEvents( PAFD_HANDLE HandleArray,
                 UINT HandleCount ) {
    UINT i;

    for( i = 0; i < HandleCount; i++ ) {
        HandleArray[i].Status = 0;
        HandleArray[i].Events = 0;
    }
}


/* you must pass either Poll OR Irp */
VOID SignalSocket(
   PAFD_ACTIVE_POLL Poll OPTIONAL,
   PIRP _Irp OPTIONAL,
   PAFD_POLL_INFO PollReq,
   NTSTATUS Status
   )
{
    UINT i;
    PIRP Irp = _Irp ? _Irp : Poll->Irp;
    AFD_DbgPrint(MID_TRACE,("Called (Status %x)\n", Status));

    if (Poll)
    {
        KeCancelTimer( &Poll->Timer );
        RemoveEntryList( &Poll->ListEntry );
        ExFreePoolWithTag(Poll, TAG_AFD_ACTIVE_POLL);
    }

    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information =
        FIELD_OFFSET(AFD_POLL_INFO, Handles) + sizeof(AFD_HANDLE) * PollReq->HandleCount;
    CopyBackStatus( PollReq->Handles,
                    PollReq->HandleCount );
    for( i = 0; i < PollReq->HandleCount; i++ ) {
        AFD_DbgPrint
            (MAX_TRACE,
             ("Handle(%x): Got %x,%x\n",
              PollReq->Handles[i].Handle,
              PollReq->Handles[i].Events,
              PollReq->Handles[i].Status));
    }
    UnlockHandles( AFD_HANDLES(PollReq), PollReq->HandleCount );
    if( Irp->MdlAddress ) UnlockRequest( Irp, IoGetCurrentIrpStackLocation( Irp ) );
    AFD_DbgPrint(MID_TRACE,("Completing\n"));
    (void)IoSetCancelRoutine(Irp, NULL);
    IoCompleteRequest( Irp, IO_NETWORK_INCREMENT );
    AFD_DbgPrint(MID_TRACE,("Done\n"));
}

static KDEFERRED_ROUTINE SelectTimeout;
static VOID NTAPI SelectTimeout( PKDPC Dpc,
                           PVOID DeferredContext,
                           PVOID SystemArgument1,
                           PVOID SystemArgument2 ) {
    PAFD_ACTIVE_POLL Poll = DeferredContext;
    PAFD_POLL_INFO PollReq;
    PIRP Irp;
    KIRQL OldIrql;
    PAFD_DEVICE_EXTENSION DeviceExt;

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);

    AFD_DbgPrint(MID_TRACE,("Called\n"));

    Irp = Poll->Irp;
    DeviceExt = Poll->DeviceExt;
    PollReq = Irp->AssociatedIrp.SystemBuffer;

    ZeroEvents( PollReq->Handles, PollReq->HandleCount );

    KeAcquireSpinLock( &DeviceExt->Lock, &OldIrql );
    SignalSocket( Poll, NULL, PollReq, STATUS_TIMEOUT );
    KeReleaseSpinLock( &DeviceExt->Lock, OldIrql );

    AFD_DbgPrint(MID_TRACE,("Timeout\n"));
}

VOID KillSelectsForFCB( PAFD_DEVICE_EXTENSION DeviceExt,
                        PFILE_OBJECT FileObject,
                        BOOLEAN OnlyExclusive ) {
    KIRQL OldIrql;
    PLIST_ENTRY ListEntry;
    PAFD_ACTIVE_POLL Poll;
    PIRP Irp;
    PAFD_POLL_INFO PollReq;
    PAFD_HANDLE HandleArray;
    UINT i;

    AFD_DbgPrint(MID_TRACE,("Killing selects that refer to %p\n", FileObject));

    KeAcquireSpinLock( &DeviceExt->Lock, &OldIrql );

    ListEntry = DeviceExt->Polls.Flink;
    while ( ListEntry != &DeviceExt->Polls ) {
        Poll = CONTAINING_RECORD(ListEntry, AFD_ACTIVE_POLL, ListEntry);
        ListEntry = ListEntry->Flink;
        Irp = Poll->Irp;
        PollReq = Irp->AssociatedIrp.SystemBuffer;
        HandleArray = AFD_HANDLES(PollReq);

        for( i = 0; i < PollReq->HandleCount; i++ ) {
            AFD_DbgPrint(MAX_TRACE,("Req: %u, This %p\n",
                                    HandleArray[i].Handle, FileObject));
            if( (PVOID)HandleArray[i].Handle == FileObject &&
                (!OnlyExclusive || (OnlyExclusive && Poll->Exclusive)) ) {
                ZeroEvents( PollReq->Handles, PollReq->HandleCount );
                SignalSocket( Poll, NULL, PollReq, STATUS_CANCELLED );
            }
        }
    }

    KeReleaseSpinLock( &DeviceExt->Lock, OldIrql );

    AFD_DbgPrint(MID_TRACE,("Done\n"));
}

NTSTATUS NTAPI
AfdSelect( PDEVICE_OBJECT DeviceObject, PIRP Irp,
           PIO_STACK_LOCATION IrpSp ) {
    NTSTATUS Status = STATUS_NO_MEMORY;
    PAFD_FCB FCB;
    PFILE_OBJECT FileObject;
    PAFD_POLL_INFO PollReq = Irp->AssociatedIrp.SystemBuffer;
    PAFD_DEVICE_EXTENSION DeviceExt = DeviceObject->DeviceExtension;
    KIRQL OldIrql;
    UINT i, Signalled = 0;
    ULONG Exclusive = PollReq->Exclusive;

    UNREFERENCED_PARAMETER(IrpSp);

    AFD_DbgPrint(MID_TRACE,("Called (HandleCount %u Timeout %d)\n",
                            PollReq->HandleCount,
                            (INT)(PollReq->Timeout.QuadPart)));

    SET_AFD_HANDLES(PollReq,
                    LockHandles( PollReq->Handles, PollReq->HandleCount ));

    if( !AFD_HANDLES(PollReq) ) {
        Irp->IoStatus.Status = STATUS_NO_MEMORY;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest( Irp, IO_NETWORK_INCREMENT );
        return STATUS_NO_MEMORY;
    }

    if( Exclusive ) {
        for( i = 0; i < PollReq->HandleCount; i++ ) {
            if( !AFD_HANDLES(PollReq)[i].Handle ) continue;

            KillSelectsForFCB( DeviceExt,
                               (PFILE_OBJECT)AFD_HANDLES(PollReq)[i].Handle,
                               TRUE );
        }
    }

    KeAcquireSpinLock( &DeviceExt->Lock, &OldIrql );

    for( i = 0; i < PollReq->HandleCount; i++ ) {
        if( !AFD_HANDLES(PollReq)[i].Handle ) continue;

        FileObject = (PFILE_OBJECT)AFD_HANDLES(PollReq)[i].Handle;
        FCB = FileObject->FsContext;

        AFD_DbgPrint(MID_TRACE, ("AFD: Select Events: "));
        PrintEvents( PollReq->Handles[i].Events );
        AFD_DbgPrint(MID_TRACE,("\n"));

        PollReq->Handles[i].Status =
            PollReq->Handles[i].Events & FCB->PollState;
        if( PollReq->Handles[i].Status ) {
            AFD_DbgPrint(MID_TRACE,("Signalling %p with %x\n",
                                    FCB, FCB->PollState));
            Signalled++;
        }
    }

    if( Signalled ) {
        Status = STATUS_SUCCESS;
        Irp->IoStatus.Status = Status;
        SignalSocket( NULL, Irp, PollReq, Status );
    } else {

       PAFD_ACTIVE_POLL Poll = NULL;

       Poll = ExAllocatePoolWithTag(NonPagedPool,
                                    sizeof(AFD_ACTIVE_POLL),
                                    TAG_AFD_ACTIVE_POLL);

       if (Poll){
          Poll->Irp = Irp;
          Poll->DeviceExt = DeviceExt;
          Poll->Exclusive = Exclusive;

          KeInitializeTimerEx( &Poll->Timer, NotificationTimer );

          KeInitializeDpc( (PRKDPC)&Poll->TimeoutDpc, SelectTimeout, Poll );

          InsertTailList( &DeviceExt->Polls, &Poll->ListEntry );

          KeSetTimer( &Poll->Timer, PollReq->Timeout, &Poll->TimeoutDpc );

          Status = STATUS_PENDING;
          IoMarkIrpPending( Irp );
          (void)IoSetCancelRoutine(Irp, AfdCancelHandler);
       } else {
          AFD_DbgPrint(MAX_TRACE, ("FIXME: do something with the IRP!\n"));
          Status = STATUS_NO_MEMORY;
       }
    }

    KeReleaseSpinLock( &DeviceExt->Lock, OldIrql );

    AFD_DbgPrint(MID_TRACE,("Returning %x\n", Status));

    return Status;
}

NTSTATUS NTAPI
AfdEventSelect( PDEVICE_OBJECT DeviceObject, PIRP Irp,
                PIO_STACK_LOCATION IrpSp ) {
    PFILE_OBJECT FileObject = IrpSp->FileObject;
    NTSTATUS Status = STATUS_NO_MEMORY;
    PAFD_EVENT_SELECT_INFO EventSelectInfo =
        (PAFD_EVENT_SELECT_INFO)LockRequest( Irp, IrpSp, FALSE, NULL );
    PAFD_FCB FCB = FileObject->FsContext;

    UNREFERENCED_PARAMETER(DeviceObject);

    if( !SocketAcquireStateLock( FCB ) ) {
        return LostSocket( Irp );
    }

    if ( !EventSelectInfo ) {
         return UnlockAndMaybeComplete( FCB, STATUS_NO_MEMORY, Irp,
                                        0 );
    }
    AFD_DbgPrint(MID_TRACE,("Called (Event %p Triggers %u)\n",
                            EventSelectInfo->EventObject,
                            EventSelectInfo->Events));

    if( FCB->EventSelect ) ObDereferenceObject( FCB->EventSelect );
    FCB->EventSelect = NULL;

    if( EventSelectInfo->EventObject && EventSelectInfo->Events ) {
        Status = ObReferenceObjectByHandle( (PVOID)EventSelectInfo->
                                            EventObject,
                                            EVENT_ALL_ACCESS,
                                            *ExEventObjectType,
                                            UserMode,
                                            (PVOID *)&FCB->EventSelect,
                                            NULL );

        if( !NT_SUCCESS(Status) )
        {
            AFD_DbgPrint(MIN_TRACE,("Failed reference event (0x%x)\n", Status));
            FCB->EventSelect = NULL;
        }
        else
            FCB->EventSelectTriggers = EventSelectInfo->Events;
    } else {
        FCB->EventSelect = NULL;
        FCB->EventSelectTriggers = 0;
        Status = STATUS_SUCCESS;
    }

    if((FCB->EventSelect) &&
       (FCB->PollState & (FCB->EventSelectTriggers & ~FCB->EventSelectDisabled)))
    {
        AFD_DbgPrint(MID_TRACE,("Setting event %p\n", FCB->EventSelect));

        /* Set the application's event */
        KeSetEvent( FCB->EventSelect, IO_NETWORK_INCREMENT, FALSE );
    }

    AFD_DbgPrint(MID_TRACE,("Returning %x\n", Status));

    return UnlockAndMaybeComplete( FCB, Status, Irp,
                                   0 );
}

NTSTATUS NTAPI
AfdEnumEvents( PDEVICE_OBJECT DeviceObject, PIRP Irp,
               PIO_STACK_LOCATION IrpSp ) {
    PFILE_OBJECT FileObject = IrpSp->FileObject;
    PAFD_ENUM_NETWORK_EVENTS_INFO EnumReq =
        (PAFD_ENUM_NETWORK_EVENTS_INFO)LockRequest( Irp, IrpSp, TRUE, NULL );
    PAFD_FCB FCB = FileObject->FsContext;
    PKEVENT UserEvent;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(DeviceObject);

    AFD_DbgPrint(MID_TRACE,("Called (FCB %p)\n", FCB));

    if( !SocketAcquireStateLock( FCB ) ) {
        return LostSocket( Irp );
    }

    if ( !EnumReq ) {
         return UnlockAndMaybeComplete( FCB, STATUS_NO_MEMORY, Irp, 0 );
    }

    /* An event may optionally be provided for us to clear */
    if (EnumReq->Event != NULL)
    {
        Status = ObReferenceObjectByHandle(EnumReq->Event,
                                           EVENT_ALL_ACCESS,
                                           *ExEventObjectType,
                                           UserMode,
                                           (PVOID *)&UserEvent,
                                           NULL);
        if (!NT_SUCCESS(Status))
        {
            AFD_DbgPrint(MIN_TRACE,("Unable to reference event %x\n", Status));
            return UnlockAndMaybeComplete(FCB, Status, Irp, 0);
        }

        /* Clear the event */
        KeClearEvent(UserEvent);
        ObDereferenceObject(UserEvent);
    }

    /* Copy the poll state, masking out disabled events */
    EnumReq->PollEvents = (FCB->PollState & ~FCB->EventSelectDisabled);
    RtlCopyMemory( EnumReq->EventStatus,
                   FCB->PollStatus,
                   sizeof(EnumReq->EventStatus) );

    /* Disable the events that triggered the select until the reenabling function is called */
    FCB->EventSelectDisabled |= (FCB->PollState & FCB->EventSelectTriggers);

    return UnlockAndMaybeComplete( FCB, STATUS_SUCCESS, Irp, 0 );
}

/* * * NOTE ALWAYS CALLED AT DISPATCH_LEVEL * * */
static BOOLEAN UpdatePollWithFCB( PAFD_ACTIVE_POLL Poll, PFILE_OBJECT FileObject ) {
    UINT i;
    PAFD_FCB FCB;
    UINT Signalled = 0;
    PAFD_POLL_INFO PollReq = Poll->Irp->AssociatedIrp.SystemBuffer;

    ASSERT( KeGetCurrentIrql() == DISPATCH_LEVEL );

    for( i = 0; i < PollReq->HandleCount; i++ ) {
        if( !AFD_HANDLES(PollReq)[i].Handle ) continue;

        FileObject = (PFILE_OBJECT)AFD_HANDLES(PollReq)[i].Handle;
        FCB = FileObject->FsContext;

        PollReq->Handles[i].Status = PollReq->Handles[i].Events & FCB->PollState;
        if( PollReq->Handles[i].Status ) {
            AFD_DbgPrint(MID_TRACE,("Signalling %p with %x\n",
                                    FCB, FCB->PollState));
            Signalled++;
        }
    }

    return Signalled ? 1 : 0;
}

VOID PollReeval( PAFD_DEVICE_EXTENSION DeviceExt, PFILE_OBJECT FileObject ) {
    PAFD_ACTIVE_POLL Poll = NULL;
    PLIST_ENTRY ThePollEnt = NULL;
    PAFD_FCB FCB;
    KIRQL OldIrql;
    PAFD_POLL_INFO PollReq;

    AFD_DbgPrint(MID_TRACE,("Called: DeviceExt %p FileObject %p\n",
                            DeviceExt, FileObject));

    KeAcquireSpinLock( &DeviceExt->Lock, &OldIrql );

    /* Take care of any event select signalling */
    FCB = (PAFD_FCB)FileObject->FsContext;

    if( !FCB ) {
        KeReleaseSpinLock( &DeviceExt->Lock, OldIrql );
        return;
    }

    /* Now signal normal select irps */
    ThePollEnt = DeviceExt->Polls.Flink;

    while( ThePollEnt != &DeviceExt->Polls ) {
        Poll = CONTAINING_RECORD( ThePollEnt, AFD_ACTIVE_POLL, ListEntry );
        PollReq = Poll->Irp->AssociatedIrp.SystemBuffer;
        AFD_DbgPrint(MID_TRACE,("Checking poll %p\n", Poll));

        if( UpdatePollWithFCB( Poll, FileObject ) ) {
            ThePollEnt = ThePollEnt->Flink;
            AFD_DbgPrint(MID_TRACE,("Signalling socket\n"));
            SignalSocket( Poll, NULL, PollReq, STATUS_SUCCESS );
        } else
            ThePollEnt = ThePollEnt->Flink;
    }

    KeReleaseSpinLock( &DeviceExt->Lock, OldIrql );

    if((FCB->EventSelect) &&
       (FCB->PollState & (FCB->EventSelectTriggers & ~FCB->EventSelectDisabled)))
    {
        AFD_DbgPrint(MID_TRACE,("Setting event %p\n", FCB->EventSelect));

        /* Set the application's event */
        KeSetEvent( FCB->EventSelect, IO_NETWORK_INCREMENT, FALSE );
    }

    AFD_DbgPrint(MID_TRACE,("Leaving\n"));
}
