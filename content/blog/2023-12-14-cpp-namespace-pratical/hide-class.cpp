namespace mymod
{

struct CMyClass
{
    CMyClass(/* init-args */);
    void Foo() { /* todo */ }
    void Bar() { /* todo */ }

    // data member ...
};

CMyClass* GetInstance()
{
    static CMyClass instance(/* defalut-args */);
    return &instance;
}

CMyClass* NewObject(/* init-args */)
{
    CMyClass* pObject = new CMyclass(/* init-args */);
    return pObject;
}

void FreeObject(CMyClass* pObject)
{
    delete pObject;
}

void Foo(CMyClass* pObject)
{
    pObject->Foo();
}

void Bar(CMyClass* pObject)
{
    pObject->Bar();
}

void Work(/* arguments */)
{
    CMyClass obj(/* arguments */);
    obj.Foo();
    obj.Bar();
}
}
