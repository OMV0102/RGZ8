#include <windows.h>

extern "C" __declspec(dllexport) int height_screen()
{
	// Функция GetSystemMetrics с флагом SM_CYSCREEN
	// Возвращает высоту экрана (в пикселях) 
	return GetSystemMetrics(SM_CYSCREEN);
}

extern "C" __declspec(dllexport) int cpuid_sse3(int regs[4], int func)
{
	int sse3 = 0;
	// Ассемблерная вставка
	__asm
	{
		// Команда cpuid при значении регистра EAX = 1
		// в регистре ECX установит нулевой бит равным единице,
		// если инструкция sse3 поддерживается процессором.
		// Если инструкция sse3 не поддерживается процессором, 
		// то нулевой бит регистра ECX равен нулю.

		MOV EAX, 1		// Помещаем единицу в регистр EAX
		cpuid			// Выполняем команду cpuid
		AND ECX, 1h		// Обнуляем все биты кроме нулевого, т.к. могут попадаться единицы
		MOV sse3, ECX	// Помещаем значение нулевого бита в переменную (т.к. остальные биты точно равны нулю) 
	}
	// Возвращаем значение (ноль, либо единица)
	return sse3;
}