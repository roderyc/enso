# TODO: Add documentation for this module.

import logging

import enso.config

def install( eventManager ):
    eventManager.registerResponder( _init, "init" )

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
        except:
            logging.warn( "Error while loading plugin '%s'." % moduleName )
            raise
        logging.info( "Loaded plugin '%s'." % moduleName )
