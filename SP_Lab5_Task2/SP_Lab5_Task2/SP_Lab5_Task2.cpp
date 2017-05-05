// SP_Lab5_Task2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifdef UNICODE
#define _tsprintf swprintf
#else
#define _tsprintf sprintf
#endif // UNICODE


HANDLE semaphore = NULL;
DWORD thread_count = 0;
DWORD handles_limit = 0;
TCHAR* filename = NULL;

VOID PrintWin32Error();
VOID ErrorExit(TCHAR*);

DWORD WINAPI ThreadFunc(VOID);

INT _tmain(UINT argc, TCHAR** argv)
{
	// set of rand seed
	srand((unsigned)time(NULL));

	// Parse cmd line args
	if (argc != 6 || _tcscmp(_T("-t"), argv[1]) || _tcscmp(_T("-l"), argv[3]))
	{
		ErrorExit(_T("Wrong cmd-line args. Example: prog.exe -t 10 -l 5 file.txt"));
	}
	thread_count = _tstoi(argv[2]);
	handles_limit = _tstoi(argv[4]);
	filename = (TCHAR*)LocalAlloc(LPTR, MAX_PATH);
	_tcscpy(filename, argv[5]);
	if (!thread_count || !handles_limit || handles_limit > thread_count)
	{
		ErrorExit(_T("Wrong parameters."));
	}

	DWORD return_value = 0;
	HANDLE* threads = (HANDLE*)LocalAlloc(LPTR, thread_count * sizeof(HANDLE));
	int i = 0;

	// Create semaphore

	semaphore = CreateSemaphore(
		NULL,           // default security attributes
		handles_limit,  // initial count
		handles_limit,  // maximum count
		NULL);          // unnamed semaphore

	if (semaphore == NULL)
	{
		ErrorExit(_T("CreateSemaphore error\n"));
	}
	
	// Create multiple threads. 

	for (i = 0; i < thread_count; ++i)
	{
		// launch thread
		threads[i] = CreateThread(NULL, // default security attributes 
			0,                           // use default stack size 
			(LPTHREAD_START_ROUTINE)ThreadFunc, // thread function 
			NULL,
			0,							 // use default creation flags 
			&return_value);              // returns thread identifier 

										 // Check the return value for success. 
		if (threads[i] == NULL)
		{
			ErrorExit(_T("CreateThread error\n"));
		}
	}
	// waiting all finished
	WaitForMultipleObjects(thread_count, threads, TRUE, INFINITE);
	// releasing resources
	for (i = 0; i < thread_count; ++i)
	{
		CloseHandle(threads[i]);
	}
	CloseHandle(semaphore);
	LocalFree(threads);
	LocalFree(filename);
	_tsystem(_T("pause"));
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

DWORD WINAPI ThreadFunc(VOID)
{
	DWORD wait_time = 0, wait_result = 0;
	HANDLE hfile = NULL;

	do
	{
		wait_time = rand() % 3000 + 1;
		wait_result = WaitForSingleObject(semaphore, wait_time);
	} while (wait_result != WAIT_OBJECT_0);
	
	hfile = CreateFile((TCHAR*)filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
	if (hfile == INVALID_HANDLE_VALUE)
	{
		PrintWin32Error();
		_tprintf(_T("Thread: %d\tError openning file.\n"), GetCurrentThreadId());
	}
	else
	{
		SYSTEMTIME system_time = { 0 };
		TCHAR* buffer = (TCHAR*)LocalAlloc(LPTR, 255 * sizeof(TCHAR));
		_tsprintf(buffer, _T("Thread: %d\t"), GetCurrentThreadId());
		// inserting time
		GetSystemTime(&system_time);
		GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &system_time, NULL, buffer + _tcslen(buffer), 255);
		buffer[_tcslen(buffer)] = _T('\r');
		buffer[_tcslen(buffer)] = _T('\n');
		// writing string to file
		DWORD bytes_written = 0;
		SetFilePointer(hfile, 0, 0, FILE_END);
		WriteFile(hfile, buffer, _tcslen(buffer) * sizeof(TCHAR), &bytes_written, NULL);
		if (bytes_written != _tcslen(buffer) * sizeof(TCHAR))
		{
			_tprintf(_T("Thread: %d\tError writing to file.\n"), GetCurrentThreadId());
		}
		else
		{
			_tprintf(_T("%s"), buffer);
		}

		LocalFree(buffer);
	}

	wait_time = rand() % 3000 + 1;
	Sleep(wait_time);
	CloseHandle(hfile);

	ReleaseSemaphore(semaphore, 1, NULL);
	return 0;
}
