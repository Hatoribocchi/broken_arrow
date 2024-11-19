#include "common.h"
#include "hooks/hooks.h"
#include "../utils/render/render.h"
#include "../utils/memory/memory.h"
#include "../utils/log/log.h"

DWORD WINAPI OnDllAttach( LPVOID lpParameter )
{
    try
    {
        // get main game module
        auto pMainModule = MEM::GetModuleBaseHandle( GAMEASSEMBLY_DLL );

        // don't let proceed load until main module not loaded
        while ( pMainModule == nullptr )
            ::Sleep( 500UL );

        // attach logger
        L::AttachConsole( L"1000 euro cheat" );

        // setup render
        if ( !R::Setup( ) )
            L_PRINT( LOG_ERROR ) << XOR_STR("i cant setup render :(((");

        // setup hooks on game funcs
        H::Setup( );
    }
    catch ( const std::exception& ex )
    {
        // print error message
        L_PRINT( LOG_ERROR ) << ex.what( );

#ifdef _DEBUG
        // show error message window (or replace to your exception handler)
        _RPT0( _CRT_ERROR, ex.what( ) );
#else
        // unload
        FreeLibraryAndExitThread( static_cast< HMODULE >( lpParameter ), EXIT_FAILURE );
#endif
    }

    return 1UL;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
    if ( dwReason == DLL_PROCESS_ATTACH )
    {
        // disables the DLL_THREAD_ATTACH and DLL_THREAD_DETACH notifications for the specified dynamic-link library (DLL). This can reduce the size of the working set for some applications
        DisableThreadLibraryCalls( hModule );

        if ( const HANDLE hThread = ::CreateThread( nullptr, 0U, &OnDllAttach, hModule, 0UL, nullptr ); hThread != nullptr )
            ::CloseHandle( hThread );

        return TRUE;
    }

    return FALSE;
}