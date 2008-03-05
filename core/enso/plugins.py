# TODO: Add documentation for this module.

import logging
import atexit

import enso.config

_plugins = []

def install( eventManager ):
    eventManager.registerResponder( _init, "init" )
    atexit.register( _shutdown )

def _init():
    for moduleName in enso.config.PLUGINS:
        try:
            # Import the module; most of this code was taken from the
            # Python Library Reference documentation for __import__().
            module = __import__( moduleName, {}, {}, [], 0 )
            components = moduleName.split( "." )
            for component in components[1:]:
                module = getattr( module, component )

            module.load()
            _plugins.append( (module, moduleName) )
        except:
            logging.warn( "Error while loading plugin '%s'." % moduleName )
            raise
        logging.info( "Loaded plugin '%s'." % moduleName )

def _shutdown():
    for module, moduleName in _plugins:
        try:
            module.unload()
        except:
            logging.warn( "Error while unloading plugin '%s'." % moduleName )
            raise
        logging.info( "Unloaded plugin '%s'." % moduleName )
    _plugins[:] = []
