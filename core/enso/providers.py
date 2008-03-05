# TODO: Add documentation for this module.

import logging

import enso.config

_interfaces = {}

_providers = []

def _initDefaultProviders():
    for moduleName in enso.config.PROVIDERS:
        try:
            # Import the module; most of this code was taken from the
            # Python Library Reference documentation for __import__().
            module = __import__( moduleName, {}, {}, [], 0 )
            components = moduleName.split( "." )
            for component in components[1:]:
                module = getattr( module, component )

            _providers.append( module )
            logging.info( "Added provider %s." % moduleName )
        except ImportError:
            logging.info( "Skipping provider %s." % moduleName )

def getInterface( name ):
    if not _providers:
        _initDefaultProviders()
    if name not in _interfaces:
        for provider in _providers:
            interface = provider.provideInterface( name )
            if interface:
                logging.info( "Obtained interface '%s' from provider '%s'."
                              % (name, provider.__name__) )
                _interfaces[name] = interface
                break
    if name in _interfaces:
        return _interfaces[name]
    else:
        raise ProviderNotFoundError( name )

class ProviderNotFoundError( Exception ):
    pass
