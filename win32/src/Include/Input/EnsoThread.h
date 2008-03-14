/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2005 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   EnsoThread.h
 *   Maintainer: Jono DiCarlo
 *
 *   Header file for Mehitabel's custom thread class.
 *
 *   EnsoThread is an abstract base class that provides a
 *   platform-independent interface to encapsulate an operating system
 *   thread.
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

#ifndef _ENSOTHREAD_H
#define _ENSOTHREAD_H

/* ***************************************************************************
 * Include Files
 * **************************************************************************/

#include "WinSdk.h"


/* ***************************************************************************
 * Constants
 * **************************************************************************/

/* Amount of time we can tolerably wait for an object, in
 * milliseconds. */
#define THREAD_WAIT_TIMEOUT  10000


/* ===========================================================================
 * EnsoThread class
 * ...........................................................................
 *
 * The EnsoThread class encapsulates an OS thread. Actual threads
 * should subclass this thread and provide a concrete run() method.
 *
 * =========================================================================*/

class EnsoThread
{
public:

    /* ====================================================================
     * Construction and Destruction
     * ==================================================================*/

    /* --------------------------------------------------------------------
     * Constructor
     * --------------------------------------------------------------------
     *
     * This simple constructor creates the thread, but does not
     * start it.
     *
     * ------------------------------------------------------------------*/

    EnsoThread( void );

    /* --------------------------------------------------------------------
     * Destructor
     * --------------------------------------------------------------------
     *
     * Note that this destructor does not stop the thread from running;
     * it is assumed that the thread has already stopped running by
     * the time this destructor is called.
     *
     * ------------------------------------------------------------------*/

    virtual ~EnsoThread( void );

    /* ====================================================================
     * Public Member Functions
     * ==================================================================*/

    /* --------------------------------------------------------------------
     * Starts the thread.
     * ....................................................................
     *
     * This begins executing the thread, and does not return until the
     * thread has signaled that it is initialized.  If it does not
     * signal that it is initialied within THREAD_WAIT_TIMEOUT
     * milliseconds, or if the thread exits before it is initialized,
     * a MehitabelException is thrown.
     *
     * ------------------------------------------------------------------*/

    virtual void
    start( void );

    /* --------------------------------------------------------------------
     * Waits for the thread to exit and returns its exit code.
     * ....................................................................
     *
     * Waits for the thread to exit and returns its exit code. A
     * MehitabelException is thrown if the thread does not exit within
     * THREAD_WAIT_TIMEOUT milliseconds.
     *
     * ------------------------------------------------------------------*/

    int
    waitForThreadExitCode( void );

    /* --------------------------------------------------------------------
     * Returns whether the thread is still running or not.
     * ....................................................................
     * ------------------------------------------------------------------*/

    bool
    isAlive( void );

protected:

    /* ====================================================================
     * Protected Member Functions
     * ==================================================================*/

    /* Thread objects should call this from their run() method when
     * they're done initializing themselves. */
    void
    _signalInitializingFinished( void );

private:

    /* ====================================================================
     * Private Member Functions
     * ==================================================================*/

    static DWORD WINAPI
    _threadProc( LPVOID lpParameter );

    /* Run method implements whatever the thread is supposed to actually do.
     * It is meant to be overridden by subclasses.
     * It's private because it is called indirectly by the start() method. */
    virtual int
    _run( void ) = 0;

    /* ====================================================================
     * Private Data Members
     * ==================================================================*/

    /* Handle to the thread-initialized event. */
    HANDLE _threadInitializedEvent;

    /* The thread id. */
    DWORD _threadId;

    /* Handle to the thread. */
    HANDLE _threadHandle;
};

#endif
