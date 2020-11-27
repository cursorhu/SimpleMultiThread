#include <iostream>   
#include <windows.h>   
using namespace std;
 
DWORD WINAPI Print(LPVOID lpParamter)
{
    std::string s = (char*)lpParamter;
    for (int i = 0; i < 10; i++)
        cout << s;
    return 0;
}
 
int main()
{
    std::string s1 = "Work thread\n";
    std::string s2 = "Main thread\n";
    HANDLE hThread = CreateThread(NULL, 0, Print, (LPVOID)s1.c_str(), 0, NULL);
    Print((LPVOID)s2.c_str());
	CloseHandle(hThread);
    Sleep(100);
    return 0;
}