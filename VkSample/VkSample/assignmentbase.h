#ifndef __ASSIGNBASE__
#define __ASSIGNBASE__

#include "demobase.h"

class AssignmentBase : private DemoBase
{
public:
    AssignmentBase()          {}
    virtual ~AssignmentBase() {}

    virtual bool init();
    virtual bool run();
    virtual bool cleanUp();

    virtual bool doAssigment() { assert(0); }

    enum AssignmentType
    {
        ASSIGNMENT_1 = 0,
        NUM_ASSIGNMENT
    };

protected:
    AssignmentType _type;
};

#endif