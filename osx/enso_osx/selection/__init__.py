import AppKit
import ctypes
import time
import os

key_utils = ctypes.CDLL( os.path.join( __path__[0], "key_utils.so" ) )

def _getClipboardText():
    pb = AppKit.NSPasteboard.generalPasteboard()
    if pb.availableTypeFromArray_( [AppKit.NSStringPboardType] ):
        return pb.stringForType_( AppKit.NSStringPboardType )
    else:
        return None

def _setClipboardText( text ):
    pb = AppKit.NSPasteboard.generalPasteboard()
    pb.declareTypes_owner_( [AppKit.NSStringPboardType], None )
    return pb.setString_forType_( text, AppKit.NSStringPboardType )

def get():
    selection = {}

    # TODO: We're clobbering the clipboard here; fix this.

    oldChangeCount = AppKit.NSPasteboard.generalPasteboard().changeCount()
    key_utils.simulateCopy()
    time.sleep( 0.1 )
    newChangeCount = AppKit.NSPasteboard.generalPasteboard().changeCount()

    if newChangeCount != oldChangeCount:
        # The clipboard contents changed at some point, which
        # means our copy operation was probably successful.
        unicodeText = unicode( _getClipboardText() )
        selection["text"] = unicodeText

    return selection

def set( seldict ):
    success = False

    if seldict.get( "text" ) and _setClipboardText( seldict["text"] ):
        # TODO: Figure out whether the paste is successful.
        key_utils.simulatePaste()
        success = True

    return success
