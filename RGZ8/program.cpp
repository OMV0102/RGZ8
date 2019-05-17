#include <windows.h>
#include <string>

using namespace std;

LPCSTR szClassName = "Control Source";		// ��� ������ �����
LPCSTR szTitle = "������ ������ / SSE_3";	// ��� ���� (���������)

HWND hwnd, label;

char info[256];

DWORD WINAPI ThreadFunc(void*)
{
	SetWindowText(label, LPCSTR(""));

	HINSTANCE hinstLib = LoadLibrary(TEXT("dynamic_lib.dll"));
	if (hinstLib != NULL)
	{
		typedef int(*ImportFunction)();
		ImportFunction heightFunc = (ImportFunction)GetProcAddress(hinstLib, "height_screenf");
		ImportFunction sse3Func = (ImportFunction)GetProcAddress(hinstLib, "cpuid_sse3f");
		int height, sse3;
		// ���� ��������� �������� � �����������, ������� ����� �� �������� ��� � ������������ ���������� dynamic_lib.dll
		if (heightFunc == NULL && sse3Func == NULL)
		{
			sprintf_s(info, " � ���������� dynamic_lib.dll\n �� ������� ������ �������!\n �������� �� �����������\n �� ������������ ����������!");
		}
		else if (heightFunc != NULL && sse3Func == NULL)
		{
			int height = heightFunc();
			sprintf_s(info, "\n ������ ������: %d (� ��������)\n ��������� SSE3: �� ������� ����������", height);
		}
		else if (heightFunc != NULL && sse3Func != NULL)
		{
			int height = heightFunc();
			sse3 = sse3Func();
			if (sse3 == 1)
				sprintf_s(info, "\n ������ ������: %d (� ��������)\n ���������� SSE3: ��������������", height);
			else
				sprintf_s(info, "\n ������ ������: %d (� ��������)\n ���������� SSE3: �� ��������������", height);
		}
		else if (heightFunc == NULL && sse3Func != NULL)
		{
			sse3 = sse3Func();
			if (sse3 == 1)
				sprintf_s(info, "\n ������ ������: �� ������� ����������\n ���������� SSE3: ��������������");
			else
				sprintf_s(info, "\n ������ ������: �� ������� ����������\n ���������� SSE3: �� ��������������");
		}
		SetWindowText(label, LPCSTR(info));
		FreeLibrary(hinstLib);
	}
	else
	{
		sprintf_s(info, " ���������� dynamic_lib.dll �� �������!\n ��������� ���� dynamic_lib.dll � �����\n � ���������� � ������� ������\n \" �������� ������ \".");
		SetWindowText(label, LPCSTR(info));
	}
	return 0;
}

LRESULT CALLBACK WindowFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HANDLE hThread;
	DWORD IDThread;
	switch (msg)
	{ 
		// ��� ������� �� ������ "�������� ������" �������� ������� ThreadFunc � ����� ������
		case WM_COMMAND:
		{
			hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
			CloseHandle(hThread);
		}
		break;
		// ��� �������� ���������� ���� ������������� ��� � ���� ������
		case WM_CTLCOLORSTATIC:
		{		
				SetTextColor((HDC)wParam, RGB(255, 255, 255));
				SetBkColor((HDC)wParam, RGB(0, 0, 0));
				return (INT_PTR)GetStockObject(BLACK_BRUSH);
		}
		break;
		// �������� ���������
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
		// �� ���������
		default:
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst,	HINSTANCE hPrevInst, LPSTR str,int nWinMode)
{
	MSG msg;
	HANDLE hThread;
	DWORD IDThread;
	WNDCLASS wcl;
	wcl.hInstance = hThisInst;
	wcl.lpszClassName = szClassName;
	wcl.lpfnWndProc = WindowFunc;
	wcl.style = CS_HREDRAW | CS_VREDRAW;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.lpszMenuName = NULL;
	wcl.cbWndExtra = 0;
	wcl.cbClsExtra = 0;
	wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	RegisterClass(&wcl);

	hwnd = CreateWindow(szClassName, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		600, 500, 320, 155, NULL, NULL, hThisInst, NULL);

	label = CreateWindow("static", "",
		WS_CHILD | WS_VISIBLE,
		2, 2, 300, 70,
		hwnd, NULL, hThisInst, NULL);

	CreateWindow("button", "�������� ������",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		75, 80, 150, 30,
		hwnd, NULL, hThisInst, NULL);

	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	CloseHandle(hThread);

	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);

	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
