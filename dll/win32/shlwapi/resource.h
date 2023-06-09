/*
 * Resource defines for shlwapi
 *
 * Copyright 2004 Jon Griffiths
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#ifndef WINE_SHLWAPI_RESOURCE_H
#define WINE_SHLWAPI_RESOURCE_H

#include <windef.h>
#include <winuser.h>

#define IDS_BYTES_FORMAT 64
#define IDS_TIME_INTERVAL_HOURS 65
#define IDS_TIME_INTERVAL_MINUTES 66
#define IDS_TIME_INTERVAL_SECONDS 67
#ifdef __MERGEOS__
#define IDS_KB_FORMAT 68
#define IDS_MB_FORMAT 69
#define IDS_GB_FORMAT 70
#define IDS_TB_FORMAT 71
#define IDS_PB_FORMAT 72
#define IDS_EB_FORMAT 73
#endif

/* These numbers match native IDs and shouldn't be arbitrarily changed */
#define IDD_ERR_DIALOG    0x1200
#define IDS_ERR_USER_MSG  0x1201
#define IDC_ERR_DONT_SHOW 0x1202
#define IDS_ERR_USER_MSG2 0x1203

#endif /* WINE_SHLWAPI_RESOURCE_H */
