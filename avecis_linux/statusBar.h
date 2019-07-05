#include <X11/Xlib.h>

#define STATUS_BAR_HEIGHT 20

void createStatusBar(Display *, Window, int, int);
void textToStatusBar(char *);
void refreshStatusBar();
