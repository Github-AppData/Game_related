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
using namespace std;

int i = 0;





// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

WCHAR Game_over[128] = { 0, };

// 전역 선언부
POINT resolution; // 해상도를 저장할 변수
RECT client_area; // 해상도를 토대로 클라이언트 영역을 저장할 변수

BOOL g_press = false; // 스페이스 바 누름 여부 확인 - 게임 시작
BOOL g_portal_crash = false; // 포탈 충돌 여부 확인
BOOL g_Key_D = false; // 

HANDLE handle; // Thread 핸들
BOOL is_item_die = false; // 아이템을 먹었는지 확인 유무
int g_jump_limit = 0; // 점프 제한
int speed_plus = 0; // 
BOOL is_hp_decrease = false; // hp 감소
BOOL is_hp_skel_decrease = false; // skeleton hp 감소
int counts = 0; // Player가 Skeleton에 맞는 횟수 - 유효하는 횟수 
float dx, dy;
float degree;
BOOL is_no_crash = false; // 플레이어가 움직일 때는 


// HP 
// 브러시 자료형 선언
HBRUSH os_b, my_b;


BOOL is = false;
BOOL is_thread = false;

// Thread
HANDLE thread;
DWORD tid;

// 비트맵
HBITMAP bitmap, bitmap1, bitmap2, bitmap3, bitmap4, bitmap5, bitmap6, bitmap7, bitmap8, bitmap9, bitmap10; // 로드한 비트맵을 저장하는 변수
BITMAP bitmap_info, bitmap_info2, Speed_item_info, skeleton_info, attack_info; // 로드한 비트맵의 정보를 저장하는 변수
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
	BOOL is_death = false; // 완전히 죽는 것
	int lifes = 3; // 라이프 수를 의미한다.

	// hp
	int hp = 100;

	// item
	int plus_speed = 0;
};


//struct S_Player_Attack
//{
//    int x = s_player.x - s_player.width / 2 + 30;
//    int y = s_player.y + s_player.height / 2 - 30;
//    int max_range = 0;
//    int damage = 0;
//    int speed = 0;
//};

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

struct SPlayerAttack
{
	int x = 0; // 좌표
	int y = 0; // 좌표
	int max_range = 0; // 사정거리 - V 값으로 대입 될 예정
};

skeleton_area_limit skel_limit;
SPlayerAttack s_Attack;
SPlayerAttack* p = new SPlayerAttack();
vector<SPlayerAttack> player_attack; // 플레이어 어택 비트맵

/* ---- The End Struct ---- */

// Player - struct, vector 선언
Player s_player;
Player* p_player = new Player();
vector<Player> v_player;

// Vector iterator 선언
vector<Player>::iterator iter = v_player.begin();
int a, b;

// struct 선언
Ground ground;
Floor floors;
//S_Player_Attack player_attack;
Portal portal;
Speed_item speed_item;
HWND hWnd;
Skeleton skel;
Skeleton* p_skel = new Skeleton();

// ball bitmap
void attack_bitmap(HDC hdc)
{
	TransparentBlt(hdc, p->x, p->y, 11, 10, bitmap_dc9, 0, 0, 11, 10, RGB(255, 0, 255));
}

HDC hdc;
// Player 공격 
DWORD WINAPI D_Player_Attack(LPVOID param)
{
	// 쓰레드가 발동되면 무한루프
	hdc = GetDC(hWnd);
	while (true)
	{
		// Thread 잠시멈춤
		Sleep(50);
		attack_bitmap(hdc);
		p->y += dy * 50.f;
		p->x += dx * 50.f;

		// 이게 문제다
		if (p->y > 270)
		{
			if (p->x < skel.x - skel.width / 2 && p->x > skel.x - 20)
			{
				is_hp_skel_decrease = true;
			}
		}
		if (p->y > 270)
		{
			if (p->x > skel.x - skel.width / 2 && p->x < skel.x - skel.width / 2 + 36)
			{
				is_hp_skel_decrease = true;
			}
		}

		// 화면 무효화
		InvalidateRect(hWnd, NULL, false);
	}
	return 0;
}

//DWORD WINAPI skeleton_auto_move(LPVOID param)
//{
//    while (true)
//    {
//		// Thread 잠시 멈춤
//        Sleep(100);
//		
//		// 스켈레톤의 이동 영역제한
//		//clamp(p_skel->x, 200, 600);
//
//		// 왼쪽이동 조건문
//		if (p_skel->x > skel_limit.left_limit)
//		{
//			p_skel->x -= 10;
//		}
//
//		// 오른 쪽 이동 조건문
//		if (p_skel->x < skel_limit.right_limit)
//		{
//			p_skel->x += 10;
//		}
//
//		InvalidateRect(hWnd, NULL, false);
//
//    }
//	return 0;
//}


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
void move_p()
{
	p->x = s_player.x - s_player.width / 2;
	p->y = s_player.y - s_player.height / 2;
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
		move_p();

		s_player.x -= 5;
		s_player.x -= s_player.plus_speed;
		break;
	case 'D':
		if (is_thread == true)
		{
			break;
		}
		move_p();
		s_player.x += 5;
		s_player.x += s_player.plus_speed;
		break;
	case 'W':
		if (is_thread == true)
		{
			break;
		}
		move_p();
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
		move_p();
		s_player.y += 10;
		break;
	}
}

void is_crash()
{
	// 플레이어가 충돌 여부 체크
	if (s_player.x - s_player.width / 2 > skel.x - skel.width / 2 || s_player.x - s_player.width / 2 - 55 < skel.x - skel.width / 2)
	{
		// 부딪히지 않음.
		is_no_crash = true;
	}
	else {
		is_no_crash = false;
	}

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

		// 화면 무효화
		InvalidateRect(hWnd, NULL, false);
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

	if (is_hp_skel_decrease == true)
	{
		if (p_skel->hp <= 0)
		{
			p_skel->is_death = true;
		}
		p_skel->hp -= 10;
	}

	// Skeleton - 부딪히면, hp 감소
	if (is_hp_decrease == true && is_no_crash == true)
	{
		// HP가 0 보다 적으면
		if (p_player->hp <= 0)
		{
			p_player->lifes -= 1;

			// 총알 위치 재 지정
			p->x = s_player.x - s_player.width / 2;
			p->y = s_player.y - s_player.height / 2;

			// 플레이어 위치 재 지정
			s_player.x = 16;
			s_player.y = 336;
			s_player.width = 30;
			s_player.height = 30;

			// 위치좌표 벡터에 넣어주기
			v_player.push_back(s_player);

			// 플레이어의 HP 재 지정
			p_player->hp = 100 + 3;
			s_player.plus_speed = 0;
		}

		wsprintf(Game_over, L"라이프를 다 소모하셨습니다.");

		// 라이프를 다 소모하면, 게임 종료
		if (p_player->lifes == 0)
		{
			p_player->is_death = true;
			MessageBox(hWnd, Game_over, L"게임종료", MB_OK);
			exit(1);
		}
		// 위의 두 if문이 성립이 되지 않을 때 실행
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
//



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		// Player x,y / Size
		s_player.x = 400;
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
		//CreateThread(NULL, 0, skeleton_auto_move, hWnd, 0, NULL);
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
		// 플레이어가 스켈레톤을 지나치면 hp 감소 적용 X
		if (skel.x - skel.width / 2 + 25 > s_player.x + s_player.width / 2)
		{
			is_hp_decrease = false;
		}
		Player_Area_Limit(); // 플레이어 영역 제한
		is_crash();



		// 공이 skeleton을 지나피면, hp 감소 적용 X


		// 라이프 - Retry Chance
		// 포션 존재 함 - 상점에서 구매 가능 - int lifes = 3; - if(lifes == 0) { is_real_death = true };
		// 충돌 조건 충족
		if (s_player.x - s_player.width / 2 > skel.x - skel.width / 2 - 28)
		{
			// 플레이어가 스켈레톤을 지나치면 hp 감소 적용 X
			if (skel.x - skel.width / 2 + 68 < s_player.x + s_player.width / 2 - 30)
			{
				is_hp_decrease = false;
				break;
			}
			is_hp_decrease = true;
		}

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
		WCHAR player_hp[128];
		WCHAR ball[128];

		wsprintf(speed, L"player speed = %d", s_player.plus_speed);
		wsprintf(ball, L"ball x = %d | ball y = %d", p->x, p->y);
		wsprintf(player_hp, L"player_hp = %d", p_player->hp);

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

		// skeleton1.bmp - 8
		bitmap8 = (HBITMAP)LoadImage(NULL, L"Skeleton1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc8 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_8 = (HBITMAP)SelectObject(bitmap_dc8, bitmap8);
		DeleteObject(old_bitmap_8);

		// attack.bmp - 9
		bitmap9 = (HBITMAP)LoadImage(NULL, L"attack.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bitmap_dc9 = CreateCompatibleDC(hdc);
		HBITMAP old_bitmap_9 = (HBITMAP)SelectObject(bitmap_dc9, bitmap9);
		DeleteObject(old_bitmap_9);

		// 비트맵에 대한 정보를 담는다.
		GetObject(bitmap1, sizeof(BITMAP), &bitmap_info); // Background
		GetObject(bitmap6, sizeof(BITMAP), &bitmap_info2); // Player
		GetObject(bitmap7, sizeof(BITMAP), &Speed_item_info); // Speed_item
		GetObject(bitmap8, sizeof(BITMAP), &skeleton_info); // Skeleton
		GetObject(bitmap9, sizeof(BITMAP), &attack_info); // Attack

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

		// Skeleton 비트맵의 가로 사이즈
		LONG skel_width = skeleton_info.bmWidth;

		// Skeleton 비트맵의 세로 사이즈 
		LONG skel_height = skeleton_info.bmHeight;

		// attack 비트맵의 가로 사이즈
		LONG attack_width = attack_info.bmWidth;

		// attack 비트맵의 세로 사이즈 
		LONG attack_height = attack_info.bmHeight;

		if (g_press == true)
		{
			// 이 쓰레드가 항상 돌게 하고 싶은데 while (true) 쓰레드에다가 적용 했구요
			// WM_Paint에서 설정 하는 거 아닌가요
			// WM_PAINT 호출 될때 마다 쓰레드 무한 생성 버그

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

			// Skeleton Bitmap
			if (p_skel->is_death == false)
			{
				TransparentBlt(hdc, skel.x - skel.width / 2, skel.y - skel.height / 2 + 19, skel_width + 18, skel_height + 15, bitmap_dc8, 0, 0, skel_width, skel_height, RGB(255, 0, 255));
			}
			if (p_skel->is_death == true)
			{
				skel.x = 0;
				skel.y = 0;
				skel.width = 0;
				skel.height = 0;
			}

			// Player Bitmap, attack bitmap
			if (p_player->is_death == false)
			{
				TransparentBlt(hdc, s_player.x - s_player.width / 2 - 22, s_player.y - s_player.height / 2 - 32, player_width + 30, player_height + 25, bitmap_dc6, 0, 0, player_width, player_height, RGB(255, 0, 255));
			}
			TransparentBlt(hdc, p->x, p->y, attack_width, attack_height, bitmap_dc9, 0, 0, attack_width, attack_height, RGB(255, 0, 255));

			//attack_bitmap(hdc, s_player, attack_width + 30, attack_height + 30);


			// 아이템이 Player와 충돌 할 경우 그리지 않는다.
			if (is_item_die == false)
			{
				// Speed_item Bitmap
				TransparentBlt(hdc, speed_item.x, speed_item.y, Speed_item_width + 30, Speed_item_height + 30, bitmap_dc7, 0, 0, Speed_item_width, Speed_item_height, RGB(255, 255, 255));
				InvalidateRect(hWnd, NULL, false);
			}

			// Floors, HP_BAR, Portal Rectangle - Bitmap 으로 전환 예정
			Rectangle(hdc, floors.x, floors.y, floors.width, floors.y + floors.height);


			// Player HP Fix bar
			if (p_player->is_death == false)
			{
				Rectangle(hdc, s_player.x - 45, s_player.y + 18, s_player.x + 45, s_player.y + 23); // Player - hp_bar
			}

			// Skeleton HP Fix Bar
			if (p_skel->is_death == false)
			{
				Rectangle(hdc, skel.x - 20, skel.y + 85, skel.x + 80, skel.y + 90); // Skeleton - hp_bar
			}

			Rectangle(hdc, portal.x, portal.y, portal.x + portal.width, portal.y + portal.height);

			// hp_bar config - skeletom, Player
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

			TextOut(hdc, 10, 10, speed, wcslen(speed));
			TextOut(hdc, 10, 30, ball, wcslen(ball));

			//TextOut(hdc, 10, 30, player_hp, wcslen(player_hp));

			// 공격이 나가는 좌표 : s_player.x - s_player.width / 2 + 30, s_player.y + s_player.height / 2 - 30

			// 공격 구현 하는데 필요한 거

			// 조준점 시험 Textout
			//TextOut(hdc, s_player.x - s_player.width / 2 + 30, s_player.y + s_player.height / 2 - 30, L"3", lstrlen(L"3"));
			//TextOut(hdc, 10, 30, player_hp, wcslen(player_hp));

			//TextOut(hdc, 10, 10, player, wcslen(player));
			//TextOut(hdc, 10, 30, ground, wcslen(ground));

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
		int mouse_x, mouse_y;

		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);

		// p - 공격이 날아가는 방향
		// 식 - 역 탄젠트의 세터 값 = atan2(dy, dx); 

		dx = mouse_x - s_player.x; // Vx의 값을 구하기 
		dy = mouse_y - s_player.y; // Vy의 값을 구하기

		float m = sqrt(pow(dx, 2) + pow(dy, 2));

		if (m > 0)
		{
			// 정규화 - 그대로 값을 넣으면 700x400 화면에 나타나지 않기 때문에 정규화를 시킨다.
			dx /= m;
			dy /= m;
		}

		// 쓰레드 실행
		handle = CreateThread(NULL, 0, D_Player_Attack, hWnd, 0, NULL);
	}
	break;
	case WM_LBUTTONUP:
	{
		is_thread = false;
		// 쓰레드 일시정지
		p->max_range = 0;

		SuspendThread(handle);
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
