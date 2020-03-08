#include <Windows.h>
#include <cassert>
#include "th09address.h"
#include <string>

#include "inject.h"
#include "menu.h"

namespace th09mp {
	HMODULE hModule;
	HMODULE hThcrap;
}

extern "C" {
	int thcrap_plugin_init(void) {
		if (th09mp::hThcrap) {
			void* (*runconfig_get)(void) = (void* (*)(void))::GetProcAddress(th09mp::hThcrap, "runconfig_get"); // This looks confusing because it's a function poiner
			const char* (*json_object_get_string)(void* json_object, const char* key) = (const char* (*)(void*, const char*))::GetProcAddress(th09mp::hThcrap, "json_object_get_string");

			if (runconfig_get && json_object_get_string) {
				void* run_cfg = runconfig_get();
				const char* game_id = json_object_get_string(run_cfg, "game");
				if (strcmp(game_id, "th09") != 0) {
					return 1;
				}
			}
		}

		while (!th09mp::hModule) {
			::Sleep(1); // Epic ZUN style thread syncing
		}
		const char* title_ver_1_5 = "ìåï˚â‘âfíÀÅ@Å` Phantasmagoria of Flower View. ver 1.50a";
		if (::memcmp(th09mp::address::addr_window_title.ver1_5, title_ver_1_5, ::strlen(title_ver_1_5)) == 0)
		{
			th09mp::InjectOnFrameUpdate();
			th09mp::InjectOnReplayUpdate();
			th09mp::InjectOnGameStart();
			th09mp::InjectOnGameEnd();
			th09mp::InjectOnRNG();
			th09mp::InjectOnZUNNetplay();
			th09mp::InjectOnDifficultyMenu();

			// Temporary, for debugging
			AllocConsole();
			FILE* fDummy;
			freopen_s(&fDummy, "CONIN$", "r", stdin);
			freopen_s(&fDummy, "CONOUT$", "w", stderr);
			freopen_s(&fDummy, "CONOUT$", "w", stdout);
			return 0;
		}
		else
		{
			// Not PoFV 1.5a, will not work
			return 1;
		}
	}
}

void Detach(void){

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    switch(ul_reason_for_call){
    case DLL_PROCESS_ATTACH:
		th09mp::hModule = hModule;
		th09mp::hThcrap = ::GetModuleHandleW(L"thcrap.dll");
		if (!th09mp::hThcrap) {
			th09mp::hThcrap = ::GetModuleHandleW(L"thcrap_d.dll");
		}
		if (!th09mp::hThcrap) {
			if (::thcrap_plugin_init() != 0) {
				assert(0);
			}
		}
        break;
    case DLL_PROCESS_DETACH:
        Detach();
        break;
    }
    return TRUE;
}
