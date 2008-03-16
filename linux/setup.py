#!/usr/bin/env python

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

import sys, os, glob
from stat import *
from distutils.core import setup
from distutils.command.install import install as _install
from distutils.command.install_data import install_data as _install_data

INSTALLED_FILES = "installed_files"

class install (_install):

    def run (self):
        _install.run (self)
        outputs = self.get_outputs ()
        length = 0
        if self.root:
            length += len (self.root)
        if self.prefix:
            length += len (self.prefix)
        if length:
            for counter in xrange (len (outputs)):
                outputs[counter] = outputs[counter][length:]
        data = "\n".join (outputs)
        try:
            file = open (INSTALLED_FILES, "w")
        except:
            self.warn ("Could not write installed files list %s" % \
                       INSTALLED_FILES)
            return 
        file.write (data)
        file.close ()

class install_data (_install_data):

    def run (self):
        def chmod_data_file (file):
            try:
                os.chmod (file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
            except:
                self.warn ("Could not chmod data file %s" % file)
        _install_data.run (self)
        map (chmod_data_file, self.get_outputs ())

class uninstall (_install):

    def run (self):
        try:
            file = open (INSTALLED_FILES, "r")
        except:
            self.warn ("Could not read installed files list %s" % \
                       INSTALLED_FILES)
            return 
        files = file.readlines ()
        file.close ()
        prepend = ""
        if self.root:
            prepend += self.root
        if self.prefix:
            prepend += self.prefix
        if len (prepend):
            for counter in xrange (len (files)):
                files[counter] = prepend + files[counter].rstrip ()
        for file in files:
            print "Uninstalling %s" % file
            try:
                os.unlink (file)
            except:
                self.warn ("Could not remove file %s" % file)

ops = ("install", "build", "sdist", "uninstall", "clean")

if len (sys.argv) < 2 or sys.argv[1] not in ops:
    print "Please specify operation : %s" % " | ".join (ops)
    raise SystemExit

prefix = None
if len (sys.argv) > 2:
    i = 0
    for o in sys.argv:
        if o.startswith ("--prefix"):
            if o == "--prefix":
                if len (sys.argv) >= i:
                    prefix = sys.argv[i + 1]
                sys.argv.remove (prefix)
            elif o.startswith ("--prefix=") and len (o[9:]):
                prefix = o[9:]
            sys.argv.remove (o)
            break
        i += 1
if not prefix and "PREFIX" in os.environ:
    prefix = os.environ["PREFIX"]
if not prefix or not len (prefix):
    prefix = "/usr/local"

if sys.argv[1] in ("install", "uninstall") and len (prefix):
    sys.argv += ["--prefix", prefix]

version_file = open ("VERSION", "r")
version = version_file.read ().strip ()
if "=" in version:
    version = version.split ("=")[1]

data_files = []

podir = os.path.join (os.path.abspath (os.path.curdir), "po")
if os.path.isdir (podir):
    buildcmd = "msgfmt -o build/locale/%s/enso_linux.mo po/%s.po"
    mopath = "build/locale/%s/enso_linux.mo"
    destpath = "share/locale/%s/LC_MESSAGES"
    for name in os.listdir (podir):
        if name[-2:] == "po":
            name = name[:-3]
            if sys.argv[1] == "build" \
               or (sys.argv[1] == "install" and \
                   not os.path.exists (mopath % name)):
                if not os.path.isdir ("build/locale/" + name):
                    os.makedirs ("build/locale/" + name)
                os.system (buildcmd % (name, name))
            data_files.append ((destpath % name, [mopath % name]))

setup (
        name             = "Enso Linux",
        version          = version,
        description      = "Enso Linux backend",
        author           = "Guillaume Seguin",
        author_email     = "guillaume@segu.in",
        url              = "http://guillaume.segu.in/enso/",
        license          = "GPL",
        data_files       = data_files,
        packages         = [
                               "enso_linux",
                           ],
        cmdclass         = {"uninstall" : uninstall,
                            "install" : install,
                            "install_data" : install_data}
     )
