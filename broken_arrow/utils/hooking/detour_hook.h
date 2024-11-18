#pragma once
#include "../../core/common.h"
#include "../../dependencies/minhook/minhook.h"
#include "../log/log.h"

class CBaseHookObject
{
public:
	CBaseHookObject( ) = default;

	explicit CBaseHookObject( void* pFunction, void* pDetour )
		: pBaseFn( pFunction ), pReplaceFn( pDetour ) { }

	/// setup hook and replace function
	/// @returns: true if hook has been successfully created, false otherwise
	bool Create( void* pFunction, void* pDetour )
	{
		if ( pFunction == nullptr || pDetour == nullptr )
			return false;

		pBaseFn = pFunction;
		pReplaceFn = pDetour;

		if ( const MH_STATUS status = MH_CreateHook( pBaseFn, pReplaceFn, &pOriginalFn ); status != MH_OK )
		{
#ifdef _DEBUG
			L_PRINT( LOG_ERROR ) << XOR_STR( "failed to create hook, status: \"" ) << MH_StatusToString( status ) << XOR_STR( "\" with base address: " ) << L::AddFlags( LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX ) << reinterpret_cast< std::uintptr_t >( pBaseFn );
#else
			L_PRINT( LOG_ERROR ) << XOR_STR( "failed to create hook" );
#endif

			ASSERT( false );
			return false;
		}

		if ( !Replace( ) )
			return false;

		return true;
	}

	/// patch memory to jump to our function instead of original
	/// @returns: true if hook has been successfully applied, false otherwise
	bool Replace( )
	{
		// check is hook has been created
		if ( pBaseFn == nullptr )
			return false;

		// check that function isn't already hooked
		if ( bIsHooked )
			return false;

		if ( const MH_STATUS status = MH_EnableHook( pBaseFn ); status != MH_OK )
		{
#ifdef _DEBUG
			L_PRINT( LOG_ERROR ) << XOR_STR( "failed to enable hook, status: \"" ) << MH_StatusToString( status ) << XOR_STR( "\" with base address: " ) << L::AddFlags( LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX ) << reinterpret_cast< std::uintptr_t >( pBaseFn );
#else
			L_PRINT( LOG_ERROR ) << XOR_STR( "failed to enable hook" );
#endif

			ASSERT( false );
			return false;
		}

		// switch hook state
		bIsHooked = true;
		return true;
	}

	/// restore original function call and cleanup hook data
	/// @returns: true if hook has been successfully removed, false otherwise
	bool Remove( )
	{
		// restore it at first
		if ( !Restore( ) )
			return false;

		if ( const MH_STATUS status = MH_RemoveHook( pBaseFn ); status != MH_OK )
		{
#ifdef _DEBUG
			L_PRINT( LOG_ERROR ) << XOR_STR( "failed to remove hook, status: \"" ) << MH_StatusToString( status ) << XOR_STR( "\" with base address: " ) << L::AddFlags( LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX ) << reinterpret_cast< std::uintptr_t >( pBaseFn );
#else
			L_PRINT( LOG_ERROR ) << XOR_STR( "failed to remove hook" );
#endif

			ASSERT( false );
			return false;
		}

		return true;
	}

	/// restore patched memory to original function call
	/// @returns: true if hook has been successfully restored, false otherwise
	bool Restore( )
	{
		// check that function is hooked
		if ( !bIsHooked )
			return false;

		if ( const MH_STATUS status = MH_DisableHook( pBaseFn ); status != MH_OK )
		{
#ifdef _DEBUG
			L_PRINT( LOG_ERROR ) << XOR_STR( "failed to restore hook, status: \"" ) << MH_StatusToString( status ) << XOR_STR( "\" with base address: " ) << L::AddFlags( LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX ) << reinterpret_cast< std::uintptr_t >( pBaseFn );
#else
			L_PRINT( LOG_ERROR ) << XOR_STR( "failed to restore hook" );
#endif

			ASSERT( false );
			return false;
		}

		// switch hook state
		bIsHooked = false;
		return true;
	}

	/// @returns: original, unwrapped function that would be called without the hook
	template <typename Fn>
	__forceinline Fn GetOriginal( )
	{
		return static_cast< Fn >( pOriginalFn );
	}

	/// @returns: true if hook is applied at the time, false otherwise
	__forceinline bool IsHooked( ) const
	{
		return bIsHooked;
	}

private:
	// current hook state
	bool bIsHooked = false;
	// function base handle
	void* pBaseFn = nullptr;
	// function that being replace the original call
	void* pReplaceFn = nullptr;
	// original function
	void* pOriginalFn = nullptr;
};