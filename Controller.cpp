#include <io.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <FL/Fl_Native_File_Chooser.H>
#include "Controller.h"
using namespace std;

MyInterface::MyInterface(){
    input_file_edt->callback(input_file_changed, this);
    open_file_btn->callback(open_btn_cb, this);
    save_btn->callback(save_btn_cb, this);
    find_file_btn->callback(find_file_cb, this);
}
MyInterface::~MyInterface(){
}
void MyInterface::find_file_cb(Fl_Widget *wid, void *pVoid) {
    auto p = (MyInterface *) pVoid;

    Fl_Native_File_Chooser fnfc;
	fnfc.title("Open a file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
	fnfc.filter("SREC\t*.srec\n");
	// Show native chooser
	switch ( fnfc.show() ) {
		case -1: Fl::error(fnfc.errmsg()); break; // ERROR
		case 1:  break; // CANCEL
		default:
		    p->input_file_edt->value(fnfc.filename());
		    p->open_file_btn->activate();
	};

}
void MyInterface::input_file_changed(Fl_Widget *wid, void *pVoid) {
    auto p = (MyInterface *) pVoid;
    char buf[1000];
    memset(buf, 0, 1000);
    fl_utf8to_mb(p->input_file_edt->value(), strlen(p->input_file_edt->value()), buf, 999);
    //strcpy(buf, p->input_file_edt->value());

    if (_access(buf,0)==0)
        p->open_file_btn->activate();
    else
        p->open_file_btn->deactivate();
}
void MyInterface::open_btn_cb(Fl_Widget *wid, void *pVoid) {
    auto p = (MyInterface *) pVoid;
    p->open_btn();
}
void MyInterface::save_btn_cb(Fl_Widget *wid, void *pVoid) {
    auto p = (MyInterface *) pVoid;
    p->_save_btn();
}

char buf[1000];
char* mb(const char* p) {
    memset(buf, 0, 1000);
    fl_utf8to_mb(p, strlen(p), buf, 999);
    return buf;
}
char* mb(const string &s) {
    memset(buf, 0, 1000);
    fl_utf8to_mb(s.c_str(), strlen(s.c_str()), buf, 999);
    return buf;
}
void menu_cb(Fl_Widget* w, void* v) {
  Fl_Menu_* mw = (Fl_Menu_*)w;
  MyHexBinTable* p_table = (MyHexBinTable*)v;

  int _v = mw->value();
  switch (_v) {
  case 0:
      p_table->row_position(0);
      break;
  case 1:
      p_table->row_position(p_table->vec.size()/16+10);
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
  }
}
Controller::Controller() {
    //table->vec.resize(16);
    std::vector<unsigned char> v1;

    for (auto i=0; i<3*16; ++i)
        v1.push_back(0);
    table->set_vector(v1);
    inp_ch->add("00");
    inp_ch->add("ff");
    inp_ch->value(0);

    table->m->add("В начало");
    table->m->add("В конец");
    table->m->add("байт");
    table->m->add("полуслово");
    table->m->add("слово");
    table->m->callback(menu_cb, table);
}
Controller::~Controller(){
}
class my_file_exception : public exception {
    string str;
public:
    my_file_exception(const string s) : str{s} {}
    virtual const char* what() const _GLIBCXX_USE_NOEXCEPT {
        return str.c_str();
    }
};
void Controller::_save_btn() {
    string s = input_file_edt->value();
    auto pos = s.rfind('.');
    string tmp;
    if (pos) {
        string subs = s.substr(0,pos);
        subs += ".bin";
        tmp = subs;
    } else {
        tmp = s + ".bin";
    }
    ofstream ofs(mb(tmp), ios::binary | ios::trunc);
    try {
        if (ofs.is_open() == false)
            throw my_file_exception(string("Файл ") + input_file_edt->value() + string(" не удаётся открыть!"));
        for (auto i=0; i<table->vec.size(); ++i)
            ofs.put(table->vec[i]);
        ofs.close();
    } catch (exception &e) {
        Fl::error(mb(e.what()));
        if (ofs.is_open())
            ofs.close();
    }
}
void Controller::open_btn(){
    // activity of button warranted existence of file
    ifstream input_f(mb(input_file_edt->value()));
    try {
        if (input_f.is_open() == false)
            throw my_file_exception(string("Нет файла ") + input_file_edt->value());
        unsigned long adr_begin = 0xffffffff;
        unsigned int len{};
        // find start address
        while (input_f) {
            string ls;
            getline(input_f, ls);
            if (ls.length()==0)
                continue;
            if (ls[0]!='S')
                throw my_file_exception(string("Неверный формат файла ") + input_file_edt->value());
            if (ls[1]=='0') // block's header
                continue;
            if (ls[1]=='3') {
                string s = ls.substr(4, 8);
                unsigned long v = stoul(s, nullptr, 16);
                if (v<adr_begin)
                    adr_begin = v;
            }
        }
        input_f.close();
        {
            stringstream ss;
            ss << hex << adr_begin;
            string s;
            ss >> s;
            base_addr_oput->value(s.c_str());
        }
        // find length of vector
        if (input_f.is_open() == false)
            input_f.open(mb(input_file_edt->value()));
        input_f.seekg(0, ios::beg);
        while (input_f) {
            string ls;
            getline(input_f, ls);
            if (ls.length()==0)
                continue;
            if (ls[1]=='0') // block's header
                continue;
            if (ls[1]=='3') {
                string s = ls.substr(4, 8);
                unsigned long v = stoul(s, nullptr, 16);
                string s2 = ls.substr(2, 2);
                int v2 = stoi(s2,nullptr,16);
                v2 -= (4+1); // remove address and cs
                int tst = v2+v-adr_begin;
                if ( tst > len)
                    len = tst;
            }
        }
        input_f.close();
        {
            stringstream ss;
            ss << hex << len;
            string s;
            ss >> s;
            len_oput->value(s.c_str());
        }
        // create new vector
        vector<unsigned char> tmp_v;
        tmp_v.resize(len);
        unsigned char val = (unsigned char)stoul(inp_ch->value(), nullptr, 16);
//        generate(tmp_v.begin(), tmp_v.end(),[] {return val;} );
        for (auto i=0; i<tmp_v.size(); ++i)
            tmp_v[i] = val;

        if (input_f.is_open() == false)
            input_f.open(mb(input_file_edt->value()));

        input_f.seekg(0, ios::beg);
        while (input_f) {
            string ls;
            getline(input_f, ls);
            if (ls.length()==0)
                continue;
            if (ls[1]=='0') // block's header
                continue;
            if (ls[1]=='3') {
                string s = ls.substr(4, 8);
                unsigned long cur_addr = stoul(s, nullptr, 16);
                string s2 = ls.substr(2, 2);
                int v2 = stoi(s2,nullptr,16);
                v2 -=(4+1);  // remove address and cs
                for (auto i=0; i<v2; ++i) {
                    string s3 = ls.substr(12+i*2,2);
                    unsigned long val = stoul(s3,nullptr,16);
                    tmp_v[cur_addr-adr_begin+i] = val;
                }
            }
        }
        this->table->set_vector(tmp_v);
        table->redraw();
        input_f.close();
        save_btn->activate();
    } catch(exception& e) {
        Fl::error(mb(e.what()));
        if (input_f.is_open())
            input_f.close();
    }
}
