#ifndef STRING_H
#define STRING_H 

#include "list.h" 

int strlen (const char* s, const int l = 0)
{
	if (s[l])
	{
		return strlen(s, l + 1);
	}

	return l - 1;
}

List<char> strList (const char* str, const int len, const int back = 0)
{
	if(len != back)
		return push( strList(str, len, back + 1), str[back]);

	return List<char>(str[len]);
}

class String : public List<char>
{
public:
	String (const char* str)
		: List<char>(strList(str, strlen(str)))
	{}
};

#endif

