namespace mymod
{

struct CMyClass;
CMyClass* GetInstance();
CMyClass* NewObject(/* init-args */);
void FreeObject(CMyClass* pObject);
void Foo(CMyClass* pObject);
void Bar(CMyClass* pObject);
void Work(/* arguments */);

}

