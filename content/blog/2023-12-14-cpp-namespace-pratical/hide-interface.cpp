#include "hide-interface.h"
#include <stdio.h>

namespace mymod {
    struct CHisClass : public Interface {
        CHisClass() {}
        virtual void Foo() override { printf("CHisClass:Foo\n"); }
        virtual void Bar() override { printf("CHisClass:Bar\n"); }
    };

    struct CHerClass : public Interface {
        CHerClass() {}
        virtual void Foo() override { printf("CHerClass:Foo\n"); }
        virtual void Bar() override { printf("CHerClass:Bar\n"); }
    };

    Interface* CreateObject(int arg) {
        if (arg == 1) {
            return new CHisClass();
        } else {
            return new CHerClass();
        }
    }
    void FreeObject(Interface* pObject) {
        delete pObject;
    }
} // end of namespace

