//
// Created by Сергей on 01.07.2017.
//

#ifndef CLIONMYSTEND_MYTABLE_H
#define CLIONMYSTEND_MYTABLE_H

#include <FL/Fl.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Menu_Button.H>
#include <vector>
#include <set>
#include <string>
using namespace std;

class MyTable : public Fl_Table
{
protected:
    Fl_Color cell_bgcolor; // color of cell's bg color
    Fl_Color cell_fgcolor; // color of cell's fg color
public:
    MyTable(int X, int Y, int W, int H, const char* l = 0, const unsigned int base = 16);
    virtual void draw_cell(TableContext context, int R = 0, int C = 0, int X = 0, int Y = 0, int W = 0, int H = 0);
    unsigned int _base;
};
class MyHexBinTable : public MyTable
{
public:
    enum TABLE_TYPE {_BT, _SH, _WD};
    MyHexBinTable(int X, int Y, int W, int H, const char* l=0, TABLE_TYPE tp=_BT);
    ~MyHexBinTable() {delete m;}
    void set_vector(std::vector<unsigned char>& v);
    virtual void to_end_row();
    std::vector<unsigned char> vec;
    TABLE_TYPE _tp {_BT};
    void change_type(TABLE_TYPE t);
    Fl_Menu_Button *m {};
protected:
    virtual void draw_cell(TableContext context, int R = 0, int C = 0, int X = 0, int Y = 0, int W = 0, int H = 0);
};
struct MyHexInput : public Fl_Input
{
    MyHexInput(int x, int y, int w, int h, const char* t = 0);
    std::set<char> st;
    int handle(int event);
};
class MyHBEdTable : public MyHexBinTable
{
public:
    MyHBEdTable(int X, int Y, int W, int H, const char* l = 0);
    virtual ~MyHBEdTable();
    int handle(int event);
    void change_type(TABLE_TYPE t);
private:
    MyHexInput* input;
    int row_edit, col_edit;               // row/col being modified
    int s_left, s_top, s_right, s_bottom; // kb nav + mouse selection
    Fl_Menu_Button *m;
protected:
    void event_callback2();                        // table's event callback(instance)
    static void event_callback(Fl_Widget*, void*); // table's event callback (static)
    static void input_cb(Fl_Widget*, void*); // input widget's callback
    virtual void draw_cell(TableContext context, int R = 0, int C = 0, int X = 0, int Y = 0, int W = 0, int H = 0);
    // Apply value from input widget to values[row][col] array and hide (done editing)
    void set_value_hide();
    // Start editing a new cell: move the Fl_Int_Input widget to specified row/column
    //    Preload the widget with the cell's current value,
    //    and make the widget 'appear' at the cell's location.
    void start_editing(int R, int C);
    // Tell the input widget it's done editing, and to 'hide'
    void done_editing();
};

class MyBinFileTable : public MyHexBinTable
{
public:
    MyBinFileTable(int X, int Y, int W, int H, const char* l = 0)
        : MyHexBinTable(X, Y, W, H, l)    {}
    void set_file(const std::string& fn);
private:
    std::string fname;
};
#endif // CLIONMYSTEND_MYTABLE_H
