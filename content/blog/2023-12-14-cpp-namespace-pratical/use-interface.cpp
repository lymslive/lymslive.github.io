#include "hide-interface.h"

int main()
{
    mymod::Interface* p = mymod::CreateObject(1);
    p->Foo();
    mymod::FreeObject(p);
    p = mymod::CreateObject(2);
    p->Bar();
    mymod::FreeObject(p);
}
