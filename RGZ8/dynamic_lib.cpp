#include <windows.h>

extern "C" __declspec(dllexport) int height_screen()
{
	// ������� GetSystemMetrics � ������ SM_CYSCREEN
	// ���������� ������ ������ (� ��������) 
	return GetSystemMetrics(SM_CYSCREEN);
}

extern "C" __declspec(dllexport) int cpuid_sse3(int regs[4], int func)
{
	int sse3 = 0;
	// ������������ �������
	__asm
	{
		// ������� cpuid ��� �������� �������� EAX = 1
		// � �������� ECX ��������� ������� ��� ������ �������,
		// ���� ���������� sse3 �������������� �����������.
		// ���� ���������� sse3 �� �������������� �����������, 
		// �� ������� ��� �������� ECX ����� ����.

		MOV EAX, 1		// �������� ������� � ������� EAX
		cpuid			// ��������� ������� cpuid
		AND ECX, 1h		// �������� ��� ���� ����� ��������, �.�. ����� ���������� �������
		MOV sse3, ECX	// �������� �������� �������� ���� � ���������� (�.�. ��������� ���� ����� ����� ����) 
	}
	// ���������� �������� (����, ���� �������)
	return sse3;
}