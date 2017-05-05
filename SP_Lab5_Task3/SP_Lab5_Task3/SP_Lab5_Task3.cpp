// SP_Lab5_Task3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define THREAD_COUNT 3
#define DATA_LENGTH 655350

BOOL mode = FALSE; // FALSE - without critical section
HANDLE semaphore = NULL;


VOID PrintWin32Error();
VOID ErrorExit(TCHAR*);

DWORD WINAPI ThreadSum(VOID*);
DWORD WINAPI ThreadAvr(VOID*);
DWORD WINAPI ThreadMin(VOID*);

INT _tmain(UINT argc, TCHAR** argv)
{
	// set of rand seed
	srand((unsigned)time(NULL));

	// Parse cmd line args
	if (argc != 2)
	{
		ErrorExit(_T("Wrong cmd-line args. Pass 'true' or 'false' as arg."));
	}
	else if (argc == 2)
	{
		if (!_tcscmp(_T("true"), argv[1]))
		{
			mode = TRUE;
		}
		else if (!_tcscmp(_T("false"), argv[1]))
		{
			mode = FALSE;
		}
		else
		{
			ErrorExit(_T("Wrong cmd-line args. Pass 'true' or 'false' as arg."));
		}
	}

	HANDLE* threads = (HANDLE*)LocalAlloc(LPTR, THREAD_COUNT * sizeof(HANDLE));
	int i = 0;

	// Create semaphore

	semaphore = CreateSemaphore(
		NULL,           // default security attributes
		1,  // initial count
		1,  // maximum count
		NULL);          // unnamed semaphore

	if (semaphore == NULL)
	{
		ErrorExit(_T("CreateSemaphore error\n"));
	}
	// prepare data
	DWORD* data = (DWORD*)LocalAlloc(LPTR, DATA_LENGTH * sizeof(DWORD));
	for (i = 0; i < DATA_LENGTH; i++)
	{
		data[i] = (DWORD)(rand() % 100);
	}
	// start timer
	SYSTEMTIME start_time = { 0 };
	GetSystemTime(&start_time);
	// launch threads
	for (i = 0; i < THREAD_COUNT; ++i)
	{
		switch (i)
		{
		case 0:
			// launch thread
			threads[i] = CreateThread(NULL, // default security attributes 
				0,                           // use default stack size 
				(LPTHREAD_START_ROUTINE)ThreadSum, // thread function 
				data,
				0,							 // use default creation flags 
				NULL);              // returns thread identifier 
			break;
		case 1:
			// launch thread
			threads[i] = CreateThread(NULL, // default security attributes 
				0,                           // use default stack size 
				(LPTHREAD_START_ROUTINE)ThreadAvr, // thread function 
				data,
				0,							 // use default creation flags 
				NULL);              // returns thread identifier 
			break;
		case 2:
			// launch thread
			threads[i] = CreateThread(NULL, // default security attributes 
				0,                           // use default stack size 
				(LPTHREAD_START_ROUTINE)ThreadMin, // thread function 
				data,
				0,							 // use default creation flags 
				NULL);              // returns thread identifier 
			break;
		default:
			break;
		}

		if (threads[i] == NULL)
		{
			ErrorExit(_T("CreateThread error\n"));
		}
	}
	// waiting all finished
	WaitForMultipleObjects(THREAD_COUNT, threads, TRUE, INFINITE);
	// stop timer
	SYSTEMTIME stop_time = { 0 };
	GetSystemTime(&stop_time);
	// print results
	if (mode == true)
	{
		_tprintf(_T("[ENABLED] Mutual exclusion.\n"));
	}
	else
	{
		_tprintf(_T("[DISABLED] Mutual exclusion.\n"));
	}
	_tprintf(_T("Time passed: %d ms.\n"), stop_time.wMilliseconds - start_time.wMilliseconds);
	// releasing resources
	for (i = 0; i < THREAD_COUNT; ++i)
	{
		CloseHandle(threads[i]);
	}
	CloseHandle(semaphore);
	LocalFree(threads);
	LocalFree(data);

	return 0;
}

VOID PrintWin32Error()
{
	VOID* lpMsg = NULL;
	DWORD dwError = GetLastError();
	// Formating message.
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsg,
		0,
		NULL);
	_tprintf(_T("%s\n"), lpMsg);
	LocalFree(lpMsg);
	ExitProcess(0);
}

VOID ErrorExit(TCHAR* msg)
{
	_tprintf(_T("%s\n"), msg);
	ExitProcess(0);
}

DWORD WINAPI ThreadSum(VOID* data)
{
	if (mode == TRUE)
	{
		DWORD wait_result = 0;
		do
		{
			wait_result = WaitForSingleObject(semaphore, INFINITE);
		} while (wait_result != WAIT_OBJECT_0);
	}

	DWORD sum = 0;
	for (DWORD i = 0; i < DATA_LENGTH; i++)
	{
		sum += ((DWORD*)data)[i];
	}
	_tprintf(_T("Summary: %d\n"), sum);

	if (mode == TRUE)
	{
		ReleaseSemaphore(semaphore, 1, NULL);
	}
	return 0;
}

DWORD WINAPI ThreadAvr(VOID* data)
{
	if (mode == TRUE)
	{
		DWORD wait_result = 0;
		do
		{
			wait_result = WaitForSingleObject(semaphore, INFINITE);
		} while (wait_result != WAIT_OBJECT_0);
	}

	DWORD sum = 0;
	for (DWORD i = 0; i < DATA_LENGTH; i++)
	{
		sum += ((DWORD*)data)[i];
	}
	long double average = (long double)sum / DATA_LENGTH;
	_tprintf(_T("Average: %.4llf\n"), average);

	if (mode == TRUE)
	{
		ReleaseSemaphore(semaphore, 1, NULL);
	}
	return 0;
}

DWORD WINAPI ThreadMin(VOID* data)
{
	if (mode == TRUE)
	{
		DWORD wait_result = 0;
		do
		{
			wait_result = WaitForSingleObject(semaphore, INFINITE);
		} while (wait_result != WAIT_OBJECT_0);
	}

	DWORD min = MAXDWORD;
	for (DWORD i = 0; i < DATA_LENGTH; i++)
	{
		if (min > ((DWORD*)data)[i])
		{
			min = ((DWORD*)data)[i];
		}
	}
	_tprintf(_T("Min: %d\n"), min);

	if (mode == TRUE)
	{
		ReleaseSemaphore(semaphore, 1, NULL);
	}
	return 0;
}