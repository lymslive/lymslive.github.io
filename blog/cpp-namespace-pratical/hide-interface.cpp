#include "hide-interface.h"
#include <stdio.h>

namespace mymod {
    struct CHisClass : public Interface {
        int value_ = 11;
        CHisClass() {}
        virtual void Foo() override { printf("CHisClass::Foo %d\n", value_); }
        virtual void Bar() override { printf("CHisClass:Bar %d\n", value_); }
    };

    struct CHerClass : public Interface {
        int value_ = 22;
        CHerClass() {}
        virtual void Foo() override { printf("CHerClass::Foo %d\n", value_); }
        virtual void Bar() override { printf("CHerClass::Bar %d\n", value_); }
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

