//David Oguns
//February 27, 2011

#ifndef _DXHELPERTEST_H_
#define _DXHELPERTEST_H_

#include "ComponentTest.h"

class DirectXHelperTest : public ComponentTest
{
public:
	virtual bool Test();
	virtual const char * GetName() { return "DirectXHelperTest"; }
	virtual const char * GetDescription() { return "Tests the loading xml data from a boost::property_tree and using DirectXDataHelper static methods to create DirectX11 description structures."; }

};

#endif