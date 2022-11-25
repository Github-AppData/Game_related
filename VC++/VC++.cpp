// VC++_Project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#pragma comment(lib, "msimg32.lib")


#include "framework.h"
#include "VC++.h"
#include "resource.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 전역 선언부
POINT resolution; // 해상도를 저장할 변수
RECT client_area; // 해상도를 토대로 클라이언트 영역을 저장할 변수

// 비트맵
HBITMAP bitmap; // 로드한 비트맵을 저장하는 변수
BITMAP bitmap_info; // 로드한 비트맵의 정보를 저장하는 변수
HDC bitmap_dc; // 비트맵을 메모리 DC에 저장하는 변수

/* ------- Struct ------- */
struct Player
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};

struct Ground
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};
/* ---- The End Struct ---- */

// struct 객체 생성
Player player;
Ground ground;


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void OnPaint(HDC hdc);


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
	LoadStringW(hInstance, IDC_VC, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VC));

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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VC));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VC);
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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
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
		// 플레이어 - 초기 위치 (x, y), 크기 (width, height)
		player.x = 100;
		player.y = 100;
		player.width = 30;
		player.height = 30;

		// 그라운드 사이즈 지정
		ground.x = 0;
		ground.y = -1;
		ground.width = 700;
		ground.height = 400;
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
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case 'A':
			player.x -= 10;
			break;
		case 'D':
			player.x += 10;
			break;
		case 'W':
			player.y -= 10;
			break;
		case 'S':
			player.y += 210;
			break;
		}

		// 플레이어가 그라운드를 못 벗어나게 하는 연산
		// Left
		if (ground.x > player.x - player.width / 2) // 이렇게 해주면 정확하게 사각형의 왼쪽 면의 좌표로 계산
		{
			player.x = 16;
		}
		// Bottom
		if (ground.height < player.y + player.height / 2)
		{
			player.y = 400 - 16;
		}
		// Right
		if (ground.width < player.x + player.width / 2)
		{
			player.x = 700 - 16;
		}
		// Top
		if (ground.y > player.y - player.height / 2)
		{
			player.y = 16;
		}

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

		/** 더블버퍼링 시작처리입니다. **/
		static HDC hdc, MemDC, tmpDC;
		static HBITMAP BackBit, oldBackBit;
		static RECT bufferRT;

		hdc = BeginPaint(hWnd, &ps);

		OnPaint(hdc);
		//MessageBox(hWnd, L"sss", L"ss", MB_OK);


		/** 더블버퍼링 끝 처리 **/
		GetClientRect(hWnd, &bufferRT);
		MemDC = CreateCompatibleDC(hdc);
		BackBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
		oldBackBit = (HBITMAP)SelectObject(MemDC, BackBit);
		PatBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
		tmpDC = hdc;
		hdc = MemDC;
		MemDC = tmpDC;
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

void OnPaint(HDC hdc)
{
	// 가능한 외부로 빼는게 좋음
	bitmap = (HBITMAP)LoadImage(NULL, L"asdf.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
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
	TransparentBlt(hdc, 0, 0, 300, 300, bitmap_dc, 0, 0, width, height, RGB(255, 0, 255));

	// 편의성을 위해 중심을 기준으로 이동
	Rectangle(hdc, ground.x, ground.y, ground.width, ground.height);
	Rectangle(hdc, player.x - player.width / 2, player.y - player.height / 2, player.x + player.width / 2, player.y + player.height / 2);

	// 비트맵 메모리 해제
	DeleteDC(bitmap_dc);
	DeleteObject(bitmap);
}
