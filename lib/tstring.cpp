#include "tstring.h"
#include <bits/ostream_insert.h>

namespace tld
{

string::string() :
	text_((char*) new char[Max_Str_Len]),
	size_(0)
{
	text_[0] = '\0';
}

string::string(const string& that) :
	text_((char*) new char[Max_Str_Len]),
	size_(that.size_) 
{
	for (size_t i = 0; i < Max_Str_Len; i++)
		this->text_[i] = that.text_[i];
}

string& string::operator =(const string& that)
{
	if (this != &that) {
		for (size_t i = 0; i < Max_Str_Len; i++)
			this->text_[i] = that.text_[i];
		this->size_ = that.size_;
	}
	return *this;
}

string operator +(const string& lht, const string& rht)
{
	string tmp(lht);
	tmp.size_ += rht.size_;
	size_t j = 0;
	for (size_t i = lht.size_; i < tmp.size_; i++) {
		tmp.text_[i] = rht.text_[j];
		j++;
	}
	return tmp;
}

bool operator ==(const string& lht, const string& rht)
{
	if (lht.size_ != rht.size_)
		return false;
	for (size_t i = 0; i < lht.size_; i++)
		if (lht.text_[i] != rht.text_[i])
			return false;
	return true;
}

bool operator >(const string& lht, const string& rht)
{
	if (lht.size_ < rht.size_) {
		for (size_t i = 0; i < lht.size_; i++)
			if (lht.text_[i] > rht.text_[i])
				return true;
		return false;
	} else {
		for (size_t i = 0; i < rht.size_; i++)
			if (lht.text_[i] < rht.text_[i])
				return false;
		return true;
	}
}

bool operator <(const string& lht, const string& rht)
{
	if (lht.size_ < rht.size_) {
		for (size_t i = 0; i < lht.size_; i++)
			if (lht.text_[i] > rht.text_[i])
				return false;
		return true;
	} else {
		for (size_t i = 0; i < rht.size_; i++)
			if (lht.text_[i] < rht.text_[i])
				return true;
		return false;
	}
}

bool operator !=(const string& lht, const string& rht)
{
	return !(lht == rht);
}

bool operator >=(const string& lht, const string& rht)
{
	return !(lht < rht);
}

bool operator <=(const string& lht, const string& rht)
{
	return !(lht > rht);
}

string& operator +=(string& lht, const string& rht)
{
	size_t j = 0;
	while ((rht.text_[j] != '\0') && (lht.size_ < Max_Str_Len))
	{
		lht.text_[lht.size_] = rht.text_[j];
		lht.size_++;
		j++;
	}
	lht.text_[lht.size_++] = '\0';	
	return lht;
}

std::ostream& operator <<(std::ostream& os, const string& str)
{
	return __ostream_insert(os, str.text_, str.size_);
}

string::string(const char* str) :
	text_((char*) new char[Max_Str_Len]),
	size_(0)
{
	while ((str[size_] != '\0') && (size_ < Max_Str_Len))
	{
		text_[size_] = str[size_];
		size_++;
	}
	text_[Max_Str_Len] - '\0';
}

string& string::operator =(const char* str)
{
	this->size_ = 0;
	while ((str[size_] != '\0') && (size_ < Max_Str_Len))
	{
		text_[size_] = str[size_];
		size_++;
	}
	text_[size_] = '\0';
	return *this;
}

string& operator +=(string& lht, const char* str)
{
	size_t j = 0;
	while ((str[j] != '\0') && (lht.size_ < Max_Str_Len))
	{
		lht.text_[lht.size_] = str[j];
		lht.size_++;
		j++;
	}
	lht.text_[lht.size_++] = '\0';
	return lht;
}

string& operator +=(string& lht, const char rht)
{
	if (lht.size_ < Max_Str_Len)
	{
		lht.text_[lht.size_++] = rht;
		lht.text_[lht.size_] = '\0';
	}
	return lht;
}


string::~string()
{
	delete[] text_;	
}

string::string(string&& that) noexcept :
	text_(that.text_),
	size_(that.size_)
{
	that.text_ = nullptr;
}



string& string::operator =(string&& that) noexcept
{
	delete[] this->text_;
	this->text_ = that.text_;
	that.text_ = nullptr;
	return *this;
}

char& string::operator [](size_t pos)
{
	return text_[pos];
}

const char& string::operator [](size_t pos) const
{
	return text_[pos];
}

size_t string::size() const
{
	return this->size_;
}

} //namespace tld
