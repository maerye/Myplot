// MyPlot.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "MyPlot.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <commdlg.h>
#include<stack>
#include<queue>
#include"calculator.h"
#include <fstream>
#include<string>
#include <sstream>
#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
wchar_t szPathName[6][MAX_PATH] = { 0 };
INT filenum=-1;
//LPCWSTR OPENPATH=NULL;
LONG POS_X;//原点x
LONG POS_Y;//原点y
COLORREF BG_COLOR=RGB(248 ,248 ,255);
COLORREF LINE_COLOR=RGB(0,0,0);

FLOAT XTICK_DIS = 1;
FLOAT YTICK_DIS = 1;
FLOAT XRangeFrom = -20;
FLOAT XRangeTo = 20;
FLOAT YRangeFrom = -20;
FLOAT YRangeTo = 20;
INT LableEveryXTicks = 5;
INT LableEveryYTicks = 5;
BOOL SHOW_GRID = true;
BOOL SHOW_AXIS = true;
BOOL ISIMPORT=false;
BOOL pt = false;
LONG PLOTRIGHT;
LONG PLOTBOTTOM;

HWND hwnd1;
HWND hwndtogo= NULL;
TCHAR fx1[30];
TCHAR fx2[30];
TCHAR fx3[30];
TCHAR fx4[30];
TCHAR fx5[30];
TCHAR fx6[30];
std::queue<TCHAR>q1;
std::queue<TCHAR>q2;
std::queue<TCHAR>q3;
std::queue<TCHAR>q4;
std::queue<TCHAR>q5;
std::queue<TCHAR>q6;
TCHAR  buf[30];
COLORREF LINE_COLOR1 = RGB(0, 0, 0);
COLORREF LINE_COLOR2 = RGB(0, 0, 0);
COLORREF LINE_COLOR3 = RGB(0, 0, 0);
COLORREF LINE_COLOR4 = RGB(0, 0, 0);
COLORREF LINE_COLOR5 = RGB(0, 0, 0);
COLORREF LINE_COLOR6 = RGB(0, 0, 0);

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    PlotItem(HWND, UINT, WPARAM, LPARAM);
//double f(double x);
void PaintAxis(HWND hwnd, HDC hdc, PAINTSTRUCT ps);
void PaintGrid(HWND hwnd, HDC hdc, PAINTSTRUCT ps);
void Plot(HWND hwnd, HDC hdc, PAINTSTRUCT ps, std::queue<TCHAR>q, COLORREF color);
void Plotsin(HWND hwnd, HDC hdc, PAINTSTRUCT ps, COLORREF color);
COLORREF cscolor();
int isp(TCHAR ch);
int icp(TCHAR ch);
std::queue<TCHAR> Convert(TCHAR* fx);
double f(std::queue<TCHAR>rpn, double x);
BOOL plotdata(HWND hwnd,LPCWSTR path);
void save(LPCWSTR,HWND);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MYPLOT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYPLOT));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYPLOT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYPLOT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   hwnd1 = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SCROLLINFO si;
	static int xCurrentScroll;
	static int xMaxScroll;
	static int yCurrentScroll;
	static int yMaxScroll;
	switch (message)
	{
	case WM_CREATE:
	{	RECT pr;
	GetClientRect(hWnd, &pr);
	long width = pr.right;
	long height = pr.bottom;

	xCurrentScroll = width / 2;
	xMaxScroll = 1000;
	yCurrentScroll = height / 2;
	yMaxScroll = 1000;
	break;
	}
	case WM_SIZE:
		int xNewSize;
		int yNewSize;

		xNewSize = LOWORD(lParam);
		yNewSize = HIWORD(lParam);
		xCurrentScroll = xNewSize / 2;
		xMaxScroll = xNewSize;
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = 0;
		si.nMax = xNewSize;
		si.nPage = xNewSize/16;
		si.nPos = xCurrentScroll;
		SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);

		yMaxScroll = yNewSize;
		yCurrentScroll = yNewSize / 2;
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = 0;
		si.nMax = yNewSize;
		si.nPage = yNewSize/16;
		si.nPos = yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		break;
	case WM_HSCROLL:
	{
		int xDelta;     // xDelta = new_pos - current_pos  
		int xNewPos;    // new position 
		int yDelta = 0;
		int move;
		switch (LOWORD(wParam))
		{
			// User clicked the scroll bar shaft left of the scroll box. 
		case SB_PAGEUP:
			xNewPos = xCurrentScroll - 50;
			XRangeFrom -= 10;
			XRangeTo -= 10;
			break;
			// User clicked the scroll bar shaft right of the scroll box. 
		case SB_PAGEDOWN:
			xNewPos = xCurrentScroll + 50;
			XRangeFrom += 10;
			XRangeTo += 10;
			break;
			// User clicked the left arrow. 
		case SB_LINEUP:
			xNewPos = xCurrentScroll - 5;
			XRangeFrom -= 3;
			XRangeTo -= 3;
			break;
			// User clicked the right arrow. 
		case SB_LINEDOWN:
			xNewPos = xCurrentScroll + 5;
			XRangeFrom += 3;
			XRangeTo += 3;
			break;
			// User dragged the scroll box. 
		case SB_THUMBPOSITION:
		{	//xNewPos = HIWORD(wParam);
			xNewPos = HIWORD(wParam);			
			 move = xNewPos - xCurrentScroll;
			XRangeFrom += move/5;
			XRangeTo += move/5;
       }
			break;
		default:
			xNewPos = xCurrentScroll;		
				break;
		}
			xNewPos = max(0, xNewPos);
			xNewPos = min(xMaxScroll, xNewPos);
			
			if (xNewPos == xCurrentScroll)
			xDelta = xNewPos - xCurrentScroll;

			// Reset the current scroll position. 
			xCurrentScroll = xNewPos;

			ScrollWindowEx(hWnd, -xDelta, -yDelta, (CONST RECT *) NULL,
				(CONST RECT *) NULL, (HRGN)NULL, (PRECT)NULL,
				SW_INVALIDATE);
			SendMessage(hWnd, WM_PAINT, NULL, NULL);

			// Reset the scroll bar. 
			si.cbSize = sizeof(si);
			si.fMask = SIF_POS;
			si.nPos = xCurrentScroll;
			SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
		
		break;
	}
	case WM_VSCROLL:
	{
		int xDelta = 0;
		int yDelta;     // yDelta = new_pos - current_pos 
		int yNewPos;    // new position 
		int move;
		switch (LOWORD(wParam))
		{
			// User clicked the scroll bar shaft above the scroll box. 
		case SB_PAGEUP:
			yNewPos = yCurrentScroll - 50;
			YRangeFrom += 10;
			YRangeTo += 10;
			break;

			// User clicked the scroll bar shaft below the scroll box. 
		case SB_PAGEDOWN:
			yNewPos = yCurrentScroll + 50;
			YRangeFrom -= 10;
			YRangeTo -= 10;
			break;

			// User clicked the top arrow. 
		case SB_LINEUP:
			yNewPos = yCurrentScroll - 5;
			YRangeFrom += 3;
			YRangeTo += 3;
			break;

			// User clicked the bottom arrow. 
		case SB_LINEDOWN:
			yNewPos = yCurrentScroll + 5;
			YRangeFrom -= 3;
			YRangeTo -= 3;
			break;

			// User dragged the scroll box. 
		case SB_THUMBPOSITION: {
			yNewPos = HIWORD(wParam);
			 move = yNewPos - yCurrentScroll;
			YRangeFrom -= move/5;
			YRangeTo -= move/5;
		}
			break;

		default:
			yNewPos = yCurrentScroll;
		}

		yNewPos = max(0, yNewPos);
		yNewPos = min(yMaxScroll, yNewPos);

		// If the current position does not change, do not scroll.
		if (yNewPos == yCurrentScroll)
			break;

		// Determine the amount scrolled (in pixels). 
		yDelta = yNewPos - yCurrentScroll;

		// Reset the current scroll position. 
		yCurrentScroll = yNewPos;

		ScrollWindowEx(hWnd, -xDelta, -yDelta, (CONST RECT *) NULL,
			(CONST RECT *) NULL, (HRGN)NULL, (PRECT)NULL,
			SW_INVALIDATE);
		SendMessage(hWnd, WM_PAINT, NULL, NULL);

		// Reset the scroll bar. 
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		break;
	}
	case WM_MOUSEWHEEL:
	{
		INT upordown = GET_WHEEL_DELTA_WPARAM(wParam);
	if (upordown > 0)
	{
		//zoom in
		if(abs(XRangeFrom)>1)XRangeFrom +=1;
		else XRangeFrom += 0.1;
		if(abs(XRangeTo>1))XRangeTo -=1;
		else XRangeTo -= 0.1;
		if(abs(YRangeFrom)>1)YRangeFrom +=1;
		else YRangeFrom += 0.1;
		if(abs(YRangeTo)>1)		YRangeTo -=1;
		else YRangeTo -= 0.1;
		SendMessage(hWnd, WM_PAINT, NULL, NULL);
	}
	else if (upordown < 0)
	{
		//zoom out
		XRangeFrom -= 1;
		XRangeTo += 1;
		YRangeFrom -= 1;
		YRangeTo += 1;
		SendMessage(hWnd, WM_PAINT, NULL, NULL);
	}

	//return (INT_PTR)TRUE;
	break;
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
			switch (wmId)
			{
			case IDM_IMPORT:
			{
				filenum++;
				OPENFILENAME ofn = { OPENFILENAME_SIZE_VERSION_400 };//or  {sizeof (OPENFILENAME)}    
				ofn.hwndOwner = GetForegroundWindow();          // 拥有者句柄      
																// 过滤器,以\0相隔: 显示名称\0过滤器\0显示名称\0过滤器\0\0  
				ofn.lpstrFilter = TEXT("csv文件(*.csv)\0*.csv\0所有文件(*.*)\0*.*\0\0");
				ofn.lpstrFile = szPathName[filenum];             // 存放用户选择文件的 路径及文件名 缓冲区   
				ofn.nMaxFile = sizeof(szPathName);      // 缓冲区大小,单位为字节，至少256  
				ofn.lpstrTitle = TEXT("选择文件");      // 选择文件对话框标题    
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST /*| OFN_ALLOWMULTISELECT 允许选择多个文件*/;

				BOOL bOk = GetOpenFileName(&ofn);
				if (!bOk)
				{
					return FALSE;
				}

				if (plotdata(hWnd, szPathName[filenum]))
					ISIMPORT = true;
				break;
			}
			case IDM_CIMPORT:
				ISIMPORT = false;
				SendMessage(hWnd, WM_PAINT, NULL, NULL);
				break;
			case IDM_PLOT:
				hwndtogo = CreateDialog(hInst, MAKEINTATOM(IDD_PLOTBOX), hWnd, PlotItem);
				ShowWindow(hwndtogo, SW_SHOW);
				break;
			case IDM_SAVE:
			{
				wchar_t szPathName[MAX_PATH] = { 0 };
				OPENFILENAME ofn = { OPENFILENAME_SIZE_VERSION_400 };//or  {sizeof (OPENFILENAME)}    
				ofn.hwndOwner = GetForegroundWindow();          // 拥有者句柄      
				wcscpy_s(szPathName, TEXT("haha.bmp"));      // 定义预先的存储文件名  
				ofn.lpstrFilter = TEXT("bmp文件(*.bmp)\0*.bmp\0所有文件(*.*)\0*.*\0\0");
				ofn.lpstrFile = szPathName;
				ofn.nMaxFile = sizeof(szPathName);
				ofn.lpstrTitle = TEXT("保存文件");
			//	ofn.Flags = OFN_OVERWRITEPROMPT;        // 覆盖提示  

				BOOL bOk = GetSaveFileName(&ofn);
				if (!bOk)
				{
					return FALSE;
				}
				
		
				save(szPathName, hWnd);
				break;
			}
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
			InvalidateRect(hWnd, NULL, TRUE);
			
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			HBRUSH hbrush = CreateSolidBrush(BG_COLOR);
			SelectObject(hdc, hbrush);
			RECT  area;
			GetClientRect(hWnd, &area);
			FillRect(hdc, &area, hbrush);
			DeleteObject(hbrush);

			if(SHOW_GRID)   PaintGrid(hWnd, hdc, ps);
			if(SHOW_AXIS)   PaintAxis(hWnd, hdc, ps);
			if (ISIMPORT) {
				for (int i = -1;i < filenum;i++)
				plotdata(hWnd, szPathName[i+1]); 
			}
			if(!pt)Plot(hWnd, hdc, ps,q1,LINE_COLOR1);
			else { Plotsin(hWnd, hdc, ps, LINE_COLOR1); }
			Plot(hWnd, hdc, ps, q2, LINE_COLOR2);
			Plot(hWnd, hdc, ps, q3, LINE_COLOR3);
			Plot(hWnd, hdc, ps, q4, LINE_COLOR4);
			Plot(hWnd, hdc, ps, q5, LINE_COLOR5);
			Plot(hWnd, hdc, ps, q6, LINE_COLOR6);

			EndPaint(hWnd, &ps); 
			break;
        }


    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
INT_PTR CALLBACK PlotItem(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{   
		
	return (INT_PTR)TRUE;
	}
	case WM_PAINT:
	{
	HWND bt;
	HDC btdc;
	HBRUSH hbrush;
	RECT rect;
	
	bt = GetDlgItem(hDlg, IDC_BUTTON1);
	btdc = GetDC(bt);
	hbrush = CreateSolidBrush(LINE_COLOR1);
	SelectObject(btdc, hbrush);
	GetClientRect(bt, &rect);
	FillRect(btdc, &rect, hbrush);
	DeleteObject(hbrush);
	
	bt = GetDlgItem(hDlg, IDC_BUTTON2);
	btdc = GetDC(bt);
	hbrush = CreateSolidBrush(LINE_COLOR2);
	SelectObject(btdc, hbrush);
	GetClientRect(bt, &rect);
	FillRect(btdc, &rect, hbrush);
	DeleteObject(hbrush);

	bt = GetDlgItem(hDlg, IDC_BUTTON3);
	btdc = GetDC(bt);
	hbrush = CreateSolidBrush(LINE_COLOR3);
	SelectObject(btdc, hbrush);
	GetClientRect(bt, &rect);
	FillRect(btdc, &rect, hbrush);
	DeleteObject(hbrush);

	bt = GetDlgItem(hDlg, IDC_BUTTON4);
	btdc = GetDC(bt);
	hbrush = CreateSolidBrush(LINE_COLOR4);
	SelectObject(btdc, hbrush);
	GetClientRect(bt, &rect);
	FillRect(btdc, &rect, hbrush);
	DeleteObject(hbrush);

	bt = GetDlgItem(hDlg, IDC_BUTTON5);
	btdc = GetDC(bt);
	hbrush = CreateSolidBrush(LINE_COLOR5);
	SelectObject(btdc, hbrush);
	GetClientRect(bt, &rect);
	FillRect(btdc, &rect, hbrush);
	DeleteObject(hbrush);

	bt = GetDlgItem(hDlg, IDC_BUTTON6);
	btdc = GetDC(bt);
	hbrush = CreateSolidBrush(LINE_COLOR6);
	SelectObject(btdc, hbrush);
	GetClientRect(bt, &rect);
	FillRect(btdc, &rect, hbrush);
	DeleteObject(hbrush);

	bt = GetDlgItem(hDlg, IDC_BUTTON7);
	btdc = GetDC(bt);
	hbrush = CreateSolidBrush(BG_COLOR);
	SelectObject(btdc, hbrush);
	GetClientRect(bt, &rect);
	FillRect(btdc, &rect, hbrush);
	DeleteObject(hbrush);

	break;
	}
	case WM_LBUTTONDOWN:
		SendMessage(hDlg, WM_PAINT, NULL, NULL);
		break;
	case WM_COMMAND:
		SendMessage(hDlg, WM_PAINT, NULL, NULL);
		if ( LOWORD(wParam) == IDCANCEL)
		{
			//EndDialog(hDlg, LOWORD(wParam));
			DestroyWindow(hwndtogo);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDOK)
		{
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			GetDlgItemText(hDlg, IDC_EDIT1, fx1, 30);
		
			size_t funbufsize = wcslen(fx1) + 1;
			size_t convertedChars = 0;
			const size_t newsize1 = funbufsize * 2;
			char* nstr = new char[newsize1];
			wcstombs_s(&convertedChars, nstr, newsize1, fx1, _TRUNCATE);
			string ns(nstr);

			if (ns!="sin(x)")q1 = Convert(fx1);
			else {
				pt = true;
			}
			GetDlgItemText(hDlg, IDC_EDIT2, fx2, 30);
			q2 = Convert(fx2);
			GetDlgItemText(hDlg, IDC_EDIT3, fx3, 30);
			q3 = Convert(fx3);
			GetDlgItemText(hDlg, IDC_EDIT4, fx4, 30);
			q4 = Convert(fx4);
			GetDlgItemText(hDlg, IDC_EDIT5, fx5, 30);
			q5 = Convert(fx5);
			GetDlgItemText(hDlg, IDC_EDIT6, fx6, 30);
			q6 = Convert(fx6);
		
			if(GetDlgItemText(hDlg, IDC_EDIT7, buf, 30)!=0)		
			 XRangeFrom = _wtof(buf);
			if(GetDlgItemText(hDlg, IDC_EDIT8, buf, 30)!=0)
			  XRangeTo = _wtof(buf);
			
			 if(GetDlgItemText(hDlg, IDC_EDIT9, buf, 30)!=0)
			 YRangeFrom = _wtof(buf);
			 if(GetDlgItemText(hDlg, IDC_EDIT10, buf, 30) != 0)
			 YRangeTo = _wtof(buf);

			 if(GetDlgItemText(hDlg, IDC_EDIT11, buf, 30) != 0)
			 XTICK_DIS= _wtof(buf);
			 if (XTICK_DIS < 0)
				 XTICK_DIS = -XTICK_DIS;
			if(GetDlgItemText(hDlg, IDC_EDIT12, buf, 30)!=0)
			 YTICK_DIS = _wtof(buf);
			 if (YTICK_DIS < 0)
				 YTICK_DIS = -YTICK_DIS;			 
			
			 INT temp;			
			 temp = GetDlgItemInt(hDlg, IDC_EDIT13, NULL, TRUE);
			 if (temp <=0)
			 {
				 LableEveryXTicks = 5;
			 }
			 else {
				 LableEveryXTicks = temp;
			 }

			temp = GetDlgItemInt(hDlg, IDC_EDIT14, NULL, TRUE);
			 if (temp <=0)
			 {
				 LableEveryYTicks = 5;
			 }
			 else {
				 LableEveryYTicks = temp;
			 }
			bool checked = IsDlgButtonChecked(hDlg, IDC_CHECK1);
			if (checked)
				SHOW_GRID = true;
			else
				SHOW_GRID= false;

			 checked =IsDlgButtonChecked(hDlg, IDC_CHECK2);
			if (checked)
				SHOW_AXIS = true;
			else
				SHOW_AXIS = false;

			SendMessage(hwnd1, WM_PAINT,NULL,NULL);
			
		//	DestroyWindow(hwndtogo);
			//return (INT_PTR)TRUE;
			
			//return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam)==IDC_BUTTON1)
		{
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			COLORREF color=cscolor();
			LINE_COLOR1= color;
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			SendMessage(hwnd1, WM_PAINT, NULL, NULL);
		}
		else if (LOWORD(wParam) == IDC_BUTTON2)
		{
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			COLORREF color = cscolor();
			LINE_COLOR2 = color;
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			SendMessage(hwnd1, WM_PAINT, NULL, NULL);
		}
		else if (LOWORD(wParam) == IDC_BUTTON3)
		{
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			COLORREF color = cscolor();
			LINE_COLOR3 = color;
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			SendMessage(hwnd1, WM_PAINT, NULL, NULL);
		}
		else if (LOWORD(wParam) == IDC_BUTTON4)
		{
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			COLORREF color = cscolor();
			LINE_COLOR4 = color;
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			SendMessage(hwnd1, WM_PAINT, NULL, NULL);
		}
		else if (LOWORD(wParam) == IDC_BUTTON5)
		{
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			COLORREF color = cscolor();
			LINE_COLOR5 = color;
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			SendMessage(hwnd1, WM_PAINT, NULL, NULL);
		}
		else if (LOWORD(wParam) == IDC_BUTTON6)
		{
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			COLORREF color = cscolor();
			LINE_COLOR6 = color;
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			SendMessage(hwnd1, WM_PAINT, NULL, NULL);
		}
		else if (LOWORD(wParam) == IDC_BUTTON7)
		{
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			COLORREF color = cscolor();
			BG_COLOR = color;
			SendMessage(hDlg, WM_PAINT, NULL, NULL);
			SendMessage(hwnd1, WM_PAINT, NULL, NULL);
		}
		break;

	}
	return (INT_PTR)FALSE;
}
void PaintAxis(HWND hwnd, HDC hdc, PAINTSTRUCT ps)
{
	RECT plotarea;
	HPEN hpen;
	TCHAR buf[1000];
	hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

	SelectObject(hdc, hpen);
	GetClientRect(hwnd, &plotarea);

	PLOTRIGHT = plotarea.right;
	PLOTBOTTOM = plotarea.bottom;

	POS_X = -XRangeFrom*PLOTRIGHT / (-XRangeFrom + XRangeTo);
	POS_Y = YRangeTo*PLOTBOTTOM / (-YRangeFrom + YRangeTo);

	MoveToEx(hdc, 0, POS_Y, NULL);
	LineTo(hdc, PLOTRIGHT, POS_Y);
	MoveToEx(hdc, POS_X, PLOTBOTTOM, NULL);
	LineTo(hdc, POS_X, 0);


	FLOAT intervalnuml = -XRangeFrom / XTICK_DIS;
	INT iintervalnuml = (INT)intervalnuml;
	FLOAT leftintervalnuml = intervalnuml - iintervalnuml;

	FLOAT intervalnumr = XRangeTo / XTICK_DIS;
	INT iintervalnumr = (INT)intervalnumr;
	FLOAT leftintervalnumr = intervalnumr - iintervalnumr;

	FLOAT intervalnumup = YRangeTo / YTICK_DIS;
	INT iintervalnumup = (INT)intervalnumup;
	FLOAT  leftintervalnumup = intervalnumup - iintervalnumup;

	FLOAT intervalnumdn = -YRangeFrom / YTICK_DIS;
	INT iintervalnumdn = (INT)intervalnumdn;
	FLOAT leftintervalnumdn = intervalnumdn - iintervalnumdn;

	INT intervalx = PLOTRIGHT / (intervalnuml + intervalnumr);
	INT intervaly = PLOTBOTTOM / (intervalnumup + intervalnumdn);

	for (int i = 0;i < iintervalnuml;i++)
	{
		MoveToEx(hdc, POS_X - (i + 1)*intervalx, POS_Y - 3, NULL);
		LineTo(hdc, POS_X - (i + 1)*intervalx, POS_Y + 3);
		if ((i + 1) % LableEveryXTicks == 0)
		{
			swprintf(buf, L"-%-2.1f", ((FLOAT)i + 1)*XTICK_DIS);
			TextOut(hdc, POS_X - (i + 1)*intervalx, POS_Y + 6, buf, wcslen(buf));
		}
	}

	for (int i = 0;i < iintervalnumr;i++)
	{
		MoveToEx(hdc, POS_X + (i + 1)*intervalx, POS_Y - 3, NULL);
		LineTo(hdc, POS_X + (i + 1)*intervalx, POS_Y + 3);
		if ((i + 1) % LableEveryXTicks == 0)
		{
			swprintf(buf, L"%-2.1f", ((FLOAT)i + 1)*XTICK_DIS);
			TextOut(hdc, POS_X + (i + 1)*intervalx, POS_Y + 6, buf, wcslen(buf));
		}
	}
	for (int i = 0;i < iintervalnumup;i++)
	{
		MoveToEx(hdc, POS_X - 3, POS_Y - (i + 1)*intervaly, NULL);
		LineTo(hdc, POS_X + 3, POS_Y - (i + 1)*intervaly);
		if ((i + 1) % LableEveryYTicks == 0)
		{
			swprintf(buf, L"%2.1f", ((FLOAT)i + 1)*YTICK_DIS);
			TextOut(hdc, POS_X + 6, POS_Y - (i + 1)*intervaly, buf, wcslen(buf));
		}
	}
	for (int i = 0;i < iintervalnumdn;i++)
	{
		MoveToEx(hdc, POS_X - 3, POS_Y + (i + 1)*intervaly, NULL);
		LineTo(hdc, POS_X + 3, POS_Y + (i + 1)*intervaly);
		if ((i + 1) % LableEveryYTicks == 0)
		{
			swprintf(buf, L"-%2.1f", ((FLOAT)i + 1)*YTICK_DIS);
			TextOut(hdc, POS_X + 6, POS_Y + (i + 1)*intervaly, buf, wcslen(buf));
		}
	}


	DeleteObject(hpen);
}
void PaintGrid(HWND hwnd, HDC hdc, PAINTSTRUCT ps)
{
	RECT plotarea;
	HPEN hpen;
	TCHAR buf[1000];
	hpen = CreatePen(PS_SOLID, 1, RGB(176, 224, 230));

	SelectObject(hdc, hpen);
	GetClientRect(hwnd, &plotarea);
	PLOTRIGHT = plotarea.right;
	PLOTBOTTOM = plotarea.bottom;

	POS_X = -XRangeFrom*PLOTRIGHT / (-XRangeFrom + XRangeTo);
	POS_Y = YRangeTo*PLOTBOTTOM / (-YRangeFrom + YRangeTo);

	FLOAT intervalnuml = -XRangeFrom / XTICK_DIS;
	INT iintervalnuml = (INT)intervalnuml;
	FLOAT leftintervalnuml = intervalnuml - iintervalnuml;

	FLOAT intervalnumr = XRangeTo / XTICK_DIS;
	INT iintervalnumr = (INT)intervalnumr;
	FLOAT leftintervalnumr = intervalnumr - iintervalnumr;

	FLOAT intervalnumup = YRangeTo / YTICK_DIS;
	INT iintervalnumup = (INT)intervalnumup;
	FLOAT  leftintervalnumup = intervalnumup - iintervalnumup;

	FLOAT intervalnumdn = -YRangeFrom / YTICK_DIS;
	INT iintervalnumdn = (INT)intervalnumdn;
	FLOAT leftintervalnumdn = intervalnumdn - iintervalnumdn;

	INT intervalx = PLOTRIGHT / (intervalnuml + intervalnumr);
	INT intervaly = PLOTBOTTOM / (intervalnumup + intervalnumdn);

	MoveToEx(hdc, 0, POS_Y, NULL);
	LineTo(hdc, PLOTRIGHT, POS_Y);
	
	MoveToEx(hdc, POS_X, 0, NULL);
	LineTo(hdc, POS_X, PLOTBOTTOM);
	for (int i = 0;i < iintervalnuml;i++)
	{
		MoveToEx(hdc, POS_X - (i + 1)*intervalx, 0, NULL);
		LineTo(hdc, POS_X - (i + 1)*intervalx, PLOTBOTTOM);
	}

	for (int i = 0;i < iintervalnumr;i++)
	{
		MoveToEx(hdc, POS_X + (i + 1)*intervalx, 0, NULL);
		LineTo(hdc, POS_X + (i + 1)*intervalx, PLOTBOTTOM);
	}
	for (int i = 0;i < iintervalnumup;i++)
	{
		MoveToEx(hdc, 0, POS_Y - (i + 1)*intervaly, NULL);
		LineTo(hdc, PLOTRIGHT, POS_Y - (i + 1)*intervaly);
	}
	for (int i = 0;i < iintervalnumdn;i++)
	{
		MoveToEx(hdc, 0, POS_Y + (i + 1)*intervaly, NULL);
		LineTo(hdc, PLOTRIGHT, POS_Y + (i + 1)*intervaly);
	}
	DeleteObject(hpen);
}
void Plot(HWND hwnd, HDC hdc, PAINTSTRUCT ps,std::queue<TCHAR>q,COLORREF color)
{
	POINT* points;
	POINT point;
	RECT plotarea;
	HPEN hpen;
	TCHAR buf[1000];

	hpen = CreatePen(PS_SOLID, 1, color);
	SelectObject(hdc, hpen);

	GetClientRect(hwnd, &plotarea);

	PLOTRIGHT = plotarea.right;
	PLOTBOTTOM = plotarea.bottom;

	POS_X = -XRangeFrom*PLOTRIGHT / (-XRangeFrom + XRangeTo);
	POS_Y = YRangeTo*PLOTBOTTOM / (-YRangeFrom + YRangeTo);

	FLOAT intervalnuml = -XRangeFrom / XTICK_DIS;
	INT iintervalnuml = (INT)intervalnuml;
	FLOAT leftintervalnuml = intervalnuml - iintervalnuml;

	FLOAT intervalnumr = XRangeTo / XTICK_DIS;
	INT iintervalnumr = (INT)intervalnumr;
	FLOAT leftintervalnumr = intervalnumr - iintervalnumr;

	FLOAT intervalnumup = YRangeTo / YTICK_DIS;
	INT iintervalnumup = (INT)intervalnumup;
	FLOAT  leftintervalnumup = intervalnumup - iintervalnumup;

	FLOAT intervalnumdn = -YRangeFrom / YTICK_DIS;
	INT iintervalnumdn = (INT)intervalnumdn;
	FLOAT leftintervalnumdn = intervalnumdn - iintervalnumdn;

	INT intervalx = PLOTRIGHT / (intervalnuml + intervalnumr);
	INT intervaly = PLOTBOTTOM / (intervalnumup + intervalnumdn);
	
	//std::queue<TCHAR>q=Convert(fx);
	points = new POINT[PLOTRIGHT];
	for (LONG i = 0;i < PLOTRIGHT;i++)
	{
		FLOAT xcoordirate = (i - POS_X)*XTICK_DIS / intervalx;
		FLOAT ycoordirate = f(q, xcoordirate);

		FLOAT temp = ycoordirate*intervaly / YTICK_DIS;
		LONG j = POS_Y - temp;
		point.x = i;
		point.y = j;
		points[i] = point;
	}
	Polyline(hdc, points, PLOTRIGHT);
	DeleteObject(hpen);
}
COLORREF cscolor()
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	static COLORREF rgbCurrent=RGB(0,0,0);        // initial color selection

									   // Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.hwndOwner = hwnd1;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE)
	{
		//hbrush = CreateSolidBrush(cc.rgbResult);
		rgbCurrent = cc.rgbResult;
		return rgbCurrent;
	}
}
int isp(TCHAR ch)
{
	switch (ch)
	{
	case '#':
		return 0;
	case '(':
		return 1;
	case '*':
		return 5;
	case  '/':
		return 5;
	case '%':
		return 5;
	case '+':
		return 3;
	case '-':
		return 3;
	case ')':
		return 8;
	case  '^':
		return 7;
	default:
		return -1;
	}
}
int icp(TCHAR ch)
{
	switch (ch)
	{
	case '#':
		return 0;
	case '(':
		return 8;
	case '*':
		return 4;
	case  '/':
		return 4;
	case '%':
		return 4;
	case '+':
		return 2;
	case '-':
		return 2;
	case ')':
		return 1;
	case  '^':
		return 6;
	default:
		return -1;
	}
}
std::queue<TCHAR> Convert(TCHAR* fx1)
{
	
	INT i = 0;
	for (i = 0;fx1[i] != '\0';i++) {}
	TCHAR* fx = new TCHAR[i + 1];
	for (i = 0;fx1[i] != '\0';i++) {
		fx [i]= fx1[i];
	}
	fx[i] = '#';
	i = 0;

	std::stack <TCHAR>s;
	std::queue<TCHAR>rpn;
	char ch = '#', ch1;
	s.push(ch);
	ch = fx[i++];
	while (!s.empty() && ch != '\0') {
		if (isdigit(ch) != 0 || ch == 'x' || ch == 'X')
		{
			rpn.push(ch);
			ch = fx[i++];
		}
		else
		{
			ch1 = s.top();

			if (isp(ch1) < icp(ch))
			{
				s.push(ch);
				ch = fx[i++];
			}
			else if (isp(ch1) > icp(ch))
			{
				rpn.push(ch1);
				s.pop();
			}
			else
			{
				s.pop();
				if (ch1 == '(')
					ch = fx[i++];
			}
		}
	}
	return rpn;
}
double f(std::queue<TCHAR>rpn, double x)
{
	calculator  myCal;
	myCal.setQueue(rpn);
	double y;
	myCal.getX(x, y);
	return y;
}

BOOL plotdata(HWND hwnd,LPCWSTR path)
{
	if (path == NULL)
	{
		return false;
	}
	ifstream fin(path);
	string line;
	vector<float> points;
	POINT* realpoints;
	POINT point;
	RECT plotarea;
	HPEN hpen;
	TCHAR buf[1000];
	HDC hdc = GetDC(hwnd);
	LONG pointnum;
	//
	while (getline(fin, line))
	{
		istringstream sin(line);	
		string coordirate;
		while (getline(sin, coordirate, ',')) {
			const char * c = coordirate.c_str();
			float p = atof(c);
			points.push_back(p);
		}
	}
	if (points.size() % 2 !=0)
	{
		return false;
	}
	//get points cooridate
	pointnum = points.size() / 2;
	hpen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	SelectObject(hdc, hpen);

	GetClientRect(hwnd, &plotarea);

	PLOTRIGHT = plotarea.right;
	PLOTBOTTOM = plotarea.bottom;

	POS_X = -XRangeFrom*PLOTRIGHT / (-XRangeFrom + XRangeTo);
	POS_Y = YRangeTo*PLOTBOTTOM / (-YRangeFrom + YRangeTo);

	FLOAT intervalnuml = -XRangeFrom / XTICK_DIS;
	INT iintervalnuml = (INT)intervalnuml;
	FLOAT leftintervalnuml = intervalnuml - iintervalnuml;

	FLOAT intervalnumr = XRangeTo / XTICK_DIS;
	INT iintervalnumr = (INT)intervalnumr;
	FLOAT leftintervalnumr = intervalnumr - iintervalnumr;

	FLOAT intervalnumup = YRangeTo / YTICK_DIS;
	INT iintervalnumup = (INT)intervalnumup;
	FLOAT  leftintervalnumup = intervalnumup - iintervalnumup;

	FLOAT intervalnumdn = -YRangeFrom / YTICK_DIS;
	INT iintervalnumdn = (INT)intervalnumdn;
	FLOAT leftintervalnumdn = intervalnumdn - iintervalnumdn;

	INT intervalx = PLOTRIGHT / (intervalnuml + intervalnumr);
	INT intervaly = PLOTBOTTOM / (intervalnumup + intervalnumdn);
	
	realpoints = new POINT[pointnum];
	LONG x;
	LONG y;
	for (int i = 0;i < points.size();i+=2)
	{
		 x = points[i] * intervalx / XTICK_DIS + POS_X;
		 y = POS_Y - points[i + 1] * intervaly / YTICK_DIS;
		point.x = x;
		point.y = y;
		realpoints[i / 2] = point;
	}
	
//	HDC cdc = CreateCompatibleDC(hdc);
	Polyline(hdc, realpoints,pointnum);
//	BitBlt(hdc, 0, 0, PLOTRIGHT,PLOTBOTTOM,
//		hdcScreenCompat, 0, 0, SRCCOPY);
	DeleteObject(hpen);
	return true;
}
void save(LPCWSTR pFilePath,HWND hWnd)
{
	//首先得到整个屏幕的句柄
	HDC hDisplay = GetDC(hWnd);
	//得到屏幕的大小
	RECT rect;
	GetClientRect(hWnd, &rect);
	int nScreenWidth = rect.right;
	int nScreenHeight = rect.bottom;
	//创建内存DC
	HDC hMemDc = CreateCompatibleDC(hDisplay);
	_ASSERT(NULL != hMemDc);
	//创建位图
	HBITMAP hBmp = CreateCompatibleBitmap(hDisplay, nScreenWidth, nScreenHeight);
	_ASSERT(NULL != hBmp);
	//选择位图
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDc, hBmp);
	_ASSERT(NULL != (DWORD)hOldBmp || GDI_ERROR != (DWORD)hOldBmp);
	//复制图形
	BOOL blnReturnValue = BitBlt(hMemDc, 0, 0, nScreenWidth, nScreenHeight, hDisplay, 0, 0, SRCCOPY);
	_ASSERT(0 != blnReturnValue);
	//再次选择
	hBmp = (HBITMAP)SelectObject(hMemDc, hOldBmp);
	_ASSERT(NULL != (DWORD)hBmp || GDI_ERROR != (DWORD)hBmp);
	DeleteDC(hMemDc);
	hMemDc = NULL;
	int nBits = GetDeviceCaps(hDisplay, BITSPIXEL);  //每个象素的位数
	DWORD dwLineBits = 0;  //每行字节数
	switch (nBits)
	{
	case 16:
		dwLineBits = nScreenWidth * 2;
		break;
	case 24:
		dwLineBits = (nScreenWidth + 1) * 3 - ((nScreenWidth + 1) * 3) % 4;
		break;
	case 32:
		dwLineBits = nScreenWidth * 4;
		break;
	default:
	
		MessageBox(NULL, _T("不支持该格式！"), _T("不支持！"), MB_OK);
		break;
	}
	//分配内存
	DWORD dwBitsCount = dwLineBits * nScreenHeight;
	DWORD dwFileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBitsCount;
	BYTE * pMem = (BYTE *)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, dwFileSize);
	if (NULL == pMem)
	{
		MessageBox(hWnd, _T("分配内存失败！"), _T("错误！"), MB_OK);
		return;
	}
	BITMAPFILEHEADER* pBMPHeader = (BITMAPFILEHEADER*)pMem;
	BITMAPINFOHEADER* pInfoHeader = (BITMAPINFOHEADER*)((BYTE *)pBMPHeader + sizeof(BITMAPFILEHEADER));
	BYTE* pBits = (BYTE*)pInfoHeader + sizeof(BITMAPINFOHEADER);
	//初始化
	pBMPHeader->bfType = ('M' << 8) | 'B';
	pBMPHeader->bfSize = dwFileSize;
	pBMPHeader->bfReserved1 = 0;
	pBMPHeader->bfReserved2 = 0;
	pBMPHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	pInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	pInfoHeader->biWidth = nScreenWidth;
	pInfoHeader->biHeight = nScreenHeight;
	pInfoHeader->biPlanes = GetDeviceCaps(hDisplay, PLANES);  //调色板数
	pInfoHeader->biBitCount = nBits;
	pInfoHeader->biCompression = BI_RGB;
	pInfoHeader->biSizeImage = dwBitsCount;
	pInfoHeader->biXPelsPerMeter = GetDeviceCaps(hDisplay, LOGPIXELSX);
	pInfoHeader->biYPelsPerMeter = GetDeviceCaps(hDisplay, LOGPIXELSY);
	pInfoHeader->biClrUsed = 0;
	pInfoHeader->biClrImportant = 0;

	//得到数据
	int nLines = GetDIBits(hDisplay, hBmp, 0, nScreenHeight,
		pBits,
		(BITMAPINFO *)pInfoHeader,
		DIB_RGB_COLORS);
	if (0 == nLines)
	{
		DWORD erro = GetLastError();
		MessageBox(hWnd, _T("复制位图数据失败"), _T("错误！"), MB_OK);
		return;
	}
	DeleteDC(hDisplay);
	hDisplay = NULL;
	DeleteObject(hBmp);
	hBmp = NULL;
	DeleteObject(hOldBmp);
	hOldBmp = NULL;
	//保存文件
	HANDLE hFile = CreateFile(pFilePath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (NULL == hFile || INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(hWnd, _T("文件已经存在！"), _T("错误！"), MB_OK);
		return;
	}
	//写入
	DWORD dwWriteCount = 0;
	if (!WriteFile(hFile, (void *)pBMPHeader, dwFileSize, &dwWriteCount, NULL))
	{
		MessageBox(NULL, _T("写入文件失败！"), _T("错误"), MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(hFile);
		return;
	}
	//关闭文件
	CloseHandle(hFile);
	GlobalFree((void *)pBMPHeader);
}

void Plotsin(HWND hwnd, HDC hdc, PAINTSTRUCT ps, COLORREF color)
{
	POINT* points;
	POINT point;
	RECT plotarea;
	HPEN hpen;
	TCHAR buf[1000];

	hpen = CreatePen(PS_SOLID, 1, color);
	SelectObject(hdc, hpen);

	GetClientRect(hwnd, &plotarea);

	PLOTRIGHT = plotarea.right;
	PLOTBOTTOM = plotarea.bottom;

	POS_X = -XRangeFrom*PLOTRIGHT / (-XRangeFrom + XRangeTo);
	POS_Y = YRangeTo*PLOTBOTTOM / (-YRangeFrom + YRangeTo);

	FLOAT intervalnuml = -XRangeFrom / XTICK_DIS;
	INT iintervalnuml = (INT)intervalnuml;
	FLOAT leftintervalnuml = intervalnuml - iintervalnuml;

	FLOAT intervalnumr = XRangeTo / XTICK_DIS;
	INT iintervalnumr = (INT)intervalnumr;
	FLOAT leftintervalnumr = intervalnumr - iintervalnumr;

	FLOAT intervalnumup = YRangeTo / YTICK_DIS;
	INT iintervalnumup = (INT)intervalnumup;
	FLOAT  leftintervalnumup = intervalnumup - iintervalnumup;

	FLOAT intervalnumdn = -YRangeFrom / YTICK_DIS;
	INT iintervalnumdn = (INT)intervalnumdn;
	FLOAT leftintervalnumdn = intervalnumdn - iintervalnumdn;

	INT intervalx = PLOTRIGHT / (intervalnuml + intervalnumr);
	INT intervaly = PLOTBOTTOM / (intervalnumup + intervalnumdn);

	//std::queue<TCHAR>q=Convert(fx);
	points = new POINT[PLOTRIGHT];
	for (LONG i = 0;i < PLOTRIGHT;i++)
	{
		FLOAT xcoordirate = (i - POS_X)*XTICK_DIS / intervalx;
		//FLOAT ycoordirate = f(q, xcoordirate);
		FLOAT ycoordirate = sin(xcoordirate);
		FLOAT temp = ycoordirate*intervaly / YTICK_DIS;
		LONG j = POS_Y - temp;
		point.x = i;
		point.y = j;
		points[i] = point;
	}
	Polyline(hdc, points, PLOTRIGHT);
	DeleteObject(hpen);
}