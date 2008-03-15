/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/*
 Copyright (c) 2008, Humanized, Inc.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    3. Neither the name of Enso nor the names of its contributors may
       be used to endorse or promote products derived from this
       software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY Humanized, Inc. ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Humanized, Inc. BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*   Header file for Enso's tray icon manager. */

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
