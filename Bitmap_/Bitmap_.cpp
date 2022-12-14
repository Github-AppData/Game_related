// Bitmap_.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#pragma comment(lib, "msimg32.lib")

#include <math.h>
#include <vector>
#include <iostream>
#include "framework.h"
#include "Bitmap_.h"
#include "resource.h"
#include <algorithm>

#define MAX_LOADSTRING 100
void is_crash();

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

WCHAR Game_over[128];

// 전역 선언부
POINT resolution; // 해상도를 저장할 변수
RECT client_area; // 해상도를 토대로 클라이언트 영역을 저장할 변수

BOOL g_press = false; // 스페이스 바 누름 여부 확인 - 게임 시작
BOOL g_portal_crash = false; // 포탈 충돌 여부 확인
BOOL g_Key_D = false; // 
BOOL is_no_crash = false;
BOOL is_no_skel_crash = false;

HANDLE handle; // Thread 핸들
BOOL is_item_die = false; // 아이템을 먹었는지 확인 유무
int g_jump_limit = 0; // 점프 제한
int speed_plus = 0; // 
BOOL is_hp_decrease = false; // hp 감소
BOOL is_hp_skel_decrease = false; // skeleton hp 감소
int dx, dy;
int mouse_x, mouse_y;
BOOL ground_is_crash = false;

// HP 브러시 자료형 선언
HBRUSH os_b, my_b;

BOOL is = false;
BOOL is_thread = false;

// Thread
HANDLE thread;
DWORD tid;

// 비트맵
HBITMAP bitmap, bitmap1, bitmap2, bitmap3, bitmap4, bitmap5, bitmap6, bitmap7, bitmap8, bitmap9, bitmap10; // 로드한 비트맵을 저장하는 변수
BITMAP bitmap_info, bitmap_info2, Speed_item_info, skeleton_info, bullet_info; // 로드한 비트맵의 정보를 저장하는 변수
HDC bitmap_dc, bitmap_dc1, bitmap_dc2, bitmap_dc3, bitmap_dc4, bitmap_dc5, bitmap_dc6, bitmap_dc7, bitmap_dc8, bitmap_dc9, bitmap_dc10; // 비트맵을 메모리 DC에 저장하는 변수

/* ------- Struct ------- */
struct skeleton_area_limit
{
	int left_limit = 200;
	int right_limit = 600;
};

struct Player
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int lifes = 3; // 라이프 수를 의미한다.
	int hp = 100;
	int plus_speed = 0;
	BOOL is_death = false; // 완전히 죽는 것
};

// 1. 공격 비트맵 (작은 원) / 2. 공격이 나갈 수 있는 Thread / 3. Thread - Sleep (연사 방지)
// LBD → Attack Thread 작동 - 사정거리 있다. (MAX 사정거리 도달 시 - 공격 비트맵 삭제)
// Attack Struct의 인수들 - range (사정거리), damage, attack_speed 

// x, y 좌표 / Size (width, height) / hp / damage
struct Skeleton
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int hp = 100;
	int damage = 20;
	BOOL is_death = false;
};

struct Bullet
{
	float x = 0;
	float y = 0;
	int damage = 10;
	BOOL is_crash = false; // 충돌 여부확인
};

struct Object
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};


/* ---- The End Struct ---- */

// Player - struct, vector 선언
Player s_player;
Player* p_player = new Player();
std::vector<Player> v_player;

// Vector iterator 선언

// struct 객체 선언
HDC hdc;
HWND hWnd;
Object ground;
Object floors;
Object portal; // 포탈
Object speed_item;
Skeleton skel; // skeleton
Skeleton* p_skel = new Skeleton();
DWORD thread_id_Player_bullet; // Thread id 선언
skeleton_area_limit skel_limit; // skeleton 이동 영역제한
Bullet* bullet = new Bullet(); // 총알
HANDLE bullet_handle;

// attack bitmap
void Player_bullet(HDC hdc)
{
	TransparentBlt(hdc, bullet->x, bullet->y, 11, 10, bitmap_dc9, 0, 0, 11, 10, RGB(255, 0, 255));
}

// Player 공격 
DWORD WINAPI D_Player_bullet(LPVOID param)
{
	// 쓰레드가 발동되면 무한루프
	hdc = GetDC(hWnd);
	while (true)
	{
		/**
		* 충돌 여부를 체크한 다음에 
		*/

		// Thread 잠시멈춤
		Sleep(100);

		// 충돌 여부 체크
		is_crash(); 

		// 그라운드 오른쪽 충돌 구현 - 왼쪽 위쪽 아래쪽 구현 해야댐.
		if (static_cast<int>(bullet->x) + 20 > ground.width)
		{
			int ad = 0;
			ground_is_crash = true;
			bullet->x = s_player.x - s_player.width / 2 + 30;
			bullet->y = s_player.y - s_player.height / 2;
		}

		// 마우스 포인터 방향으로 날아가기 위한 빗변길이 계산
		float degree = sqrt(pow(dx, 2) + pow(dy, 2)); 

		// 이동 - 속
		bullet->x += 10.f * (dx / degree);
		bullet->y += 10.f * (dy / degree);

		// bullet이 mouse_x 까지 가면, skeleton에 충돌 하면 Thread 종료
		if (bullet->is_crash == true || ground_is_crash == true)
		{
			// Thread end
			ExitThread(thread_id_Player_bullet);
		}

		// Skeleton Crash Judgment
		if (bullet->y > 270)
		{
			// 왼쪽 충돌 판정 - Complete
			// 1. Player가 Skeleton의 왼쪽에 있으면서, 2. bullet의 x가 skeleton의 x보다 클 경우 조건을 충족한다.
			if (s_player.x - s_player.width / 2 < skel.x - skel.width / 2 && bullet->x > skel.x - skel.width / 2)
			{
				// 조건 충족
				bullet->is_crash = true;
				is_hp_skel_decrease = true;
			}

			// 오른쪽 충돌 판정 - Complete
			// 1. Player가 Skeleton의 오른쪽에 있으면서, 2. bullet의 x가 skeleton의 x보다 작을 경우 조건을 충족한다.
			if (s_player.x - s_player.width / 2 > skel.x - skel.width / 2 && bullet->x < skel.x - skel.width / 2 + 60)
			{
				// 조건 충족
				bullet->is_crash = true;
				is_hp_skel_decrease = true;
			}
		}
		
		// 화면 무효화
		InvalidateRect(hWnd, NULL, false);
	}
	ResumeThread(bullet_handle);
	return 0;
}

// 중력 Thread
DWORD WINAPI gravity(LPVOID param)
{
	// 쓰레드가 발동되면 무한루프
	while (true)
	{
		// Thread 잠시멈춤
		Sleep(100);

		// 조건을 검사한 뒤에
		if (floors.y < s_player.y + s_player.height / 2)
		{
			// 성립 O → 중력 적용 X
			s_player.y = 336;
			// 바닥에 닿으면, 점프 초기화
			g_jump_limit = 0;
			break;
		}
		// 성립 X → 중력 적용 O
		s_player.y += 1;

		// 화면 무효화
		InvalidateRect(hWnd, NULL, false);
	}

	return 0;
}

void player_move(WPARAM wParam)
{
	switch (wParam)
	{
		// 플레이어 이동시 총알의 좌표도 같이 이동할 수 있도록
	case 'A':
		if (is_thread == true)
		{
			break;
		}
		s_player.x -= 5;
		s_player.x -= s_player.plus_speed;
		break;
	case 'D':
		if (is_thread == true)
		{
			break;
		}
		s_player.x += 5;
		s_player.x += s_player.plus_speed;
		break;
	case 'W':
		if (is_thread == true)
		{
			break;
		}
		// 점프 기회 다 쓰면, 더 이상 점프를 하지 못하도록 하는 조건 문이다.
		if (g_jump_limit >= 2) { break; }
		g_jump_limit++;
		s_player.y -= 20;
		break;
	case 'S':
		if (is_thread == true)
		{
			break;
		}
		s_player.y += 10;
		break;
	}
}

void is_crash()
{
	//  Speed_item
	if (speed_item.x + 7 < s_player.x + s_player.width / 2)
	{
		//MessageBox(hWnd, L"speed up !", L"Up", MB_OK);
		if (speed_plus == 0)
		{
			s_player.plus_speed += 5;
			speed_plus++;
		}

		// size 만 줄이고, 메모리 해제는 안 된 상태
		v_player.clear();

		// 메모리의 여부 상관없이, vector의 사이즈가 0인지 아닌지를 판단해서 BOOL 반환
		if (v_player.empty() == true)
		{
			is_item_die = true;
		}
	}

	// Player와 Skeleton에 충돌 여부 check - 앞쪽
	if (s_player.x - s_player.width / 2 + 20 > skel.x - skel.width / 2)
	{
		// Player와 Skeleton이 부딪혔다.
		is_hp_decrease = true;
		if (s_player.x - s_player.width / 2 > skel.x - skel.width / 2 + 70)
		{
			// Player가 Skeleton을 지나쳤다.
			is_no_crash = true;
		}
		else
		{
			// 아니면 부딪힌 것 이다.
			is_no_crash = false;
		}
	}

	// Player와 Skeleton에 충돌 여부 check - 뒤쪽
	if (s_player.x - s_player.width / 2 < skel.x - skel.width / 2 + 65)
	{
		// Player와 Skeleton이 부딪혔다.
		is_hp_decrease = true;
		if (s_player.x - s_player.width / 2 < skel.x - skel.width / 2 - 28)
		{
			// Player가 Skeleton을 지나쳤다.
			is_no_crash = true;
		}
		else
		{
			// 아니면 부딪힌 것 이다.
			is_no_crash = false;
		}
	}

	// Portal - 특정 조건을 충족하면 장소 이동하도록
	if (portal.y < s_player.y + s_player.height / 2)
	{
		if (portal.x + 10 < s_player.x + s_player.width / 2)
		{
			g_press = false;
			g_portal_crash = true;
		}
	}

	if (is_hp_skel_decrease == true && is_no_skel_crash == false)
	{
		if (p_skel->hp <= 0)
		{
			p_skel->is_death = true;

			// memory clear
			//delete p_skel;
		}
		else 
		{
			p_skel->hp -= 20;
			is_hp_skel_decrease = false;
			is_no_skel_crash = true;
		}
	}

	// 라이프를 다 소모하면, 게임 종료
	if (p_player->lifes == 0)
	{
		p_player->is_death = true;
		delete p_player;

		MessageBox(hWnd, Game_over, L"게임종료", MB_OK);
		InvalidateRect(hWnd, NULL, false);
		exit(1);
	}


	if (is_hp_decrease == true && is_no_crash == false)
	{
		if (p_player->hp <= 0)
		{
			p_player->lifes -= 1;

			// 플레이어 위치 재 지정
			s_player.x = 16;
			s_player.y = 336;
			s_player.width = 30;
			s_player.height = 30;

			// 플레이어의 HP 재 지정
			p_player->hp = 100 + 3;
			s_player.plus_speed = 0;

			// 원 상태 복귀
			is_hp_decrease = false;
			is_no_crash = true;
		}
		p_player->hp -= 3;
	}
	InvalidateRect(hWnd, NULL, false);
}

void Player_Area_Limit()
{
	// 플레이어가 그라운드를 못 벗어나게 하는 연산
	// Left
	if (ground.x > s_player.x - s_player.width / 2) // 이렇게 해주면 정확하게 사각형의 왼쪽 면의 좌표로 계산
	{
		s_player.x = 16;
	}
	// bottom
	if (floors.y < s_player.y + s_player.height / 2)
	{
		s_player.y = 336;
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
	if (!InitInstance(hInstance, nCmdShow))
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

	return (int)msg.wParam;
}

//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BITMAP));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BITMAP);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		// Player x,y / Size
		s_player.x = 50;
		s_player.y = 336;
		s_player.width = 30;
		s_player.height = 30;
		v_player.push_back(s_player);

		// Skeleton x,y / Size / damage
		skel.x = 200;
		skel.y = 269;
		skel.width = 30;
		skel.height = 30;
		skel.damage = 10;

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
		speed_item.width = -220;
		speed_item.height = -220;
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
		WCHAR player_hp[128];
		WCHAR ball[128];
		WCHAR buf[128];

		wsprintf(buf, L"bullet->x = %d", (int)bullet->x);

		/******** Bitmap ********/

		// Background Bitmap
		bitmap1 = (HBITMAP)LoadImage(NULL, L"b1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc1 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_1 = (HBITMAP)SelectObject(bitmap_dc1, bitmap1);
		DeleteObject(old_bitmap_1);

		bitmap2 = (HBITMAP)LoadImage(NULL, L"b2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc2 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_2 = (HBITMAP)SelectObject(bitmap_dc2, bitmap2);
		DeleteObject(old_bitmap_2);

		bitmap3 = (HBITMAP)LoadImage(NULL, L"b3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc3 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_3 = (HBITMAP)SelectObject(bitmap_dc3, bitmap3);
		DeleteObject(old_bitmap_3);

		bitmap4 = (HBITMAP)LoadImage(NULL, L"b4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc4 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_4 = (HBITMAP)SelectObject(bitmap_dc4, bitmap4);
		DeleteObject(old_bitmap_4);

		bitmap5 = (HBITMAP)LoadImage(NULL, L"b5.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc5 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_5 = (HBITMAP)SelectObject(bitmap_dc5, bitmap5);
		DeleteObject(old_bitmap_5);

		// Player = basic_standing.bmp - 6
		bitmap6 = (HBITMAP)LoadImage(NULL, L"basic_standing.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc6 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_6 = (HBITMAP)SelectObject(bitmap_dc6, bitmap6);
		DeleteObject(old_bitmap_6);

		// speed_item.bmp - 7
		bitmap7 = (HBITMAP)LoadImage(NULL, L"speed_item.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc7 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_7 = (HBITMAP)SelectObject(bitmap_dc7, bitmap7);
		DeleteObject(old_bitmap_7);

		// skeleton1.bmp - 8
		bitmap8 = (HBITMAP)LoadImage(NULL, L"Skeleton1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc8 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_8 = (HBITMAP)SelectObject(bitmap_dc8, bitmap8);
		DeleteObject(old_bitmap_8);

		// attack.bmp - 9
		bitmap9 = (HBITMAP)LoadImage(NULL, L"bullet.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc9 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_9 = (HBITMAP)SelectObject(bitmap_dc9, bitmap9);
		DeleteObject(old_bitmap_9);

		// 비트맵에 대한 정보를 담는다.
		GetObject(bitmap1, sizeof(BITMAP), &bitmap_info); // Background
		GetObject(bitmap6, sizeof(BITMAP), &bitmap_info2); // Player
		GetObject(bitmap7, sizeof(BITMAP), &Speed_item_info); // Speed_item
		GetObject(bitmap8, sizeof(BITMAP), &skeleton_info); // Skeleton
		GetObject(bitmap9, sizeof(BITMAP), &bullet_info); // Attack

		// Background 비트맵의 가로,세로 사이즈
		LONG width = bitmap_info.bmWidth;
		LONG height = bitmap_info.bmHeight;

		// Player 비트맵의 가로, 세로 사이즈
		LONG player_width = bitmap_info2.bmWidth;
		LONG player_height = bitmap_info2.bmHeight;

		// Speed_item 비트맵의 가로, 세로 사이즈
		LONG Speed_item_width = Speed_item_info.bmWidth;
		LONG Speed_item_height = Speed_item_info.bmHeight;

		// Skeleton 비트맵의 가로, 세로 사이즈
		LONG skel_width = skeleton_info.bmWidth;
		LONG skel_height = skeleton_info.bmHeight;

		// attack 비트맵의 가로, 세로 사이즈
		LONG bullet_width = bullet_info.bmWidth;
		LONG bullet_height = bullet_info.bmHeight;

		if (g_press == true)
		{
			// 바닥의 y 좌표보다 player의 y 좌표가 더 높을 때 Thread 실행
			if (floors.y > s_player.y + s_player.height / 2)
			{
				CreateThread(NULL, 0, gravity, hWnd, 0, NULL);
			}

			//Rectangle(hdc, ground.x, ground.y, ground.width, ground.height);

			// Background Bitmap
			TransparentBlt(hdc, 0, 0, 700, 400, bitmap_dc1, 0, 0, width, height, RGB(255, 0, 255));
			TransparentBlt(hdc, 0, 0, 700, 400, bitmap_dc2, 0, 0, width, height, RGB(255, 255, 255));
			TransparentBlt(hdc, 0, 0, 700, 400, bitmap_dc3, 0, 0, width, height, RGB(255, 255, 255));
			TransparentBlt(hdc, 0, 0, 700, 400, bitmap_dc4, 0, 0, width, height, RGB(255, 255, 255));
			TransparentBlt(hdc, 0, 0, 700, 400, bitmap_dc5, 0, 0, width, height, RGB(255, 255, 255));

			// Skeleton Bitmap
			if (p_skel->is_death == false)
			{
				TransparentBlt(hdc, skel.x - skel.width / 2, skel.y - skel.height / 2 + 19, skel_width + 18, skel_height + 15, bitmap_dc8, 0, 0, skel_width, skel_height, RGB(255, 0, 255));
			}

			// Player Bitmap
			if (p_player->is_death == false)
			{
				TransparentBlt(hdc, s_player.x - s_player.width / 2 - 22, s_player.y - s_player.height / 2 - 32, player_width + 30, player_height + 25, bitmap_dc6, 0, 0, player_width, player_height, RGB(255, 0, 255));
			}

			// 아이템이 Player와 충돌 할 경우 그리지 않는다.
			if (is_item_die == false)
			{
				// Speed_item Bitmap
				TransparentBlt(hdc, speed_item.x, speed_item.y, Speed_item_width + 30, Speed_item_height + 30, bitmap_dc7, 0, 0, Speed_item_width, Speed_item_height, RGB(255, 0, 255));
				InvalidateRect(hWnd, NULL, false);
			}

			// Floors
			Rectangle(hdc, floors.x, floors.y, floors.width, floors.y + floors.height);

			// Fix Hp_Bar config - Player
			if (p_player->is_death == false)
			{
				Rectangle(hdc, s_player.x - 45, s_player.y + 18, s_player.x + 45, s_player.y + 23); // Player - hp_bar
			}

			// Fix Hp_Bar config - Skeleton
			if (p_skel->is_death == false)
			{
				Rectangle(hdc, skel.x - 20, skel.y + 85, skel.x + 80, skel.y + 90); // Skeleton - hp_bar
			}

			// portal
			Rectangle(hdc, portal.x, portal.y, portal.x + portal.width, portal.y + portal.height);

			// Flow Hp_Bar config - skeletom, Player
			my_b = CreateSolidBrush(RGB(255, 0, 0));
			os_b = (HBRUSH)SelectObject(hdc, my_b);
			DeleteObject(os_b);

			// skeleton, Player HP Bar
			if (p_skel->is_death == false)
			{
				Rectangle(hdc, skel.x - 20, skel.y + 85, (skel.x - 20) + p_skel->hp, skel.y + 90); // Skeleton - hp_bar
			}
			if (p_player->is_death == false)
			{
				Rectangle(hdc, s_player.x - 45, s_player.y + 18, (s_player.x - 45) + p_player->hp, s_player.y + 23); // Player - hp_bar
			}
			DeleteObject(my_b);

			Player_bullet(hdc);

			TextOut(hdc, 10, 10, buf, wcslen(buf));

			//TextOut(hdc, 10, 10, speed, wcslen(speed));
			//TextOut(hdc, skel.x - skel.width / 2, 330, L"  ", lstrlen(L"  "));
			//TextOut(hdc, s_player.x - s_player.width / 2 + 30, 330, L"  ", lstrlen(L"  "));
		}

		BitBlt(memDC, 0, 0, resolution.x, resolution.y, hdc, 0, 0, SRCCOPY);

		// 비트맵 메모리 해제
		DeleteDC(bitmap_dc1);
		DeleteObject(bitmap1);
		DeleteDC(bitmap_dc2);
		DeleteObject(bitmap2);
		DeleteDC(bitmap_dc3);
		DeleteObject(bitmap3);
		DeleteDC(bitmap_dc4);
		DeleteObject(bitmap4);
		DeleteDC(bitmap_dc5);
		DeleteObject(bitmap5);
		DeleteDC(bitmap_dc6);
		DeleteObject(bitmap6);
		DeleteDC(bitmap_dc7);
		DeleteObject(bitmap7);
		DeleteDC(bitmap_dc8);
		DeleteObject(bitmap8);
		DeleteDC(bitmap_dc9);
		DeleteObject(bitmap9);

		DeleteDC(hdc);
		DeleteDC(memDC);
		DeleteObject(new_bitmap);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_LBUTTONDOWN:
	{
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);

		// Thread 종료 코드는 Thread 내부에 있다.
		bullet->x = s_player.x - s_player.width / 2 + 30;
		bullet->y = s_player.y - s_player.height / 2;

		// 클릭할 때마다 달라지는 mouse 좌표와 Player 좌표를 구해서 dx, dy 값을 적용
		dx = mouse_x - (s_player.x - s_player.width / 2 + 30); // 삼각 밑변 길이
		dy = mouse_y - (s_player.y - s_player.height / 2); // 삼각높이
		
		// 쓰레드 생성
		bullet_handle = CreateThread(NULL, 0, D_Player_bullet, hWnd, 0, &thread_id_Player_bullet);
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
