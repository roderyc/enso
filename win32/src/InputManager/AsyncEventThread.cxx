/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2005 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   AsyncEventThread.cxx
 *   Maintainer: Jono DiCarlo
 *
 *   Implementation for Mehitabel's asynchronous event processing thread.
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

/* ***************************************************************************
 * Include Files
 * **************************************************************************/

#include <stdio.h>

#include "Input/AsyncEventThread.h"
#include "Logging/Logging.h"
#include "GlobalConstants.h"


/* ***************************************************************************
 * Static Member Variables
 * **************************************************************************/

ATOM AsyncEventThread::_asyncMessageWindowClass = NULL;


/* ***************************************************************************
 * Class Member Implementations
 * **************************************************************************/

/* ------------------------------------------------------------------------
 * Constructor for the async event thread.
 * ........................................................................
 * ----------------------------------------------------------------------*/

AsyncEventThread::AsyncEventThread( void ) :
    EnsoThread(),
    _asyncMessageWindow( NULL ),
    _terminating( false )
{
    _parentThreadId = GetCurrentThreadId();

    /* LONGTERM TODO: There should be some way for us to bail if an
     * error occurs. */

    if ( !initAsyncEventProcessorRegistry() )
        errorMsg( "initAsyncEventProcessorRegistry() failed." );
    else {
        int wasSuccessful;

        /* Register our event processor function. */

        wasSuccessful = registerAsyncEventProc(
            WM_USER_QUIT,
            &AsyncEventThread::_asyncEventProc
            );

        if ( !wasSuccessful )
            errorMsg( "Registering WM_USER_QUIT failed." );
    }
}

/* ------------------------------------------------------------------------
 * Destructor for the async event thread.
 * ........................................................................
 * ----------------------------------------------------------------------*/

AsyncEventThread::~AsyncEventThread( void )
{
    if ( !shutdownAsyncEventProcessorRegistry() )
        errorMsg( "shutdownAsyncEventProcessorRegistry() failed." );
}

/* ------------------------------------------------------------------------
 * Stops the thread, blocking until it is finished running.
 * ........................................................................
 * ----------------------------------------------------------------------*/

void
AsyncEventThread::stop( void )
{
    _terminating = true;
    if ( PostMessage(_asyncMessageWindow, WM_USER_QUIT, 0, 0) == 0 )
    {
        errorMsg( "PostMessage() failed." );
    }
}

/* ------------------------------------------------------------------------
 * Creates the asynchronous message-only window.
 * ........................................................................
 * ----------------------------------------------------------------------*/

bool
AsyncEventThread::_createMessageWindow( void )
{
    bool success = false;
    char errorText[256];
    DWORD errorCode;

    /* Make the invis window class, if needed, with
     * _asyncMessageWindowProc as its window procedure. */
    if ( !_asyncMessageWindowClass )
    {
        WNDCLASSEX info;

        memset( &info, 0, sizeof(info) );

        info.cbSize = sizeof( info );
        info.hInstance = GetModuleHandle( NULL );
        info.lpszClassName = ASYNC_EVENT_WINDOW_CLASS_NAME;

        info.style = 0;
        info.lpfnWndProc = _asyncMessageWindowProc;

        _asyncMessageWindowClass = RegisterClassEx( &info );
    
        if ( !_asyncMessageWindowClass )
        {
            errorMsg( "RegisterClassEx failed." );

            errorCode = GetLastError();
            sprintf( errorText, "Called RegisterClassEx, got error %d.",
                     errorCode );
            infoMsg( errorText );
        }
    }

    /* Make the instance of the message-only window */
    if ( _asyncMessageWindowClass )
    {
        _asyncMessageWindow = CreateWindowEx( 
            0,                              /* dwExStyle    */         
            ASYNC_EVENT_WINDOW_CLASS_NAME,  /* lpClassName  */
            NULL,                           /* lpWindowName */
            0,                              /* dwStyle      */
            0,                              /* x            */
            0,                              /* y            */
            0,                              /* nWidth       */
            0,                              /* nHeight      */
            NULL,                           /* hWndParent   */
            NULL,                           /* hMenu        */
            GetModuleHandle( NULL ),        /* hInstance    */
            this                            /* lpParam      */
            );

        if ( _asyncMessageWindow )
        {
            /* Tell async event proc registry where to find this window */
            setAsyncEventWindow( _asyncMessageWindow );
            success = true;
        } else {
            /* Handle errors */
            errorCode = GetLastError();
            sprintf( errorText, "CreateWindowEx failed with error %d.",
                     errorCode );
            errorMsg( errorText );
        }
    }

    /* If there were errors, bail out gracefully */
    if ( !success )
        _closeMessageWindow();

    return success;
}

/* ------------------------------------------------------------------------
 * Closes the message-only window.
 * ........................................................................
 * ----------------------------------------------------------------------*/

void
AsyncEventThread::_closeMessageWindow( void )
{
    if ( _asyncMessageWindow )
    {
        DestroyWindow( _asyncMessageWindow );
        _asyncMessageWindow = NULL;
    }
}

/* ------------------------------------------------------------------------
 * Window function for the async event thread.
 * ........................................................................
 * 
 * Window creation events are handled here, but everything else is
 * delegated to the AsyncEventProcessorRegistry.
 *
 * ----------------------------------------------------------------------*/

LRESULT CALLBACK
AsyncEventThread::_asyncMessageWindowProc( HWND theWindow,
                                           UINT msg,
                                           WPARAM wParam,
                                           LPARAM lParam )
{
    /* Pass the event along to asyncEventProcessorRegistry */
    return dispatchAsyncEvent( theWindow, msg, wParam, lParam );
}

/* ------------------------------------------------------------------------
 * Message processor function for quit events.
 * ........................................................................
 *
 * This function gets registered with the AsyncEventProcessorRegistry,
 * so it will be called when this thread processes WM_USER_QUIT type
 * events.
 * 
 * ----------------------------------------------------------------------*/

LRESULT
AsyncEventThread::_asyncEventProc( HWND theWindow,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam )
{
    switch ( msg )
    {
    case WM_USER_QUIT:
        /* Sent by our owner when we're terminating.  We don't need to
         * do anything; this is just to kick the event loop. */
        infoMsg( "async event thread: quit signal received." );
        return (LRESULT)0;
    }

    /* This shouldn't happen, because this function should only be called
     * in response to the event types it was registered for. */
    errorMsg( "Bad event type message passed in." );
    return (LRESULT)0;
}

/* ------------------------------------------------------------------------
 * Main execution loop for the async event thread. 
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
AsyncEventThread::_run( void )
{
    _terminating = false;

    /* Set up some things, handling errors. */
    if ( _createMessageWindow() == false ) {
        errorMsg( "_createMessageWindow() failed!" );
        return ASYNC_EVENT_THREAD_ABORTED;
    } else
        infoMsg( "Created invisible window." );

    /* Let everyone know we're done initializing. */
    _signalInitializingFinished();

    int returnValue = ASYNC_EVENT_THREAD_SUCCESSFUL;

    /* Main event loop for this thread: process events until
     * _terminating is true */
    while ( !_terminating )
    {
        MSG theMessage;
        BOOL result;

        /* Get the next message and dispatch it. */
        result = GetMessage( &theMessage, 0, 0, 0 );

        if ( result != -1 )
        {
            TranslateMessage( &theMessage );
            DispatchMessage( &theMessage );
        } else {
            errorMsg( "GetMessage() error." );
            returnValue = ASYNC_EVENT_THREAD_ABORTED;
            _terminating = true;
        }
    }

    _closeMessageWindow();
    infoMsg( "Closed invisible window." );

    return returnValue;
}
