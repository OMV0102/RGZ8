#include <windows.h>
#include <string>

HWND hwnd, button_start, button_exit, button_clear, screenWidthLabel;
static HBRUSH hBrush = CreateSolidBrush(BLACK_BRUSH);
int Height = 150;
int Width = 300;
int ButtonHeight = 20;
int ButtonWidth = 80;

void clear()
{
	SetWindowText(screenWidthLabel, LPCSTR(""));
}

// ������� ��������� � DLL
void DLL()
{
	// �������� ����������
	HINSTANCE hinstLib = LoadLibrary(TEXT("dynamic_lib.dll"));
	if (hinstLib != NULL)
	{
		typedef int(*ImportFunction)();
		ImportFunction getSystemMetrics = (ImportFunction)GetProcAddress(hinstLib, "height_screen");
		ImportFunction cpuid = (ImportFunction)GetProcAddress(hinstLib, "cpuid_sse3");

		// �������� ������ � ���������� � ��, ����� ��������� ������� ���������� SSE2 � ��������� ����������
		int Width = getSystemMetrics();
		int sse3 = cpuid(); // ��������� ����������
		FreeLibrary(hinstLib);

		char result[128];
		if(sse3 == 1)
			sprintf_s(result, "������ ������ %d ��������\n���������� SSE3 %s", Width, "��������������");
		else
			sprintf_s(result, "������ ������ %d ��������\n���������� SSE3 %s", Width, "�� ��������������");
		SetWindowText(screenWidthLabel, LPCSTR(result));
	}
	else
	{
		MessageBox(hwnd,"����������� ����������!", "������", 0);
	}
}

LONG WINAPI WndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
	switch (Message)
	{ 
	case WM_CTLCOLORSTATIC: // ����������� ������� ����� ���������� ���������� ����
	{		
			HDC hdcStatic = (HDC)wparam;
			SetTextColor(hdcStatic, RGB(255, 0, 0));
			SetBkColor(hdcStatic, RGB(0, 0, 0));
			return (INT_PTR)hBrush;
	}
	break;
	case WM_COMMAND:
	{
		if (LOWORD(wparam) == 1) // ������
		{
			HANDLE hThread;
			DWORD IDThread;
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DLL, NULL, 0, &IDThread); // ��������� ������� � ����� ������
			CloseHandle(hThread);
		}
		if (LOWORD(wparam) == 2) // �����
		{
			PostQuitMessage(0);
		}
		
		if (LOWORD(wparam) == 3) // �������� ���� � �������
		{
			HANDLE hThread;
			DWORD IDThread;
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)clear, NULL, 0, &IDThread); // ��������� ������� � ����� ������
			CloseHandle(hThread);
		}
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, Message, wparam, lparam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nCmdShow)
{
	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.lpszClassName = "proj";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;			// ���������� ���������
	wc.hInstance = hInstance;
	wc.hbrBackground = CreateSolidBrush(RGB(200, 200, 200));
	
	RegisterClass(&wc);

	HDC hDCScreen = GetDC(NULL);


	hwnd = CreateWindow("proj", "�������� ������ � SSE2",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		600, 600, Width, Height, NULL, NULL, hInstance, NULL);

	screenWidthLabel = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON,
		5, 5, Width - 15, Height * 0.3, hwnd, (HMENU)0, hInstance, NULL);

	button_start = CreateWindow("button", "������",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_VCENTER | BS_CENTER, Width - 2 * ButtonWidth - 10, Height - ButtonHeight*3,
		ButtonWidth, ButtonHeight, hwnd, (HMENU)1, hInstance, NULL);

	button_exit = CreateWindow("button", "�����",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_VCENTER | BS_CENTER, Width - ButtonWidth - 10, Height - ButtonHeight * 3,
		ButtonWidth, ButtonHeight, hwnd, (HMENU)2, hInstance, NULL);

	button_clear = CreateWindow("button", "�������� �����",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_VCENTER | BS_CENTER, Width - 3.5*ButtonWidth - 20, Height - ButtonHeight * 3,
		ButtonWidth * 1.75, ButtonHeight, hwnd, (HMENU)3, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);

	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
