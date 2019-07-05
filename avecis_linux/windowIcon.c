#include <X11/Xutil.h>

static XImage *ximage;

void setWindowIcon(Display *dpy, Window win, char *iconData, int iconWidth, int iconHeight)
{
   Visual *vis;
   int scrn;
   GC dgc;
   XWMHints* win_hints;
   Pixmap icon_pixmap;
   
   scrn = DefaultScreen(dpy);
   vis = DefaultVisual(dpy, scrn);
   dgc = DefaultGC(dpy, scrn);
   
   // copy the icon data into a XImage
   icon_pixmap = XCreatePixmap(dpy, win, iconWidth, iconHeight, 24);
   ximage = XCreateImage(dpy, vis, 24, ZPixmap, 0, &iconData[0], iconWidth, iconHeight, 32, 0);
   XPutImage(dpy, icon_pixmap, dgc, ximage, 0, 0, 0, 0, iconWidth, iconHeight);
   
   // let the window manager know about the icon
   win_hints = XAllocWMHints();  
   
   win_hints->flags = IconPixmapHint | StateHint | IconPositionHint;                        
   win_hints->icon_pixmap = icon_pixmap;
   win_hints->initial_state = NormalState;// the window is in normal state
   win_hints->icon_x = 0;
   win_hints->icon_y = 0;

   XSetWMHints(dpy, win, win_hints);

   XFree(win_hints);
}

void unsetWindowIcon()
{
   ximage->data = 0;//to keep the manually supplied data from getting freed
   XDestroyImage(ximage);
}
