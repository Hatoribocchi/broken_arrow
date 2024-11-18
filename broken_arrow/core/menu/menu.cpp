#include "menu.h"
#include "../config/config.h"
#include "../globals.h"

void MENU::Instance( )
{
	if ( !G::bMenuOpen )
		return;

	ImGui::Begin( XOR_STR( "1000$ menu" ) );
	{
		ImGui::Checkbox( "Rapid Fire", &CFG::bRapidFire );
		ImGui::Checkbox( "Unlimited Ammo", &CFG::bUnlimitAmmo );
		ImGui::SliderFloat( "Force Damage", &CFG::flForceDamage, 0.f, 99999.f );
		ImGui::SliderFloat( "Force Penetration", &CFG::flForcePenetration, 0.f, 99999.f );
	}
	ImGui::End( );
}