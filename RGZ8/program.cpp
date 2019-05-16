#include <windows.h>
#include <string>

using namespace std;

LPCSTR szClassName = "Control Source"; // имя класса формы
LPCSTR szTitle = "Высота экрана / SSE_3"; // имя окна (заголовок)

HWND hwnd, button_start, button_exit, button_clear, label;
static HBRUSH hBrush = CreateSolidBrush(BLACK_BRUSH);
int Height = 150;
int Width = 300;
int ButtonHeight = 20;
int ButtonWidth = 80;

char info[256];

DWORD WINAPI ThreadFunc(void*)
{
	SetWindowText(label, LPCSTR(""));

	HINSTANCE hinstLib = LoadLibrary(TEXT("dynamic_lib.dll"));
	if (hinstLib != NULL)
	{
		typedef int(*ImportFunction)();
		ImportFunction heightFunc = (ImportFunction)GetProcAddress(hinstLib, "height_screen");
		ImportFunction sse3Func = (ImportFunction)GetProcAddress(hinstLib, "cpuid_sse3Q");
		int height, sse3;
		// если программа запущена с библиотекой, имеющей такое же название как и оригинальная библиотека dynamic_lib.dll
		if (heightFunc == NULL && sse3Func == NULL)
		{
			sprintf_s(info, " В динамической библиотеки dynamic_lib.dll не найдены нужные функции!\nВозможно вы используете не оригинальную библиотеку!");
		}
		else if (heightFunc != NULL && sse3Func == NULL)
		{
			int height = heightFunc();
			sprintf_s(info, " Высота экрана: %d (в пикселях)\nПоддержка SSE3: не удалось определить", height);
		}
		else if (heightFunc != NULL && sse3Func != NULL)
		{
			int height = heightFunc();
			sse3 = sse3Func();
			if (sse3 == 1)
				sprintf_s(info, " Высота экрана: %d (в пикселях)\nТехнология SSE3: поддерживается", height);
			else
				sprintf_s(info, " Высота экрана: %d (в пикселях)\nТехнология SSE3: не поддерживается", height);
		}
		else if (heightFunc == NULL && sse3Func != NULL)
		{
			sse3 = sse3Func();
			if (sse3 == 1)
				sprintf_s(info, " Высота экрана: не удалось определить\nТехнология SSE3: поддерживается");
			else
				sprintf_s(info, " Высота экрана: не удалось определить\nТехнология SSE3: не поддерживается");
		}
		SetWindowText(label, LPCSTR(info));
		FreeLibrary(hinstLib);
	}
	else
	{
		sprintf_s(info, " Библиотека dynamic_lib.dll не найдена!\nПоместите файл dynamic_lib.dll в папку\nс программой и нажмите кнопку \"Обновить данные\"!");
		SetWindowText(label, LPCSTR(info));
	}
	return 0;
}

LRESULT CALLBACK WindowFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{ 
		case WM_CTLCOLORSTATIC: // выставление свойств перед отрисовкой текстового поля
		{		
				HDC hdcStatic = (HDC)wParam;
				SetTextColor(hdcStatic, RGB(255, 0, 0));
				SetBkColor(hdcStatic, RGB(0, 0, 0));
				return (INT_PTR)hBrush;
				break;
		}
		
		case WM_COMMAND:
		{
			if (LOWORD(wParam) == 1) // запуск
			{
				HANDLE hThread;
				DWORD IDThread;
				hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread); // Запускаем функцию в новом потоке
				CloseHandle(hThread);
			}
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst,	HINSTANCE hPrevInst, LPSTR str,int nWinMode)
{
	MSG msg;
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

	HDC hDCScreen = GetDC(NULL);


	hwnd = CreateWindow(szClassName, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		600, 600, Width, Height, NULL, NULL, hThisInst, NULL);

	label = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON,
		5, 5, Width - 15, Height * 0.3, hwnd, (HMENU)0, hThisInst, NULL);

	button_start = CreateWindow("button", "Обновить данные",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_VCENTER | BS_CENTER, Width - 2 * ButtonWidth - 10, Height - ButtonHeight*3,
		ButtonWidth, ButtonHeight, hwnd, (HMENU)1, hThisInst, NULL);

	ShowWindow(hwnd, nWinMode);

	UpdateWindow(hwnd);

	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
