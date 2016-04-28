#include <windows.h>
#include <stdio.h>

int main(){
	HKEY hKey;
	HANDLE hChangedEvent;
	LONG lErrorCode;
	DWORD value;
	DWORD valueON = 0x00000001;
	DWORD valueOFF = 0x00000000;
	DWORD size = sizeof(DWORD);
	bool dolbyON;
	
	lErrorCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Dolby\\DolbyDigitalPlus\\DynamicKeys\\", 0, KEY_NOTIFY|KEY_READ|KEY_SET_VALUE|0x0100, &hKey);
	if (lErrorCode != ERROR_SUCCESS)
	{
		printf("Error in RegOpenKeyEx (%d).\n", lErrorCode);
		return 1;
	}
	
	lErrorCode = RegQueryValueEx(hKey, "{DD0DF94B-53A2-4A92-854E-91F42E28FD7B},1", NULL, NULL, (BYTE *)&value, &size);
	if (lErrorCode != ERROR_SUCCESS)
	{
		printf("Error in RegQueryValueEx (%d).\n", lErrorCode);
		return 1;
	}
	if(value == 1) {
		dolbyON = true;
	}else{
		dolbyON = false;
	}
	
	hChangedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hChangedEvent == NULL)
	{
		printf("Error in CreateEvent (%d).\n", GetLastError());
		return 1;
	}
	
	while(true){	
		lErrorCode = RegQueryValueEx(hKey, "{DD0DF94B-53A2-4a92-854E-91F42E28FD7B},0", NULL, NULL, (BYTE *)&value, &size);
		if (lErrorCode != ERROR_SUCCESS)
		{
			printf("Error in RegQueryValueEx (%d).\n", lErrorCode);
			return 1;
		}
		
		if(value == 3) {
			if(dolbyON) {
				printf("Headphones plugged in. Dolby OFF.\n");
				lErrorCode = RegSetValueEx(hKey, "{DD0DF94B-53A2-4A92-854E-91F42E28FD7B},1", 0, REG_DWORD, (const BYTE *)&valueOFF, size);
				if (lErrorCode != ERROR_SUCCESS)
				{
					printf("Error in RegSetValueEx (%d).\n", lErrorCode);
					return 1;
				}
				dolbyON = false;
			}
		}else{
			if(!dolbyON) {
				printf("Headphones unplugged. Dolby ON.\n");
				lErrorCode = RegSetValueEx(hKey, "{DD0DF94B-53A2-4A92-854E-91F42E28FD7B},1", 0, REG_DWORD, (const BYTE *)&valueON, size);
				if (lErrorCode != ERROR_SUCCESS)
				{
					printf("Error in RegSetValueEx (%d).\n", lErrorCode);
					return 1;
				}
				dolbyON = true;
			}
		}
		
		lErrorCode = RegNotifyChangeKeyValue(hKey, false, REG_NOTIFY_CHANGE_LAST_SET, hChangedEvent, true);
		if (lErrorCode != ERROR_SUCCESS)
		{
			printf("Error in RegNotifyChangeKeyValue (%d).\n", lErrorCode);
			return 1;
		}
	
		if (WaitForSingleObject(hChangedEvent, INFINITE) == WAIT_FAILED)
		{
			printf("Error in WaitForSingleObject (%d).\n", GetLastError());
			return 1;
		}
	}

	lErrorCode = RegCloseKey(hKey);
	if (lErrorCode != ERROR_SUCCESS)
	{
		printf("Error in RegCloseKey (%d).\n", GetLastError());
		return 1;
	}
   
	if (!CloseHandle(hChangedEvent))
	{
		printf("Error in CloseHandle.\n");
		return 1;
	}
   
	return 0;
}