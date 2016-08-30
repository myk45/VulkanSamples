#ifndef __ASSIGNMENT1__
#define __ASSIGNMENT1__

#include "../assignmentbase.h"

class Assignment1 : public AssignmentBase
{
public:
    Assignment1()   { _type = ASSIGNMENT_1; }
    ~Assignment1()  {}

    virtual bool doAssignment();
};

#endif