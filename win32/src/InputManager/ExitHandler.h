/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2005 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   ExitHandler.h
 *   Maintainer: Jono DiCarlo
 *
 *   Header file for Mehitabel's exit handler.  The exit handler
 *   provides a simple mechanism that allows applications to tell Enso
 *   to gracefully shut itself down by sending the async event window
 *   a WM_CLOSE message.
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
 * Installs the exit handler.
 * ........................................................................
 *
 * Returns true (nonzero) iff the operation was successful.
 *
 * ----------------------------------------------------------------------*/

int
installExitHandler( DWORD threadId );

/* ------------------------------------------------------------------------
 * Removes the exit handler, if it's currently active.
 * ........................................................................
 *
 * Returns true (nonzero) iff the operation was successful.
 *
 * ----------------------------------------------------------------------*/

int
removeExitHandler( void );

#ifdef __cplusplus
};
#endif
