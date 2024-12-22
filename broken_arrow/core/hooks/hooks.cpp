#include "hooks.h"
#include "../config/config.h"
#include "../menu/menu.h"
#include "../../utils/render/render.h"
#include "../../utils/memory/memory.h"
#include "../../utils/log/log.h"

bool H::Setup( )
{
	if (MH_Initialize() != MH_OK)
	{
		L_PRINT(LOG_ERROR) << XOR_STR("failed to initialize minhook");
		return false;
	}

	L_PRINT(LOG_INFO) << XOR_STR("minhook initialization completed");

	static LPVOID pPresent = (LPVOID)R::MethodsTable[8];
	if (!DTR::Present.Create(pPresent, &Present))
		return false;

	static LPVOID pSubtractAmmo = (LPVOID)(MEM::FindPattern(GAMEASSEMBLY_DLL, XOR_STR("29 51 1C 79 07"))); //AmmunitionContainer
	if (!DTR::SubtractAmmo.Create(pSubtractAmmo, &SubtractAmmo))
	{
		L_PRINT(LOG_ERROR) << XOR_STR("i cant hook SubtractAmmo");
		return false;
	}

	static LPVOID pSubtractReserve = (LPVOID)(MEM::FindPattern(GAMEASSEMBLY_DLL, XOR_STR("29 51 18 79 07"))); //AmmunitionContainer
	if (!DTR::SubtractReserve.Create(pSubtractReserve, &SubtractReserve))
	{
		L_PRINT(LOG_ERROR) << XOR_STR("i cant hook SubtractReserve");
		return false;
	}

	static LPVOID pDamageFormulaHEAT = (LPVOID)(MEM::FindPattern(
		GAMEASSEMBLY_DLL, XOR_STR("40 53 48 83 EC 50 80 3D DA 1C CD 05 00 0F 29 74 24 40"))); //BattleSystemHelpers
	if (!DTR::DamageFormulaHEAT.Create(pDamageFormulaHEAT, &DamageFormulaHEAT))
	{
		L_PRINT(LOG_ERROR) << XOR_STR("i cant hook DamageFormulaHEAT");
		return false;
	}

	static LPVOID pDamageFormulaKinetic = (LPVOID)(MEM::FindPattern(
		GAMEASSEMBLY_DLL, XOR_STR("48 83 EC 68 80 3D 0B 1C CD 05 00 0F 29 7C 24 40"))); //BattleSystemHelpers
	if (!DTR::DamageFormulaKinetic.Create(pDamageFormulaKinetic, &DamageFormulaKinetic))
	{
		L_PRINT(LOG_ERROR) << XOR_STR("i cant hook DamageFormulaKinetic");
		return false;
	}

	static LPVOID pCalculateHitDamage = (LPVOID)(MEM::FindPattern(
		GAMEASSEMBLY_DLL, XOR_STR("48 89 6C 24 20 48 89 4C 24 08 57 48 81 EC 90 00 00 00"))); //BattleSystemHelpers
	if (!DTR::CalculateHitDamage.Create(pCalculateHitDamage, &CalculateHitDamage))
	{
		L_PRINT(LOG_ERROR) << XOR_STR("i cant hook CalculateHitDamage");
		return false;
	}

	static LPVOID pShotCycleAndReload = (LPVOID)(MEM::FindPattern(
		GAMEASSEMBLY_DLL, XOR_STR("40 53 57 41 54 41 56 41 57 48 83 EC 30 80 3D B1 40 CB 05 00"))); //ShootingSystem
	if (!DTR::ShotCycleAndReload.Create(pShotCycleAndReload, &ShotCycleAndReload))
	{
		L_PRINT(LOG_ERROR) << XOR_STR("i cant hook ShotCycleAndReload");
		return false;
	}

	L_PRINT(LOG_INFO) << XOR_STR("hooks initialization completed");

	return true;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT STDCALL H::WndProc( const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( uMsg == WM_KEYDOWN && wParam == VK_INSERT )
		G::bMenuOpen = !G::bMenuOpen;

	if ( G::bMenuOpen && ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) )
		return true;

	return CallWindowProc( G::OldWndProc, hWnd, uMsg, wParam, lParam );
}

HRESULT STDCALL H::Present( IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags )
{
	static auto oPresent = DTR::Present.GetOriginal<decltype( &Present )>( );

	if ( !G::bGuiInit )
	{
		if ( SUCCEEDED( pSwapChain->GetDevice( __uuidof( ID3D11Device ), ( void** ) &G::pDevice ) ) )
		{
			G::pDevice->GetImmediateContext( &G::pContext );
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc( &sd );
			G::Window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* ) &pBackBuffer );
			G::pDevice->CreateRenderTargetView( pBackBuffer, NULL, &G::pMainRenderTargetView );
			pBackBuffer->Release( );
			G::OldWndProc = ( WNDPROC ) SetWindowLongPtr( G::Window, GWLP_WNDPROC, ( LONG_PTR ) WndProc );
			R::InitImGui( );
			G::bGuiInit = true;
		}

		else
			return oPresent( pSwapChain, SyncInterval, Flags );
	}

	ImGui_ImplDX11_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	MENU::Instance( );

	ImGui::EndFrame( );
	ImGui::Render( );

	G::pContext->OMSetRenderTargets( 1, &G::pMainRenderTargetView, NULL );
	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
	return oPresent( pSwapChain, SyncInterval, Flags );
}

__m128 FASTCALL H::GetPenetration( int armorType, double PenetrationAtEffectiveRange, double PenetrationAtMinimalRange, float EffectiveRange, float MinimalRange, float ActualRange )
{
	static auto oGetPenetration = DTR::GetPenetration.GetOriginal<decltype( &GetPenetration )>( );
	if (CFG::flForcePenetration <= 0.f )
		return oGetPenetration( armorType, PenetrationAtEffectiveRange, PenetrationAtMinimalRange, EffectiveRange, MinimalRange, ActualRange );

	return oGetPenetration( armorType, CFG::flForcePenetration, CFG::flForcePenetration, EffectiveRange, MinimalRange, ActualRange );
}

__m128 FASTCALL H::GetPenetrationForAmmunition( __int64 ammunition, float distance )
{
	static auto oGetPenetrationForAmmunition = DTR::GetPenetrationForAmmunition.GetOriginal<decltype( &GetPenetrationForAmmunition )>( );
	if ( CFG::flForcePenetration <= 0.f )
		return oGetPenetrationForAmmunition( ammunition, distance );

	return oGetPenetrationForAmmunition( ammunition, 0.f );
}

__m128 FASTCALL H::DamageFormulaHEAT( float baseDamage, double penetration, float armor )
{
	static auto oDamageFormulaHEAT = DTR::DamageFormulaHEAT.GetOriginal<decltype( &DamageFormulaHEAT )>( );
	if ( CFG::flForceDamage <= 0.f )
		return oDamageFormulaHEAT( baseDamage, penetration, armor );

	return oDamageFormulaHEAT( CFG::flForceDamage, penetration, armor );
}

double FASTCALL H::DamageFormulaKinetic( double baseDamage, float penetration, float armor )
{
	static auto oDamageFormulaKinetic = DTR::DamageFormulaKinetic.GetOriginal<decltype( &DamageFormulaKinetic )>( );
	if ( CFG::flForceDamage <= 0.f )
		return oDamageFormulaKinetic( baseDamage, penetration, armor );

	return oDamageFormulaKinetic( 99999.f, penetration, armor );
}

double __fastcall H::CalculateHitDamage(
		__int64 target,
		__int64* globalImpactVector,
		double baseDamage,
		__int64 ammoInfo,
		float penetration,
		char topArmorAttack,
		char armorSide )
{
	static auto oCalculateHitDamage = DTR::CalculateHitDamage.GetOriginal<decltype( &CalculateHitDamage )>( );
	if ( CFG::flForceDamage <= 0.f )
		return oCalculateHitDamage( target, globalImpactVector, baseDamage, ammoInfo, penetration, topArmorAttack, armorSide );

	return CFG::flForceDamage;
}

void FASTCALL H::SubtractAmmo( __int64 a1, int a2 )
{
	static auto oSubtractAmmo = DTR::SubtractAmmo.GetOriginal<decltype( &SubtractAmmo )>( );
	if ( !CFG::bUnlimitAmmo )
		return oSubtractAmmo( a1, a2 );

	return;
}

void FASTCALL H::SubtractReserve( __int64 a1, int a2 )
{
	static auto oSubtractReserve = DTR::SubtractReserve.GetOriginal<decltype( &SubtractReserve )>( );
	if ( !CFG::bUnlimitAmmo )
		return oSubtractReserve( a1, a2 );

	return;
}

void* FASTCALL H::ShotCycleAndReload( void* weapon, void* unitEntity, void* ammunitionBox, int32_t shootAmmoCount, void* method )
{
	static auto oShotCycleAndReload = DTR::ShotCycleAndReload.GetOriginal<decltype( &ShotCycleAndReload )>( );
	if ( !CFG::bRapidFire )
		return oShotCycleAndReload( weapon, unitEntity, ammunitionBox, shootAmmoCount, method );

	return oShotCycleAndReload( weapon, unitEntity, ammunitionBox, 0, method );
}
