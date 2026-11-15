#pragma warning(push)
#pragma warning(disable:4005)
#include "windowhelper.h"
#pragma warning(pop)
#pragma once
std::map<HWND,HINSTANCE> hwnd_hinst_mp;
ATOM MyRegisterClass(HINSTANCE hInstance,WCHAR szWindowClass[],WndProc_t WndProc)
{
    WNDCLASSEXW wcex;

    wcex.cbSize=sizeof(WNDCLASSEX);

    wcex.style=CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc=WndProc;
    wcex.cbClsExtra=0;
    wcex.cbWndExtra=0;
    wcex.hInstance=hInstance;
    wcex.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MY4));
    wcex.hCursor=LoadCursor(nullptr,IDC_ARROW);
    wcex.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName=MAKEINTRESOURCEW(IDC_MY4);
    wcex.lpszClassName=szWindowClass;
    wcex.hIconSm=LoadIcon(wcex.hInstance,MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
#include "direct3d.h"
BOOL InitInstance(HINSTANCE hinst,int nCmdShow,HWND& hwnd,WCHAR szWindowClass[],WCHAR szTitle[])
{
    hwnd=CreateWindowW(szWindowClass,szTitle,WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT,0,W,H,nullptr,nullptr,hinst,nullptr);
    if(!hwnd) return FALSE;
    ShowWindow(hwnd,nCmdShow);
    UpdateWindow(hwnd);
    hwnd_hinst_mp[hwnd]=hinst;
    return TRUE;
}
