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

from Xlib.display import Display
import gtk
import os

def get_display ():
    return Display (os.environ["DISPLAY"])

def get_keycode (key, display = None):
    '''Helper function to get a keycode from raw key name'''
    if not display:
        display = get_display ()
    keysym = gtk.gdk.keyval_from_name (key)
    keycode = display.keysym_to_keycode (keysym)
    return keycode
