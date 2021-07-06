// Direct2D.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Direct2D.h"

#include <d2d1.h>
#include <d2d1helper.h>

#define MAX_LOADSTRING 100

template<class Interface>
inline void SafeRelease(
    Interface** ppInterfaceToRelease
)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Define our game of life grid
const int grid_size = 200;
int current_grid = 0;
unsigned char grid1[grid_size][grid_size];
unsigned char grid2[grid_size][grid_size];
unsigned char* grids[2] = { &grid1[0][0], &grid2[0][0] };

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void initialise_grid(unsigned char *pGrid);
void Update(unsigned char*, unsigned char*);
void Render(unsigned char *);

ID2D1Factory* m_pDirect2dFactory;
ID2D1HwndRenderTarget* m_pRenderTarget;
ID2D1SolidColorBrush* m_pCornflowerBlueBrush;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

    if (SUCCEEDED(hr)) {
        // Initialize global strings
        LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadStringW(hInstance, IDC_DIRECT2D, szWindowClass, MAX_LOADSTRING);
        MyRegisterClass(hInstance);

        // Perform application initialization:
        if (!InitInstance(hInstance, nCmdShow))
        {
            return FALSE;
        }

        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DIRECT2D));

        initialise_grid(&grid1[0][0]);

        // Main message loop:
        bool running = true;
        while (running)
        {
            MSG msg;
            while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                if (msg.message == WM_QUIT)
                {
                    running = false;
                }
            }

            // Do update, rendering and all the real game loop stuff
            Update(grids[current_grid], grids[current_grid ^ 1]);
            Render(grids[current_grid]);
            current_grid = (current_grid + 1) % 2;
        }

        SafeRelease(&m_pDirect2dFactory);
        SafeRelease(&m_pRenderTarget);
        SafeRelease(&m_pCornflowerBlueBrush);

        return 0;
    }
    return -1;
}

void initialise_grid(unsigned char* pGrid)
{
    for (int y = 0; y < grid_size; ++y)
    {
        for (int x = 0; x < grid_size; ++x)
        {
            *(pGrid + ((y * grid_size) + x)) = (unsigned char)(rand() & 1);
        }
    }
}

unsigned char get_grid_value(unsigned char* pGrid, int x, int y)
{
    return *(pGrid + (y * grid_size) + x);
}

void Update(unsigned char* pSourceGrid, unsigned char* pDestGrid)
{
    for (int y = 0; y < grid_size; ++y)
    {
        for (int x = 0; x < grid_size; ++x)
        {
            unsigned char num_neighbours = get_grid_value(pSourceGrid, (x - 1) % grid_size, (y - 1) % grid_size);
            num_neighbours += get_grid_value(pSourceGrid, x, (y - 1) % grid_size);
            num_neighbours += get_grid_value(pSourceGrid, (x + 1) % grid_size, (y - 1) % grid_size);
            num_neighbours += get_grid_value(pSourceGrid, (x - 1) % grid_size, y);
            num_neighbours += get_grid_value(pSourceGrid, (x + 1) % grid_size, y);
            num_neighbours += get_grid_value(pSourceGrid, (x - 1) % grid_size, (y + 1) % grid_size);
            num_neighbours += get_grid_value(pSourceGrid, x, (y + 1) % grid_size);
            num_neighbours += get_grid_value(pSourceGrid, (x + 1) % grid_size, (y + 1) % grid_size);

            unsigned char next_generation = 0;

            // Any live cell with two or three live neighbours survives.
            if (get_grid_value(pSourceGrid, x, y) == 1)
            {
                // Any live cell with two or three live neighbours survives.
                if ((num_neighbours > 1) && (num_neighbours < 4))
                {
                    next_generation = 1;
                }
            }
            else
            {
                // Any dead cell with three live neighbours becomes a live cell.
                if (num_neighbours == 3)
                {
                    next_generation = 1;
                }
            }
            // All other live cells die in the next generation.Similarly, all other dead cells stay dead.
            *(pDestGrid + (y * grid_size) + x) = next_generation;
        }
    }
}

void Render(unsigned char *pGrid)
{
    m_pRenderTarget->BeginDraw();
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

    float cell_size_x = rtSize.width / grid_size;
    float cell_size_y = rtSize.height / grid_size;

    for (int y = 0; y < grid_size; ++y)
    {
        for (int x = 0; x < grid_size; ++x)
        {
            unsigned char *this_cell = pGrid + (y * grid_size) + x;
            if (*this_cell)
            {
                D2D1_RECT_F rectangle1 = D2D1::RectF(
                    x * cell_size_x,
                    y * cell_size_y,
                    (x * cell_size_x) + cell_size_x,
                    (y * cell_size_y) + cell_size_y
                );

                m_pRenderTarget->FillRectangle(&rectangle1, m_pCornflowerBlueBrush);
            }
        }
    }
    HRESULT hr = m_pRenderTarget->EndDraw();
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECT2D));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DIRECT2D);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);

   RECT rc;
   GetClientRect(hWnd, &rc);

   D2D1_SIZE_U size = D2D1::SizeU(
       rc.right - rc.left,
       rc.bottom - rc.top
   );

   // Create a Direct2D render target.
   HRESULT hr = m_pDirect2dFactory->CreateHwndRenderTarget(
       D2D1::RenderTargetProperties(),
       D2D1::HwndRenderTargetProperties(hWnd, size),
       &m_pRenderTarget
   );

   if (SUCCEEDED(hr))
   {
       // Create a blue brush.
       hr = m_pRenderTarget->CreateSolidColorBrush(
           D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
           &m_pCornflowerBlueBrush
       );
   }
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            ValidateRect(hWnd, NULL);
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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
