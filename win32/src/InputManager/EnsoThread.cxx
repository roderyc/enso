/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2005 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   EnsoThread.cxx
 *   Maintainer: Jono DiCarlo
 *
 *   Implementation for Mehitabel's custom thread class.
 *
 *   The EnsoThread abstract base class provides a wrapper around a
 *   win32 system thread, and is subclassed to create
 *   AsyncEventThread.
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

#include "Input/EnsoThread.h"
#include "Logging/Logging.h"
#include "InputManagerExceptions.h"


/* ***************************************************************************
 * Public Methods
 * **************************************************************************/

/* ------------------------------------------------------------------------
 * This constructor initializes the EnsoThread.
 * ........................................................................
 * ----------------------------------------------------------------------*/

EnsoThread::EnsoThread( void )
{
    _threadHandle = CreateThread(
        NULL,                          /* lpThreadAttributes */
        0,                             /* dwStackSize */
        EnsoThread::_threadProc,       /* lpStartAddress */
        this,                          /* lpParameter */
        CREATE_SUSPENDED,              /* dwCreationFlags */
        &_threadId                     /* lpThreadId */
        );

    if ( _threadHandle == NULL )
    {
        errorMsg( "CreateThread() failed." );
        throw MehitabelException();
    }

    _threadInitializedEvent = CreateEvent(
        NULL,                          /* lpEventAttributes */
        TRUE,                          /* bManualReset */
        FALSE,                         /* bInitialState */
        NULL                           /* lpName */
        );

    if ( _threadInitializedEvent == NULL )
    {
        CloseHandle( _threadHandle );

        errorMsg( "CreateEvent() failed." );
        throw MehitabelException();        
    }
}

/* ------------------------------------------------------------------------
 * This destructor shuts down the EnsoThread.
 * ........................................................................
 * ----------------------------------------------------------------------*/

EnsoThread::~EnsoThread( void )
{
    if ( CloseHandle(_threadHandle) == 0 )
        errorMsg( "CloseHandle() failed for thread handle." );

    if ( CloseHandle(_threadInitializedEvent) == 0 )
        errorMsg( "CloseHandle() failed for thread-initialized event." );
}

/* ------------------------------------------------------------------------
 * Starts the thread.
 * ........................................................................
 * ----------------------------------------------------------------------*/

void
EnsoThread::start( void )
{
    if ( ResumeThread(_threadHandle) == (DWORD) (-1) )
    {
        errorMsg( "ResumeThread() failed." );
        throw MehitabelException();
    }

    HANDLE handles[2];

    handles[0] = _threadHandle;
    handles[1] = _threadInitializedEvent;

    DWORD waitResult = WaitForMultipleObjects(
        2,                                     /* nCount */
        handles,                               /* lpHandles */
        FALSE,                                 /* bWaitAll */
        THREAD_WAIT_TIMEOUT                    /* dwMilliseconds */
        );

    if ( waitResult != (WAIT_OBJECT_0 + 1) )
    {
        DWORD lastError = GetLastError();

        errorMsg( "Thread did not initialize properly." );
        infoMsgInt( "WaitForMultipleObjects() returned: ", waitResult );
        if ( waitResult == WAIT_FAILED )
            infoMsgInt( "  GetLastError(): ", lastError );
        throw MehitabelException();
    }
}

/* ------------------------------------------------------------------------
 * Waits for the thread to exit and returns its exit code.
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
EnsoThread::waitForThreadExitCode( void )
{
    DWORD waitResult = WaitForSingleObject( _threadHandle,
                                            THREAD_WAIT_TIMEOUT );

    if ( waitResult != WAIT_OBJECT_0 )
    {
        errorMsg( "WaitForSingleObject() failed." );
        throw MehitabelException();
    }

    DWORD exitCode;

    if ( GetExitCodeThread(_threadHandle, &exitCode) == 0 )
    {
        errorMsg( "GotExitCodeThread() failed." );
        throw MehitabelException();
    }

    return exitCode;
}

/* ------------------------------------------------------------------------
 * Returns whether the thread is still running or not.
 * ........................................................................
 * ----------------------------------------------------------------------*/

bool
EnsoThread::isAlive( void )
{
    DWORD exitCode;
    BOOL retVal;

    retVal = GetExitCodeThread( _threadHandle, &exitCode );
    if ( retVal == 0 )
    {
        errorMsg( "GotExitCodeThread() failed." );
        throw MehitabelException();
    }

    return ( exitCode == STILL_ACTIVE );
}

/* ------------------------------------------------------------------------
 * Wrapper for starting the thread.
 * ........................................................................
 * ----------------------------------------------------------------------*/

DWORD WINAPI
EnsoThread::_threadProc( LPVOID lpParameter )
{
    EnsoThread *thread = (EnsoThread *) lpParameter;

    return thread->_run();
}

/* ------------------------------------------------------------------------
 * Signal that initializing is complete.
 * ........................................................................
 * ----------------------------------------------------------------------*/

void
EnsoThread::_signalInitializingFinished( void )
{
    if ( SetEvent(_threadInitializedEvent) == 0 )
        /* LONGTERM TODO: Is there some way we can bail out here?
         * Otherwise we'll hang... */
        errorMsg( "SetEvent() failed." );
}
