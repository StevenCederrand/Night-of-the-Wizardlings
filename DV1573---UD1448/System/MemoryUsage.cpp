#include "Pch/Pch.h"
#include "MemoryUsage.h"

void MemoryUsage::printVramUsage()
{
	IDXGIFactory* dxgifactory = nullptr;
	HRESULT ret_code = ::CreateDXGIFactory(
		__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(&dxgifactory));

	if (SUCCEEDED(ret_code))
	{
		IDXGIAdapter* dxgiAdapter = nullptr;

		if (SUCCEEDED(dxgifactory->EnumAdapters(0, &dxgiAdapter)))
		{
			IDXGIAdapter4* dxgiAdapter4 = NULL;
			if (SUCCEEDED(dxgiAdapter->QueryInterface(__uuidof(IDXGIAdapter4), (void**)&dxgiAdapter4)))
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO info;

				if (SUCCEEDED(dxgiAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
				{
					float memoryUsage = float(info.CurrentUsage / 1024.0 / 1024.0); //MiB
					printf("[VRAM USAGE]: %f\n", memoryUsage);

				};

				dxgiAdapter4->Release();
			}
			dxgiAdapter->Release();
		}
		dxgifactory->Release();
	}
}

void MemoryUsage::printRamUsage()
{
	//src: https://docs.microsoft.com/en-us/windows/desktop/api/psapi/ns-psapi-_process_memory_counters

	DWORD currentProcessID = GetCurrentProcessId();

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, currentProcessID);

	if (NULL == hProcess)
		return;

	PROCESS_MEMORY_COUNTERS pmc{};
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		//PagefileUsage is the:
			//The Commit Charge value in bytes for this process.
			//Commit Charge is the total amount of memory that the memory manager has committed for a running process.

		float memoryUsage = float(pmc.PagefileUsage / 1024.0 / 1024.0); //MiB

	
		printf("[RAM USAGE]: %f\n", memoryUsage);
	}

	CloseHandle(hProcess);
}

void MemoryUsage::printBoth(std::string string)
{
	std::cout << string << "\n";
	printRamUsage();
	printVramUsage();
	std::cout << "----------------------------\n";
}
