//David Oguns
//February 2, 2011

#ifndef _COMPONENTTEST_H_
#define _COMPONENTTEST_H_


class ComponentTest
{
public:
	virtual bool Test() = 0;
	virtual const char * GetName() = 0;
	virtual const char * GetDescription() = 0;

};

#endif
