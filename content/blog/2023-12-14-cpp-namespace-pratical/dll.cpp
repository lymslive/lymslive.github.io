#include "dll.h"
#include "foo.h"
#include "bar.h"

namespace dll
{
    void Foo()
    {
        foo::Work();
    }
    void Bar()
    {
        bar::Work();
    }
} // end of namespace

void mydll_foo()
{
    dll::Foo();
}

void mydll_bar()
{
    dll::Bar();
}
