//
// Created by Сергей on 01.07.2017.
//
#include <sstream>
#include <iomanip>
#include <fstream>
#include "MyTable.h"

class string;

MyTable::MyTable(int X, int Y, int W, int H, const char *l, const unsigned int base)
        : Fl_Table(X, Y, W, H), cell_bgcolor{FL_WHITE}, cell_fgcolor{FL_BLACK}, _base{base} {
    col_header_color(FL_LIGHT1);
}
void MyTable::draw_cell(Fl_Table::TableContext context, int R, int C, int X, int Y, int W, int H) {
    switch (context) {
        case CONTEXT_STARTPAGE:
            fl_font(FL_COURIER, 8);
            return;
        case CONTEXT_COL_HEADER:
            fl_push_clip(X, Y, W, H);
            {
                fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
                fl_color(FL_BLACK);
                std::stringstream ss;
                ss << std::hex << C;
                fl_draw(ss.str().c_str(), X, Y, W, H, FL_ALIGN_CENTER);
            }
            fl_pop_clip();
            return;
        case CONTEXT_ROW_HEADER:
            fl_push_clip(X, Y, W, H);
            {
                fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
                fl_color(FL_BLACK);
                std::stringstream ss;
                ss << std::hex << R * 16 + _base;
                fl_draw(ss.str().c_str(), X, Y, W, H, FL_ALIGN_CENTER);
            }
            fl_pop_clip();
            return;
        case CONTEXT_CELL: {
            fl_push_clip(X, Y, W, H);
            {
                // BG COLOR
                // fl_color( row_selected(R) ? selection_color() : cell_bgcolor);
                fl_color(cell_bgcolor);
                fl_rectf(X, Y, W, H);

                // TEXT
                fl_color(cell_fgcolor);
                std::stringstream ss;
                ss << std::hex << std::setfill('0') << std::setw(2) << ((R * 16 + C) & 0xff);
                fl_draw(ss.str().c_str(), X, Y, W, H, FL_ALIGN_CENTER);

                // BORDER
                fl_color(color());
                fl_rect(X, Y, W, H);
            }
            fl_pop_clip();
            return;
        }
        case CONTEXT_ENDPAGE:
        case CONTEXT_RC_RESIZE:
        case CONTEXT_NONE:
        case CONTEXT_TABLE:
            return;
    }
    //    Fl_Table::draw_cell(context, R, C, X, Y, W, H);
}
MyHexBinTable::MyHexBinTable(int X, int Y, int W, int H, const char *l, TABLE_TYPE tp)
        : MyTable(X, Y, W, H, l) {
    row_header(1);
    col_header(1);
    switch (tp) {
        case _BT:
            cols(16);
            col_width_all(27);
            break;
        case _SH:
            cols(8);
            col_width_all(56);
            break;
        case _WD:
            cols(4);
            col_width_all(108);
            break;
    }
//    cols(16);
//    col_width_all(25);
    row_header_width(80);
    m = new Fl_Menu_Button(0, 0, 700, 400, nullptr);
    Fl_Menu_Button &mb = *m;
    mb.type(Fl_Menu_Button::POPUP3);
    mb.box(FL_NO_BOX);
    end();
}
void MyHexBinTable::change_type(TABLE_TYPE t) {
    _tp = t;
    switch (t) {
        case _BT:
            cols(16);
            col_width_all(27);
            break;
        case _SH:
            cols(8);
            col_width_all(54);
            break;
        case _WD:
            cols(4);
            col_width_all(108);
            break;
    }
    this->redraw();
}
void MyHexBinTable::set_vector(std::vector<unsigned char> &v) {
    swap(vec, v);
    unsigned long long int val;
    val = vec.size() % 16 ? vec.size() / 16 + 1 : vec.size() / 16;
    rows((int) val);
}
void MyHexBinTable::to_end_row() {
    row_position(rows() - (botrow - toprow));
}
void MyHexBinTable::draw_cell(Fl_Table::TableContext context, int R, int C, int X, int Y, int W, int H) {
    switch (context) {
        case CONTEXT_STARTPAGE:
            fl_font(FL_COURIER, 16);
            return;
        case CONTEXT_COL_HEADER:
            fl_push_clip(X, Y, W, H);
            {
                fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
                fl_color(FL_BLACK);
                std::stringstream ss;
                switch (_tp) {
                    case _BT:
                        ss << std::hex << C;
                        break;
                    case _SH:
                        ss << std::hex << C * 2;
                        break;
                    case _WD:
                        ss << std::hex << C * 4;
                        break;
                }
                fl_draw(ss.str().c_str(), X, Y, W, H, FL_ALIGN_CENTER);
            }
            fl_pop_clip();
            return;
        case CONTEXT_ROW_HEADER:
            fl_push_clip(X, Y, W, H);
            {
                fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
                fl_color(FL_BLACK);
                std::stringstream ss;
                ss << std::hex << R * 16; // + _base;
                fl_draw(ss.str().c_str(), X, Y, W, H, FL_ALIGN_CENTER);
            }
            fl_pop_clip();
            return;
        case CONTEXT_CELL: {
            fl_push_clip(X, Y, W, H);
            {
                // BG COLOR
                // fl_color( row_selected(R) ? selection_color() : cell_bgcolor);
                fl_color(cell_bgcolor);
                fl_rectf(X, Y, W, H);

                // TEXT
                fl_color(cell_fgcolor);
                std::stringstream ss;
                switch (_tp) {
                    case _BT:
                        if ((R * 16 + C) < (int) vec.size())
                            ss << std::hex << std::setfill('0') << std::setw(2) << (unsigned) vec[R * 16 + C];
                            //                ss << hex << setfill('0') << setw(2) << (unsigned int)vec.at(R*16+C);
                        else
                            ss << "  ";
                        break;
                    case _SH:
                        if ((R * 16 + C * 2) < (int) vec.size())
                            ss << std::hex << std::setfill('0') << std::setw(2)
                               << (unsigned) vec[R * 16 + C * 2 + 1] << std::setw(2)
                               << (unsigned) vec[R * 16 + C * 2];
                            //                ss << hex << setfill('0') << setw(2) << (unsigned int)vec.at(R*16+C);
                        else
                            ss << "    ";
                        break;
                    case _WD:
                        if ((R * 16 + C * 4) < (int) vec.size())
                            ss << std::hex << std::setfill('0') << std::setw(2)
                               << (unsigned) vec[R * 16 + C * 4 + 3] << std::setw(2)
                               << (unsigned) vec[R * 16 + C * 4 + 2] << std::setw(2)
                               << (unsigned) vec[R * 16 + C * 4 + 1] << std::setw(2)
                               << (unsigned) vec[R * 16 + C * 4];
                            //                ss << hex << setfill('0') << setw(2) << (unsigned int)vec.at(R*16+C);
                        else
                            ss << "        ";
                        break;
                }
                fl_draw(ss.str().c_str(), X, Y, W, H, FL_ALIGN_CENTER);

                // BORDER
                fl_color(color());
                fl_rect(X, Y, W, H);
            }
            fl_pop_clip();
            return;
        }
        case CONTEXT_ENDPAGE:
        case CONTEXT_RC_RESIZE:
        case CONTEXT_NONE:
        case CONTEXT_TABLE:
            return;
    }
}
int MyHexInput::handle(int event) {
    switch (event) {
        case FL_KEYDOWN:
            if (st.find((const char &) Fl::event_key()) != st.end())
                return Fl_Input::handle(event);
            return 0;
        default:
            return Fl_Input::handle(event);
    }
}
MyHexInput::MyHexInput(int x, int y, int w, int h, const char *t)
        : Fl_Input(x, y, w, h, t), st{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
                                      'c', 'd', 'e', 'f', 'A', 'B', 'C', 'D', 'E', 'F', 8,   /*81, 83,*/ -1} {
    textfont(FL_COURIER);
    textsize(16);
    type(FL_INT_INPUT);
}
/*
Fl_Menu_Item pulldown[] = {
  {"Первый",	0, 0, (void*)1},
  {"Второй",	0, 0, (void*)2},
  {"Третий",	0, 0, (void*)3},
  {"Четвёртый",	0, 0, (void*)4},
  {"&Charm",	0, 0, (void*)5},
  {"Truth",	0, 0, (void*)6},
  {"Beauty",	0, 0, (void*)7},
  {0}
};
*/
void test_cb(Fl_Widget *w, void *v) {
    auto *mw = (Fl_Menu_ *) w;
    auto *p_table = (MyHBEdTable *) v;
    int _v = mw->value();
    switch (_v) {
        case 0:
            p_table->row_position(0);
            break;
        case 1:
//      p_table->row_position(p_table->vec.size()%16 ? p_table->vec.size()/16 : p_table->vec.size()/16+1);
            p_table->row_position(static_cast<int>(p_table->vec.size() / 16 + 10));
            break;
        case 2:
            p_table->change_type(MyHexBinTable::_BT);
            break;
        case 3:
            p_table->change_type(MyHexBinTable::_SH);
            break;
        case 4:
            p_table->change_type(MyHexBinTable::_WD);
            break;
        default:
            break;
    }
}
MyHBEdTable::MyHBEdTable(int X, int Y, int W, int H, const char *l)
        : MyHexBinTable(X, Y, W, H, l) {
    callback(&event_callback, (void *) this);
    when(FL_WHEN_NOT_CHANGED | when());
    // Create input widget that we'll use whenever user clicks on a cell
    input = new MyHexInput(W / 2, H / 2, 20, 20);
    input->box(FL_DOWN_BOX);
    input->hide();
    input->callback(input_cb, (void *) this);
    input->when(FL_WHEN_ENTER_KEY_ALWAYS); // callback triggered when user hits Enter
    input->maximum_size(2);
    // input->textcolor(FL_RED);
    row_edit = col_edit = 0;
    s_left = s_top = s_right = s_bottom = 0;
    m = new Fl_Menu_Button(0, 0, 700, 400, nullptr);
    Fl_Menu_Button &mb = *m;
    mb.type(Fl_Menu_Button::POPUP3);
    mb.box(FL_NO_BOX);
//    mb.menu(pulldown);
    mb.add("В начало", 0, nullptr, nullptr);
    mb.add("В конец", 0, nullptr, nullptr);
    mb.add("Байт", 0, nullptr, nullptr);
    mb.add("Полуслово", 0, nullptr, nullptr);
    mb.add("Слово", 0, nullptr, nullptr);
    mb.callback(test_cb, this);
    end();
}
void MyHBEdTable::event_callback(Fl_Widget *, void *v) {
    ((MyHBEdTable *) v)->event_callback2();
}
void MyHBEdTable::event_callback2() {
    int R = callback_row();
    int C = callback_col();
    TableContext context = callback_context();
    switch (context) {
        case CONTEXT_CELL: {         // A table event occurred on a cell
            switch (Fl::event()) {    // see what FLTK event caused it

                case FL_PUSH: {          // mouse click?
                    if (Fl::event_button() == FL_RIGHT_MOUSE) {
                        return;
                    }
                    done_editing();      // finish editing previous
                    start_editing(R, C); // start new edit
                }
                    return;
                case FL_KEYBOARD: // key press in table?
                    if (Fl::event() == FL_SHORTCUT)
                        return;
                    if (/*Fl::event()==FL_SHORTCUT &&*/ Fl::event_key() == ' ') {
                        if (R == -1 || C == -1)
                            return;
                        if (input->visible())
                            done_editing();
                        else
                            start_editing(R, C); // start new edit
                        return;
                    }
                    // if (C == cols()-1 || R == rows()-1) return;	// no editing of totals column

                    done_editing();            // finish any previous editing
                    set_selection(R, C, R, C); // select the current cell
                    start_editing(R, C);       // start new edit
                    if (Fl::event() == FL_KEYBOARD && Fl::e_text[0] != '\r')
                        input->handle(Fl::event()); // pass keypress to input widget
                    return;
                default:;
            }
            return;
        }
        case CONTEXT_TABLE:      // A table event occurred on dead zone in table
        case CONTEXT_ROW_HEADER: // A table event occurred on row/column header
        case CONTEXT_COL_HEADER:
            done_editing(); // done editing, hide
            return;
        default:
            return;
    }
}
MyHBEdTable::~MyHBEdTable() {
    delete m;
    delete input;
}
void MyHBEdTable::draw_cell(Fl_Table::TableContext context, int R, int C, int X, int Y, int W, int H) {
    switch (context) {
        case CONTEXT_CELL:
            if (R == row_edit && C == col_edit && input->visible())
                return; // dont draw for cell with input widget over it
        default:;
    }
    MyHexBinTable::draw_cell(context, R, C, X, Y, W, H);
}
void MyHBEdTable::set_value_hide() {
    try {
        std::string s(input->value());
        switch (_tp) {
            case _BT: {
                if (s.length() > 2)
                    s = s.substr(s.length() - 2, 2);
                int tmp = stoi(s, nullptr, 16);
                // vec[row_edit*16 + col_edit] =  tmp & 0xff;
                auto n = row_edit * 16 + col_edit;
                vec.at((unsigned int) n) = (unsigned char) (tmp & 0xff);
            }
                break;
            case _SH: {
                if (s.length() > 4)
                    s = s.substr(s.length() - 4, 4);
                int tmp = stoi(s, nullptr, 16);
                // vec[row_edit*16 + col_edit] =  tmp & 0xff;
                auto n = row_edit * 16 + col_edit * 2;
                vec.at((unsigned int) n) = static_cast<unsigned char>((unsigned short) (tmp & 0xff));
                vec.at((unsigned int) n + 1) = static_cast<unsigned char>((unsigned short) ((tmp >> 8) & 0xff));
            }
                break;
            case _WD: {
                if (s.length() > 8)
                    s = s.substr(s.length() - 8, 8);
                int tmp = static_cast<int>(stoul(s, nullptr, 16));
                // vec[row_edit*16 + col_edit] =  tmp & 0xff;
                auto n = row_edit * 16 + col_edit * 4;
                vec.at((unsigned int) n) = static_cast<unsigned char>((unsigned int) (tmp & 0xff));
                vec.at((unsigned int) n + 1) = static_cast<unsigned char>((unsigned int) ((tmp >> 8) & 0xff));
                vec.at((unsigned int) n + 2) = static_cast<unsigned char>((unsigned int) ((tmp >> 16) & 0xff));
                vec.at((unsigned int) n + 3) = static_cast<unsigned char>((unsigned int) ((tmp >> 24) & 0xff));
            }
                break;
        }
        input->hide();
        window()->cursor(FL_CURSOR_DEFAULT); // XXX: if we don't do this, cursor can disappear!
    }catch (...) {
        Fl::error("My Error");
    }
}
void MyHBEdTable::start_editing(int R, int C) {
    row_edit = R; // Now editing this row/col
    col_edit = C;
    int X, Y, W, H;
    find_cell(CONTEXT_CELL, R, C, X, Y, W, H); // Find X/Y/W/H of cell
    input->resize(X, Y, W, H);                 // Move Fl_Input widget there
    // char s[30]; sprintf(s, "%d", vec[R*16+C]);		// Load input widget with cell's current value
    std::stringstream ss;
    switch (_tp) {
        case _BT:
            ss << std::hex << (unsigned) vec[R * 16 + C];
            break;
        case _SH:
            ss << std::hex << std::setfill('0') << std::setw(2) << (unsigned) vec[R * 16 + C * 2 + 1] << std::setw(2)
               << (unsigned) vec[R * 16 + C * 2];
            break;
        case _WD:
            ss << std::hex << std::setfill('0') << std::setw(2) << (unsigned) vec[R * 16 + C * 4 + 3] << std::setw(2)
               << (unsigned) vec[R * 16 + C * 4 + 2]
               << std::setw(2) << (unsigned) vec[R * 16 + C * 4 + 1] << std::setw(2) << (unsigned) vec[R * 16 + C * 4];
            break;
    }
//    ss << std::hex << (unsigned) vec[R * 16 + C];
    input->value(ss.str().c_str());
    // input->selection_color(FL_GREEN);
    // input->textfont(FL_COURIER);
    input->position(0, (int) ss.str().length());
    input->show(); // Show the input widget, now that we've positioned it
    input->take_focus();
    switch (_tp) {
        case _BT:
            input->position(0, 2);
            break;
        case _SH:
            input->position(0, 4);
            break;
        case _WD:
            input->position(0, 8);
            break;
    }
}
void MyHBEdTable::done_editing() {
    if (input->visible()) { // input widget visible, ie. edit in progress?
        set_value_hide();  // Transfer its current contents to cell and hide
    }
}
void MyHBEdTable::input_cb(Fl_Widget *, void *v) {
    ((MyHBEdTable *) v)->set_value_hide();
}
void MyBinFileTable::set_file(const std::string &fn) {
    fname = fn;
    //    if ( access(fn.c_str(), 0) != 0)
    //        throw std::string("MyBFEdTable::set_file - отсутствует файл");
    std::ifstream ifs(fname, std::ios_base::binary);
    ifs.seekg(0);
    ifs.seekg(0, std::ios::end);
    std::fpos<mbstate_t> sz = ifs.tellg();
    ifs.seekg(0);
    this->vec.resize((unsigned int) sz);
    std::vector<unsigned char> tmp((unsigned int) sz);
    ifs.read(reinterpret_cast<char *>(&tmp[0]), (streamsize) sz);
    ifs.close();
    set_vector(tmp);
}
int MyHBEdTable::handle(int event) {
    switch (event) {
        case FL_KEYBOARD: {
            int _event_key = Fl::event_key();
            int _event_state = Fl::event_state();
            if (_event_state & FL_SHIFT)
                return 0;
            if (_event_state & FL_ALT)
                return 0;
            if (_event_state & FL_CTRL)
                return 0;
            switch (_event_key) {
                case FL_Right: {
                    if (select_col == (cols() - 1))
                        return 1;
                    break;
                    case ' ':
                        do_callback(CONTEXT_CELL, select_row, select_col);
                    return 1;
                }
                default:
                    break;
            }
        }
        default:
            break;
    }
    return Fl_Table::handle(event);
}
void MyHBEdTable::change_type(TABLE_TYPE t) {
    _tp = t;
    switch (t) {
        case _BT:
            cols(16);
            col_width_all(27);
            input->maximum_size(2);
            break;
        case _SH:
            cols(8);
            col_width_all(54);
            input->maximum_size(4);
            break;
        case _WD:
            cols(4);
            col_width_all(108);
            input->maximum_size(8);
            break;
    }
    this->redraw();
}
