/* -*-Mode:C++; c-basic-indent:4; c-basic-offset:4; indent-tabs-mode:nil-*- */

/*
 Copyright (c) 2008, Humanized, Inc.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    3. Neither the name of Enso nor the names of its contributors may
       be used to endorse or promote products derived from this
       software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY Humanized, Inc. ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Humanized, Inc. BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*   Implementation file for Enso's tray icon manager. */

/* ***************************************************************************
 * Include Files
 * **************************************************************************/

#include "TrayIconManager.h"
#include "Logging/Logging.h"
#include "GlobalConstants.h"

#include "WinSdk.h"


/* ***************************************************************************
 * Macros
 * **************************************************************************/

/* Application-defined icon id for the tray icon. */
#define TRAY_ICON_ID            1

/* Maximum number of times we'll attempt to retry Shell_NotifyIcon(). */
#define MAX_SHELL_NOTIFYICON_RETRIES 10

/* Number of milliseconds we'll wait between retries of
 * Shell_NotifyIcon(). */
#define SHELL_NOTIFYICON_RETRY_INTERVAL 1000


/* ***************************************************************************
 * Module Variables
 * **************************************************************************/

/* File path to the system tray icon. */
static char _trayIconPath[MAX_PATH+1] = "";

/* Context menu for the system tray icon. */
static HMENU _trayMenu = NULL;

/* Whether our module is initialized or not. */
static int _isInitialized = 0;

/* Window that will own the shortcut menu that apepars. */
static HWND _associatedWindow = NULL;

/* Parent thread to send menu item selection information to. */
static DWORD _parentThreadId = NULL;

/* Window message broadcasted when the taskbar is restarted. */
static UINT _taskbarRestartMessage = 0;


/* ***************************************************************************
 * Private Function Declarations
 * **************************************************************************/

static LRESULT
_trayIconEventProc( HWND theWindow,
                    UINT msg,
                    WPARAM wParam,
                    LPARAM lParam );

static int
_addTrayIcon( void );

static int
_installTrayIcon( void );

static int
_uninstallTrayIcon( void );

static bool
_doesTaskbarExist( void );


/* ***************************************************************************
 * Private Function Implementations
 * **************************************************************************/

/* ------------------------------------------------------------------------
 * The Tray Icon Manager's Event Processing Function
 * ........................................................................
 * ----------------------------------------------------------------------*/

static LRESULT
_trayIconEventProc( HWND theWindow,
                    UINT msg,
                    WPARAM wParam,
                    LPARAM lParam )
{
    switch ( msg )
    {
    case WM_USER_TRAY_ICON_INSTALL:
        return _installTrayIcon();

    case WM_USER_TRAY_ICON_UNINSTALL:
        return _uninstallTrayIcon();

    case WM_USER_TRAY_ICON:
        /* If we get a tray icon event, verify that the button has
         * been released, figure out which item if any was selected,
         * and process.  This calls non-static methods, so make sure
         * _pThis is not null and then use it! */
        if ( _isInitialized &&
             (UINT) wParam == TRAY_ICON_ID &&
             ((UINT) lParam == WM_RBUTTONUP ||
              (UINT) lParam == WM_LBUTTONUP) )
        {
            POINT pt;
            int itemSelected;
            UINT popupMenuFlags;

            infoMsg( "Tray icon clicked." );

            GetCursorPos( &pt );

            popupMenuFlags = TPM_RIGHTALIGN | TPM_LEFTBUTTON |
                TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_NONOTIFY |
                TPM_RETURNCMD;

            /* Pop-up the menu, track the mouse, and find out which
             * item the user selected.  See the "Remarks" section of
             * the MSDN documentation for TrackPopupMenu() for more
             * information on the following code. */
            SetForegroundWindow( _associatedWindow );
            itemSelected = TrackPopupMenu( _trayMenu,
                                           popupMenuFlags,
                                           pt.x,
                                           pt.y,
                                           0,
                                           _associatedWindow,
                                           NULL );

            /* Pass the partially-digested event along to the parent thread,
             * I.E. the message-only window in InputManager. */
            if ( itemSelected != 0 )
            {
                PostThreadMessage( _parentThreadId,
                                   WM_USER_TRAY_MENU_ITEM,
                                   itemSelected, 0 );
            }
        }
        return (LRESULT)0;
    default:
        if ( msg == _taskbarRestartMessage )
        {
            infoMsg( "TaskbarCreated message received." );
            _addTrayIcon();
            return (LRESULT) 0;
        } else {
            /* This shouldn't happen, because this function should
             * only be called in response to the event types it was
             * registered for. */
            errorMsg( "Bad event type message passed in." );
            return (LRESULT)0;            
        }
    }
}


/* ------------------------------------------------------------------------
 * Adds the tray icon to the notification area (system tray).
 * ........................................................................
 * ----------------------------------------------------------------------*/

static int
_addTrayIcon( void )
{
    BOOL res;
    NOTIFYICONDATA tnid;
    HICON hicon;
    static const char *tooltipText = TRAY_ICON_TOOLTIP_TEXT;
    char *result;

    /* Load the icon from its file */
    hicon = ExtractIcon( GetModuleHandle(NULL),
                         _trayIconPath,
                         0 );
    if ( (int) hicon == 1 || (int) hicon == NULL )
    {
        errorMsg( "ExtractIcon() failed." );
        return false;
    }

    /* Set up fields of tnid record */
    tnid.cbSize = sizeof( NOTIFYICONDATA );
    tnid.hWnd = _associatedWindow;
    tnid.uID = TRAY_ICON_ID;
    tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tnid.uCallbackMessage = WM_USER_TRAY_ICON;
    tnid.hIcon = hicon;

    /* Set up tooltip for Enso tray icon */
    result = strncpy( tnid.szTip, tooltipText, sizeof(tnid.szTip) );
    if ( result == 0 )
    {
        errorMsg( "strncpy() failed." );
        /* LONGTERM TODO: We're exiting without destroying our icon
         * handle. */
        return false;
    }

    bool done = false;
    int retries = 0;

    /* We sometimes erroneously get a TaskbarCreated message when the
     * taskbar is still around and our tray icon is still in the
     * notification area; if this is the case, then adding the tray
     * icon will always fail.  So just to be safe, we're going to tell
     * the shell to delete our tray icon here, so that we're sure our
     * tray icon doesn't exist when we try adding it later.  We expect
     * this call to fail most of the time, but in the unusual
     * circumstances just described it will actually succeed.  See
     * #468 for more information. */
    if ( Shell_NotifyIcon( NIM_DELETE, &tnid ) == TRUE )
        infoMsg( "Shell_NotifyIcon() with NIM_DELETE successful." );

    /* For more information on why we're performing the following
     * retry-loop, see trac ticket #416. */
    while ( !done )
    {
        if ( _doesTaskbarExist() )
            infoMsg( "Taskbar appears to exist." );
        else
            infoMsg( "Taskbar does not appear to exist." );

        /* Use the tnid record to tell the shell to use our tray icon */
        res = Shell_NotifyIcon( NIM_ADD, &tnid );

        if ( (res == FALSE) && (retries < MAX_SHELL_NOTIFYICON_RETRIES) )
        {
            infoMsg( "Shell_NotifyIcon() failed.  Retrying." );
            Sleep( SHELL_NOTIFYICON_RETRY_INTERVAL );
            retries += 1;
        } else
            done = true;
    }

    if ( res == FALSE )
    {
        if ( _doesTaskbarExist() )
            errorMsg( "Shell_NotifyIcon() failed while taskbar exists." );
        else {
            /* For more information on this course of action, see #416. */
            infoMsg( "Shell_NotifyIcon() failed, but taskbar doesn't "
                     "appear to exist.  Hopefully we will receive a "
                     "TaskbarCreated message at some point." );
            /* Pretend that the call was successful. */
            res = TRUE;
        }
    } else
        infoMsgInt( "Shell_NotifyIcon() returned: ", res );

    /* Clean up and return success value */
    if ( hicon )
        DestroyIcon( hicon );

    return ( res == TRUE );
}


/* ------------------------------------------------------------------------
 * Installs the tray icon.
 * ........................................................................
 *
 * Must be called from the same thread as the tray icon's associated
 * window.
 *
 * ----------------------------------------------------------------------*/

static int
_installTrayIcon( void )
{
    /* LONGTERM TODO: Cleanup in case of failure should be better here. */

    /* Set up the tray icon menu */
    _trayMenu = CreatePopupMenu();
    if ( _trayMenu == NULL )
    {
        errorMsg( "CreatePopupMenu() failed." );
        return 0;
    }

    int wasSuccessful = registerAsyncEventProc(
        WM_USER_TRAY_ICON,
        _trayIconEventProc
        );

    if ( !wasSuccessful )
    {
        errorMsg( "Registering WM_USER_TRAY_ICON failed.\n" );
        return 0;
    }

    _taskbarRestartMessage = RegisterWindowMessage( TEXT("TaskbarCreated") );

    if ( _taskbarRestartMessage == 0 )
    {
        errorMsg( "Registering TaskbarCreated window message failed.\n" );
        return 0;
    }

    wasSuccessful = registerAsyncEventProc(
        _taskbarRestartMessage,
        _trayIconEventProc
        );

    if ( !wasSuccessful )
    {
        errorMsg( "Registering TaskbarCreated failed.\n" );
        return 0;
    }

    return _addTrayIcon();
}


/* ------------------------------------------------------------------------
 * Uninstalls the tray icon.
 * ........................................................................
 *
 * Must be called from the same thread as the tray icon's associated
 * window.
 *
 * ----------------------------------------------------------------------*/

static int
_uninstallTrayIcon( void )
{
    /* LONGTERM TODO: Cleanup in case of failure should be better here. */
    int success = 1;

    int wasSuccessful = unregisterAsyncEventProc( WM_USER_TRAY_ICON );

    if ( !wasSuccessful )
    {
        errorMsg( "Unregistering WM_USER_TRAY_ICON failed.\n" );
        success = 0;
    }

    NOTIFYICONDATA tnid;

    tnid.cbSize = sizeof( NOTIFYICONDATA );
    tnid.hWnd = _associatedWindow;
    tnid.uID = TRAY_ICON_ID;

    Shell_NotifyIcon( NIM_DELETE, &tnid );

    DestroyMenu( _trayMenu );

    return success;
}

/* ------------------------------------------------------------------------
 * Returns whether the taskbar currently exists.
 * ........................................................................
 *
 * This is implemented by asking the taskbar how big it is.  If we get
 * a failure response, this means that the taskbar doesn't exist.
 * Note that it does appear to be the case that the taskbar provides a
 * size if it exists, even if it's set to auto-hide or if the user
 * makes it as small as possible.
 *
 * ----------------------------------------------------------------------*/

static bool
_doesTaskbarExist( void )
{
    APPBARDATA abd;

    ZeroMemory( &abd, sizeof(APPBARDATA) );
    abd.cbSize = sizeof( APPBARDATA );
    abd.hWnd = NULL;

    BOOL result = SHAppBarMessage( ABM_GETTASKBARPOS, &abd );

    return ( result == TRUE );
}


/* ***************************************************************************
 * Public Function Implementations
 * **************************************************************************/

/* ------------------------------------------------------------------------
 * Sets the path to the icon file to be used by the tray icon.
 * ........................................................................
 * ----------------------------------------------------------------------*/

void
setTrayIconPath( const char *path )
{
    strncpy( _trayIconPath, path, MAX_PATH );
    _trayIconPath[MAX_PATH] = 0;
}


/* ------------------------------------------------------------------------
 * Adds an item to the system tray menu with the given title and id.
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
addTrayMenuItem( const char *menuTitle,
                 int menuId )
{
    if ( !_isInitialized )
    {
        errorMsg( "TrayIconManager not inited." );
        return 0;
    }

    if ( menuTitle[0] == '-' ) {
        AppendMenu( _trayMenu,
                    MF_SEPARATOR,
                    0,
                    0 );
    } else {
        AppendMenu( _trayMenu,
                    MF_STRING | MF_ENABLED,
                    menuId,
                    menuTitle );
    }

    /* LONGTERM TODO: Check for errors. 
     * LONGTERM TODO: Also handle menuID collisions. */

    return 1;
}


/* ------------------------------------------------------------------------
 * Returns whether or not the module is initialized.
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
isTrayIconManagerInitialized( void )
{
    return _isInitialized;
}


/* ------------------------------------------------------------------------
 * Initializes the Tray Icon Manager.
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
initTrayIconManager( DWORD parentThreadId )
{
    /* LONGTERM TODO: Cleanup in case of failure should be better here. */

    if ( _isInitialized )
    {
        errorMsg( "TrayIconManager already inited." );
        return 0;
    }

    _associatedWindow = getAsyncEventWindow();
    if ( _associatedWindow == NULL )
    {
        errorMsg( "getAsyncEventWindow() returned NULL." );
        return 0;
    }

    _parentThreadId = parentThreadId;

    if ( !sendMessageToAsyncEventWindow(WM_USER_TRAY_ICON_INSTALL, 0, 0,
                                        _trayIconEventProc) )
    {
        errorMsg( "_sendMessageToAsyncEventWindow() reported failure." );
        return 0;
    }

    _isInitialized = 1;

    return 1;
}


/* ------------------------------------------------------------------------
 * Shuts down the Tray Icon Manager.
 * ........................................................................
 * ----------------------------------------------------------------------*/

int
shutdownTrayIconManager( void )
{
    /* LONGTERM TODO: Cleanup in case of failure should be better here. */

    if ( !_isInitialized )
    {
        errorMsg( "TrayIconManager not inited." );
        return 0;
    }

    if ( !sendMessageToAsyncEventWindow(WM_USER_TRAY_ICON_UNINSTALL, 0, 0,
                                        _trayIconEventProc) )
    {
        errorMsg( "_sendMessageToAsyncEventWindow() reported failure." );
        return 0;
    }

    _associatedWindow = NULL;
    _parentThreadId = NULL;

    _isInitialized = 0;

    return 1;
}
