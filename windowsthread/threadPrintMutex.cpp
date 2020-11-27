#include <iostream>   
#include <windows.h>   
using namespace std;
 
HANDLE hMutex = NULL;//�������ľ��

DWORD WINAPI Print(LPVOID lpParamter)
{
    std::string s = (char*)lpParamter;

    for (int i = 0; i < 10; i++)
    {
        WaitForSingleObject(hMutex, INFINITE);//������
        cout << s << endl;
        ReleaseMutex(hMutex);//�ͷ���
    }

    return 0;
}
 
int main()
{
    std::string s1 = "Work thread";
    std::string s2 = "Main thread";

    hMutex = CreateMutex(NULL, FALSE, NULL); //����������
    HANDLE hThread = CreateThread(NULL, 0, Print, (LPVOID)s1.c_str(), 0, NULL);
    Print((LPVOID)s2.c_str());
	
    CloseHandle(hThread);
    CloseHandle(hMutex);//���ٻ�����
    
    return 0;
}