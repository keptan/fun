#ifndef STRING_H
#define STRING_H 

#include "list.h" 



//pretty much a typedef and two constructors 
class String : public List<char>
{
	int strlen (const char* s, const int l = 0) const
	{
		if (s[l])
		{
			return strlen(s, l + 1);
		}

		return l ;
	}

	List<char> strList (const char* str) const
	{
		if(*str)
			return *str + strList(str +1);

		return List<char>(nullptr);
	}

public:
	String (const char* str)
		: List<char>(strList(str))
	{}

	String (const List<char> l)
		: List<char>(l)
	{}
};


template<typename T>
List<T> operator + (const String  a, const String b)
{
	return a.push_back(b);
}

template<typename T>
std::ostream& operator<< (std::ostream& os, const String& l) //ostreams the elements, maybe move this OUT of the lib because OS isn't const 
{
	if(!l.length())
		return os;

	os << l.peek();
	
	return os <<  l.pop();
}




#endif
