/*
 * COPYRIGHT:        See COPYING in the top level directory
 * PROJECT:          MergeOS Win32k subsystem
 * PURPOSE:          Visibility computations interface definition
 * FILE:             win32ss/user/ntuser/vis.h
 * PROGRAMMER:       Ge van Geldorp (ge@gse.nl)
 *
 */

#pragma once

PREGION FASTCALL VIS_ComputeVisibleRegion(PWND Window, BOOLEAN ClientArea, BOOLEAN ClipChildren, BOOLEAN ClipSiblings);
VOID FASTCALL co_VIS_WindowLayoutChanged(PWND Window, PREGION UncoveredRgn);

/* EOF */
