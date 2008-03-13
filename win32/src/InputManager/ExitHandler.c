/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2005 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   ExitHandler.c
 *   Maintainer: Jono DiCarlo
 *
 *   Implementation for the exit handler.
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
#include "Logging/Logging.h"
#include "GlobalConstants.h"


/* ***************************************************************************
 * Private Module Variables
 * **************************************************************************/

/* The parent thread to send generated events to. */
static DWORD _parentThreadId;


/* ***************************************************************************
 * Private Module Function Declarations
 * **************************************************************************/

static LRESULT
_exitHandlerEventProc( HWND theWindow,
                       UINT msg,
                       WPARAM wParam,
                       LPARAM lParam );


/* ***************************************************************************
 * Private Module Functions
 * **************************************************************************/

/* ------------------------------------------------------------------------
 * The hook handler Event Processing Function
 * ........................................................................
 * ----------------------------------------------------------------------*/

static LRESULT
_exitHandlerEventProc( HWND theWindow,
                       UINT msg,
                       WPARAM wParam,
                       LPARAM lParam )
{
    switch ( msg )
    {
    case WM_CLOSE:
        PostThreadMessage( _parentThreadId,
                           WM_USER_EXIT_REQUESTED,
                           0, 0 );
        return (LRESULT)0;
    }

    /* This shouldn't happen, because this function should
     * only be called in response to the event types it was
     * registered for. */
    errorMsg( "Bad event type message passed in." );
    return (LRESULT)0;
}


/* ***************************************************************************
 * Public Module Functions
 * **************************************************************************/

/* ------------------------------------------------------------------------
 * Installs the exit handler.
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
installExitHandler( DWORD threadId )
{
    int wasSuccessful;

    _parentThreadId = threadId;

    wasSuccessful = registerAsyncEventProc(
        WM_CLOSE,
        _exitHandlerEventProc
        );

    if ( !wasSuccessful )
    {
        errorMsg( "Registering WM_CLOSE failed.\n" );
        return 0;
    }

    return 1;
}

/* ------------------------------------------------------------------------
 * Removes the exit handler, if it's currently active.
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
removeExitHandler( void )
{
    if ( !unregisterAsyncEventProc( WM_CLOSE ) )
    {
        errorMsg( "Unregistering WM_CLOSE failed.\n" );
        return 0;
    }

    return 1;
}
