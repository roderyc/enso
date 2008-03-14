/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/* ***************************************************************************
 * Copyright (c) 2005 Humanized, Inc. All rights reserved.
 * ***************************************************************************
 *
 *   LoggingConstants.h
 *   Maintainer: Jono DiCarlo
 *
 *   Defines constants used by Logging.h and Logging.i
 *
 * ***************************************************************************
 * This file contains trade secrets of Humanized, Inc. No part may be
 * reproduced or transmitted in any form by any means or for any
 * purpose without the express written permission of Humanized, Inc.
 * **************************************************************************/

#ifndef _LOGGINGCONSTANTS_H_
#define _LOGGINGCONSTANTS_H_

/* Constants defining severity levels of logging messages.  Use one of
 * these as an argument to initLogging. */

#define LOGGING_ERROR 4
#define LOGGING_WARN 3
#define LOGGING_INFO 2
#define LOGGING_DEBUG 1

/* Constants defining the success of logging functions. */

#define LOGGING_RESULT_SUCCESS 0
#define LOGGING_RESULT_ERROR 1

/* File encoding for the logging file. */

#define LOGGING_FILE_ENCODING "utf-8"

#endif





