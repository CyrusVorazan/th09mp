#include "menu.h"

#include "th09address.h"
#include "network.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();

namespace th09mp {
    
    namespace network
    {
        extern bool isHost;
        extern bool connected;
        extern PlayerSide hostSide;
    }

    HHOOK ms_hHook;

    LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        LPMSG pMsg = reinterpret_cast<LPMSG>(lParam);

        if (nCode == HC_ACTION)
        {
            ImGuiIO& io = ImGui::GetIO();
            switch (pMsg->message)
            {
            case WM_LBUTTONDOWN: io.MouseDown[0] = true;  break;
            case WM_LBUTTONUP:   io.MouseDown[0] = false; break;
            case WM_RBUTTONDOWN: io.MouseDown[1] = true;  break;
            case WM_RBUTTONUP:   io.MouseDown[1] = false; break;
            case WM_MBUTTONDOWN: io.MouseDown[2] = true;  break;
            case WM_MBUTTONUP:   io.MouseDown[2] = false; break;
            case WM_MOUSEWHEEL:  io.MouseWheel += GET_WHEEL_DELTA_WPARAM(pMsg->wParam) > 0 ? +1.0f : -1.0f; break;
            
            case WM_MOUSEMOVE:
                io.MousePos.x = (signed short)(pMsg->lParam);
                io.MousePos.y = (signed short)(pMsg->lParam >> 16);
                break;

            case WM_KEYDOWN:
                if (pMsg->wParam < 256)
                    io.KeysDown[pMsg->wParam] = 1;
                break;

            case WM_KEYUP:
                if (pMsg->wParam < 256)
                    io.KeysDown[pMsg->wParam] = 0;
                break;

            case WM_CHAR:
                if (pMsg->wParam > 0 && pMsg->wParam < 0x10000)
                {
                    io.AddInputCharacter(static_cast<ImWchar>(pMsg->wParam));
                }
                break;
            }
        }
        return CallNextHookEx(ms_hHook, nCode, wParam, lParam);
    }

    void PopupSelect(std::string title, std::vector<std::string> options, int* selectedIdx)
    {
        if (!selectedIdx)
            return;

        ImGui::Text(title.c_str());
        ImGui::SameLine();

        std::string selectId = title + "_select_popup";

        if (ImGui::Button(options[*selectedIdx].c_str()))
            ImGui::OpenPopup(selectId.c_str());
        if (ImGui::BeginPopup(selectId.c_str()))
        {
            for (int i = 0; i < options.size(); i++)
                if (ImGui::Selectable(options[i].c_str()))
                    *selectedIdx = i;
            ImGui::EndPopup();
        }
    }

    void ShowImGuiNetplayMenu()
    {
        HWND hwnd = (HWND)address::globals_ver1_5->hwnd;

        ms_hHook = SetWindowsHookEx(WH_GETMESSAGE, MessageHookProc, nullptr, GetCurrentThreadId());
        
        // Dear ImGui uses d3d9 here although the game uses d3d8.
        // imgui_impl_dx9 should be rewritten for d3d8 to make them consistent.
        // For now it works even with two d3d versions at once so it's not a big deal.

        // Initialize Direct3D
        if (!CreateDeviceD3D(hwnd))
        {
            CleanupDeviceD3D();
            return;
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX9_Init(g_pd3dDevice);

        int selected_mode = 0;

        // Main loop
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while (msg.message != WM_QUIT)
        {
            // Poll and handle messages (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                continue;
            }

            network::ProcessPackets();

            // Start the Dear ImGui frame
            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // Keep a demo window around just for reference
            ImGui::ShowDemoWindow();

            {
                bool cross;
                ImGui::Begin("Touhou Kaeidzuka Multiplayer", &cross);

                // General info
                ImGui::Text("Select the mode and input relevant data");

                // Select mode
                PopupSelect("Mode", { "Host", "Client", "Spectator" }, &selected_mode);

                if (selected_mode == 0)
                {
                    // Show host options
                    static int port = 17723;
                    ImGui::InputInt("Port", &port);

                    // TODO: implement side selection
                    // The idea is to display a list of connected peers which may choose their side/role individually.
                    // Possible roles: player, spectator
                    // Possible sides: left, right, undecided
                    // Not sure how to handle choice conflicts. Simplest way would be to just not start the game until the choices are consistent. Perhaps host preference.
                    // Host also may choose to be a spectator, hosting a game for two client players

                    static int selected_side = 0;
                    PopupSelect("Side", { "Left", "Right" }, &selected_side);
                    network::hostSide = (PlayerSide)selected_side;

                    if (ImGui::Button("Host the game"))
                    {
                        th09mp::network::Host(port);
                    }
                }

                if (selected_mode > 0)
                {
                    // Show client/spectator options
                    static char ip[128] = "192.168.0.107";
                    ImGui::InputText("IP Address", ip, IM_ARRAYSIZE(ip));

                    static int port = 17723;
                    ImGui::InputInt("Port", &port);

                    if (ImGui::Button("Connect"))
                    {
                        th09mp::network::Connect(ip, port);
                    }
                }

                if (!cross || network::connected)
                    msg.message = WM_QUIT;

                // TODO: currently both players should start and end the game independently of each other.
                // There's still work to be done to let both players navigate the pre-match and in-match menus
                // and transmit their inputs. In theory it shouldn't be difficult to do with some Win32 hooks.

                ImGui::End();
            }

            // Rendering
            ImGui::EndFrame();
            g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
            g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.5f);
            D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * 255.0f), (int)(clear_color.y * 255.0f), (int)(clear_color.z * 255.0f), (int)(clear_color.w * 255.0f));
            g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
            if (g_pd3dDevice->BeginScene() >= 0)
            {
                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
                g_pd3dDevice->EndScene();
            }
            HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

            // Handle loss of D3D9 device
            if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
                ResetDevice();
        }

        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();

        UnhookWindowsHookEx(ms_hHook);
    }
}



// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}
