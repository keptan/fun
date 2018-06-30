#ifndef STRING_H
#define STRING_H 

#include "list.h" 

int strlen (const char* s, const int l = 0)
{
	if (s[l])
	{
		return strlen(s, l + 1);
	}

	return l ;
}

List<char> strList (const char* str)
{
	if(*str)
		return push( strList( str + 1), *str);

	return List<char>(nullptr);
}

//pretty much a typedef and two constructors 
class String : public List<char>
{
public:
	String (const char* str)
		: List<char>(strList(str))
	{}

	String (const List<char> l)
		: List<char>(l)
	{}
};

#endif
