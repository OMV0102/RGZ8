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

// Функция обращения к DLL
void DLL()
{
	// загрузка библиотеки
	HINSTANCE hinstLib = LoadLibrary(TEXT("dynamic_lib.dll"));
	if (hinstLib != NULL)
	{
		typedef int(*ImportFunction)();
		ImportFunction getSystemMetrics = (ImportFunction)GetProcAddress(hinstLib, "height_screen");
		ImportFunction cpuid = (ImportFunction)GetProcAddress(hinstLib, "cpuid_sse3");

		// получаем ширину и информацию о ЦП, затем проверяем наличие технологии SSE2 и выгружаем библиотеку
		int Width = getSystemMetrics();
		int sse3 = cpuid(); // получение информации
		FreeLibrary(hinstLib);

		char result[128];
		if(sse3 == 1)
			sprintf_s(result, "Высота экрана %d пикселей\nТехнология SSE3 %s", Width, "поддерживается");
		else
			sprintf_s(result, "Высота экрана %d пикселей\nТехнология SSE3 %s", Width, "не поддерживается");
		SetWindowText(screenWidthLabel, LPCSTR(result));
	}
	else
	{
		MessageBox(hwnd,"Отсутствует библиотека!", "Ошибка", 0);
	}
}

LONG WINAPI WndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
	switch (Message)
	{ 
	case WM_CTLCOLORSTATIC: // выставление свойств перед отрисовкой текстового поля
	{		
			HDC hdcStatic = (HDC)wparam;
			SetTextColor(hdcStatic, RGB(255, 0, 0));
			SetBkColor(hdcStatic, RGB(0, 0, 0));
			return (INT_PTR)hBrush;
	}
	break;
	case WM_COMMAND:
	{
		if (LOWORD(wparam) == 1) // запуск
		{
			HANDLE hThread;
			DWORD IDThread;
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DLL, NULL, 0, &IDThread); // Запускаем функцию в новом потоке
			CloseHandle(hThread);
		}
		if (LOWORD(wparam) == 2) // выйти
		{
			PostQuitMessage(0);
		}
		
		if (LOWORD(wparam) == 3) // очистить окно с текстом
		{
			HANDLE hThread;
			DWORD IDThread;
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)clear, NULL, 0, &IDThread); // Запускаем функцию в новом потоке
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
	wc.lpfnWndProc = WndProc;			// Обработчик сообщений
	wc.hInstance = hInstance;
	wc.hbrBackground = CreateSolidBrush(RGB(200, 200, 200));
	
	RegisterClass(&wc);

	HDC hDCScreen = GetDC(NULL);


	hwnd = CreateWindow("proj", "Проверка ширины и SSE2",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		600, 600, Width, Height, NULL, NULL, hInstance, NULL);

	screenWidthLabel = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON,
		5, 5, Width - 15, Height * 0.3, hwnd, (HMENU)0, hInstance, NULL);

	button_start = CreateWindow("button", "ЗАПУСК",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_VCENTER | BS_CENTER, Width - 2 * ButtonWidth - 10, Height - ButtonHeight*3,
		ButtonWidth, ButtonHeight, hwnd, (HMENU)1, hInstance, NULL);

	button_exit = CreateWindow("button", "ВЫХОД",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_VCENTER | BS_CENTER, Width - ButtonWidth - 10, Height - ButtonHeight * 3,
		ButtonWidth, ButtonHeight, hwnd, (HMENU)2, hInstance, NULL);

	button_clear = CreateWindow("button", "ОЧИСТИТЬ ТЕКСТ",
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
