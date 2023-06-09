/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         MergeOS kernel
 * FILE:            lib/rossym/delete.c
 * PURPOSE:         Free rossym info
 *
 * PROGRAMMERS:     Ge van Geldorp (gvg@mergeos.com)
 */

#include <precomp.h>

VOID
RosSymDelete(PROSSYM_INFO RosSymInfo)
{
	dwarfclose(RosSymInfo);
}

/* EOF */
