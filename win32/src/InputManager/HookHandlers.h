/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2005 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   HookHandlers.h
 *   Maintainer: Jono DiCarlo
 *
 *   Header file for Mehitabel's mouse/keyhook handlers.
 *
 *   The mouse/keyhook handlers set up low-level Windows hooks that
 *   enable us to intercept all keyboard and mouse input in the system
 *   and notify another thread of these events.
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

#include "WinSdk.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ------------------------------------------------------------------------
 * Installs the keyboard hook.
 * ........................................................................
 *
 * The hook will be dependent on the health of the thread owning the
 * AsyncEventWindow, meaning that if the thread ever behaves
 * abnormally (e.g., takes a long time to process some event), the
 * system's keyboard responsiveness will suffer as a result.
 *
 * "threadId" is the thread that the hook will send their generated
 * events to (as Windows messages).
 *
 * Returns true (nonzero) iff the operation was successful.
 *
 * ----------------------------------------------------------------------*/

int
installKeyboardHook( DWORD threadId );

/* ------------------------------------------------------------------------
 * Installs the mouse hook.
 * ........................................................................
 *
 * Behavior is analogous to installKeyboardHook(), only for mouse
 * instead.
 *
 * ----------------------------------------------------------------------*/

int
installMouseHook( DWORD threadId );

/* ------------------------------------------------------------------------
 * Removes the keyboard hook, if it's currently active.
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
removeKeyboardHook( void );

/* ------------------------------------------------------------------------
 * Removes the mouse hook, if it's currently active.
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
removeMouseHook( void );

/* ------------------------------------------------------------------------
 * Maps the given quasimode keycode to the given physical keycode.
 * ........................................................................
 *
 * The quasimodeKeycode should correspond to one of the
 * KEYCODE_QUASIMODE_* constants, and names the quasimode keycode
 * that should be set.
 *
 * The keycode will correspond to a KEYCODE_* constant.
 *
 * Once the quasimode begins, all keys pressed will be converted into
 * WM_USER_KEYPRESS events until the quasimode ends.  At all other
 * times, keystrokes are turned into WM_USER_SOMEKEY events.
 *
 * ----------------------------------------------------------------------*/

void
setQuasimodeKeycode( int quasimodeKeycode,
                     int keycode );

/* ------------------------------------------------------------------------
 * Returns the Quasimode keycode.
 * ........................................................................
 *
 * The returned keycode will correspond to a physical KEYCODE_*
 * constant that is currently mapped to the given quasimode
 * keycode constant.
 *
 * ----------------------------------------------------------------------*/

int
getQuasimodeKeycode( int quasimodeKeycode );

/* ------------------------------------------------------------------------
 * Pass true to make behavior modal, false to make it quasimodal (default).
 * ........................................................................
 * ----------------------------------------------------------------------*/

void
setModality( int isModal );

/* ------------------------------------------------------------------------
 * Initializes this module.
 * ........................................................................
 *
 * This must be called before using any functions in this module.
 *
 * ----------------------------------------------------------------------*/

void
initHookHandlers( void );

/* ------------------------------------------------------------------------
 * Shuts down this module.
 * ........................................................................
 *
 * This must be called when finished using this module.
 *
 * ----------------------------------------------------------------------*/

void
shutdownHookHandlers( void );

#ifdef __cplusplus
};
#endif
