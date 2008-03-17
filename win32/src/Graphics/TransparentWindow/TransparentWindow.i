/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2006 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   TransparentWindow.i
 *   Maintainer: Atul Varma
 *
 *   TransparentWindow's SWIG interface file.
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

%module TransparentWindow
%{
#include "TransparentWindow.h"
#include "pycairo.h"

static Pycairo_CAPI_t *Pycairo_CAPI = 0;

%}

/* Convert TransparentWindow exceptions to Python exceptions. */
%exception {
    try {
       $action
    } catch (FatalError &e) {
        /* Convert FatalErrors to Python RuntimeError exceptions. */
        PyErr_SetString( PyExc_RuntimeError, e.what() );
        return NULL;
    } catch (RangeError &e) {
        /* Convert RangeErrors to Python ValueError exceptions. */
        PyErr_SetString( PyExc_ValueError, e.what() );
        return NULL;
    }
}

/* Convert these two output parameters to a Python tuple. */
%include "typemaps.i"
%apply int *OUTPUT { int *width, int *height };

/* Use the TransparentWindow's header file to define our Python
 * interface. */
%include "TransparentWindow.h"

/* Define TransparentWindow.makeCairoSurface() for the Python
 * interface. This function will return a pycairo surface. */
%extend TransparentWindow {
    PyObject *makeCairoSurface( void ) {
        cairo_surface_t *surface;
        PyObject *pycairoSurface;

        if ( Pycairo_CAPI == 0 )
            Pycairo_IMPORT;

        surface = self->makeCairoSurface();
        
        pycairoSurface = PycairoSurface_FromSurface(
            surface,
            &PycairoWin32Surface_Type,
            NULL
            );

        if ( pycairoSurface == NULL )
            throw FatalError( "Couldn't init Pycairo surface." );
        else
            return pycairoSurface;
    }
};

