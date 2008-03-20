import logging
import os

from enso.commands.manager import CommandAlreadyRegisteredError
from enso.contrib.scriptotron.tracebacks import TracebackCommand
from enso.contrib.scriptotron.tracebacks import safetyNetted
from enso.contrib.scriptotron import adapters
from enso.contrib.scriptotron import cmdretriever
from enso.contrib.scriptotron import ensoapi

SCRIPTS_FILE_NAME = ".ensocommands"

class ScriptCommandTracker:
    def __init__( self, commandManager ):
        self._cmdExprs = []
        self._cmdMgr = commandManager

    def _clearCommands( self ):
        for cmdExpr in self._cmdExprs:
            self._cmdMgr.unregisterCommand( cmdExpr )
        self._cmdExprs = []

    def _registerCommand( self, cmdObj, cmdExpr ):
        try:
            self._cmdMgr.registerCommand( cmdExpr,
                                          cmdObj )
            self._cmdExprs.append( cmdExpr )
        except CommandAlreadyRegisteredError:
            logging.warn( "Command already registered: %s" % cmdExpr )

    def registerNewCommands( self, commandInfoList ):
        self._clearCommands()
        for info in commandInfoList:
            cmd = adapters.makeCommandFromInfo( info, ensoapi.EnsoApi() )
            self._registerCommand( cmd, info["cmdExpr"] )

class ScriptTracker:
    def __init__( self, eventManager, commandManager ):
        self._scriptCmdTracker = ScriptCommandTracker( commandManager )
        # TODO: os.environ["HOME"] only works 'out-of-the-box' on
        # unix; we need to do something different, preferably
        # in platform-specific backends.
        self._scriptFilename = os.path.join( os.environ["HOME"],
                                             SCRIPTS_FILE_NAME )
        self._fileDependencies = [
            self._scriptFilename
            ]
        self._lastMods = {}
        for fileName in self._fileDependencies:
            self._lastMods[fileName] = None

        eventManager.registerResponder(
            self._updateScripts,
            "startQuasimode"
            )

        commandManager.registerCommand( TracebackCommand.NAME,
                                        TracebackCommand() )

    @classmethod
    def install( cls, eventManager, commandManager ):
        cls( eventManager, commandManager )

    @safetyNetted
    def _getGlobalsFromSourceCode( self, text, filename ):
        allGlobals = {}
        code = compile( text, self._scriptFilename, "exec" )
        exec code in allGlobals
        return allGlobals

    def _reloadPyScripts( self ):
        text = open( self._scriptFilename, "r" ).read()

        allGlobals = self._getGlobalsFromSourceCode(
            text,
            self._scriptFilename
            )

        if allGlobals is not None:
            infos = cmdretriever.getCommandsFromObjects( allGlobals )
            self._scriptCmdTracker.registerNewCommands( infos )

    def _updateScripts( self ):
        shouldReload = False
        for fileName in self._fileDependencies:
            if os.path.exists( fileName ):
                lastMod = os.stat( fileName ).st_mtime
                if lastMod != self._lastMods[fileName]:
                    self._lastMods[fileName] = lastMod
                    shouldReload = True

        if shouldReload:
            self._reloadPyScripts()
