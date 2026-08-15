#include "Plugin.h"

#include <RakNet/PacketEnumerations.h>
#include <RakNet/StringCompressor.h>
#include <RakNet/BitStream.h>
#include <RakHook/samp.hpp>

#include <sampapi/CInput.h>
#include <sampapi/CGame.h>
#include <sampapi/CDialog.h>
#include <sampapi/CScoreboard.h>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <array>
#include <atomic>
#include <thread>
#include <chrono>

HWND hWnd;

WNDPROC oWndProc = nullptr;
std::atomic<bool> initialized{false};

VTableHookManager* g_vmtHooks = nullptr;
_Present oPresent = nullptr;
_Reset oReset = nullptr;

float g_mouseX;
float g_mouseY;

float g_screenWidth = 0.0f;
float g_screenHeight = 0.0f;
DWORD g_lastScreenSizeSendTime = 0;

bool IsPlayerWritingInChatBox()
{
    rakhook::samp_ver version = rakhook::samp_version();
	bool _writing = false;
	switch (version) {
		case rakhook::samp_ver::v037r1:
		{
			SAMPAPI_EXPORT sampapi::v037r1::CInput*& p_input = sampapi::v037r1::RefInputBox();
			_writing = (p_input && p_input->m_bEnabled != 0);
			break;
		}
		case rakhook::samp_ver::v037r31:
		{
			SAMPAPI_EXPORT sampapi::v037r3::CInput*& p_input = sampapi::v037r3::RefInputBox();
			_writing = (p_input && p_input->m_bEnabled != 0);
			break;
		}
		case rakhook::samp_ver::v037r5:
		{
			SAMPAPI_EXPORT sampapi::v037r5::CInput*& p_input = sampapi::v037r5::RefInputBox();
			_writing = (p_input && p_input->m_bEnabled != 0);
			break;
		}
		case rakhook::samp_ver::v03dlr1:
		{
			SAMPAPI_EXPORT sampapi::v03dl::CInput*& p_input = sampapi::v03dl::RefInputBox();
			_writing = (p_input && p_input->m_bEnabled != 0);
			break;
		}
		default:{
			_writing = false;
			break;
		}
	}
	return _writing;
}

bool IsDialogActive()
{
    rakhook::samp_ver version = rakhook::samp_version();
	bool _active = false;
	switch (version) {
		case rakhook::samp_ver::v037r1:
		{
			SAMPAPI_EXPORT sampapi::v037r1::CDialog*& p_dialog = sampapi::v037r1::RefDialog();
			_active = (p_dialog && p_dialog->m_bIsActive != false);
			break;
		}
		case rakhook::samp_ver::v037r31:
		{
			SAMPAPI_EXPORT sampapi::v037r3::CDialog*& p_dialog = sampapi::v037r3::RefDialog();
			_active = (p_dialog && p_dialog->m_bIsActive != false);
			break;
		}
		case rakhook::samp_ver::v037r5:
		{
			SAMPAPI_EXPORT sampapi::v037r5::CDialog*& p_dialog = sampapi::v037r5::RefDialog();
			_active = (p_dialog && p_dialog->m_bIsActive != false);
			break;
		}
		case rakhook::samp_ver::v03dlr1:
		{
			SAMPAPI_EXPORT sampapi::v03dl::CDialog*& p_dialog = sampapi::v03dl::RefDialog();
			_active = (p_dialog && p_dialog->m_bIsActive != false);
			break;
		}
		default:{
			_active = false;
			break;
		}
	}
	return _active;
}

bool IsScoreBoardActive()
{
    rakhook::samp_ver version = rakhook::samp_version();
	bool _active = false;
	switch (version) {
		case rakhook::samp_ver::v037r1:
		{
			SAMPAPI_EXPORT sampapi::v037r1::CScoreboard*& pscoreb = sampapi::v037r1::RefScoreboard();
			_active = (pscoreb && pscoreb->m_bIsEnabled != false);
			break;
		}
		case rakhook::samp_ver::v037r31:
		{
			SAMPAPI_EXPORT sampapi::v037r3::CScoreboard*& pscoreb = sampapi::v037r3::RefScoreboard();
			_active = (pscoreb && pscoreb->m_bIsEnabled != false);
			break;
		}
		case rakhook::samp_ver::v037r5:
		{
			SAMPAPI_EXPORT sampapi::v037r5::CScoreboard*& pscoreb = sampapi::v037r5::RefScoreboard();
			_active = (pscoreb && pscoreb->m_bIsEnabled != false);
			break;
		}
		case rakhook::samp_ver::v03dlr1:
		{
			SAMPAPI_EXPORT sampapi::v03dl::CScoreboard*& pscoreb = sampapi::v03dl::RefScoreboard();
			_active = (pscoreb && pscoreb->m_bIsEnabled != false);
			break;
		}
		default:{
			_active = false;
			break;
		}
	}
	return _active;
}

bool IsMenuOpened()
{
    rakhook::samp_ver version = rakhook::samp_version();
	bool _opened = false;
	switch (version) {
		case rakhook::samp_ver::v037r1:
		{
			SAMPAPI_EXPORT sampapi::v037r1::CGame*& p_game = sampapi::v037r1::RefGame();
			_opened = (p_game && p_game->IsMenuVisible());
			break;
		}
		case rakhook::samp_ver::v037r31:
		{
			SAMPAPI_EXPORT sampapi::v037r3::CGame*& p_game = sampapi::v037r3::RefGame();
			_opened = (p_game && p_game->IsMenuVisible());
			break;
		}
		case rakhook::samp_ver::v037r5:
		{
			SAMPAPI_EXPORT sampapi::v037r5::CGame*& p_game = sampapi::v037r5::RefGame();
			_opened = (p_game && p_game->IsMenuVisible());
			break;
		}
		case rakhook::samp_ver::v03dlr1:
		{
			SAMPAPI_EXPORT sampapi::v03dl::CGame*& p_game = sampapi::v03dl::RefGame();
			_opened = (p_game && p_game->IsMenuVisible());
			break;
		}
		default:{
			_opened = false;
			break;
		}
	}
	return _opened;
}

bool IsGameInitialized()
{
    rakhook::samp_ver version = rakhook::samp_version();
	bool _initialized = false;
	switch (version) {
		case rakhook::samp_ver::v037r1:
		{
			_initialized = (sampapi::v037r1::RefGame() != nullptr);
			break;
		}
		case rakhook::samp_ver::v037r31:
		{
			_initialized = (sampapi::v037r3::RefGame() != nullptr);
			break;
		}
		case rakhook::samp_ver::v037r5:
		{
			_initialized = (sampapi::v037r5::RefGame() != nullptr);
			break;
		}
		case rakhook::samp_ver::v03dlr1:
		{
			_initialized = (sampapi::v03dl::RefGame() != nullptr);
			break;
		}
		default:{
			_initialized = false;
			break;
		}
	}
	return _initialized;
}

bool SendKeyRPC(uint32_t v_key, bool is_down) {
	if (!IsGameInitialized()) return false;
    if (IsPlayerWritingInChatBox() || IsMenuOpened() || IsDialogActive() || IsScoreBoardActive()) return false;

    RakNet::BitStream bs;

    bs.Write<uint32_t>(v_key);
    bs.Write<bool>(is_down);

    rakhook::send_rpc(RPC_KEY_LOG_EVENT, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false);
	return true;
}

bool SendClickRPC(bool is_dblclick, signed int key) {
	if (!IsGameInitialized()) return false;
    if (IsPlayerWritingInChatBox() || IsMenuOpened() || IsDialogActive() || IsScoreBoardActive()) return false;

    RakNet::BitStream bs;

    bs.Write<signed int>(key);
    bs.Write<bool>(is_dblclick);

    rakhook::send_rpc(RPC_CLICK_LOG_EVENT, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false);
	return true;
}

bool SendMouseRPC(float mouseX, float mouseY) {
	if (!IsGameInitialized()) return false;
    if (IsPlayerWritingInChatBox() || IsMenuOpened() || IsDialogActive() || IsScoreBoardActive()) return false;

    RakNet::BitStream bs;

    bs.Write<float>(mouseX);
    bs.Write<float>(mouseY);

    rakhook::send_rpc(RPC_MOUSE_LOG_EVENT, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false);
	return true;
}

bool SendScreenWdthHghtRPC(float screenWidth, float screenHeight) {
	if (!IsGameInitialized()) return false;

    RakNet::BitStream bs;

    bs.Write<float>(screenWidth);
    bs.Write<float>(screenHeight);

    rakhook::send_rpc(RPC_SCWH_LOG_EVENT, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false);
	return true;
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pp)
{
	initialized = false;
	return oReset(pDevice, pp);
}

HRESULT __stdcall hkPresent(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	D3DVIEWPORT9 vp;
	if (pDevice->GetViewport(&vp) == D3D_OK) {
	  float w = (float)vp.Width;
	  float h = (float)vp.Height;
	  DWORD now = GetTickCount();
	  if (w != g_screenWidth || h != g_screenHeight || (now - g_lastScreenSizeSendTime > 5000)) {
		  g_screenWidth = w;
		  g_screenHeight = h;
		  g_lastScreenSizeSendTime = now;
		  SendScreenWdthHghtRPC(g_screenWidth, g_screenHeight);
	  }
	}	
	return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

LRESULT CALLBACK hkWndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
	if (initialized) {
	  if (u_msg == WM_KEYDOWN || u_msg == WM_SYSKEYDOWN) {
		if (!(l_param & (1 << 30))) {
		  SendKeyRPC(static_cast<uint32_t>(w_param), true);
		}
	  }
	  else if ((u_msg == WM_KEYUP || u_msg == WM_SYSKEYUP)) {
		SendKeyRPC(static_cast<uint32_t>(w_param), false);
	  }
	  else if (u_msg == WM_MOUSEMOVE) {
		POINT pt;
		if (GetCursorPos(&pt)) {
		  if (ScreenToClient(hwnd, &pt)) {
			RECT clientRect;
			if (GetClientRect(hwnd, &clientRect)) {
			  float clientW = static_cast<float>(clientRect.right - clientRect.left);
			  float clientH = static_cast<float>(clientRect.bottom - clientRect.top);
			  if (clientW > 0.0f && clientH > 0.0f && g_screenWidth > 0.0f && g_screenHeight > 0.0f) {
				  g_mouseX = static_cast<float>(pt.x / clientW) * g_screenWidth;
				  g_mouseY = static_cast<float>(pt.y / clientH) * g_screenHeight;
			  } else {
				  g_mouseX = static_cast<float>(pt.x);
				  g_mouseY = static_cast<float>(pt.y);
			  }
			} else {
			  g_mouseX = static_cast<float>(pt.x);
			  g_mouseY = static_cast<float>(pt.y);
			}
			SendMouseRPC(g_mouseX, g_mouseY);
		  }
		}
	  }
	  else if (u_msg == WM_LBUTTONDBLCLK) {
		  SendClickRPC(true, (w_param == VK_LBUTTON ? MOUSE_LEFT_CLICK : (w_param == VK_RBUTTON ? MOUSE_RIGHT_CLICK : (w_param == VK_MBUTTON ? MOUSE_MIDDLE_CLICK : MOUSE_UNK_CLICK))));
	  }
	  else if (u_msg == WM_LBUTTONDOWN) {
		  SendClickRPC(false, MOUSE_LEFT_CLICK);
	  }
	  else if (u_msg == WM_RBUTTONDOWN) {
		  SendClickRPC(false, MOUSE_RIGHT_CLICK);
	  }
	  else if (u_msg == WM_MBUTTONDOWN) {
		  SendClickRPC(false, MOUSE_MIDDLE_CLICK);
	  }
	}

    if (oWndProc) {
        return CallWindowProcA(oWndProc, hwnd, u_msg, w_param, l_param);
    }
    return DefWindowProcA(hwnd, u_msg, w_param, l_param);
}

void BackgroundInitializationWorker() {
    while (GetModuleHandleA("samp.dll") == nullptr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }	
	
    while (!initialized) {
        if (rakhook::samp_addr() && rakhook::samp_version() != rakhook::samp_ver::unknown) {
            bool gamePtrValid = false;
            rakhook::samp_ver currentVer = rakhook::samp_version();

            if (currentVer == rakhook::samp_ver::v037r1) {
                gamePtrValid = (sampapi::v037r1::RefGame() != nullptr);
            } else if (currentVer == rakhook::samp_ver::v037r31) {
                gamePtrValid = (sampapi::v037r3::RefGame() != nullptr);
            } else if (currentVer == rakhook::samp_ver::v037r5) {
                gamePtrValid = (sampapi::v037r5::RefGame() != nullptr);
            } else if (currentVer == rakhook::samp_ver::v03dlr1) {
                gamePtrValid = (sampapi::v03dl::RefGame() != nullptr);
            }

            if (gamePtrValid) {
                if (rakhook::initialize()) {
                    initialized = true;
                    break;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    hWnd = FindWindowA("Grand Theft Auto San Andreas", NULL);
    if (hWnd && !oWndProc) {
        oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hkWndProc)));
    }
	
	void** vTableDevice = *(void***)(*(DWORD*)DEVICE_PTR);
	g_vmtHooks = new VTableHookManager(vTableDevice, D3D_VFUNCTIONS);
	oPresent = (_Present)g_vmtHooks->Hook(PRESENT_INDEX, (void*)hkPresent);
	oReset = (_Reset)g_vmtHooks->Hook(RESET_INDEX, (void*)hkReset);
	
	rakhook::on_receive_packet += [](Packet *packet) -> bool {
		if (packet->length == 0) return true;

        RakNet::BitStream bs(packet->data, packet->length, false);
        unsigned char packetId;
        bs.Read(packetId);	
		
        if (packetId == 0x1B) // ID_TIMESTAMP
		{
            bs.IgnoreBits(4 * 8); 
            bs.Read(packetId);
        }
		if (packetId == PACKET_INSTALLED_CHECK)
		{
            RakNet::BitStream _bs;

            _bs.Write<unsigned char>(PACKET_INSTALLED_CHECK);
	        _bs.Write<bool>(true);

            rakhook::send(&_bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0);
			return false;
		}
		return true;
	};
}

void c_plugin::game_loop()
{
    game_loop_hook.call_original();

    static bool threadSpawned = false;
	if (!threadSpawned) {
        threadSpawned = true;
        std::thread(BackgroundInitializationWorker).detach();
    }
}

void c_plugin::shutdown_for_unload()
{
	rakhook::on_receive_rpc.clear();
	rakhook::on_send_rpc.clear();
	rakhook::on_receive_packet.clear();
	rakhook::on_send_packet.clear();

	if (hWnd && oWndProc) {
		SetWindowLongPtrA(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));
		oWndProc = nullptr;
	}

	if (g_vmtHooks) {
		g_vmtHooks->Unhook(PRESENT_INDEX);
		g_vmtHooks->Unhook(RESET_INDEX);
		delete g_vmtHooks;
		g_vmtHooks = nullptr;
	}

	if (GetModuleHandleA("samp.dll") != nullptr) {
		rakhook::destroy();
	}

	game_loop_hook.remove();
}

c_plugin::c_plugin(HMODULE hmodule) :hmodule(hmodule)
{
    game_loop_hook.add(&c_plugin::game_loop);
}

c_plugin::~c_plugin()
{
	c_plugin::shutdown_for_unload();
}
