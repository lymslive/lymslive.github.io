#include <stdlib.h>
#include <stdio.h>

// 临时截断字符串
class CutString
{
    const char* ptr_ = nullptr;
    size_t len_ = 0;
    char last_ = '\0';

public:
    CutString(const char* ptr, size_t len)
        : ptr_(ptr), len_(len)
    {
        char* buffer = const_cast<char*>(ptr_);
        last_ = buffer[len_];
        buffer[len_] = '\0';
        printf("in CutString, save last: %c\n", last_);
    }

    ~CutString()
    {
        char* buffer = const_cast<char*>(ptr_);
        buffer[len_] = last_;
        printf("in ~CutString, restore last: %c\n", last_);
    }

    const char* c_str() const 
    {
        return ptr_;
    }
};

int main()
{
    char* buffer = (char*)malloc(1024);
    for (int i = 0; i < 1024; ++i)
    {
        buffer[i] = i % (127-32) + 32;
    }

    printf("at buffer[16]: %c\n", buffer[16]);
    printf("first buffer[16]: %s\n", CutString(buffer, 16).c_str());
    printf("at buffer[16]: %c\n", buffer[16]);

    free(buffer);
}
