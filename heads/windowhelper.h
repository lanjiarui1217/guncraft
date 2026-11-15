#pragma once
#include "framework.h"
#include "../Resource.h"
typedef LRESULT CALLBACK WndProc_t(HWND,UINT,WPARAM,LPARAM);

// 从 hwnd 到 hinst 的映射
extern std::map<HWND,HINSTANCE> hwnd_hinst_mp;

// (hinst,ClassName,WndProc) 创建实例
ATOM MyRegisterClass(HINSTANCE hInstance,WCHAR szWindowClass[],WndProc_t WndProc);

// 创建窗口，赋给 hwnd 并创建映射。
BOOL InitInstance(HINSTANCE hinst,int nCmdShow,HWND& hwnd,WCHAR szWindowClass[],WCHAR szTitle[]);
