// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "ImGuiAddons.h"
#include <d3d9.h>
#include <iostream>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

#include "CKeyHook.h"
#include <vector>
#include <unordered_map>

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};
CKeyHook* pKeyHook = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

enum PaintElementType {
    PAINT_CIRCLE = 0,
    PAINT_RECTANGLE = 1,
    PAINT_LINE
};

struct PaintElement {
    LONG                iPosX, iPosY;
    LONG                iPosX1, iPosY1;
    ImU32               iColor;
    float               fSize;
    float               fRounding;
    PaintElementType    eElemType;

    PaintElement(LONG x, LONG y, ImU32 color, float size, PaintElementType type, float rounding = 0.0f, LONG x1 = 0, LONG y1 = 0) {
        this->iPosX = x;
        this->iPosY = y;
        this->iColor = color;
        this->fSize = size;
        this->eElemType = type;
        this->fRounding = rounding;
        this->iPosX1 = x1;
        this->iPosY1 = y1;
    }
};

void Theme() {
    //ImGui::GetIO().Fonts->AddFontFromFileTTF("../data/Fonts/Ruda-Bold.ttf", 15.0f, &config);
    ImGui::GetStyle().FrameRounding = 4.0f;
    ImGui::GetStyle().GrabRounding = 4.0f;
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    pKeyHook = new CKeyHook();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX9 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    ImGui_ImplWin32_Init(hwnd);
    Theme();
    ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\HACK-REGULAR.TTF", 15.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
    ImGui_ImplDX9_Init(g_pd3dDevice);


    bool show_demo_window = true;
    bool show_another_window = false;
    bool paint_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::vector<PaintElement> Elements;
    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();


        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
        if (pKeyHook->isKeyPressed(VK_F2)) {
            paint_window ^= 1;
        }
        if (paint_window) {
            static ImVec2 PaintSize;
            static bool changeSize = false;
            static float radius = 5.0f;
            static float rounding = 0.0f;
            static float color[3] = { 1.0f, 0.0f, 0.0f };
            static float paintBG[3] = { 1.0f, 1.0f, 1.0f };
            static bool hovered = false;
            static bool eraser = false;
            static bool brounding = false;
            static int currentPainter = 0;
            static std::string DownText;
            ImGuiWindowFlags flags = 0;
            flags |= ImGuiWindowFlags_AlwaysAutoResize;
            flags |= (hovered) ? ImGuiWindowFlags_NoMove : 0;
            ImGui::Begin("Paint", &paint_window, flags);
            ImGui::SliderFloat("##radius", &radius, 1.0f, 100.0f, (currentPainter == PaintElementType::PAINT_CIRCLE) ? "Radius: %.3f" : (currentPainter == PaintElementType::PAINT_LINE) ? "Thickness: %.3f" : "Size: %.3f"); if (currentPainter == PaintElementType::PAINT_RECTANGLE) { ImGui::SameLine(); ImGui::Checkbox("Rounding", &brounding); }
            ImGui::ColorEdit3("##color", color); if (currentPainter == PaintElementType::PAINT_RECTANGLE) { ImGui::SameLine(); ImGui::SliderFloat("##rounding", &rounding, 0.0f, 40.0f, "Rounding: %.2f"); }
            ImGui::ColorEdit3("##bgCOLOR", paintBG); 
            ImGui::RadioButton("Circles", &currentPainter, PaintElementType::PAINT_CIRCLE); ImGui::SameLine();
            ImGui::RadioButton("Rectangles", &currentPainter, PaintElementType::PAINT_RECTANGLE); ImGui::SameLine();
            ImGui::RadioButton("Lines", &currentPainter, PaintElementType::PAINT_LINE);
            ImGui::Checkbox("Eraser", &eraser); ImGui::SameLine();
            ImGui::Checkbox("Change Paint Window Size", &changeSize);
            if (!changeSize) {
                PaintSize.x = 500.0f;
                PaintSize.y = 400.0f;
            }
            else {
                ImGui::OpenPopupOnItemClick("##SizePicker");
                if (ImGui::BeginPopup("##SizePicker", flags)) {
                    ImGui::BeginChild("##Slider2D", ImVec2(330, 370), flags);
                    ImGui::SliderScalar2D("##Size", &PaintSize.x, &PaintSize.y, 400.0f, 1600.0f, 400.0f, 900.0f, 0.9f);
                    ImGui::EndChild();
                    ImGui::EndPopup();
                }
            }
            if (ImGui::Button("Clear")) {
                Elements.clear();
            }
            auto wndPos = ImGui::GetCursorScreenPos();
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(paintBG[0], paintBG[1], paintBG[2], 1.0f));
            ImGui::BeginChild("##paint", ImVec2(PaintSize.x, PaintSize.y));
            static int x1, x2, y1, y2 = 0;
            if (hovered && pKeyHook->isKeyPressed(VK_LBUTTON)) {
                if (currentPainter == PaintElementType::PAINT_LINE) {
                    DownText = "Drawing";
                    float rnd = (brounding) ? rounding : 0.0f;
                    auto cursorPos = ImGui::GetMousePos();
                    
                    auto clr = (eraser) ? (paintBG) : color;
                    if (clr == paintBG) {
                        DownText += "(Erasing)";
                    }
                    DownText += ": Line";
                    if (x1 == 0 && y1 == 0) {
                        x1 = cursorPos.x - wndPos.x;
                        y1 = cursorPos.y - wndPos.y;
                    }
                    else if (x2 == 0 && y2 == 0) {
                        x2 = cursorPos.x - wndPos.x;
                        y2 = cursorPos.y - wndPos.y;
                        Elements.emplace_back(x1, y1, ImGui::ColorConvertFloat4ToU32(ImVec4(clr[0], clr[1], clr[2], 1.0f)), radius, PaintElementType::PAINT_LINE, rnd, x2, y2);
                        x1 = x2 = y1 = y2 = 0;
                    }
                }
            }
            if ((x2 == 0 && y2 == 0) && !(x1 == 0 && y1 == 0)) {
                auto cursorPos = ImGui::GetMousePos();
                auto pos1 = ImVec2(wndPos.x + x1, wndPos.y + y1);
                auto pos2 = ImVec2(min(cursorPos.x, wndPos.x + PaintSize.x), min(cursorPos.y, wndPos.y + PaintSize.y));
                auto clr = (eraser) ? (paintBG) : color;
                ImGui::GetWindowDrawList()->AddLine(pos1, pos2, ImGui::ColorConvertFloat4ToU32(ImVec4(clr[0], clr[1], clr[2], 1.0f)), radius);
            }
            else if (hovered && pKeyHook->isKeyDown(VK_LBUTTON)) {
                DownText = "Drawing";
                auto cursorPos = ImGui::GetMousePos();
                auto clr = (eraser) ? (paintBG): color;
                if (clr == paintBG) {
                    DownText += "(Erasing)";
                }
                if (currentPainter == PaintElementType::PAINT_CIRCLE) {
                    DownText += ": Circle";
                    Elements.emplace_back(cursorPos.x - wndPos.x, cursorPos.y - wndPos.y + radius / 2, ImGui::ColorConvertFloat4ToU32(ImVec4(clr[0], clr[1], clr[2], 1.0f)), radius, PaintElementType::PAINT_CIRCLE);
                }
                else if (currentPainter == PaintElementType::PAINT_RECTANGLE) {
                    DownText += ": Rectangle";
                    float rnd = (brounding) ? rounding : 0.0f;
                    Elements.emplace_back(cursorPos.x - wndPos.x, cursorPos.y - wndPos.y + radius / 2, ImGui::ColorConvertFloat4ToU32(ImVec4(clr[0], clr[1], clr[2], 1.0f)), radius, PaintElementType::PAINT_RECTANGLE, rnd);
                }
            }
            
            if (pKeyHook->isKeyDown(VK_LCONTROL) && (pKeyHook->isKeyPressed(0x5A) || pKeyHook->isKeyDown(0x5A))) {
                DownText = "Ctrl + Z";
                if (Elements.size()) Elements.pop_back();
            }
            ImGui::EndChild();
            hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
            ImGui::Text(DownText.c_str());
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            for (auto& i : Elements) {
                auto drawList = ImGui::GetWindowDrawList();
                if (i.eElemType == PaintElementType::PAINT_CIRCLE) {
                    drawList->AddCircleFilled(ImVec2(wndPos.x + i.iPosX, wndPos.y + i.iPosY), i.fSize, i.iColor);
                }
                else if (i.eElemType == PaintElementType::PAINT_RECTANGLE) {
                    auto pos1 = ImVec2(max(wndPos.x + i.iPosX - i.fSize, wndPos.x), max(wndPos.y + i.iPosY - i.fSize, wndPos.y));
                    auto pos2 = ImVec2(min(wndPos.x + i.iPosX + i.fSize, wndPos.x + PaintSize.x), min(wndPos.y + i.iPosY + i.fSize, wndPos.y + PaintSize.y));
                    drawList->AddRectFilled(pos1, pos2, i.iColor, i.fRounding);
                }
                else if (i.eElemType == PaintElementType::PAINT_LINE) {
                    drawList->AddLine(ImVec2(wndPos.x + i.iPosX, wndPos.y + i.iPosY), ImVec2(wndPos.x + i.iPosX1, wndPos.y + i.iPosY1), i.iColor, i.fSize);
                }
            }
            ImGui::End();
            DownText = "IDLE";
        }
        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
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
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
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

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    pKeyHook->ClickHandler(msg, wParam, lParam);
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}