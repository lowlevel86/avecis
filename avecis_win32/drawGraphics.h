#include <windows.h>

#define ON 1
#define OFF 0

void iniGraphics(HWND, int, int);

void viewStart(float);
void viewEnd(float);
void setPerspective(float);
void orthographicMode(int);
void fogMode(int);
void fogColor(int);
void fogStart(float);
void fogEnd(float);
void antialiasingMode(int);

void setColor(int *, int);
void clearScreen();
void drawLine(float *, int);
void drawPath(float *, int);
void showContent();

void refreshGraphics();
void closeGraphics();
