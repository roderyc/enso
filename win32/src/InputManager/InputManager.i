/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2006 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   InputManager.i
 *   Maintainer: Jono DiCarlo
 *
 *   Input file for SWIG.  Creates wrappers for the C++ classes
 *   defined in InputManager.h, and allows you to extend some of the
 *   classes in Python through the use of SWIG directors.
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

%module( directors="1" ) InputManager
%include "std_string.i"

%define MAKE_CLASS_EXTENDABLE( CLASSNAME )
%feature( "director" ) CLASSNAME;
%enddef

%feature( "director:except" ) {
    if ( $error != NULL ) {
        throw MehitabelPythonException();
    }
}

%exception {
    try {
        $action
    } catch ( MehitabelPythonException ) {
        SWIG_fail;
    } catch ( MehitabelException ) {
       PyErr_SetString( PyExc_RuntimeError,
                        "A MehitabelException occurred." );
       return NULL;
    }
}

%{
#include "InputManager.h"
%}

MAKE_CLASS_EXTENDABLE( InputManager );

%include "InputManager.h"
%include "InputManagerConstants.h"
