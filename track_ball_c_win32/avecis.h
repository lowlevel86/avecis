#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <winsock2.h>
#include <process.h>
#include <ws2tcpip.h>

#define TRUE 1
#define FALSE 0

#define ON 1
#define OFF 0

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
#define SOUND_DONE_SIGNAL 11
#define DISCONNECT_SIGNAL 0xFF

#define KC_BACKSPACE 0x8
#define KC_TAB 0x9
#define KC_ENTER 0xD
#define KC_NP_ENTER 0x100000D
#define KC_LEFT_SHIFT 0x10
#define KC_RIGHT_SHIFT 0x2000010
#define KC_LEFT_CTRL 0x11
#define KC_RIGHT_CTRL 0x1000011
#define KC_LEFT_ALT 0x12
#define KC_RIGHT_ALT 0x1000012
#define KC_PAUSE 0x13
#define KC_CAPS_LOCK 0x14
#define KC_ESC 0x1B
#define KC_SPACE 0x20

#define KC_PAGE_UP 0x1000021
#define KC_PAGE_DOWN 0x1000022
#define KC_END 0x1000023
#define KC_HOME 0x1000024

#define KC_LEFT 0x1000025
#define KC_UP 0x1000026
#define KC_RIGHT 0x1000027
#define KC_DOWN 0x1000028

#define KC_PRINT_SCREEN 0x100002C
#define KC_INSERT 0x100002D
#define KC_DELETE 0x100002E

#define KC_0 0x30
#define KC_1 0x31
#define KC_2 0x32
#define KC_3 0x33
#define KC_4 0x34
#define KC_5 0x35
#define KC_6 0x36
#define KC_7 0x37
#define KC_8 0x38
#define KC_9 0x39

#define KC_A 0x41
#define KC_B 0x42
#define KC_C 0x43
#define KC_D 0x44
#define KC_E 0x45
#define KC_F 0x46
#define KC_G 0x47
#define KC_H 0x48
#define KC_I 0x49
#define KC_J 0x4A
#define KC_K 0x4B
#define KC_L 0x4C
#define KC_M 0x4D
#define KC_N 0x4E
#define KC_O 0x4F
#define KC_P 0x50
#define KC_Q 0x51
#define KC_R 0x52
#define KC_S 0x53
#define KC_T 0x54
#define KC_U 0x55
#define KC_V 0x56
#define KC_W 0x57
#define KC_X 0x58
#define KC_Y 0x59
#define KC_Z 0x5A

#define KC_LEFT_MENU 0x100005B
#define KC_RIGHT_MENU 0x100005D

#define KC_NP_0 0x60
#define KC_NP_1 0x61
#define KC_NP_2 0x62
#define KC_NP_3 0x63
#define KC_NP_4 0x64
#define KC_NP_5 0x65
#define KC_NP_6 0x66
#define KC_NP_7 0x67
#define KC_NP_8 0x68
#define KC_NP_9 0x69

#define KC_NP_MULTIPLY 0x6A
#define KC_NP_PLUS 0x6B
#define KC_NP_MINUS 0x6D
#define KC_NP_DECIMAL 0x6E
#define KC_NP_DIVIDE 0x100006F

#define KC_F1 0x70
#define KC_F2 0x71
#define KC_F3 0x72
#define KC_F4 0x73
#define KC_F5 0x74
#define KC_F6 0x75
#define KC_F7 0x76
#define KC_F8 0x77
#define KC_F9 0x78
#define KC_F10 0x79
#define KC_F11 0x7A
#define KC_F12 0x7B

#define KC_NUM_LOCK 0x1000090
#define KC_SCROLL_LOCK 0x91

#define KC_SEMICOLON 0xBA
#define KC_PLUS 0xBB
#define KC_COMMA 0xBC
#define KC_MINUS 0xBD
#define KC_PERIOD 0xBE
#define KC_SLASH 0xBF
#define KC_BACKQUOTE 0xC0
#define KC_LEFT_BOX_BRACKET 0xDB
#define KC_BACKSLASH 0xDC
#define KC_RIGHT_BOX_BRACKET 0xDD
#define KC_SINGLE_QUOTE 0xDE


void viewStart(float);
void viewEnd(float);
void setPerspective(float);
void orthographicMode(int);
void fogMode(int);
void fogColor();
void fogStart(float);
void fogEnd(float);
void antialiasingMode(int);
void setColor(char *, int);
void clearScreen();
void drawLine(float *, int);
void drawPath(float *, int);
void showContent();
void printSB(char *, ...);
void playSound(float *, float *, int);
void stopSound();
void endTransmission();
void receiveCallback(char *, int);
int avecisConnect(char *, int);
void blockAvecisDisconnect();
void unblockAvecisDisconnect();
void avecisDisconnect();
