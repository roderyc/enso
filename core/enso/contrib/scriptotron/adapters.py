from enso.commands import CommandObject
from enso.commands.factories import GenericPrefixFactory
from enso.commands.factories import ArbitraryPostfixFactory
from enso.messages import displayMessage

ARG_REQUIRED_MSG = "<p>An argument is required.</p>"

class FuncCommand( CommandObject ):
    def __init__( self, cmdName, func, desc,
                  help, takesArg = False, argValue = None ):
        CommandObject.__init__( self )

        self.name = cmdName
        self.func = func
        self.takesArg = takesArg
        self.argValue = argValue

        self.setName( cmdName )
        self.setHelp( help )
        self.setDescription( desc )

    def run( self ):
        if self.takesArg:
            self.func(self.argValue)
        else:
            self.func()

class NoArgumentCommand( CommandObject ):
    def __init__( self, description, message ):
        CommandObject.__init__( self )
        self.setDescription( description )
        self.message = message

    def run( self ):
        displayMessage( self.message )

class ArgFuncMixin( object ):
    def __init__( self, cmdName, cmdExpr, func, argName, desc,
                  help, isArgRequired ):
        self.cmdName = cmdName
        self.func = func
        self.desc = desc
        self.isArgRequired = isArgRequired

        self.HELP_TEXT = argName
        self.NAME = cmdExpr
        self.PREFIX = "%s " % cmdName
        self.DESCRIPTION_TEXT = desc
        self.setHelp( help )

    def _generateCommandObj( self, postfix ):
        """
        Returns the command object that matches commandName, if any.
        """

        if not postfix and self.isArgRequired:
            return NoArgumentCommand( self.DESCRIPTION_TEXT,
                                      ARG_REQUIRED_MSG )
        else:
            return FuncCommand(
                cmdName = self.cmdName,
                func = self.func,
                desc = self.desc,
                help = self.getHelp(),
                takesArg = bool(postfix),
                argValue = postfix
                )

class ArbitraryArgFuncCommand( ArbitraryPostfixFactory, ArgFuncMixin ):
    def __init__( self, *args, **kwargs ):
        ArbitraryPostfixFactory.__init__( self )
        ArgFuncMixin.__init__( self, *args, **kwargs )

    _generateCommandObj = ArgFuncMixin._generateCommandObj

class BoundedArgFuncCommand( GenericPrefixFactory, ArgFuncMixin ):
    def __init__( self, *args, **kwargs ):
        GenericPrefixFactory.__init__( self )
        ArgFuncMixin.__init__( self, *args, **kwargs )

    def update( self ):
        validargs = self.func.__getvalidargs__()
        if validargs is not None:
            self._postfixes = validargs

    _generateCommandObj = ArgFuncMixin._generateCommandObj

def makeCommandFromInfo( info ):
    if info["cmdType"] == "no-arg":
        return FuncCommand(
            info["cmdName"],
            info["func"],
            info["desc"],
            info["help"]
            )
    elif info["cmdType"] in ["bounded-arg", "arbitrary-arg"]:
        if info["cmdType"] == "bounded-arg":
            CommandClass = BoundedArgFuncCommand
        else:
            CommandClass = ArbitraryArgFuncCommand
        return CommandClass(
            info["cmdName"],
            info["cmdExpr"],
            info["func"],
            info["argName"],
            info["desc"],
            info["help"],
            info["isArgRequired"]
            )
    else:
        raise ValueError( "Unknown command type: %s" % info["cmdType"] )
