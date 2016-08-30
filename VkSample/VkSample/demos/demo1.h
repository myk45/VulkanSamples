#ifndef __DEMO1__
#define __DEMO1__

#include "../demobase.h"

class Demo1 : public DemoBase
{
public:
    Demo1()  {}
    ~Demo1() {}

    virtual bool run();
    virtual const char* getInfo() const;

protected:
};

#endif