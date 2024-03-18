namespace dll
{
    void Foo();
    void Bar();
} // end of namespace

#ifdef __cplusplus
extern "C"
{
#endif
    void mydll_foo();
    void mydll_bar();
#ifdef __cplusplus
}
#endif
