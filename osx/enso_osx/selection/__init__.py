import AppKit
import ctypes
import time
import os

key_utils = ctypes.CDLL( os.path.join( __path__[0], "key_utils.so" ) )

def _getClipboardText():
    pb = AppKit.NSPasteboard.generalPasteboard()
    if pb.availableTypeFromArray_( [AppKit.NSStringPboardType] ):
        return unicode( pb.stringForType_( AppKit.NSStringPboardType ) )
    else:
        return u""

def _getClipboardFiles():
    pb = AppKit.NSPasteboard.generalPasteboard()
    if pb.availableTypeFromArray_( [AppKit.NSFilenamesPboardType] ):
        files = pb.propertyListForType_( AppKit.NSFilenamesPboardType )
        return [ unicode( filename ) for filename in files ]
    else:
        return []

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
        selection["text"] = _getClipboardText()
        selection["files"] = _getClipboardFiles()

    return selection

def set( seldict ):
    success = False

    # TODO: Set 'files' selection.

    if seldict.get( "text" ) and _setClipboardText( seldict["text"] ):
        # TODO: Figure out whether the paste is successful.
        key_utils.simulatePaste()
        success = True

    return success
