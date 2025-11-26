namespace mymod {
    struct Interface {
        virtual ~Interface() {}
        virtual void Foo() = 0;
        virtual void Bar() = 0;
    };
    Interface* CreateObject(int arg);
    void FreeObject(Interface* pObject);
} // end of namespace

