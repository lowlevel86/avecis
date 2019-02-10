#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "avecisHandler.h"
#include "sendReceive.h"
#include "drawGraphics.h"
#include "playSnd.h"
#include "cmdLineArgs.h"

#define IDI_ICON 101
#define ID_STATUS_BAR 102

#define KEY_DOWN 0
#define KEY_UP 1
#define L_MOUSE_DOWN 2
#define L_MOUSE_UP 3
#define R_MOUSE_DOWN 4
#define R_MOUSE_UP 5
#define M_MOUSE_DOWN 6
#define M_MOUSE_UP 7
#define MOUSE_MOVE 8
#define MOUSE_WHEEL_UP 9
#define MOUSE_WHEEL_DOWN 10

char flags[9] = {'t', 'x', 'y', 'w', 'h', 'p', 'b', 's', 'u'};
int argTypes[9] = {string_arg, int_arg, int_arg, int_arg,
                   int_arg, string_arg, int_arg, int_arg, only_flag};
int argCnt = 9;

struct argumentList
{
   char *title;
   int window_x;
   int window_y;
   int window_w;
   int window_h;
   char *port;
   int sound_buffer_size;
   int samples_per_second;
   int allow_popup;
};

struct argumentList argL;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT("Avecis");

HINSTANCE ghInstance = NULL;
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)
{
   HWND hwnd;
   MSG msg;
   WNDCLASS wndclass;

   ghInstance = hInstance;
   
   wndclass.style = CS_HREDRAW | CS_VREDRAW;
   wndclass.lpfnWndProc = WndProc;
   wndclass.cbClsExtra = 0;
   wndclass.cbWndExtra = 0;
   wndclass.hInstance = hInstance;
   wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
   wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
   wndclass.hbrBackground = 0;
   wndclass.lpszMenuName = szAppName;
   wndclass.lpszClassName = szAppName;

   if (!RegisterClass(&wndclass))
   return 0;

   
   // default window settings
   argL.title = "Avecis";
   argL.window_x = 0;
   argL.window_y = 0;
   argL.window_w = 640;
   argL.window_h = 360;
   argL.port = "27015";
   argL.sound_buffer_size = 4096;
   argL.samples_per_second = 11025;
   argL.allow_popup = FALSE;

   getCmdLineArgs((char *)szCmdLine, flags, argTypes, argCnt, (void ***)&argL);
   
   
   hwnd = CreateWindow(szAppName, TEXT(argL.title),
                       WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
                       argL.window_x, argL.window_y,
                       argL.window_w, argL.window_h,
                       NULL, NULL, hInstance, NULL);

   ShowWindow(hwnd, iCmdShow);
   UpdateWindow(hwnd);

   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static RECT mainWin;
   static RECT insideWin;
   static RECT statusWin;
   static HWND hwndStatus;
   static int divisionCnt = 1;
   static int divisionLocs[1];
   static int statusBarHght;
   static int mainWinWdth, mainWinHght;
   static int mainWinInsideWinWdthDiff;
   static int mainWinInsideWinHghtDiff;
   

   if (WM_CREATE == message)
   {
      GetWindowRect(hwnd, &mainWin);
      GetClientRect(hwnd, &insideWin);
      
      // create the status bar
      InitCommonControls();// ensure that the common control DLL is loaded
      
      hwndStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
                                  WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                                  hwnd, (HMENU)ID_STATUS_BAR, ghInstance, NULL);
      
      divisionLocs[0] = -1;//insideWin.right;
      SendMessage(hwndStatus, SB_SETPARTS, (WPARAM)divisionCnt, (LPARAM)divisionLocs);
      
      GetWindowRect(hwndStatus, &statusWin);
      
      
      // resize window
      statusBarHght = statusWin.bottom - statusWin.top;
      
      mainWinInsideWinWdthDiff = (mainWin.right - mainWin.left) -
                                 (insideWin.right - insideWin.left);
      
      mainWinInsideWinHghtDiff = (mainWin.bottom - mainWin.top) -
                                 (insideWin.bottom - insideWin.top);
      
      mainWinWdth = argL.window_w + mainWinInsideWinWdthDiff;
      mainWinHght = argL.window_h + statusBarHght + mainWinInsideWinHghtDiff;
      
      SetWindowPos(hwnd, 0, argL.window_x, argL.window_y, mainWinWdth, mainWinHght, 0);
      
      
      iniGraphics(hwnd, argL.window_w, argL.window_h);
      iniSndBuffs(hwnd, argL.sound_buffer_size, argL.samples_per_second);
      iniAvecisHandler(argL.port, hwnd);
   }
   
   if (WM_SIZE == message)
   SendMessage(hwndStatus, WM_SIZE, 0, 0);
   
   // send sound to audio device
   soundToDevice(message, lParam);
   
   if ((WM_KEYDOWN == message) || (WM_SYSKEYDOWN == message))
   {
      if (MapVirtualKey(HIWORD(lParam) & 0xFF, MAPVK_VSC_TO_VK_EX) == VK_RSHIFT)
      sendInputEvent(KEY_DOWN, (int)wParam | 0x02000000 | ((int)lParam & 0x01000000));
      else
      sendInputEvent(KEY_DOWN, (int)wParam | ((int)lParam & 0x01000000));
   }
   
   if ((WM_KEYUP == message) || (WM_SYSKEYUP == message))
   {
      if (MapVirtualKey(HIWORD(lParam) & 0xFF, MAPVK_VSC_TO_VK_EX) == VK_RSHIFT)
      sendInputEvent(KEY_UP, (int)wParam | 0x02000000 | ((int)lParam & 0x01000000));
      else
      sendInputEvent(KEY_UP, (int)wParam | ((int)lParam & 0x01000000));
   }
   
   if (WM_LBUTTONDOWN == message)
   sendInputEvent(L_MOUSE_DOWN, 0);
   
   if (WM_LBUTTONUP == message)
   sendInputEvent(L_MOUSE_UP, 0);
   
   if (WM_RBUTTONDOWN == message)
   sendInputEvent(R_MOUSE_DOWN, 0);
   
   if (WM_RBUTTONUP == message)
   sendInputEvent(R_MOUSE_UP, 0);
   
   if (WM_MBUTTONDOWN == message)
   sendInputEvent(M_MOUSE_DOWN, 0);
   
   if (WM_MBUTTONUP == message)
   sendInputEvent(M_MOUSE_UP, 0);
   
   if (WM_MOUSEMOVE == message)
   sendInputEvent(MOUSE_MOVE, (int)lParam);

   if (WM_MOUSEWHEEL == message)
   {
      if (HIWORD(wParam) == 120)
      sendInputEvent(MOUSE_WHEEL_UP, 0);
      
      if (HIWORD(wParam) == 65416)
      sendInputEvent(MOUSE_WHEEL_DOWN, 0);
   }
   
   if (WM_USER == message)
   {
      #define SHOW_CONTENT 13
      #define PRINT_STATUS 14
      #define CONNECTION_ESTABLISHED 17
      
      if (wParam == SHOW_CONTENT)
      showContent();
      
      if (wParam == PRINT_STATUS)
      SendMessage(hwndStatus, SB_SETTEXT, (WPARAM)0, lParam);
      
      if (wParam == CONNECTION_ESTABLISHED)
      {
         if (argL.allow_popup)
         {
            ShowWindow(hwnd, SW_MINIMIZE);
            ShowWindow(hwnd, SW_RESTORE);
         }
      }
   }
   
   if (WM_PAINT == message)
   refreshGraphics();
   
   if (WM_DESTROY == message)
   {
      endAvecisHandler();
      stopSound();
      closeGraphics();
      freeCmdLineArgs();
      PostQuitMessage(0);
   }
   
   return DefWindowProc(hwnd, message, wParam, lParam);
}