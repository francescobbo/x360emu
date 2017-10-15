/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "stdafx.h"
#include "x360emu.h"

#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <fstream>

using namespace std;

void RedirectIOToConsole()
{
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;
    // allocate a console for this app
    AllocConsole();
    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = 10000;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );

    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "r");
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );

    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );

    std::ios::sync_with_stdio();
}

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // global declaration

// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
    // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = hWnd;                                // the window to be used
    scd.SampleDesc.Count = 4;                               // how many multisamples
    scd.Windowed = TRUE;                                    // windowed/full-screen mode

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  D3D11_SDK_VERSION,
                                  &scd,
                                  &swapchain,
                                  &dev,
                                  NULL,
                                  &devcon);

    // get the address of the back buffer
    ID3D11Texture2D *pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, NULL);

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 600;

    devcon->RSSetViewports(1, &viewport);
}

void RenderFrame(void)
{
    // clear the back buffer to a deep blue
    devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

    // do 3D rendering on the back buffer here

    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}

void CleanD3D()
{
    // close and release all existing COM objects
    swapchain->Release();
    backbuffer->Release();
    dev->Release();
    devcon->Release();
}

#define MAX_LOADSTRING 100

HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
    RedirectIOToConsole();

    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_X360EMU, szWindowClass, MAX_LOADSTRING);

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_X360EMU));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_X360EMU);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassEx(&wcex);

    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    if (!hWnd)
        throw "Unable to create Window";

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    InitD3D(hWnd);

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_X360EMU));

    while(TRUE)
    {
        // Check to see if any messages are waiting in the queue
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // translate keystroke messages into the right format
            TranslateMessage(&msg);

            // send the message to the WindowProc function
            DispatchMessage(&msg);

            // check to see if it's time to quit
            if (msg.message == WM_QUIT)
                break;
        }
        else
        {
            RenderFrame();
        }
    }

    CleanD3D();

    return (int) msg.wParam;
}

#include <XDVDFSReader.h>
#include <XexParser.h>
#include <FileStream.h>
#include "XPeParser.h"
#include "Hardware.h"
#include "HLE/HLE.h"

void BootXex(std::string xexFn)
{
    try
    {
        Hardware::Init();

        FileStream stream(xexFn, FileMode::Open, FileAccess::Read);
        BinaryReaderBE xex(stream);

        XexParser xp(xex);
        xp.ExtractBaseFile("xbxExec.exe");
//      std::vector<XexResources::r
//      xp.ExtractResource(xp.GetResourcesList()[0], "xbxExec.exe", "resources.xbdf");

        XPeParser pp("xbxExec.exe", xp);
        pp.Load();

        HLE::Init();
        LinkLibraries(xp);
        Hardware::Boot();
    }
    catch (const char *exc)
    {
        cout << "Catched exception: " << exc << endl;
    }
}

void BootIso(std::string iso)
{
    try
    {
        XDVDFSReader xr(iso, false);
        xr.ExtractOne("default.xex", "\\default.xex");
    }
    catch (...)
    {
        throw;
    }
    
    BootXex("default.xex");
}

LPSTR UnicodeToAnsi(LPCWSTR s)
{
    if (s == NULL)
        return NULL;
    int cw = lstrlenW(s);
    if (cw == 0)
    {
        CHAR *psz = new CHAR[1];
        *psz='\0';
        return psz;
    }

    int cc = WideCharToMultiByte(CP_ACP,0,s,cw,NULL,0,NULL,NULL);
    if (cc==0)
        return NULL;
    CHAR *psz = new CHAR[cc+1];
    cc=WideCharToMultiByte(CP_ACP,0,s,cw,psz,cc,NULL,NULL);
    if (cc==0) {delete[] psz;return NULL;}
    psz[cc]='\0';
    return psz;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case ID_FILE_BOOTAC4:
            BootIso("c:\\RealGame.iso");
            break;
        case ID_FILE_BOOTISO:
            {
                OPENFILENAME ofn;
                WCHAR szFileName[MAX_PATH] = L"";

                ZeroMemory(&ofn, sizeof(ofn));

                ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
                ofn.hwndOwner = hWnd;
                ofn.lpstrFilter = L"ISO Files (*.iso)\0*.iso\0All Files (*.*)\0*.*\0";
                ofn.lpstrFile = szFileName;
                ofn.nMaxFile = MAX_PATH;
                ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT;
                ofn.lpstrDefExt = L"iso";

                if (GetOpenFileName(&ofn))
                {
                    BootIso(UnicodeToAnsi(szFileName));
                }
                break;
            }
        case ID_FILE_BOOTXEX:
            {
                OPENFILENAME ofn;
                WCHAR szFileName[MAX_PATH] = L"";

                ZeroMemory(&ofn, sizeof(ofn));

                ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
                ofn.hwndOwner = hWnd;
                ofn.lpstrFilter = L"XEX Files (*.xex)\0*.xex\0All Files (*.*)\0*.*\0";
                ofn.lpstrFile = szFileName;
                ofn.nMaxFile = MAX_PATH;
                ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT;
                ofn.lpstrDefExt = L"xex";

                if (GetOpenFileName(&ofn))
                {
                    BootXex(UnicodeToAnsi(szFileName));
                }
                break;
            }
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
