/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2006 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   InputManagerExceptions.h
 *   Maintainer: Jono DiCarlo
 *
 *   Header file defining our custom exception classes.
 *
 *   These exception classes are pretty trivial, so no implementation
 *   file is needed.  They are separated out from InputManager.h in
 *   order to make their inclusion easier in other places that use
 *   them.
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

#ifndef INPUT_MANAGER_EXCEPTIONS_H
#define INPUT_MANAGER_EXCEPTIONS_H

/* ===========================================================================
 * MehitabelException class
 * ...........................................................................
 *
 * Exception thrown whenever anything abnormal happens in the
 * event loop.
 *
 * =========================================================================*/

class MehitabelException { };

/* ===========================================================================
 * MehitabelPythonException class
 * ...........................................................................
 *
 * Exception thrown whenever anything abnormal happens in Python code
 * executed by the InputManager.
 *
 * =========================================================================*/

class MehitabelPythonException : public MehitabelException { };

#endif








