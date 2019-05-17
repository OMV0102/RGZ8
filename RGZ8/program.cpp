#include <windows.h>
#include <string>

using namespace std;

LPCSTR szClassName = "Control Source";		// Имя класса формы
LPCSTR szTitle = "Высота экрана / SSE_3";	// Имя окна (заголовок)

HWND hwnd, label;

char info[256];

DWORD WINAPI ThreadFunc(void*)
{
	SetWindowText(label, LPCSTR(""));

	// Загружаем библиотеку
	HINSTANCE hinstLib = LoadLibrary(TEXT("dynamic_lib.dll"));
	if (hinstLib != NULL)	// Если библиотека загрузилась
	{
		typedef int(*ImportFunction)();
		ImportFunction heightFunc = (ImportFunction)GetProcAddress(hinstLib, "height_screen");
		ImportFunction sse3Func = (ImportFunction)GetProcAddress(hinstLib, "cpuid_sse3");
		int height, sse3;
		// Если обе функции не найдены в библиотеке
		if (heightFunc == NULL && sse3Func == NULL)
		{
			sprintf_s(info, "\n В библиотеке dynamic_lib.dll\n не найдены нужные функции!");
		}
		else if (heightFunc != NULL && sse3Func == NULL)	// Если не найдена только функция sse3Func
		{
			int height = heightFunc();
			sprintf_s(info, "\n Высота экрана: %d (в пикселях)\n Поддержка SSE3: не удалось определить", height);
		}
		else if (heightFunc != NULL && sse3Func != NULL)	 // Если обе функции возаратилил значения
		{
			int height = heightFunc();
			sse3 = sse3Func();
			if (sse3 == 1)
				sprintf_s(info, "\n Высота экрана: %d (в пикселях)\n Технология SSE3: поддерживается", height);
			else
				sprintf_s(info, "\n Высота экрана: %d (в пикселях)\n Технология SSE3: не поддерживается", height);
		}
		else if (heightFunc == NULL && sse3Func != NULL)	// Если не найдена только функция heightFunc
		{
			sse3 = sse3Func();
			if (sse3 == 1)
				sprintf_s(info, "\n Высота экрана: не удалось определить\n Технология SSE3: поддерживается");
			else
				sprintf_s(info, "\n Высота экрана: не удалось определить\n Технология SSE3: не поддерживается");
		}
		FreeLibrary(hinstLib); // Закрываем библиотеку
	}
	else	// Если библиотека не загрузилась
	{
		sprintf_s(info, " Библиотека dynamic_lib.dll не найдена!\n Поместите файл dynamic_lib.dll в папку\n с программой и нажмите кнопку\n \" Обновить данные \".");
	}
	SetWindowText(label, LPCSTR(info));	// Записываем текст в текстовое поле
	return 0;
}

LRESULT CALLBACK WindowFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HANDLE hThread;
	DWORD IDThread;
	switch (msg)
	{ 
		// При нажатии на кнопку "Обновить данные" вызываем функцию ThreadFunc в новом потоке
		case WM_COMMAND:
		{
			hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
			CloseHandle(hThread);
		}
		break;
		// При создании текстового поля устанавливаем фон и цвет текста
		case WM_CTLCOLORSTATIC:
		{		
				SetTextColor((HDC)wParam, RGB(255, 255, 255));
				SetBkColor((HDC)wParam, RGB(0, 0, 0));
				return (INT_PTR)GetStockObject(BLACK_BRUSH);
		}
		break;
		// Закрытие программы
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
		// По умолчанию
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
	// Создание и заполнение класса окна
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
	RegisterClass(&wcl);	// Регистрация класса окна

	// Создание окна
	hwnd = CreateWindow(szClassName, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		600, 500, 320, 155, NULL, NULL, hThisInst, NULL);

	// Создание текстового поля
	label = CreateWindow("static", "",
		WS_CHILD | WS_VISIBLE,	// Стиль
		2, 2, 300, 70,			// Положение и размеры
		hwnd, NULL, hThisInst, NULL);

	// Создание кнопки
	CreateWindow("button", "Обновить данные",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	// Стиль кнопки
		75, 80, 150, 30,						// Положение и размеры
		hwnd, NULL, hThisInst, NULL);

	// Вызов функцию ThreadFunc в новом потоке
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	CloseHandle(hThread);

	// Отображение окна на экране
	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);

	// Обработка сообщений
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
