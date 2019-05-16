#include <windows.h>

extern "C" __declspec(dllexport) int height_screen()
{
	// функци€ GetSystemMetrics с флагом SM_CYSCREEN
	// возвращает высоту экрана (в пиксел€х) 
	return GetSystemMetrics(SM_CYSCREEN);
}

extern "C" __declspec(dllexport) int cpuid_sse3(int regs[4], int func)
{
	int sse3 = 0;
	// ассемблерна€ вставка
	__asm
	{
		//  оманда cpuid при значении регистра EAX = 1
		// в регистре ECX установит нулевой бит равным единице,
		// если инструкци€ sse3 поддерживаетс€ процессором.
		// ≈сли инструкци€ sse3 не поддерживаетс€ процессором, 
		// то нулевой бит регистра ECX равен нулю.

		MOV EAX, 1		// помещаем единицу в регистр EAX
		cpuid			// выполн€ем команду cpuid
		AND ECX, 1h		// обнул€ем все биты кроме нулевого, т.к. могут попадатьс€ единицы
		MOV sse3, ECX	// помещаем значение нулевого бита в переменную (т.к. остальные биты точно равны нулю) 
	}
	//возвращаем значение (ноль, либо единица)
	return sse3;
}