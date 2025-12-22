#include <windows.h>
#include <commctrl.h>
#include <vector>
#include <string>
#include <random>
#include <mutex>
#include <algorithm>
#include <math.h>

#define ID_BTN_CREATE     101
#define ID_BTN_TERMINATE  102
#define ID_BTN_SUSPEND    103
#define ID_BTN_PRI_UP     104
#define ID_BTN_PRI_DOWN   105
#define ID_BTN_SYNC       106
#define ID_BTN_EXIT       107
#define ID_LISTBOX        108
#define ID_TRACKBAR_SPEED 109
#define RECT_LIMIT        600
#define MAX_THREADS       6

enum ShapeType {SQUARE=0,CIRCLE,TRIANGLE,DIAMOND,ELLIPSE,PENTAGON};

struct Rect { int x, y, w, h; COLORREF color; ShapeType shape; };
struct ThreadInfo {
    HANDLE hThread = NULL;
    HANDLE pauseEvent = NULL;
    HANDLE stopEvent = NULL;
    int priority = 0;
    COLORREF color;
    ShapeType shape;
    int id;
};

HWND hwndMain = NULL;
HWND hwndList = NULL;
HWND hwndCanvas = NULL;
HWND hwndTrackbar = NULL;
std::vector<ThreadInfo*> threads;
std::vector<Rect> rects;
std::mutex rectsMutex;
CRITICAL_SECTION csThreads;

bool useLock = true;
double baseSpeed = 0.5;
COLORREF colors[MAX_THREADS] = { RGB(76,175,80), RGB(0,188,212), RGB(255,193,7),
                                 RGB(233,30,99), RGB(156,39,176), RGB(255,87,34) };
ShapeType shapes[MAX_THREADS] = { SQUARE,CIRCLE,TRIANGLE,DIAMOND,ELLIPSE,PENTAGON };

int RandomInt(int a, int b) {
    thread_local std::mt19937 rng((unsigned int)GetTickCount() ^ (unsigned int)GetCurrentThreadId());
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}

// Малювання всіх фігур
void DrawShape(HDC hdc, const Rect& r) {
    HBRUSH br = CreateSolidBrush(r.color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, br);
    HPEN oldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));

    switch(r.shape){
        case SQUARE:{
            RECT rc = { r.x, r.y, r.x+r.w, r.y+r.h };
            FillRect(hdc, &rc, br);
        } break;
        case CIRCLE:
            Ellipse(hdc, r.x, r.y, r.x+r.w, r.y+r.h);
            break;
        case TRIANGLE:{
            POINT pts[3] = { {r.x+r.w/2,r.y}, {r.x,r.y+r.h}, {r.x+r.w,r.y+r.h} };
            Polygon(hdc, pts, 3);
        } break;
        case DIAMOND:{
            POINT pts[4] = { {r.x+r.w/2,r.y}, {r.x,r.y+r.h/2}, {r.x+r.w/2,r.y+r.h}, {r.x+r.w,r.y+r.h/2} };
            Polygon(hdc, pts, 4);
        } break;
        case ELLIPSE:
            Ellipse(hdc, r.x, r.y, r.x+r.w, r.y+r.h/2);
            break;
        case PENTAGON:{
            POINT pts[5];
            for(int i=0;i<5;i++){
                pts[i].x = r.x + r.w/2 + (int)((r.w/2)*cos(3.1415*2*i/5 - 3.1415/2));
                pts[i].y = r.y + r.h/2 + (int)((r.h/2)*sin(3.1415*2*i/5 - 3.1415/2));
            }
            Polygon(hdc, pts, 5);
        } break;
    }

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(br);
}

// Малювання канви
void DrawRectsToHDC(HDC hdc, const RECT& paintRect) {
    std::vector<Rect> local;
    if (useLock) {
        std::lock_guard<std::mutex> lock(rectsMutex);
        local = rects;
    } else local = rects;

    HBRUSH bg = CreateSolidBrush(RGB(0,0,0));
    FillRect(hdc, &paintRect, bg);
    DeleteObject(bg);

    for(auto &r:local){
        RECT rc={r.x,r.y,r.x+r.w,r.y+r.h};
        RECT inter;
        if(IntersectRect(&inter, &paintRect, &rc))
            DrawShape(hdc,r);
    }
}

LRESULT CALLBACK CanvasProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if(msg==WM_PAINT){
        PAINTSTRUCT ps; HDC hdc=BeginPaint(hwnd,&ps);
        DrawRectsToHDC(hdc, ps.rcPaint);
        EndPaint(hwnd,&ps);
        return 0;
    }
    if(msg==WM_ERASEBKGND) return 1;
    return DefWindowProcW(hwnd,msg,wParam,lParam);
}

DWORD WINAPI WorkerThread(LPVOID lpParam){
    ThreadInfo* info = (ThreadInfo*)lpParam;

    RECT rc; GetClientRect(hwndCanvas,&rc);
    int canvasW = rc.right-rc.left; if(canvasW<=0) canvasW=1;
    int canvasH = rc.bottom-rc.top; if(canvasH<=0) canvasH=1;

    while(WaitForSingleObject(info->stopEvent,0)!=WAIT_OBJECT_0){
        WaitForSingleObject(info->pauseEvent, INFINITE);

        Rect r;
        r.w=RandomInt(20,120); r.h=RandomInt(20,120);
        r.x=RandomInt(0,std::max(0,canvasW-r.w));
        r.y=RandomInt(0,std::max(0,canvasH-r.h));
        r.color=info->color;
        r.shape=info->shape;

        if(useLock){
            std::lock_guard<std::mutex> lock(rectsMutex);
            rects.push_back(r);
            if(rects.size()>RECT_LIMIT) rects.erase(rects.begin());
        } else {
            rects.push_back(r);
            if(rects.size()>RECT_LIMIT) rects.erase(rects.begin());
        }

        InvalidateRect(hwndCanvas,NULL,FALSE);

        int adjustedPriority = info->priority + 3; 
        Sleep((int)(baseSpeed*1000 / adjustedPriority));
    }
    return 0;
}

void RefreshListBox(){
    int sel = (int)SendMessageW(hwndList, LB_GETCURSEL, 0, 0); 
    SendMessageW(hwndList, LB_RESETCONTENT, 0, 0);

    EnterCriticalSection(&csThreads);
    for(size_t i=0;i<threads.size();++i){
        std::wstring txt=L"[" + std::to_wstring(i)+L"] PRI="+std::to_wstring(threads[i]->priority)+L" ";
        DWORD pauseState=WaitForSingleObject(threads[i]->pauseEvent,0);
        DWORD stopState=WaitForSingleObject(threads[i]->stopEvent,0);
        if(stopState==WAIT_OBJECT_0) txt+=L"STOPPED";
        else if(pauseState==WAIT_OBJECT_0) txt+=L"RUNNING";
        else txt+=L"PAUSED";
        SendMessageW(hwndList,LB_ADDSTRING,0,(LPARAM)txt.c_str());
    }
    LeaveCriticalSection(&csThreads);

    if(sel >= 0 && sel < (int)threads.size())
        SendMessageW(hwndList, LB_SETCURSEL, sel, 0);

    InvalidateRect(hwndList,NULL,TRUE);
}

ThreadInfo* GetSelectedThread(){
    int sel=(int)SendMessageW(hwndList,LB_GETCURSEL,0,0);
    if(sel==LB_ERR) return nullptr;
    EnterCriticalSection(&csThreads);
    ThreadInfo* t=(sel>=0 && sel<(int)threads.size())?threads[sel]:nullptr;
    LeaveCriticalSection(&csThreads);
    return t;
}

void OnCreateThread(){
    EnterCriticalSection(&csThreads);
    if(threads.size()>=MAX_THREADS){ LeaveCriticalSection(&csThreads); 
        MessageBoxW(hwndMain,L"Maximum 6 threads reached",L"Info",MB_OK|MB_ICONINFORMATION); return; }

    ThreadInfo* info=new ThreadInfo();
    info->color=colors[threads.size()];
    info->shape=shapes[threads.size()];
    info->pauseEvent=CreateEvent(NULL,TRUE,TRUE,NULL);
    info->stopEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
    info->id=(int)threads.size();
    info->priority=0;
    info->hThread=CreateThread(NULL,0,WorkerThread,info,0,NULL);
    if(!info->hThread){ CloseHandle(info->pauseEvent); CloseHandle(info->stopEvent); delete info;
        LeaveCriticalSection(&csThreads);
        MessageBoxW(hwndMain,L"Failed to create thread",L"Error",MB_OK|MB_ICONERROR); return; }

    threads.push_back(info); LeaveCriticalSection(&csThreads);
    RefreshListBox();
}

void OnTerminateThread(){
    ThreadInfo* t=GetSelectedThread(); if(!t) return;
    SetEvent(t->stopEvent); WaitForSingleObject(t->hThread,INFINITE);
    CloseHandle(t->hThread); CloseHandle(t->pauseEvent); CloseHandle(t->stopEvent);
    EnterCriticalSection(&csThreads);
    auto it=std::find(threads.begin(),threads.end(),t);
    if(it!=threads.end()) threads.erase(it);
    LeaveCriticalSection(&csThreads);
    delete t; RefreshListBox(); InvalidateRect(hwndCanvas,NULL,FALSE);
}

void OnSuspendResumeThread(){
    ThreadInfo* t=GetSelectedThread(); if(!t) return;
    DWORD state=WaitForSingleObject(t->pauseEvent,0);
    if(state==WAIT_OBJECT_0) ResetEvent(t->pauseEvent);
    else SetEvent(t->pauseEvent);
    RefreshListBox();
}

void OnPriorityUp(){
    ThreadInfo* t=GetSelectedThread(); if(!t) return;
    if(t->priority < 2) t->priority++;
    RefreshListBox();
}

void OnPriorityDown(){
    ThreadInfo* t=GetSelectedThread(); if(!t) return;
    if(t->priority > -2) t->priority--;
    RefreshListBox();
}

void OnToggleSync(){ useLock=!useLock; MessageBoxW(hwndMain,useLock?L"Sync ON":L"Sync OFF",L"Sync",MB_OK); }
void OnExit(){ PostMessage(hwndMain,WM_CLOSE,0,0); }

LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
    case WM_CREATE:{
        CreateWindowW(L"BUTTON",L"Create",WS_CHILD|WS_VISIBLE,20,20,160,30,hwnd,(HMENU)ID_BTN_CREATE,(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);
        CreateWindowW(L"BUTTON",L"Terminate",WS_CHILD|WS_VISIBLE,20,60,160,30,hwnd,(HMENU)ID_BTN_TERMINATE,(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);
        CreateWindowW(L"BUTTON",L"Suspend/Resume",WS_CHILD|WS_VISIBLE,20,100,160,30,hwnd,(HMENU)ID_BTN_SUSPEND,(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);
        CreateWindowW(L"BUTTON",L"Priority +",WS_CHILD|WS_VISIBLE,20,140,80,30,hwnd,(HMENU)ID_BTN_PRI_UP,(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);
        CreateWindowW(L"BUTTON",L"Priority -",WS_CHILD|WS_VISIBLE,100,140,80,30,hwnd,(HMENU)ID_BTN_PRI_DOWN,(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);
        CreateWindowW(L"BUTTON",L"Sync ON/OFF",WS_CHILD|WS_VISIBLE,20,180,160,30,hwnd,(HMENU)ID_BTN_SYNC,(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);
        CreateWindowW(L"BUTTON",L"Exit",WS_CHILD|WS_VISIBLE,20,220,160,30,hwnd,(HMENU)ID_BTN_EXIT,(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);

        hwndList=CreateWindowW(L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,20,260,160,180,hwnd,(HMENU)ID_LISTBOX,(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE),NULL);

        hwndTrackbar = CreateWindowExW(0, TRACKBAR_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
            20, 460, 160, 30,
            hwnd, (HMENU)ID_TRACKBAR_SPEED, (HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE), NULL);

        SendMessage(hwndTrackbar, TBM_SETRANGE, TRUE, MAKELONG(1, 100));
        SendMessage(hwndTrackbar, TBM_SETPOS, TRUE, 50);
        return 0;
    }
    case WM_COMMAND:
        switch(LOWORD(wParam)){
        case ID_BTN_CREATE: OnCreateThread(); break;
        case ID_BTN_TERMINATE: OnTerminateThread(); break;
        case ID_BTN_SUSPEND: OnSuspendResumeThread(); break;
        case ID_BTN_PRI_UP: OnPriorityUp(); break;
        case ID_BTN_PRI_DOWN: OnPriorityDown(); break;
        case ID_BTN_SYNC: OnToggleSync(); break;
        case ID_BTN_EXIT: OnExit(); break;
        }
        break;
    case WM_HSCROLL:
        if ((HWND)lParam == hwndTrackbar) {
            int pos = SendMessage(hwndTrackbar, TBM_GETPOS, 0, 0);
            baseSpeed = 5.0 * (100 - pos) / 99.0 + 0.05;
            RefreshListBox();
        }
        break;
    case WM_TIMER: RefreshListBox(); InvalidateRect(hwndCanvas,NULL,FALSE); break;
    case WM_DESTROY:
        EnterCriticalSection(&csThreads);
        for(auto t:threads) SetEvent(t->stopEvent);
        for(auto t:threads){
            if(t->hThread) WaitForSingleObject(t->hThread,INFINITE);
            if(t->hThread) CloseHandle(t->hThread);
            if(t->pauseEvent) CloseHandle(t->pauseEvent);
            if(t->stopEvent) CloseHandle(t->stopEvent);
            delete t;
        }
        threads.clear(); LeaveCriticalSection(&csThreads);
        DeleteCriticalSection(&csThreads); PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd,msg,wParam,lParam);
}

int WINAPI wWinMain(HINSTANCE hInst,HINSTANCE, PWSTR, int nCmdShow){
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_BAR_CLASSES };
    InitCommonControlsEx(&icex);

    InitializeCriticalSection(&csThreads);

    WNDCLASSW wc={}; wc.lpfnWndProc=WndProc; wc.hInstance=hInst; wc.lpszClassName=L"MultiThreadDemoMain";
    wc.hCursor=LoadCursorW(NULL,IDC_ARROW); wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    RegisterClassW(&wc);

    WNDCLASSW canvasWc={}; canvasWc.lpfnWndProc=CanvasProc; canvasWc.hInstance=hInst;
    canvasWc.lpszClassName=L"MyCanvasClass"; canvasWc.hCursor=LoadCursorW(NULL,IDC_ARROW);
    canvasWc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassW(&canvasWc);

    hwndMain=CreateWindowExW(0,wc.lpszClassName,L"Multithread Rects Demo",
        WS_OVERLAPPEDWINDOW,100,100,950,540,NULL,NULL,hInst,NULL);
    hwndCanvas=CreateWindowExW(0,L"MyCanvasClass",NULL,WS_CHILD|WS_VISIBLE|WS_BORDER,220,10,700,500,hwndMain,NULL,hInst,NULL);

    ShowWindow(hwndMain,nCmdShow); UpdateWindow(hwndMain);
    SetTimer(hwndMain,1,300,NULL);

    MSG msg; while(GetMessageW(&msg,NULL,0,0)){ TranslateMessage(&msg); DispatchMessageW(&msg);}
    KillTimer(hwndMain,1);
    return 0;
}
