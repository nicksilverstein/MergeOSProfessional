/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         MergeOS kernel
 * FILE:            lib/rossym/delete.c
 * PURPOSE:         Free rossym info
 *
 * PROGRAMMERS:     Ge van Geldorp (gvg@mergeos.com)
 */

#define NTOSAPI
#include <ntdef.h>
#include <mergeos/rossym.h>
#include "rossympriv.h"

VOID
RosSymDelete(PROSSYM_INFO RosSymInfo)
{
  RosSymFreeMem(RosSymInfo);
}
