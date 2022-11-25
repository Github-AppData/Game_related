// Project1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#pragma comment(lib, "Msimg32.lib")

#include "framework.h"
#include "Project1.h"
#include <string>
#include <random>
#include <iostream>

// 메모리 누수 체크를 위한 인클루드 및 디파인
#include <stdlib.h>
#include <crtdbg.h>

#define _CRTDBG_MAP_ALLOC

#include <cassert>

using namespace std;


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd;

// 비트맵
HBITMAP bitmap; // 로드한 비트맵을 저장하는 변수
BITMAP bitmap_info; // 로드한 비트맵의 정보를 저장하는 변수
HDC bitmap_dc; // 비트맵을 메모리 DC에 저장하는 변수

// 쓰레드는 알아서 할 것
// 프로젝트를 새로 생성하세요.
// 오늘은 여기까지
struct Enemy
{
	int x;
	int y;
	int width;
	int height;
	bool is_death; // 몬스터가 죽었는가
};

vector<Enemy> enemys;

float hp; // 현재 HP
float max_hp; // 최대 HP
int width; // HP의 최대 가로 길이

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void OnPaint(HDC hdc); // 화면에 그리는 것만 수행함
void CreateEnemy(int x, int y, int width, int height);

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
	LoadStringW(hInstance, IDC_PROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT1));

	MSG msg;

	// 메모리 누수 체크를 위한 초기 설정
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // 이거 왜 안먹히지
#endif // _DEBUG

	// 콘솔 창에 뭔가 보인다면 메모리 누수

	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// 메모리 누수 체크
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif // _DEBUG


	return (int)msg.wParam;
}



//
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL; // 메뉴 사용하지 않음
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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hWnd = hWnd;

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
//

// 두 원이 충돌 했는지 안 했는지 확인하는 함수 - 반지름1, 반지름2, 원1의 중점, 원2의 중점
//BOOL circle_is_crash(int r1, int r2, int R1x, int R1y, int R2x, int R2y);


//// 플레이어
//int c1x = g_me.left + (r1 - 2); // 268
//int c1y = g_me.top + 10;// 260
//
//// 아이템
//int c2x = g_item.left + (r2 - 2); // 173
//int c2y = g_item.top + 15; // 215

// 그라운드, 플레이어, 적 ,장애물, 아이
RECT g_ground, g_me, g_you, g_ob, g_portal, g_item;

//intersectRect 확인 사각형
RECT is;

int g_x, g_y, g_i;
BOOL g_press = false;
BOOL g_complete = false;
int g_hp = 100;
int g_mp = 100;
int g_key = 0;

LPARAM g_lParam;

int return_s(int b)
{
	return b;
}

BOOL circle_is_crash(int rs, int R1x, int R1y, int R2x, int R2y)
{
	BOOL is_crash;

	double b = 0;

	// 원 1 반지름 + 원 2 반지름
	 // 45

	// 두 점 사이의 거리 구하는 공식
	b = sqrt((pow(R2x - R1x, 2)) + (pow(R2y - R1y, 2)));

	return_s(b);

	if (b >= rs) { is_crash = false; }
	else { is_crash = true; }

	return is_crash;
}



DWORD WINAPI draw(LPVOID param)
{
	// 랜덤 시드 값을 변경
	srand(time(NULL));

	int x, y, i;
	HDC hdc;
	HPEN osp, myp;
	HWND hWnd = (HWND)param;

	x = LOWORD(g_lParam);
	y = HIWORD(g_lParam);

	hdc = GetDC(hWnd);

	//while (true)
	{
		for (i = 0; i < y; i++)
		{
			myp = CreatePen(PS_SOLID, 3, RGB(rand() % 256, rand() % 256, rand() % 256));
			osp = (HPEN)SelectObject(hdc, myp);
			MoveToEx(hdc, x, 10, NULL);

			// 임계영역 진입 : OS가 관리
			//EnterCriticalSection(&g_cs);
			// 임계 영역 시작
			g_x = x;
			// 강제 문맥 교환
			Sleep(30);

			LineTo(hdc, x, i);

			// 임계 영역 끝
			//LeaveCriticalSection(&g_cs);

			SelectObject(hdc, osp);
			DeleteObject(myp);
		}
	}
	ReleaseDC(hWnd, hdc);

	return 0;
}
HANDLE g_handle[1000] = { NULL, };  // 스레드의 핸들
int g_index;
BOOL check;
BOOL check_2 = false;
HANDLE hTimer1, hTimer2;
int watch = 1000;

HINSTANCE g_hInstance;

// 색 선언부
HBRUSH MyBrush2, OldBrush2, MyBrush3, OldBrush3, MyBrush4, OldBrush4, MyBrush5, OldBrush5;
HPEN MyPen1, OldPen1, MyPen2, OldPen2, MyPen3, OldPen3, MyPen4, OldPen4, MyPen5, OldPen5;
// HP,MP 버퍼 구현
WCHAR HP[128];
WCHAR MP[128];
WCHAR buf[128];

WCHAR NEXT_STAGE_TERMS[128];
/*WCHAR loca[128];
WCHAR loca_2[128];*/

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case IDM_ABOUT:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		DWORD tid;      // 스레드 생성 후, 스레드의 ID를 반환 받는 변수

		g_lParam = lParam;

		g_handle[g_index++] = CreateThread(NULL, 0, draw, hWnd, 0, &tid);
		// 에러 처리
		if (NULL == g_handle)
		{
			MessageBox(hWnd, L"스레드 생성 실패", L"저런", MB_OK);
			break;
		}
	}
	break;

	case WM_RBUTTONDOWN:
	{
		//SuspendThread(g_handle);

		// Base Thread 일시 중단
		//SuspendThread(GetCurrentThread());

		for (int i = 0; i < 1000; i++)
			SuspendThread(g_handle[i]);

	}
	break;


	case WM_CREATE:
	{
		check = true;
		//MessageBox(hWnd, L"ssssss", L"sss", MB_OK);
		// 
		// 이 부분에서 함수 호출
		hTimer1 = (HANDLE)SetTimer(hWnd, 1, 1000, NULL);
		PostMessage(hWnd, WM_CREATE, 0, 0);


		//hTimer2 = (HANDLE)SetTimer(hWnd, 2, 1000, NULL);
		g_me.left = 250;
		g_me.top = 250;
		g_me.right = 290;
		g_me.bottom = 290;

		g_you.left = 300;
		g_you.top = 300;
		g_you.right = 340;
		g_you.bottom = 340;

		g_ground.left = 10;
		g_ground.top = 10;
		g_ground.right = 700;
		g_ground.bottom = 450;

		g_ob.left = g_ground.left + 1;
		g_ob.top = g_ground.top + 1;
		g_ob.right = 110;
		g_ob.bottom = 300;

		g_portal.left = g_ground.right - 50;
		g_portal.top = g_ground.top + 1;
		g_portal.right = g_ground.right - 1;
		g_portal.bottom = 50;

		g_item.left = 150;
		g_item.top = 200;
		g_item.right = 200;
		g_item.bottom = 250;
		// 175, 225

		/*thread = CreateThread(NULL, 0, Thread, hWnd, NULL, &thread_id);*/

		// 몬스터 생성
		// 몬스터의 시작 위치, 몬스터의 크기로 몬스터 생성 가능
		CreateEnemy(16, 16, 32, 32);
		CreateEnemy(160, 160, 32, 32);

		// hp 초기화
		hp = 100;
		max_hp = 100;
		width = 300;
	}
	break;
	case WM_TIMER:
	{
		switch (wParam) {
		case 1:
			watch -= 1;
			if (g_hp <= 0)
			{
				KillTimer(hWnd, 1);
				MessageBox(hWnd, L"그것도 못하냐 ?", L"Game Over !", MB_OK);
				exit(0);
			}
			break;
		}
	}
	break;

	case WM_PAINT:
	{

		// 와 wm_paint 호출할 때마다 변수 선언 하시게요?
		// 아니 스태틱은 안지워져요.
		// 전역으로 빼시요.static는 메모리 적제라 값 유지
		// 때문에 아닌 녀석만 외부로
		PAINTSTRUCT ps;

		/** 더블버퍼링 시작처리입니다. **/
		static HDC hdc, MemDC, tmpDC;
		static HBITMAP BackBit, oldBackBit;
		static RECT bufferRT;
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &bufferRT);
		MemDC = CreateCompatibleDC(hdc);
		BackBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
		oldBackBit = (HBITMAP)SelectObject(MemDC, BackBit);
		PatBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
		tmpDC = hdc;
		hdc = MemDC;
		MemDC = tmpDC;

		OnPaint(hdc);

		// 임시
		Rectangle(hdc, 0, 0, (hp / max_hp) * width, 15);

		// Double buffering
		/** 더블버퍼링 끝처리 입니다. **/
		tmpDC = hdc;
		hdc = MemDC;
		MemDC = tmpDC;
		GetClientRect(hWnd, &bufferRT);
		BitBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, MemDC, 0, 0, SRCCOPY);
		SelectObject(MemDC, oldBackBit);
		DeleteObject(BackBit);
		DeleteDC(MemDC);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case 0x70: // F1 Key
			// 멈춘 쓰레드 재 실행
			for (int i = 0; i < 1000; i++) { ResumeThread(g_handle[i]); }
			break;

		case 0x20:  // 스페이스 바
			g_press = true;
			break;

		case 0x41: // A
			g_me.left -= 10;
			g_me.right -= 10;
			break;

		case 0x44: // D
			g_me.left += 10;
			g_me.right += 10;
			break;

		case 0x53: // S
			g_me.top += 10;
			g_me.bottom += 10;
			break;

		case 0x57: // W
			g_me.top -= 10;
			g_me.bottom -= 10;
			break;

		case 'N':
			if (hp > 0)
			{
				hp -= 10;
			}
			break;

		case 'M':
			if (hp <= max_hp)
			{
				hp += 10;
			}
			break;
		}

		// g_me가 g_ground를 못 벗어나게 하는 연산
		if (g_ground.left > g_me.left)
		{
			g_me.left = 10 + 1;
			g_me.right = 50;
		}
		if (g_ground.top > g_me.top)
		{
			g_me.top = 10 + 1;
			g_me.bottom = 50;
		}
		if (g_ground.right < g_me.right)
		{
			g_me.right = 700 - 1;
			g_me.left = 660;
		}
		if (g_ground.bottom < g_me.bottom)
		{
			g_me.bottom = 450 - 1;
			g_me.top = 410;
		}

		InvalidateRect(hWnd, NULL, false);
	}
	break;
	case WM_DESTROY:
	{
		KillTimer(hWnd, 1);
		//KillTimer(hWnd, 2);
		PostQuitMessage(0);
		break;
	default: 
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
	}
}

void OnPaint(HDC hdc)
{
	// 가능한 외부로 빼는게 좋음
	bitmap = (HBITMAP)LoadImage(NULL, L"1_stage_forest.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	//assert(bitmap);
	bitmap_dc = CreateCompatibleDC(hdc);
	HBITMAP old_bitmap = (HBITMAP)SelectObject(bitmap_dc, bitmap);
	DeleteObject(old_bitmap);

	// 비트맵에 대한 정보를 담음(이미지의 가로, 세로 사이즈 등)
	GetObject(bitmap, sizeof(BITMAP), &bitmap_info);


	// 비트맵의 가로 사이즈
	LONG width = bitmap_info.bmWidth;

	// 비트맵의 세로 사이즈
	LONG height = bitmap_info.bmHeight;

	// RGB : 255, 0, 255 인 색을 제거함
	// hdc에다가 bitmap_dc에 그려진 비트맵을 그림
	// HDC, x 좌표, y 좌표, 가로 크기, 세로 크기, 메모리 DC, 비트맵 내 x 좌표, y 좌표, 비트맵 내 가로 크기, 세로 크기, 제거할 배경의 색
	TransparentBlt(hdc, 0, 0, 1400, 800, bitmap_dc, 0, 0, width, height, RGB(255, 0, 255));

	if (check_2 == true)
	{
		TextOut(hdc, 300, 300, L"상점", lstrlen(L"상점"));
	}


	// g_hp, g_mp, g_key - HP, MP 바뀔 수 있도록 버퍼 안에다가 int 자료형 구현
	wsprintf(HP, L"HP [ %d / 100 ]", g_hp);
	wsprintf(MP, L"MP [ %d / 100 ]", g_mp);
	wsprintf(NEXT_STAGE_TERMS, L"KEY [ %d / 2 ]", g_key);
	wsprintf(buf, L"Time [ %d 초 ]", watch);

	/*wsprintf(loca, L"[ g_me.left = %d / g_me.top = %d / g_me.bottom = %d ]", g_me.left, g_me.top, g_me.bottom);
	wsprintf(loca_2, L"[ g_item.left = %d / g_item.top = %d / g_item.bottom = %d ]", g_item.left, g_item.top, g_item.bottom);*/

	// Game Start
	if (check == true)
	{
		TextOut(hdc, 300, 300, L"Click ! Space bar !", lstrlen(L"Click ! Space bar !"));

	}

	// if → 특정 버튼을 누르면 시작하도록 space bar
	if (g_press == true)
	{
		check_2 = true;

		// 그라운드, 장애물, 플레이어, 적

		Rectangle(hdc, g_portal.left, g_portal.top, g_portal.right, g_portal.bottom);


		// 1 - O
		MyPen1 = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		OldPen1 = (HPEN)SelectObject(hdc, MyPen1);
		DeleteObject(OldPen1);
		Rectangle(hdc, g_ground.left, g_ground.top, g_ground.right, g_ground.bottom);
		DeleteObject(MyPen1);

		// PROBLEM : 몬스터에 Pen이 적용되는 것으로 보아 제대로 해제되고 있지 않은 걸로 보임
		// 다시 만드는 게 빠를 것으로 보임

		// 임시
		// 몬스터 수 많큼 그려줌
		for (size_t i = 0; i < enemys.size(); i++)
		{
			// 추후 몬스터 삭제 시에 백터에서 제거 및 쓰레드 종료 처리 할것
			// 죽은 몬스터가 아닌 경우 화면에 그림
			if (!enemys[i].is_death)
			{
				Rectangle(hdc, enemys[i].x - enemys[i].width / 2, enemys[i].y - enemys[i].height / 2, enemys[i].x + enemys[i].width / 2, enemys[i].y + enemys[i].height / 2);
			}
		}

		// 2 - O
		MyPen2 = CreatePen(PS_SOLID, 1, RGB(0, 77, 255));
		OldPen2 = (HPEN)SelectObject(hdc, MyPen2);
		DeleteObject(OldPen2);

		MyBrush2 = CreateSolidBrush(RGB(0, 77, 255));
		OldBrush2 = (HBRUSH)SelectObject(hdc, MyBrush2);
		DeleteObject(OldBrush2);

		Ellipse(hdc, g_item.left, g_item.top, g_item.right, g_item.bottom);
		DeleteObject(MyBrush2);
		DeleteObject(MyPen2);

		// 3 - O
		MyPen3 = CreatePen(PS_SOLID, 1, RGB(121, 126, 137));
		OldPen3 = (HPEN)SelectObject(hdc, MyPen3);
		DeleteObject(OldPen3);

		MyBrush3 = CreateSolidBrush(RGB(121, 126, 137));
		OldBrush3 = (HBRUSH)SelectObject(hdc, MyBrush3);
		DeleteObject(OldBrush3);

		Rectangle(hdc, g_ob.left, g_ob.top, g_ob.right, g_ob.bottom);

		DeleteObject(MyBrush3);
		DeleteObject(MyPen3);

		// 4 - O
		MyPen4 = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		OldPen4 = (HPEN)SelectObject(hdc, MyPen4);
		DeleteObject(OldPen4);

		MyBrush4 = CreateSolidBrush(RGB(0, 255, 0));
		OldBrush4 = (HBRUSH)SelectObject(hdc, MyBrush4);
		DeleteObject(OldBrush4);

		Ellipse(hdc, g_me.left, g_me.top, g_me.right, g_me.bottom);

		DeleteObject(MyBrush4);
		DeleteObject(MyPen4);

		// 5
		MyPen5 = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		OldPen5 = (HPEN)SelectObject(hdc, MyPen5);

		DeleteObject(OldPen5);

		MyBrush5 = CreateSolidBrush(RGB(255, 0, 0));
		OldBrush5 = (HBRUSH)SelectObject(hdc, MyBrush5);

		DeleteObject(OldBrush5);

		Rectangle(hdc, g_you.left, g_you.top, g_you.right, g_you.bottom);
		DeleteObject(MyPen5);
		DeleteObject(MyBrush5);


		// 오브젝트가 누구인지
		TextOut(hdc, g_ob.right - 70, g_ob.bottom - 60, L"먹구름", 3);
		TextOut(hdc, g_me.left + 10, g_me.bottom - 25, L"나", 1);

		//TextOut(hdc, g_me.left + 18, g_me.top + 10, L".", 1); // g_me : 268, 260
		//TextOut(hdc, g_item.left + 23, g_item.top + 15, L".", 1); // g_item : 173, 215

		/*TextOut(hdc, g_me.right, g_me.top + 2, loca, wcslen(loca));
		TextOut(hdc, g_me.right, g_me.top + 20, loca_2, wcslen(loca_2));*/


		// Portal, NEXT_STAGE_TERMS 구현 => 다음 스테이지 넘어갈 수 있는 조건
		TextOut(hdc, g_portal.right - 40, g_portal.bottom - 30, L"포탈", 2);
		TextOut(hdc, g_portal.right - 70, g_portal.bottom + 5, NEXT_STAGE_TERMS, wcslen(NEXT_STAGE_TERMS));

		// 적
		TextOut(hdc, g_you.left + 10, g_you.bottom - 30, L"적", 1);

		// 글씨 폰트 수정
		SetTextColor(hdc, RGB(147, 137, 242));
		TextOut(hdc, g_ground.right - 100, 460, HP, wcslen(HP));
		TextOut(hdc, g_ground.right - 100, 475, MP, wcslen(MP));

		SetTextColor(hdc, RGB(255, 0, 0));
		TextOut(hdc, 10, 460, buf, wcslen(buf));

		if (true == IntersectRect(&is, &g_me, &g_portal))
		{
			if (g_key == 2)
			{
				g_press = false;
				check = false;
				check_2 = true;
			}
			else
			{
				MessageBox(g_hWnd, L"스테이지를 클리어 하지 못 하였습니다.", L"NO !", MB_OK);
			}
		}

		// 적 - 공격 구현예정 (쓰레드로) = 적의 공격에 맞으면 체력 감소 구현예정
		if (true == IntersectRect(&is, &g_me, &g_you))
		{
			g_hp -= 10;
		}

		// 먹구름에 닿을 때마다 체력감소 구현
		if (true == IntersectRect(&is, &g_me, &g_ob))
		{
			g_hp -= 20;
		}
		int r1 = (g_me.right - g_me.left) / 2; // 20
		int r2 = (g_item.right - g_item.left) / 2; // 25
		int rs = r1 + r2;

		// 플레이어
		int c1x = g_me.left + (r1 - 2); // 268
		int c1y = g_me.top + 10;// 260

		// 아이템
		int c2x = g_item.left + (r2 - 2); // 173
		int c2y = g_item.top + 15; // 215

		if (g_me.bottom > g_item.top && g_me.top < g_item.bottom)
		{
			if (true == circle_is_crash(rs, c1x, c2y, c2x, c2y))
			{

				// 시드값을 얻기 위한 random_device 생성.
				std::random_device rd;

				// random_device 를 통해 난수 생성 엔진을 초기화 한다.
				std::mt19937 gen(rd());

				// ground bottom = 450; ground top = 10;


					/*g_ground.left = 10;
					g_ground.top = 10;
					g_ground.right = 700;
					g_ground.bottom = 450;

					g_item.left = 150;
					g_item.top = 200;
					g_item.right = 200;
					g_item.bottom = 250;*/

				uniform_int_distribution<int> left(111, 599);
				uniform_int_distribution<int> top(10, 400);

				g_item.left = left(gen);

				g_item.top = top(gen);
				g_item.right = g_item.left + 50;
				g_item.bottom = g_item.top + 50;
				g_key++;

			}
		}
		/*else if (g_me.top < g_item.bottom)
		{
			if (true == circle_is_crash(rs, c1x, c2y, c2x, c2y))
			{
				MessageBox(hWnd, L"키를 획득하였습니다.", L"Clear !", MB_OK);
				g_key++;
			}
		}*/

	}

	// 비트맵 메모리 해제
	DeleteDC(bitmap_dc);
	DeleteObject(bitmap);
}

void CreateEnemy(int x, int y, int width, int height)
{
	Enemy enemy;

	enemy.x = x;
	enemy.y = y;
	enemy.width = width;
	enemy.height = height;
	enemy.is_death = false;

	// 숙제
	// 여기서 해당 몬스터의 쓰레드를 생성
	// 해당 쓰레드에서 몬스터 제어
	// 즉 각 몬스터에게 쓰레드를 각각 부여

	enemys.push_back(enemy);
}

