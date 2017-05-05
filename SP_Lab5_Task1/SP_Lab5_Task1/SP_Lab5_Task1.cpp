// SP_Lab5_Task1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define MUTEX_NAME _T("SP_LAB5_TASK1")

int _tmain(DWORD argc, TCHAR** argv)
{
	HANDLE mutex = CreateMutex(
		NULL,
		FALSE,             // initially owned
		MUTEX_NAME);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		_tprintf(_T("The program is already opened!\n"));
	}
	else if (mutex != NULL)
	{
		_tprintf(_T("Is Running\n"));
	}
	else
	{
		_tprintf(_T("Unknown error.\n"));
	}
	
	_tsystem(_T("pause"));

	if (mutex != NULL)
	{
		CloseHandle(mutex);
	}
    return 0;
}

