/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2006 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   LoggingBackend.i
 *   Maintainer: Jono DiCarlo
 *
 *   Input file for SWIG.  Creates wrappers for the C functions defined in
 *   Logging.h, and allows you to call certain of the functions from Python.
 *   The build process accomplishes this by running SWIG on this file.
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

%module LoggingBackend
%{
#include "Logging/Logging.h"
%}

/**************************************************************************
 * C Functions to be Exposed in Python
 **************************************************************************/

// LONGTERM TODO:
// Rename some of the C functions to meet our Python naming conventions.

%include "Logging/LoggingConstants.h"

void
_logMessageImpl( int level,
                 const char *msgString,
                 const char *file,
                 int line );

void
setLoggingThreshold( int newLevel );

int
getLoggingThreshold( void );

%rename(_initLoggingFile) initLoggingFile;
void
initLoggingFile( int level,
                 const char *fileName,
                 int useStdErrToo );

void
initLoggingStdErr( int level );

%rename(_shutdownLogging) shutdownLogging;
void
shutdownLogging( void );

int
isLoggingInitialized( void );

int
isLoggingToFile( void );

int
isLoggingToStdErr( void );

const char*
getLoggingFileName( void );

int
hasErrorBeenLogged( void );
