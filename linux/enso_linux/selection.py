"""
Author : Guillaume "iXce" Seguin
Email  : guillaume@segu.in

Copyright (C) 2008 Guillaume Seguin

Temp license:

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
"""

from time import sleep, time, clock

import Xlib
from enso_linux.utils import *

GET_TIMEOUT = 1.5
PASTE_STATE = Xlib.X.ShiftMask
PASTE_KEY = "Insert"

def get_clipboard_text_cb (clipboard, text, userdata):
    '''Callback for clipboard fetch handling'''
    global selection_text
    selection_text = text

def get_focussed_window (display):
    '''Get the currently focussed window'''
    input_focus = display.get_input_focus ()
    window = Xlib.X.NONE
    if input_focus != None and input_focus.focus:
        window = input_focus.focus
    return window

def make_key (keycode, state, window, display):
    '''Build a data dict for a KeyPress/KeyRelease event'''
    root = display.screen ().root
    event_data = {
        "time": int (time ()),
        "root": root,
        "window": window,
        "same_screen": True,
        "child": Xlib.X.NONE,
        "root_x": 0,
        "root_y": 0,
        "event_x": 0,
        "event_y": 0,
        "state": state,
        "detail": keycode,
                 }
    return event_data

def fake_key_up (key, window, display):
    '''Fake a keyboard press event'''
    event = Xlib.protocol.event.KeyPress (**key)
    window.send_event (event, propagate = True)
    display.sync ()

def fake_key_down (key, window, display):
    '''Fake a keyboard release event'''
    event = Xlib.protocol.event.KeyRelease (**key)
    window.send_event (event, propagate = True)
    display.sync ()

def fake_key_updown (key, window, display):
    '''Fake a keyboard press/release events pair'''
    fake_key_up (key, window, display)
    fake_key_down (key, window, display)

def fake_paste (display = None):
    '''Fake a "paste" keyboard event'''
    if not display:
        display = get_display ()
    window = get_focussed_window (display)
    state = PASTE_STATE
    keycode = get_keycode (key = PASTE_KEY, display = display)
    key = make_key (keycode, state, window, display)
    return fake_key_updown (key, window, display)

def get ():
    '''Fetch text from X PRIMARY selection'''
    global selection_text
    selection_text = None
    clipboard = gtk.clipboard_get (selection = "PRIMARY")
    clipboard.request_text (get_clipboard_text_cb)
    # Iterate until we actually received something, or we timed out waiting
    start = clock ()
    while not selection_text and (clock () - start) < GET_TIMEOUT:
        gtk.main_iteration (False)
    if not selection_text:
        selection_text = ""
    selection = {
                    "text": selection_text,
                }
    return selection

def set (seldict):
    '''Paste data into X CLIPBOARD selection'''
    if seldict.has_key ("text"):
        clipboard = gtk.clipboard_get (selection = "CLIPBOARD")
        clipboard.set_text (seldict["text"])
        primary = gtk.clipboard_get (selection = "PRIMARY")
        primary.set_text (seldict["text"])
        fake_paste ()
        clipboard.set_text ("")
        primary.set_text ("")
        return True
    return False

