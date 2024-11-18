#pragma once
#include <intrin.h>
#include <array>

#include "../common.h"
#include "../globals.h"

#include "../../utils/hooking/detour_hook.h"

#define FASTCALL __fastcall
#define STDCALL __stdcall
#define VECTORCALL __vectorcall

/*
 * DETOURS
 * detour hook managers
 */
namespace DTR
{
	inline CBaseHookObject WndProc;
	inline CBaseHookObject Present;
	inline CBaseHookObject GetPenetration;
	inline CBaseHookObject GetPenetrationForAmmunition;
	inline CBaseHookObject DamageFormulaHEAT;
	inline CBaseHookObject DamageFormulaKinetic;
	inline CBaseHookObject CalculateHitDamage;
	inline CBaseHookObject SubtractAmmo;
	inline CBaseHookObject SubtractReserve;
	inline CBaseHookObject ShotCycleAndReload;
}

/*
 * HOOKS
 * swap functions with given pointers
 */
namespace H
{
	// Get
	bool	Setup( );
	void	Restore( );

	// Handlers
	/* [type][call]		hk[name] (args...) */
	__m128 FASTCALL GetPenetration( int armorType, double PenetrationAtEffectiveRange, double PenetrationAtMinimalRange, float EffectiveRange, float MinimalRange, float ActualRange );
	__m128 FASTCALL GetPenetrationForAmmunition( __int64 ammunition, float distance );
	__m128 FASTCALL DamageFormulaHEAT( float baseDamage, double penetration, float armor );
	double FASTCALL DamageFormulaKinetic( double baseDamage, float penetration, float armor );
	double FASTCALL CalculateHitDamage( __int64 target, __int64* globalImpactVector, double baseDamage, __int64 ammoInfo, float penetration, char topArmorAttack, char armorSide );
	void FASTCALL SubtractAmmo( __int64 a1, int a2 );
	void FASTCALL SubtractReserve( __int64 a1, int a2 );
	__int64 FASTCALL ShotCycleAndReload( __int64 weapon, __int64 unitEntity, __int64 ammunitionBox, int shootAmmoCount );
	LRESULT STDCALL WndProc( const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	HRESULT STDCALL Present( IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags );
}