
#include <iostream>
#include "Controller.h"

using namespace std;
/*
void callback(void *p) {
    Controller *pc = reinterpret_cast<Controller *>(p);
    pc->m_tbl->redraw();
    Fl::repeat_timeout(0.005, callback, p);
}
void my_callback(Fl_Widget *, void *) {
    if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)
        return; // ignore Escape
    exit(0);
}
*/

int main(int argc, char *argv[]) {
    try {
        Controller ctr;
//        Fl::add_timeout(0.1, callback, &ctr);
//        ctr.main_window->callback(my_callback);
        ctr.main_window->show();
        return Fl::run();
    }catch (string &s) {
        Fl::error(s.c_str());
    }catch (...) {
        Fl::error("Неизвестная ошибка");
    }
}
