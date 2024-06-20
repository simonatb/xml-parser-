#include "String.h"
#include <cstring>
#pragma warning(disable:4996)

void String::copyFrom(const String& other)
{
    len = other.len;
    capacity = other.capacity;
    data = new char[capacity + 1];
    strcpy(data, other.data);
}

void String::free()
{
    delete[] data;
    data = nullptr;
    len = capacity = 0;
}

void String::resize()
{
    capacity = (capacity + 1) * 2 - 1;
    char* newData = new char[capacity + 1];
    strcpy(newData, data);
    delete[] data;

    data = newData;
}

String::String(unsigned cap)
{
    capacity = cap;
    len = 0;
    data = new char[capacity];
}

String::String() : String("")
{
}

String::String(std::stringstream& ss)
{
    std::string tmp = ss.str(); // Extract the content as a std::string
    len = tmp.size();
    capacity = std::max((int)getNextPowerOfTwo(len), 16) - 1;
    data = new char[capacity + 1];
    std::strcpy(data, tmp.c_str());
}

String::String(const char* str)
{
    len = strlen(str);
    capacity = std::max((int)getNextPowerOfTwo(len), 16) - 1;
    data = new char[capacity + 1];
    strcpy(data, str);
}

String::String(const String& other)
{
    copyFrom(other);
}

String& String::operator=(const String& other)
{
    if (this != &other) {
        free();
        copyFrom(other);
    }

    return *this;
}

String::~String()
{
    free();
}

unsigned String::length() const
{
    return len;
}

const char* String::c_str() const
{
    return data;
}

String& String::concat(const String& other)
{
    if (len + other.len <= capacity) {
        strcat(data, other.data);
        len += other.len;
        return *this;
    }
    len += other.len;
    capacity = getNextPowerOfTwo(len) - 1;
    char* res = new char[capacity + 1];
    strcpy(res, data);
    strcat(res, other.data);

    delete[] data;
    data = res;

    return *this;
}

String& String::operator+=(const String& other)
{
    return concat(other);
}

String& String::operator+=(char ch)
{
    if (len == capacity) {
        resize();
    }
    data[len++] = ch;
    return *this;
}

bool String::isAlnum(size_t index) const
{
    char ch = data[index];
    if ((ch >= 'a' && ch <= 'z')|| (ch >= 'A' && ch <= 'Z')|| (ch >= '0' && ch <= '9')) {
        return true;
    }
    return false;
}

bool String::isSpace(size_t index) const
{
    char ch = data[index];
    if (ch == ' ' || ch == '\f' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\v') {
        return true;
    }
    return false;
}

String String::substr(size_t start, size_t length) {
    if (start >= len) {
        return String(""); // Return an empty string if start is out of bounds
    }

    if (start + length > len) {
        length = len - start; // Adjust length if it exceeds the bounds of the string
    }

    char* substr = new char[length + 1];
    strncpy(substr, data + start, length);
    substr[length] = '\0';

    String result(substr);
    delete[] substr;
    return result;
}

char& String::operator[](size_t index)
{
    return data[index];
}

char String::operator[](size_t index) const
{
    return data[index];
}

unsigned getNextPowerOfTwo(unsigned n)
{
    unsigned step = 1;

    while ((n >> step) > 0) {
        n |= n >> step;
        step *= 2;
    }

    return n + 1;
}

String operator+(const String& lhs, const String& rhs)
{
    unsigned len = lhs.len + rhs.len;
    unsigned capacity = getNextPowerOfTwo(len) - 1;

    String res(capacity);
    strcpy(res.data, lhs.data);
    strcat(res.data, rhs.data);
    res.len = len;

    return res;
}

std::istream& operator>>(std::istream& is, String& str)
{
    char buff[1024];
    is >> buff;

    delete[] str.data;
    str.len = strlen(buff);
    str.capacity = std::max((int)getNextPowerOfTwo(str.len), 16) - 1;
    str.data = new char[str.capacity + 1];
    strcpy(str.data, buff);

    return is;
}

std::ostream& operator<<(std::ostream& os, const String& str)
{
    os << str.c_str();
    return os;
}

bool operator<(const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

bool operator<=(const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) <= 0;
}
bool operator>=(const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) >= 0;
}
bool operator>(const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) > 0;
}
bool operator==(const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) == 0;
}
bool operator!=(const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) != 0;
}