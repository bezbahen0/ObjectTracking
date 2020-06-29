#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>

#include <iostream>

Window* getWindowList(Display* disp, unsigned long* len)
{
    Atom prop = XInternAtom(disp, "_NET_CLIENT_LIST", false);
    Atom type;
    int form;
    unsigned long remain;
    unsigned char* list;

    if (XGetWindowProperty(disp,XDefaultRootWindow(disp),prop,0,1024, false,33,
                &type,&form,len,&remain,&list) != Success) 
    {  
        return nullptr;
    }

    return (Window*) list;
}
char* getWindowName(Display* disp, Window win)
{
    Atom prop = XInternAtom(disp, "WM_NAME", false);
    Atom type;
    int form;
    unsigned long remain;
    unsigned long len;
    unsigned char* list;


    if (XGetWindowProperty(disp, win, prop, 0, 1024, false, AnyPropertyType,
                &type, &form, &len, &remain, &list) != Success) 
    {
        return nullptr;
    }

    return (char*) list;
}

int main()
{
    unsigned long len;
    Display* disp = XOpenDisplay(nullptr);
    Window* list;
    char* name;

    list = (Window*) getWindowList(disp, &len);
    for (int i = 0; i < (int)len; i++) 
    {
        name = getWindowName(disp, list[i]);
        std::cout << i << ": " << name << std::endl;
        delete(name);
    }

    return 0;
}
