//David Oguns
//February 27, 2011

#include "DirectXHelperTest.h"

#include <accevo\DirectX11DataHelper.h>
#include <boost\property_tree\xml_parser.hpp>
#include <iostream>

using namespace std;
using namespace Accevo;
using namespace boost::property_tree;

bool DirectXHelperTest::Test()
{
	DirectX11DataHelper::Initialize();

	bool successFlag = true;

	//test input element desc layouts
	ptree layoutpt;
	read_xml("data\\cfg\\layouts.xml", layoutpt, 
		boost::property_tree::xml_parser::trim_whitespace |
		boost::property_tree::xml_parser::no_comments);
	ptree &layoutPt = layoutpt.get_child("inputLayouts");
	std::list<ptree const *> layouts;
	SelectPTreeChildren("layout", layoutPt, layouts);
	std::for_each(layouts.begin(), layouts.end(),
		[&] (ptree const *layout)
		{
			boost::optional<unsigned int> numElements = layout->get<unsigned int>("<xmlattr>.elements");
			if(numElements)
			{
				D3D11_INPUT_ELEMENT_DESC *vLayout = new D3D11_INPUT_ELEMENT_DESC[*numElements];
				if(DirectX11DataHelper::GetInputLayoutDesc(*layout, vLayout, *numElements))
				{
					cout << layout->get<std::string>("<xmlattr>.name") << " loaded successfully" << endl;
				}
				else
				{
					cout << "Couldn't fully get layout elements" << endl;
					successFlag = false;
				}
				delete [] vLayout;
			}
			else
			{
				cout << "Couldn't find number of elements in layout." << endl;
				successFlag = false;
			}
		}
	);

	//test shader resource view loading
	ptree srvpt;
	read_xml("data\\cfg\\resource-views.xml", srvpt,
		boost::property_tree::xml_parser::trim_whitespace |
		boost::property_tree::xml_parser::no_comments);
	ptree &srvsPt = srvpt.get_child("resourceviews");
	std::list<ptree const *> srvList;
	SelectPTreeChildren("shaderResourceView", srvsPt, srvList);
	std::for_each(srvList.begin(), srvList.end(),
		[&] (ptree const *srv)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			if(DirectX11DataHelper::GetShaderResourceViewDesc(*srv, srvDesc))
			{
				cout << srv->get<std::string>("<xmlattr>.name") << " loaded successfully" << endl;
			}
			else
			{				
				cout << "Failed to load " << srv->get<std::string>("<xmlattr>.name") << endl;
				successFlag = false;
			}
		}
	);

	return successFlag;
}