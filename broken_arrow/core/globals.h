#pragma once
#include "common.h"

namespace G
{
	inline HWND Window = NULL;
	inline ID3D11Device* pDevice = NULL;
	inline ID3D11DeviceContext* pContext = NULL;
	inline ID3D11RenderTargetView* pMainRenderTargetView;
	inline WNDPROC OldWndProc;
	inline bool bGuiInit = false;
	inline bool bMenuOpen = false;
}