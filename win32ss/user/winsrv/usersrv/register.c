/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         MergeOS User API Server DLL
 * FILE:            win32ss/user/winsrv/usersrv/register.c
 * PURPOSE:         Register logon window and services process
 * PROGRAMMERS:     Eric Kohl
 *                  Hermes Belusca-Maito (hermes.belusca@sfr.fr)
 */

/* INCLUDES *******************************************************************/

#include "usersrv.h"

#define NDEBUG
#include <debug.h>

/* GLOBALS ********************************************************************/

static BOOLEAN ServicesProcessIdValid = FALSE;
ULONG_PTR ServicesProcessId = 0;
ULONG_PTR LogonProcessId = 0;

#if 1 //HACK! See use below
extern HANDLE CsrApiPort;
#endif

/* PUBLIC SERVER APIS *********************************************************/

/* API_NUMBER: UserpRegisterLogonProcess */
CSR_API(SrvRegisterLogonProcess)
{
    PUSER_REGISTER_LOGON_PROCESS RegisterLogonProcessRequest = &((PUSER_API_MESSAGE)ApiMessage)->Data.RegisterLogonProcessRequest;

    if (RegisterLogonProcessRequest->Register)
    {
        if (LogonProcessId != 0)
            return STATUS_LOGON_SESSION_EXISTS;

        LogonProcessId = RegisterLogonProcessRequest->ProcessId;
    }
    else
    {
        if (ApiMessage->Header.ClientId.UniqueProcess != UlongToHandle(LogonProcessId))
        {
            DPRINT1("Current logon process 0x%x, can't deregister from process 0x%x\n",
                    LogonProcessId, ApiMessage->Header.ClientId.UniqueProcess);
            return STATUS_NOT_LOGON_PROCESS;
        }

        LogonProcessId = 0;
    }

#if 1 //HAAAACK. This should be done in UserClientConnect which is never called!

    /* Check if we don't have an API port yet */
    if (CsrApiPort == NULL)
    {
        NTSTATUS Status;

        /* Query the API port and save it globally */
        CsrApiPort = CsrQueryApiPort();

        DPRINT("Giving win32k our api port\n");

        /* Inform win32k about the API port */
        Status = NtUserSetInformationThread(NtCurrentThread(),
                                            UserThreadCsrApiPort,
                                            &CsrApiPort,
                                            sizeof(CsrApiPort));
        if (!NT_SUCCESS(Status))
        {
            return Status;
        }
    }
#endif

    return STATUS_SUCCESS;
}

/* API_NUMBER: UserpRegisterServicesProcess */
CSR_API(SrvRegisterServicesProcess)
{
    PUSER_REGISTER_SERVICES_PROCESS RegisterServicesProcessRequest = &((PUSER_API_MESSAGE)ApiMessage)->Data.RegisterServicesProcessRequest;

    if (ServicesProcessIdValid)
    {
        /* Only accept a single call */
        return STATUS_INVALID_PARAMETER;
    }
    else
    {
        ServicesProcessId = RegisterServicesProcessRequest->ProcessId;
        ServicesProcessIdValid = TRUE;
        return STATUS_SUCCESS;
    }
}

/* EOF */
