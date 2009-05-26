/*
 *   A SystemC breakpoint class
 *
 *   Not sure what it does
 *
*/

#ifndef BREAKPOINT_HPP
#define BREAKPOINT_HPP

class Condition {

    virtual void init() = 0;
    virtual bool check() = 0;
    virtual void action() = 0;

}

template<typename TYPE> class EqualityCondition : Condition {

    TYPE &var1;
    TYPE &var2;

    EqualityCondition( TYPE &var1, TYPE &var2 )
}


#endif /* BREAKPOINT_HPP */