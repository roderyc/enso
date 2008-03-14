/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2005 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   WinSdk.h
 *   Maintainer: Atul Varma
 *
 *   Header file for Windows software development.
 *
 *   This header file is mostly just a wrapper for the standard
 *   Windows include file, Windows.h. It also defines a few necessary
 *   macros needed by our software. This file should always be used in 
 *   original Humanized code instead of including Windows.h directly.
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

#ifndef _WINSDK_H_
#define _WINSDK_H_

/* ***************************************************************************
 * Macros
 * **************************************************************************/

/* Use strict type checking for all Windows data types and API
 * calls. Must be defined before including Windows.h. */
#define STRICT

/* Assume we're using Windows NT 5.0 (Windows 2000) or above. This
 * will enable certain API calls that are only availale under NT 5.0
 * or above. Must be defined before including Windows.h. */
#define _WIN32_WINNT 0x0500

/* More readable definition for a DLL exported function under MSVC++. */
#define DllExport __declspec(dllexport)

/* ***************************************************************************
 * Include Files
 * **************************************************************************/

#include <Windows.h>

#endif
