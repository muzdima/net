#pragma once
#include <string>

class texter
{
protected:
	std::string text;
	unsigned int point;
public:
	texter():text(""),point(0)								{}
	texter(std::string _text):text(_text),point(0)			{}
	bool is_end()											{ return point==text.size(); }
	bool skip(unsigned int kol)								{ point+=kol; if (point>text.size()) point=text.size(); return !is_end(); }
	void set_text(std::string _text)						{ text=_text; point=0; }
	bool skip(std::string to)								;
	std::string substr(std::string to)						;
	std::string substr(std::string from, std::string to)	;
	void back(unsigned int kol)								{ if (point>kol) point-=kol; else point=0; }
	char getNextChar()										{ if (is_end()) return '\0'; point++; return text.at(point-1); }
	unsigned int getNextUInt()								;
};