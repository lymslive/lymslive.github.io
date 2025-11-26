namespace mymod
{

struct CMyClass
{
    CMyClass();
    ~CMyClass();
    void Foo();
    void Bar();

private:
    struct Impl;
    Impl* pImpl = nullptr;
};

}

