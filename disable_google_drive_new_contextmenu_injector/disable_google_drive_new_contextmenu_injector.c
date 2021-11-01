#include <windows.h>
#include <stdio.h>
#include <psapi.h>

// コマンドプロンプトを非表示にする設定
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

void dll_injection(DWORD processID);

int main(int argc, char* argv[])
{
	BOOL is_hide = FALSE;

	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--hide") == 0)
		{
			is_hide = TRUE;
			break;
		}
	}

	if (!is_hide)
	{
		AllocConsole();

		FILE* fp = NULL;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONIN$", "r", stdin);
	}

	BOOL success = FALSE;
	do
	{
		DWORD process_id_list[1024];
		DWORD process_id_list_size;
		if (!EnumProcesses(process_id_list, sizeof(process_id_list), &process_id_list_size))
		{
			printf("プロセス一覧が取れない\n");
			return 1;
		}

		int process_num = process_id_list_size / sizeof(DWORD);
		for (int i = 0; i < process_num; i++)
		{
			HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id_list[i]);

			if (NULL != process_handle)
			{
				HMODULE module_handle;
				DWORD module_size;
				if (EnumProcessModules(process_handle, &module_handle, sizeof(module_handle), &module_size))
				{
					TCHAR process_name[1000] = "";
					GetModuleBaseName(process_handle, module_handle, process_name, sizeof(process_name) / sizeof(TCHAR));

					// GoogleDriveのプロセスを見つけたらDLLインジェクションを行う。
					if (strcmp("GoogleDriveFS.exe", process_name) == 0)
					{
						printf("%s  (PID: %u)\n", process_name, process_id_list[i]);
						dll_injection(process_id_list[i]);

						success = TRUE;
					}
				}

				CloseHandle(process_handle);
			}
		}

		if (!success)
		{
			printf("GoogleDriveの起動を待っています。\n");
			Sleep(5000);
		}
	} while (!success);

	return 0;
}

void dll_injection(DWORD process_id)
{
	char path[1000];
	GetModuleFileName(NULL, path, 1000);

	char dll_path[1000];
	char* p = strrchr(path, '\\');
	strncpy_s(dll_path, sizeof(dll_path), path, p - path);
	sprintf_s(dll_path, sizeof(dll_path), "%s\\disable_google_drive_new_contextmenu.dll", dll_path);

	HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if (process_handle == NULL)
	{
		printf("GoogleDriveのプロセスにアクセスできませんでした。\n");
		return;
	}

	HMODULE kernel32 = GetModuleHandle("kernel32.dll");
	if (kernel32 == NULL)
	{
		printf("kernel32.dllにアクセスできませんでした。\n");
		return;
	}

	FARPROC loadlibrary = GetProcAddress(kernel32, "LoadLibraryA");
	if (loadlibrary == NULL)
	{
		printf("LoadLibraryA関数が見つかりませんでした。\n");
		return;
	}

	void* memory_data = VirtualAllocEx(process_handle, NULL, sizeof(dll_path), MEM_COMMIT, PAGE_READWRITE);
	if (memory_data != NULL)
	{
		if (WriteProcessMemory(process_handle, memory_data, (void*)dll_path, sizeof(dll_path), NULL) != 0)
		{
			HANDLE thread_handle = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)loadlibrary, memory_data, 0, NULL);
			if (thread_handle != NULL)
			{
				WaitForSingleObject(thread_handle, INFINITE);
				CloseHandle(thread_handle);
			}
			else
			{
				printf("GoogleDriveのプロセスにスレッドを作れませんでした。\n");
			}
		}
		else
		{
			printf("GoogleDriveのプロセスにDLL Pathを書き込めませんでした。\n");
		}

		VirtualFreeEx(process_handle, memory_data, 0, MEM_RELEASE);
	}
	else
	{
		printf("メモリが確保できませんでした。\n");
	}
}
