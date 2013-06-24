#include "texter.h"
using namespace std;


bool texter::skip(string to)
{
	if (is_end()) return false;
	unsigned int pos;
	if ((pos=text.find(to,point))==string::npos) 
	{
		point=text.size();
		return false;
	}
	point=pos+to.size();
	return !is_end();
}


string texter::substr(string to)
{
	if (is_end()) return "";
	unsigned int pos1=point,pos2;
	if ((pos2=text.find(to,point))==string::npos)
	{
		point=text.size();
		return text.substr(pos1);
	}
	point=pos2+to.size();
	return text.substr(pos1,pos2-pos1);
}


string texter::substr(string from, string to)
{
	skip(from);
	return substr(to);
}


unsigned int texter::getNextUInt()
{
	unsigned int res=0,status=0;
	while (status==0 && !is_end())
	{
		if (text.at(point)>='0' && text.at(point)<='9') { status=1; res=text.at(point)-'0'; }
		point++;
	}
	while (status==1 && !is_end())
	{
		status=3;
		if (text.at(point)>='0' && text.at(point)<='9') { status=1; res=res*10+text.at(point)-'0'; }
		point++;
	}
	return res;
}