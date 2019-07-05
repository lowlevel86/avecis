#include <X11/Xlib.h>

#define ON 1
#define OFF 0

void iniGraphics(Display *, Window, int, int);

void viewStart(float);
void viewEnd(float);
void setPerspective(float);
void orthographicMode(int);
void fogMode(int);
void fogColor(int);
void fogStart(float);
void fogEnd(float);
void antialiasingMode(int);

void setColor(char *, int);
void clearScreen();
void drawLine(float *, int);
void drawPath(float *, int);
void showContent();

void refreshGraphics();
void closeGraphics();
