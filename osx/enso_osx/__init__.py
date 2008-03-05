def provideInterface( name ):
    if name == "input":
        import enso_osx.input
        return enso_osx.input
    elif name == "graphics":
        import enso_osx.graphics
        return enso_osx.graphics
    elif name == "cairo":
        import enso_osx.cairo
        return enso_osx.cairo
    else:
        return None
