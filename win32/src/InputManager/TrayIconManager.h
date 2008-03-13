/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2005 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   TrayIconManager.h
 *   Maintainer: Jono DiCarlo
 *
 *   Header file for Mehitabel's tray icon manager.
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

/* ***************************************************************************
 * Include Files
 * **************************************************************************/

#include "Input/AsyncEventProcessorRegistry.h"

#include "WinSdk.h"


/* ***************************************************************************
 * Macros
 * **************************************************************************/

/* Tooltip text displayed when mouse hovers over the tray icon. */
#define TRAY_ICON_TOOLTIP_TEXT "Enso is running."


/* ***************************************************************************
 * Public Function Declarations
 * **************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/* ------------------------------------------------------------------------
 * Sets the path to the icon file to be used by the tray icon.
 * ........................................................................
 * ----------------------------------------------------------------------*/

void
setTrayIconPath( const char *path );


/* ------------------------------------------------------------------------
 * Adds an item to the system tray menu with the given title and id.
 * ........................................................................
 *
 * Return value is true (nonzero) iff the operation was successful.
 *
 * ----------------------------------------------------------------------*/

int
addTrayMenuItem( const char *menuTitle,
                 int menuId );


/* ------------------------------------------------------------------------
 * Returns whether or not the module is initialized.
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
isTrayIconManagerInitialized( void );


/* ------------------------------------------------------------------------
 * Initializes the Tray Icon Manager.
 * ........................................................................
 *
 * This must be called before any other functions in this module are
 * used.
 *
 * Return value is true (nonzero) iff the operation was successful.
 *
 * ----------------------------------------------------------------------*/

int
initTrayIconManager( DWORD parentThreadId );

/* ------------------------------------------------------------------------
 * Shuts down the Tray Icon Manager.
 * ........................................................................
 *
 * This must be called after the client is finished using this module.
 *
 * Return value is true (nonzero) iff the operation was successful.
 *
 * ----------------------------------------------------------------------*/

int
shutdownTrayIconManager( void );

#ifdef __cplusplus
};
#endif
