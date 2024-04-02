#include "hide-impl.h"
#include <stdio.h>

namespace mymod
{

struct CMyClass::Impl
{
    void Foo() { printf("Impl::Foo\n"); }
    void Bar() { printf("Impl::Bar\n"); }
};

CMyClass::CMyClass()
{
    pImpl = new Impl;
}

CMyClass::~CMyClass()
{
    delete pImpl;
}

void CMyClass::Foo()
{
    pImpl->Foo();
}

void CMyClass::Bar()
{
    pImpl->Bar();
}

}
