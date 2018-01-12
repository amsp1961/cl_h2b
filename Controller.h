#pragma once
#include "hex2bin.h"

class MyInterface : public UserInterface
{
public:
    MyInterface();
    ~MyInterface();
protected:
    static void input_file_changed(Fl_Widget *wid, void *pVoid);
    static void open_btn_cb(Fl_Widget *wid, void *pVoid);
    static void save_btn_cb(Fl_Widget *wid, void *pVoid);
    static void find_file_cb(Fl_Widget *wid, void *pVoid);
    virtual void open_btn()=0;
    virtual void _save_btn()=0;
};

class Controller : public MyInterface
{
public:
    Controller();
    ~Controller();
protected:
    virtual void open_btn();
    virtual void _save_btn();
};

