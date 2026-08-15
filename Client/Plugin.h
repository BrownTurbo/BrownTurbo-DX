#ifndef PLUGIN_H
#define PLUGIN_H

#include <windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <mutex>

#include <d3d9.h>

#include <MinHook.h>
#include <RakHook/rakhook.hpp>
#include <sampapi/sampapi.h>
#include <RakNet/PacketEnumerations.h>
#include <RakNet/StringCompressor.h>
#include <RakNet/BitStream.h>

#include "Utils.h"

#define RPC_KEY_LOG_EVENT   199
#define RPC_MOUSE_LOG_EVENT 198
#define RPC_CLICK_LOG_EVENT 197
#define RPC_SCWH_LOG_EVENT  196

#define PACKET_INSTALLED_CHECK 254

#define MOUSE_UNK_CLICK    -1
#define MOUSE_LEFT_CLICK   0
#define MOUSE_RIGHT_CLICK  1
#define MOUSE_MIDDLE_CLICK 2

#define D3D_VFUNCTIONS 	(119)
#define DEVICE_PTR 		(0xC97C28)
#define RESET_INDEX 	(16)
#define PRESENT_INDEX 	(17)

typedef HRESULT(__stdcall* _Present)(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
typedef long(__stdcall* _Reset)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pp);

class c_plugin
{
	public:
		c_plugin(HMODULE hmodule);
		~c_plugin();

		static void game_loop();
		static void shutdown_for_unload();
		static c_hook<void(*)()> game_loop_hook;
	private:
		HMODULE hmodule;
};
inline c_hook<void(*)()> c_plugin::game_loop_hook = { 0x561B10 };
bool SendKeyRPC(uint32_t v_key, bool is_down);
bool SendClickRPC(bool is_dblclick, int key);
bool SendMouseRPC(float mouseX, float mouseY);
bool SendScreenWdthHghtRPC(float screenWidth, float screenHeight);
bool IsPlayerWritingInChatBox();
bool IsGameInitialized();
bool IsMenuOpened();
bool IsDialogActive();
bool IsScoreBoardActive();

#endif