/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2005 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   AsyncEventThread.h
 *   Maintainer: Jono DiCarlo
 *
 *   Header file for Mehitabel's asynchronous event processing thread.
 *
 *   This module contains a message-only window, along with a thread
 *   to run it, which handles all user input events at the system
 *   level by doing any necessary low-level processing, forwarding
 *   them to a parent thread, and returning immediately.  Doing this
 *   allows the low-level user interface and host operating system to
 *   remain completely responsive even if Enso hangs or takes a long
 *   time to process something.
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
#include "Input/EnsoThread.h"
#include "WinSdk.h"



/* ***************************************************************************
 * Macros
 * **************************************************************************/

/* Window class name for the invisible window. */
#define ASYNC_EVENT_WINDOW_CLASS_NAME "MehitabelAsyncEventWindow"

/* Thread exit codes */
#define ASYNC_EVENT_THREAD_ABORTED      0
#define ASYNC_EVENT_THREAD_SUCCESSFUL   1   


/* ***************************************************************************
 * Class Declaration
 * **************************************************************************/

/* ===========================================================================
 * AsyncEventThread class
 * ...........................................................................
 *
 * This class encapsulates a message-only window which is used for all-purpose
 * asynchronous event handling.   It is a singleton.
 *
 * The thread's return code is ASYNC_EVENT_THREAD_ABORTED if it
 * failed, and ASYNC_EVENT_THREAD_SUCCESSFUL if it was successful.
 *
 * =========================================================================*/

class AsyncEventThread : public EnsoThread
{
public:

    /* ====================================================================
     * Construction and Destruction
     * ==================================================================*/

    /* --------------------------------------------------------------------
     * Constructor
     * ------------------------------------------------------------------*/

    AsyncEventThread( void );

    /* --------------------------------------------------------------------
     * Destructor
     * ------------------------------------------------------------------*/

    virtual
    ~AsyncEventThread( void );

    /* ====================================================================
     * Public Member Functions
     * ==================================================================*/

    /* --------------------------------------------------------------------
     * Stops the thread.
     * ....................................................................
     * ------------------------------------------------------------------*/

    void
    stop( void );

private:

    /* ====================================================================
     * Private Member Functions
     * ==================================================================*/

    bool
    _createMessageWindow( void );

    void
    _closeMessageWindow( void );

    static LRESULT CALLBACK
    _asyncMessageWindowProc( HWND theWindow,
                             UINT msg,
                             WPARAM wParam,
                             LPARAM lParam );

    static LRESULT
    _asyncEventProc( HWND theWindow,
                     UINT msg,
                     WPARAM wParam,
                     LPARAM lParam );

    virtual int
    _run( void );

    /* ====================================================================
     * Private Data Members
     * ==================================================================*/

    /* ID of the parent thread. */
    DWORD _parentThreadId;

    /* Invisible, message-only window handle to receive events. */
    HWND _asyncMessageWindow;

    /* Invisible window class. */
    static ATOM _asyncMessageWindowClass;

    /* Flag that is set to terminate the event thread. */
    volatile bool _terminating;
};
