namespace mymod
{

struct CMyClass
{
    struct Impl;
    CMyClass();
    ~CMyClass();
    void Foo();
    void Bar();

private:
    Impl* pImpl = nullptr;
};

}

