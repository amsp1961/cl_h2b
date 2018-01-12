#ifndef _MY_UTF8_H_
#define _MY_UTF8_H_

#include <FL/fl_utf8.h>
#include <FL/Fl_Input.H>
#include <FL/Fl.h>
#include <string>
#include <fstream>
#include <set>

class str_utf8
{
	std::string str;
public:
	str_utf8(std::string s) : str{s} { memset(_buf, 0, 1000); }
	char* utf8() {
		fl_utf8from_mb(_buf, 996, str.c_str(), str.length() );
		return (char*)_buf;
	}
	void operator=(const char* p) {
		str = p;
	}
	char _buf[1000];
};

class ansi
{
	std::string str;
public:
	ansi(const char* l) {
		int len = strlen(l);
		char * p = new char[len*4]{};
		fl_utf8to_mb(l, len, p, len*4-1);
		str = p;
		delete [] p;
	}
	std::string rstr() {
		return str;
	}
};

class MyInput : public Fl_Input
{
public:
	MyInput(int x, int y, int w, int h, const char* t = 0)
		: Fl_Input{x,y,w,h,t} {}
	int handle(int event) {
		int ret = Fl_Input::handle(event);
		switch(event) {
			case FL_PASTE:
				value("");
				value(Fl::event_text());
				break;
		}
		return ret;
	}
};

class MyHexInput : public Fl_Input
{
public:
    MyHexInput(int x, int y, int w, int h, const char* t = 0)
		: Fl_Input{x,y,w,h,t}, st{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','A','B','C','D','E','F', 8, 81, 83, -1} {}
    int handle(int event) {
        switch(event) {
        case FL_KEYDOWN:
            if (st.find(Fl::event_key()) != st.end() )
                return Fl_Input::handle(event);
            return 0;
        default:
            return Fl_Input::handle(event);
        }
    }
    std::set<char> st;
};

void protocol_print();

#endif

