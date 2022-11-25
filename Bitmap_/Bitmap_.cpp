// Bitmap_.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#pragma comment(lib, "msimg32.lib")

#include <vector>
#include <iostream>
#include "framework.h"
#include "Bitmap_.h"
#include "resource.h"

#define MAX_LOADSTRING 100
using namespace std;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 전역 선언부
POINT resolution; // 해상도를 저장할 변수
RECT client_area; // 해상도를 토대로 클라이언트 영역을 저장할 변수
BOOL g_press = false;
BOOL g_portal_crash = false;
BOOL g_Key_D = false;
int g_jump_limit = 0;

// Thread
HANDLE thread;
DWORD tid;

// 비트맵
HBITMAP bitmap, bitmap1, bitmap2, bitmap3, bitmap4, bitmap5, bitmap6, bitmap7, bitmap8, bitmap9, bitmap10; // 로드한 비트맵을 저장하는 변수
BITMAP bitmap_info, bitmap_info2, Speed_item_info; // 로드한 비트맵의 정보를 저장하는 변수
HDC bitmap_dc, bitmap_dc1, bitmap_dc2, bitmap_dc3, bitmap_dc4, bitmap_dc5, bitmap_dc6, bitmap_dc7, bitmap_dc8, bitmap_dc9, bitmap_dc10; // 비트맵을 메모리 DC에 저장하는 변수

/* ------- Struct ------- */
struct Player
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int plus_speed = 0;
    
    // 출력 연산자오버로드
    friend ostream& operator <<(ostream& os, const Player& player);
};

struct Ground
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

struct Floor
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

struct Portal
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

struct Speed_item
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

/* ---- The End Struct ---- */

// Player - struct, vector 선언
Player s_player;
vector<Player> v_player;

// struct 선언
Ground ground;
Floor floors;
Portal portal;
Speed_item speed_item;
HWND hWnd;

// vector 출력 연산자 오버로드 선언
ostream& operator <<(ostream& os, const Player& s_player)
{
    os << "s_player.x: " << s_player.x << " s_player.y : " << s_player.y << " s_player.width : "
        << s_player.width << " s_player.height: " << s_player.height << " s_player.plus_speed: " << s_player.plus_speed;

    return os;
}

void ShowAll(const vector<Player>& v_player)
{
    // 출력연산자가오버로드되어있기때문에벡터의내용을
    // 쉽게출력가능함

    copy(v_player.begin(), v_player.end(), ostream_iterator<Player>(cout, "\n"));
}

// 중력 Thread
DWORD WINAPI gravity(LPVOID param)
{
    while (true)
    {
        // Thread 잠시멈춤
        Sleep(100);

        if (floors.y < s_player.y + s_player.height / 2)
        {
            s_player.y = 336;
            g_jump_limit = 0;
            break;
        }
        s_player.y += 1;
        InvalidateRect(hWnd, NULL, false);
    }
    return 0;
}

void player_move(WPARAM wParam)
{

    switch (wParam)
    {
    case 'A':
        s_player.x -= 10;
        break;
    case 'D':
        s_player.x += 5;
        s_player.x += s_player.plus_speed;
        break;
    case 'W':
        if (g_jump_limit >= 2)
        {
            break;
        }
        g_jump_limit++;
        s_player.y -= 20;
        break;
    case 'S':
        s_player.y += 10;
        break;
    }

}

void is_crash()
{
    if (speed_item.x + 7 < s_player.x + s_player.width / 2)
    {
        //MessageBox(hWnd, L"speed up !", L"Up", MB_OK);

        v_player.clear();
        s_player.plus_speed += 2;
    }

    // 특정 조건을 충족하면 장소 이동하도록
    if (portal.y < s_player.y + s_player.height / 2)
    {
        if (portal.x + 10 < s_player.x + s_player.width / 2)
        {
            g_press = false;
            g_portal_crash = true;
        }
    }
}

void Player_Area_Limit()
{
    // 플레이어가 그라운드를 못 벗어나게 하는 연산
    
    // Left
    if (ground.x > s_player.x - s_player.width / 2) // 이렇게 해주면 정확하게 사각형의 왼쪽 면의 좌표로 계산
    {
        s_player.x = 16;
    }
    
    // Right
    if (ground.width < s_player.x + s_player.width / 2)
    {
        s_player.x = 700 - 16;
    }
    // Top
    if (ground.y > s_player.y - s_player.height / 2 - 21)
    {
        s_player.y = 40;
    }
}

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BITMAP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BITMAP));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BITMAP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BITMAP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   resolution = { 700, 400 };
   client_area = { 0, 0, resolution.x, resolution.y };

   // 캡션 등의 크기를 계산하여 client_area에 반영
   // TRUE : 메뉴바를 포함할 경우
   // FALSE : 메뉴바를 포함하지 않을 경우
   AdjustWindowRect(&client_area, WS_OVERLAPPEDWINDOW, TRUE);

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, client_area.right - client_area.left, client_area.bottom - client_area.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Player x,y / Size
        s_player.x = 16;
        s_player.y = 336;
        s_player.width = 30;
        s_player.height = 30;

        v_player.push_back(s_player);

        // Ground x,y / Size
        ground.x = 0;
        ground.y = 0;
        ground.width = 700;
        ground.height = 400;
        
        // Floors x,y / Size
        floors.x = 700;
        floors.y = 350;
        floors.width = 0;
        floors.height = 50;

        // Portal x,y / Size
        portal.x = 650;
        portal.y = 300;
        portal.width = 50;
        portal.height = 50;

        // Speed_item x,y / Size
        speed_item.x = 270;
        speed_item.y = 270;
        speed_item.width = - 220;
        speed_item.height = - 220;
    }
    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
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

    case WM_GETMINMAXINFO:
    {
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = client_area.right - client_area.left;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = client_area.bottom - client_area.top;
        ((MINMAXINFO*)lParam)->ptMaxTrackSize.x = client_area.right - client_area.left;
        ((MINMAXINFO*)lParam)->ptMaxTrackSize.y = client_area.bottom - client_area.top;
    }
    break;
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case 0x20:  // 스페이스 바
            g_press = true;
            break;
        }

        // 스테이지에 돌아가야 이동 가능
        if (g_press == true)
        {
            player_move(wParam);
        }

        Player_Area_Limit(); // 플레이어 영역 제한
        is_crash();

        /*if (ground.y < player.y)
        {
            player.y = 400 - 1;
            player.height = 30;
        }*/

        // 이 프로젝트에는 더블 버퍼링이 없기 때문에 FALSE로 할 경우 잔상이 생김
        // 더블 버퍼링이라면 FALSE
        InvalidateRect(hWnd, NULL, FALSE);
    }
    break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc, memDC;
            HBITMAP new_bitmap, old_bitmap;
            memDC = BeginPaint(hWnd, &ps);
            new_bitmap = CreateCompatibleBitmap(memDC, resolution.x, resolution.y);
            hdc = CreateCompatibleDC(memDC);
            old_bitmap = (HBITMAP)SelectObject(hdc, new_bitmap);
            DeleteObject(old_bitmap);
            PatBlt(hdc, 0, 0, resolution.x, resolution.y, WHITENESS);

            WCHAR speed[128];

            wsprintf(speed, L"player speed = %d", s_player.plus_speed);

            /*WCHAR player[128];
            WCHAR ground[128];

            wsprintf(player, L"[ player.x = %d player.y = %d ]\n[ player.width = %d player.height  = %d ]", player.x - player.width / 2, player.y - player.height / 2, player.x + player.width / 2, player.y + player.height / 2);
            wsprintf(ground, L"[ ground.x = %d / ground.y = %d /  ground.width = %d / ground.height  = %d ]", ground.x, ground.y, ground.width, ground.height);*/

            /******** Bitmap ********/

            // b1.bmp
            bitmap1 = (HBITMAP)LoadImage(NULL, L"b1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bitmap_dc1 = CreateCompatibleDC(hdc);
            HBITMAP old_bitmap_1 = (HBITMAP)SelectObject(bitmap_dc1, bitmap1);
            DeleteObject(old_bitmap_1);

            // b2.bmp
            bitmap2 = (HBITMAP)LoadImage(NULL, L"b2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bitmap_dc2 = CreateCompatibleDC(hdc);
            HBITMAP old_bitmap_2 = (HBITMAP)SelectObject(bitmap_dc2, bitmap2);
            DeleteObject(old_bitmap_2);

            // b3.bmp
            bitmap3 = (HBITMAP)LoadImage(NULL, L"b3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bitmap_dc3 = CreateCompatibleDC(hdc);
            HBITMAP old_bitmap_3 = (HBITMAP)SelectObject(bitmap_dc3, bitmap3);
            DeleteObject(old_bitmap_3);

            // b4.bmp
            bitmap4 = (HBITMAP)LoadImage(NULL, L"b4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bitmap_dc4 = CreateCompatibleDC(hdc);
            HBITMAP old_bitmap_4 = (HBITMAP)SelectObject(bitmap_dc4, bitmap4);
            DeleteObject(old_bitmap_4);
            
            // b5.bmp
            bitmap5 = (HBITMAP)LoadImage(NULL, L"b5.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bitmap_dc5 = CreateCompatibleDC(hdc);
            HBITMAP old_bitmap_5 = (HBITMAP)SelectObject(bitmap_dc5, bitmap5);
            DeleteObject(old_bitmap_5);

            // basic_standing.bmp - 6
            bitmap6 = (HBITMAP)LoadImage(NULL, L"basic_standing.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bitmap_dc6 = CreateCompatibleDC(hdc);
            HBITMAP old_bitmap_6 = (HBITMAP)SelectObject(bitmap_dc6, bitmap6);
            DeleteObject(old_bitmap_6);

            // Speed_item.bmp - 7
            bitmap7 = (HBITMAP)LoadImage(NULL, L"bitmap3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bitmap_dc7 = CreateCompatibleDC(hdc);
            HBITMAP old_bitmap_7 = (HBITMAP)SelectObject(bitmap_dc7, bitmap7);
            DeleteObject(old_bitmap_7);

            // 비트맵에 대한 정보를 담는다.
            GetObject(bitmap1, sizeof(BITMAP), &bitmap_info); // Background
            GetObject(bitmap6, sizeof(BITMAP), &bitmap_info2); // Player
            GetObject(bitmap7, sizeof(BITMAP), &Speed_item_info); // Speed_item

            // Background 비트맵의 가로 사이즈
            LONG width = bitmap_info.bmWidth;

            // Background 비트맵의 세로 사이즈 
            LONG height = bitmap_info.bmHeight;

            // Player 비트맵의 가로 사이즈
            LONG player_width = bitmap_info2.bmWidth;

            // Player 비트맵의 세로 사이즈 
            LONG player_height = bitmap_info2.bmHeight;

            // Speed_item 비트맵의 가로 사이즈
            LONG Speed_item_width = Speed_item_info.bmWidth;

            // Speed_item 비트맵의 세로 사이즈 
            LONG Speed_item_height = Speed_item_info.bmHeight;

            if (g_press == true)
            {
                // 바닥의 y 좌표보다 player의 y 좌표가 더 높을 때 Thread 실행
                if (floors.y > s_player.y + s_player.height / 2)
                {
                    CreateThread(NULL, 0, gravity, hWnd, 0, NULL);
                }

                // 순서대로 화면에다가 그린다 - 겹친다.
                Rectangle(hdc, ground.x, ground.y, ground.width, ground.height);

                // TransparentBlt Explain
                // RGB : 255, 0, 255 인 색을 제거함
                // hdc에다가 bitmap_dc에 그려진 비트맵을 그림
                // 편의성을 위해 중심을 기준으로 이동
                // HDC, x 좌표, y 좌표, 가로 크기, 세로 크기, 메모리 DC, 비트맵 내 x 좌표, y 좌표, 비트맵 내 가로 크기, 세로 크기, 제거할 배경의 색
                
                // Background Bitmap
                TransparentBlt(hdc, 0, 0, 700, 400, bitmap_dc1, 0, 0, width, height, RGB(255, 0, 255));
                TransparentBlt(hdc, 0, 0, 700, 400, bitmap_dc2, 0, 0, width, height, RGB(255, 255, 255));
                TransparentBlt(hdc, 0, 0, 700, 400, bitmap_dc3, 0, 0, width, height, RGB(255, 255, 255));
                TransparentBlt(hdc, 0, 0, 700, 400, bitmap_dc4, 0, 0, width, height, RGB(255, 255, 255));
                TransparentBlt(hdc, 0, 0, 700, 400, bitmap_dc5, 0, 0, width, height, RGB(255, 255, 255));

                // Player Bitmap
                TransparentBlt(hdc, s_player.x - s_player.width / 2 - 22, s_player.y - s_player.height / 2 - 32, player_width + 30, player_height + 25, bitmap_dc6, 0, 0, player_width, player_height, RGB(255, 0, 255));

                // Speed_item Bitmap
                TransparentBlt(hdc, speed_item.x, speed_item.y, Speed_item_width + 30, Speed_item_height + 30, bitmap_dc7, 0, 0, Speed_item_width, Speed_item_height, RGB(255, 255, 255));

                // Floors, Portal Rectangle - Bitmap 으로 전환 예정
                Rectangle(hdc, floors.x, floors.y, floors.width, floors.y + floors.height);
                Rectangle(hdc, portal.x, portal.y, portal.x + portal.width, portal.y + portal.height);

                TextOut(hdc, 10, 10, speed, wcslen(speed));
            /*    TextOut(hdc, 10, 10, player, wcslen(player));
                TextOut(hdc, 10, 30, ground, wcslen(ground));*/

            }
            // 비트맵 메모리 해제
            DeleteDC(bitmap_dc);
            DeleteObject(bitmap);
            // 비트맵 메모리 해제
            DeleteDC(bitmap_dc2);
            DeleteObject(bitmap2);
            // 비트맵 메모리 해제
            DeleteDC(bitmap_dc3);
            DeleteObject(bitmap3);

            BitBlt(memDC, 0, 0, resolution.x, resolution.y, hdc, 0, 0, SRCCOPY);

            DeleteDC(hdc);
            EndPaint(hWnd, &ps);
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

// 정보 대화 상자의 메시지 처리기입니다.
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
