# ----------------------------------------------------------------------------
# Copyright (c) 2005 Humanized, Inc. All rights reserved.
# ----------------------------------------------------------------------------
#
#   Graphics/__init__.py
#   Maintainer: Atul Varma
#
#   Python Version - 2.4
#
# ----------------------------------------------------------------------------

"""
    This is largely just a placeholder script that allows Python to
    interpret this directory as the Graphics package.

    Behind the scenes, however, it also internally performs a few
    tweaks to the namespace and defines a few functions that don't fit
    into any particular submodule of this package.
"""

# ----------------------------------------------------------------------------
# This file contains trade secrets of Humanized, Inc. No part
# may be reproduced or transmitted in any form by any means or for any purpose
# without the express written permission of Humanized, Inc.
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Imports
# ----------------------------------------------------------------------------

import pyAA
import win32gui

import TransparentWindow

# Aliases to external names.
getDesktopSize = TransparentWindow._getDesktopSize


# ----------------------------------------------------------------------------
# Utilities
# ----------------------------------------------------------------------------

def _safeCallFunc( method ):
    """
    pyAA objects refere to windows that can vanish at any time.

    Calling a pyAA object's method indirectly through this function
    guarantees either a successful return value, or None, without
    any errors being raised because the refered-to window has
    ceased to exist.
    """
    
    try:
        return method()
    except pyAA.Error:
        return None


def getStartBarRect():
    """
    Returns the rectangle outlining the dimensions of the host
    system's Windows Start Bar, as a tuple of the form ((left, top),
    (width, height)).
    """
    
    hwnd = win32gui.GetDesktopWindow()
    objid = pyAA.Constants.OBJID_WINDOW
    desktop = pyAA.AccessibleObjectFromWindow(hwnd, objid)
    
    clients = [ child
                for child in desktop.Children
                if _safeCallFunc( child.GetRoleText ) == "client" ]

    if not clients:
        return (0,0), (0,0)
        # LONGTERM TODO: For some reason, this causes crashes on Japanese
        # language versions of Windows. See trac ticket #149.
        #raise Exception( "No Accessible Object of role 'client'." )
    
    trays = [ child
              for child in clients[0].Children
              if _safeCallFunc( child.GetClassName ) == "Shell_TrayWnd" ]

    if trays:
        tray = trays[0]
        left, top, width, height = tray.Location
    else:
        # This means that the tray isn't in existence. This might occur when a
        # fullscreen application is being opened. For instance, the Pinball
        # game that is included with Windows.
        left, top, width, height = (0, 0, 0, 0)

    return (left, top), (width, height)
