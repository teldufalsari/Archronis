#ifndef TLD_TSTRING_H
#define TLD_TSTRING_H

#include <iostream>
#include <string>


namespace tld
{

const size_t Max_Str_Len = 1023;

class string
{

	
private:
	char* text_;
	size_t size_;
	
public:
	string();

	string(const string& that);
	
	string(string&& that) noexcept;
	
	~string();
	
	string(const char* str);
	
	string& operator =(const string& that);
	
	string& operator =(string&& that) noexcept;
	
	string& operator =(const char* str);

	char& operator [](size_t pos);

	const char& operator [](size_t pos) const;

	size_t size() const;
	
	friend string operator +(const string& lht, const string& rht);
	
	friend string& operator +=(string& lht, const string& rht);
	
	friend inline bool operator ==(const string& lht, const string& rht);
	
	friend inline bool operator >(const string& lht, const string& rht);

	friend inline bool operator <(const string& lht, const string& rht);
	
	friend inline bool operator !=(const string& lht, const string& rht);
	
	friend inline bool operator >=(const string& lht, const string& rht);
	
	friend inline bool operator <=(const string& lht, const string& rht);
	
	friend std::ostream& operator <<(std::ostream& os, const string& str);
	
	friend string& operator +=(string& lht, const char* rht);
	
	friend string& operator +=(string& lht, char rht);
	
};


} //namespace tld
#endif //TLD_TSTRING_H
