/*
 * PROJECT:         MergeOS HAL
 * LICENSE:         BSD - See COPYING.ARM in the top level directory
 * FILE:            hal/halarm/generic/beep.c
 * PURPOSE:         Speaker support (beeping)
 * PROGRAMMERS:     MergeOS Portable Systems Group
 */

/* INCLUDES ******************************************************************/

#include <hal.h>
#define NDEBUG
#include <debug.h>

/* FUNCTIONS *****************************************************************/

/*
 * @implemented
 */
BOOLEAN
NTAPI
HalMakeBeep(IN ULONG Frequency)
{
    UNIMPLEMENTED;
    return FALSE;
}
