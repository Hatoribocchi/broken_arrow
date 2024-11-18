#pragma once
#include "../../core/globals.h"
#include "../../dependencies/imgui/imgui.h"
#include "../../dependencies/imgui/imgui_impl_win32.h"
#include "../../dependencies/imgui/imgui_impl_dx11.h"

namespace R
{
	bool Setup( );
	void InitImGui( );

	extern uint64_t* MethodsTable;
}