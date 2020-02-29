#include <Windows.h>
#include <cassert>
#include "th09address.h"
#include <string>

#include "inject.h"

void Attach(HANDLE hModule){
    const char* title_ver_1_5 = "ìåï˚â‘âfíÀÅ@Å` Phantasmagoria of Flower View. ver 1.50a";
    if (::memcmp(th09mp::address::addr_window_title.ver1_5, title_ver_1_5, ::strlen(title_ver_1_5)) == 0)
    {
       th09mp::InjectOnFrameUpdate();
       th09mp::InjectOnReplayUpdate();
       th09mp::InjectOnGameStart();
       th09mp::InjectOnGameEnd();
	   th09mp::InjectOnRNG();
	   th09mp::InjectOnZUNNetplay();

       // Temporary, for debugging
       AllocConsole();
       FILE* fDummy;
       freopen_s(&fDummy, "CONIN$", "r", stdin);
       freopen_s(&fDummy, "CONOUT$", "w", stderr);
       freopen_s(&fDummy, "CONOUT$", "w", stdout);
    }
    else
    {
       // Not PoFV 1.5a, will not work
       assert(0);
    }
}

void Detach(void){

}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
    switch(ul_reason_for_call){
    case DLL_PROCESS_ATTACH:
        Attach(hModule);
        break;
    case DLL_PROCESS_DETACH:
        Detach();
        break;
    }
    return TRUE;
}
