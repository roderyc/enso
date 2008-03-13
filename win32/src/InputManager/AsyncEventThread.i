/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2006 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   AsyncEventThread.i
 *   Maintainer: Jono DiCarlo
 *
 *   SWIG interface file for the AsyncEventThread singleton.
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

%module AsyncEventThread

%exception {
    try {
        $action
    } catch ( MehitabelException ) {
       PyErr_SetString( PyExc_RuntimeError,
                        "A MehitabelException occurred." );
       return NULL;
    }
}

%{
#include "Input/AsyncEventThread.h"
#include "Logging/Logging.h"
#include "InputManagerExceptions.h"

/* The AsyncEventThread singleton. */
static AsyncEventThread *_asyncEventThread = NULL;

/* Starts the async event thread. */
static void
startAsyncEventThread( void )
{
    if ( _asyncEventThread != NULL )
    {
        errorMsg( "Async event thread is already running." );
        throw MehitabelException();
    }

    _asyncEventThread = new AsyncEventThread();

    /* Initialize the async event thread. Note that it will throw an
     * exception if it isn't able to properly initialize itself. */
    _asyncEventThread->start();
}

/* Stops the async event thread. */
static void
stopAsyncEventThread( void )
{
    if ( _asyncEventThread == NULL )
    {
        errorMsg( "Async event thread is not running." );
        throw MehitabelException();
    }

    /* Clean up the async event thread. */
    _asyncEventThread->stop();
    if ( _asyncEventThread->waitForThreadExitCode() != \
         ASYNC_EVENT_THREAD_SUCCESSFUL )
    {
        errorMsg( "Async event thread didn't shutdown properly." );
        throw MehitabelException();
    }

    delete _asyncEventThread;
    _asyncEventThread = NULL;
}

%}

%rename(start) startAsyncEventThread;
void
startAsyncEventThread( void );

%rename(stop) stopAsyncEventThread;
void
stopAsyncEventThread( void );
