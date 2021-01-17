
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include<tchar.h>
#include <cstdio>
#include <cmath>
#include <string>

#define  PI 3.1415926

using namespace std;
HANDLE Hprocess = NULL;

DWORD baseAddress = 0x04047694;
DWORD firstoffset = 0x504;
DWORD c;
DWORD myHpAddress;
DWORD myXAddress;
DWORD myYAddress;
DWORD myZAddress;
DWORD mouseYAddress = 0x484A044;
DWORD mouseXAddress = 0x484A044 + 0x4;
DWORD TBase = 0x04035C04;
int TCount;
int CTCount;
DWORD fovAddress = 0x03987584;

HDC hDC;

LPCWSTR stringToLPCWSTR(std::string orig)
{
    size_t origsize = orig.length() + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
    mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);

    return wcstring;
}
float* getXY(DWORD ea) {
    int xy[2];
    float ix;
    float iy;
    float iz;
    float ex;
    float ey;
    float ez;
    float mouseY;
    float mouseX;
    float fov;

    ReadProcessMemory(Hprocess, LPVOID(fovAddress), &fov, sizeof(DWORD), 0);
    ReadProcessMemory(Hprocess, LPVOID(myXAddress), &ix, sizeof(DWORD), 0);
    ReadProcessMemory(Hprocess, LPVOID(myYAddress), &iy, sizeof(DWORD), 0);
    ReadProcessMemory(Hprocess, LPVOID(myZAddress), &iz, sizeof(DWORD), 0);
    ReadProcessMemory(Hprocess, LPVOID(ea- 0x9c), &ex, sizeof(DWORD), 0);
    ReadProcessMemory(Hprocess, LPVOID(ea - 0x9c + 0x4), &ey, sizeof(DWORD), 0);
    ReadProcessMemory(Hprocess, LPVOID(ea - 0x9c + 0x4 + 0x4), &ez, sizeof(DWORD), 0);
    ReadProcessMemory(Hprocess, LPVOID(mouseXAddress), &mouseX, sizeof(DWORD), 0);
    ReadProcessMemory(Hprocess, LPVOID(mouseYAddress), &mouseY, sizeof(DWORD), 0);
    mouseY = -mouseY;

    float juli = abs(sqrt(pow(ix - ex, 2) + pow(iy - ey, 2)));
    float ememyToX = 0; //敌人与x的夹角

    //敌人在我第一象限
    if (ix - ex < 0 && iy - ey < 0) {
        float x = ex - ix;
        float y = ey - iy;
        ememyToX = atan2(y, x) * 180 / PI;
    }

    //第二象限
    if (ix - ex > 0 && iy - ey < 0) {
        float x = ix - ex;
        float y = ey - iy;
        ememyToX = 180 - (atan2(y, x) * 180 / PI);
    }

    //第三象限
    if (ix - ex > 0 && iy - ey > 0) {
        float x = ix - ex;
        float y = iy - ey;
        ememyToX = 180 + (atan2(y, x) * 180 / PI);
    }

    //第四象限
    if (ix - ex < 0 && iy - ey > 0) {
        float x = ex - ix;
        float y = iy - ey;
        ememyToX = 360 - (atan2(y, x) * 180 / PI);
    }

    if (ix - ex == 0 && iy - ey > 0) {
        ememyToX = 270;
    }
    if (ix - ex == 0 && iy - ey < 0) {
        ememyToX = 90;
    }

    if (ix - ex < 0 && iy - ey == 0) {
        ememyToX = 0;
    }
    if (ix - ex > 0 && iy - ey == 0) {
        ememyToX = 180;
    }

    if (mouseX < 0)
    {
        mouseX = 360 + mouseX;
    }

    float mouseToEmemy = abs(mouseX - ememyToX);//鼠标与敌人的夹角


    int i = 1;

    //夹角大于180
    if (mouseToEmemy > 180)
    {
        mouseToEmemy = 360 - mouseToEmemy;

        i = -1;
    }

    if ((int)mouseToEmemy > 45) {
        return NULL;

    }


    float x;

    //敌人在右侧
    if ((ememyToX < mouseX && i == 1) || (ememyToX > mouseX && i == -1))
    {
        x = (1366 / 2) + tan(mouseToEmemy * PI / 180.0f) * (1366 / 2) * (90 / fov);
    }
    else
    {
        x = (1366 / 2) - tan(mouseToEmemy * PI / 180.0f) * (1366 / 2) * (90 / fov);
    }
    float h = ez - iz;
    float ememyToI = atan2(h, juli) * 180 / PI;


    float y;
    float mouseToEmemyZ = abs(mouseY - ememyToI);
    //敌人在视角下方
    if (mouseY > ememyToI)
    {
        y = (768 / 2) + tan(mouseToEmemyZ * PI / 180.0f) * (1366 / 2) * (90 / fov);

    }
    else
    {
        y = (768 / 2) - tan(mouseToEmemyZ * PI / 180.0f) * (1366 / 2) * (90 / fov);
    }
    string s = to_string(juli);
    LPCWSTR pStr = stringToLPCWSTR(s);
    TextOut(hDC, x, y, reinterpret_cast<LPCSTR>(pStr), 5);


}


int main()
{
    HWND h = ::FindWindow(NULL, _T("Condition Zero"));//获取窗口句柄
    DWORD processid;
    ::GetWindowThreadProcessId(h, &processid);//获取进程id
    printf("进程id为%d \n", processid);

    Hprocess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processid);//打开进程
    if (Hprocess == 0)
        printf("打开进程失败！\n");

    else
    {
        hDC = GetDC(h);
        printf("打开进程成功！%d\n",Hprocess);



        ReadProcessMemory(Hprocess, LPVOID(0x04035C04), &TBase, sizeof(DWORD), 0);
        ReadProcessMemory(Hprocess, LPVOID(TBase+ 0x49C), &TBase, sizeof(DWORD), 0);
        ReadProcessMemory(Hprocess, LPVOID(TBase + 0x22C), &TBase, sizeof(DWORD), 0);
        ReadProcessMemory(Hprocess, LPVOID(TBase + 0x4EC), &TBase, sizeof(DWORD), 0);
        ReadProcessMemory(Hprocess, LPVOID(TBase + 0x71C), &TBase, sizeof(DWORD), 0);
        ReadProcessMemory(Hprocess, LPVOID(TBase + 0x54), &TCount, sizeof(DWORD), 0);
        ReadProcessMemory(Hprocess, LPVOID(TBase + 0x54 + 0x4), &CTCount, sizeof(DWORD), 0);


        while (true)
        {
            for (size_t i = 1; i <= TCount; i++)
            {
                ReadProcessMemory(Hprocess, LPVOID(baseAddress), &c, sizeof(DWORD), 0);
                myHpAddress = c + firstoffset;
                myXAddress = myHpAddress - 0x9c;
                myYAddress = myXAddress + 0x4;
                myZAddress = myYAddress + 0x4;
                getXY(myHpAddress + (0x324 * i));
            }


        };

        ReleaseDC(h, hDC);

    }
    return 0;
}

