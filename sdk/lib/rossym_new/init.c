/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         MergeOS kernel
 * FILE:            lib/rossym/data.c
 * PURPOSE:         Definition of external variables
 *
 * PROGRAMMERS:     Ge van Geldorp (gvg@mergeos.com)
 */

#include <precomp.h>

ROSSYM_CALLBACKS RosSymCallbacks;

VOID
RosSymInit(PROSSYM_CALLBACKS Callbacks)
{
  RosSymCallbacks = *Callbacks;
}

/* EOF */
