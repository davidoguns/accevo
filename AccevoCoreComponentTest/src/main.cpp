//David Oguns
//February 2, 2011


#include <iostream>
#include <fstream>
#include <accevo\XmlStreamLogger.h>
#include <accevo\DebugOutputLogger.h>
#include <accevo\HandleManager.h>
#include <accevo\StringIdentifier.h>
#include <accevo\StringMap.h>
#include <boost\format.hpp>
#include <accevo/DirectX11DataHelper.h>

#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>

#include "DirectXHelperTest.h"

using std::cout;
using std::endl;
using std::wofstream;
using boost::format;

using namespace Accevo;
using boost::property_tree::read_xml;
using boost::property_tree::ptree;

int main(int argc, char **argv)
{
	//get current flags for debug reporting
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	//turn flag on for debug reporting memory leaks before app exist
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	tmpDbgFlag |= _CRTDBG_ALLOC_MEM_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
	//_crtBreakAlloc = 6904;

	Clock::InitClock();
	Clock clock;
	//DebugOutputLogger logger("AccevoCoreComponentTest", clock, Logger::AELOGLEVEL_TRACE);
	XmlStreamLogger logger("log.xml", "AccevoCoreComponentTest", clock, Logger::AELOGLEVEL_TRACE);
	
	Accevo::StringMap<int> intStrings;
	
	intStrings.Insert(SID("a1"), 42);
	intStrings.Insert(SID("a2"), 39);
	
	cout << "Sizeof handle: " << sizeof(Handle) << endl;
	cout << "MAX INDEX: " << Handle::AEHANDLE_MAX_INDEX << " MAX MAGIC: " << Handle::AEHANDLE_MAX_MAGIC << endl;

	cout << (format("A1: %1%")%intStrings.Get(SID("a1"))).str() << endl;
	cout << (format("A2: %1%")%(*intStrings.GetOptional(SID("a2")))).str() << endl;

	boost::optional<int> a3 = intStrings.GetOptional(SID("a3"));
	if(a3)
	{
		cout << (format("A3: %1%")%(*a3)).str().c_str() << endl;
	}

	ptree propTree;
	read_xml("C:\\Users\\EbonySeraph\\Documents\\Programs\\accevo\\ModelViewer\\log.xml", propTree, 
		boost::property_tree::xml_parser::trim_whitespace |
		boost::property_tree::xml_parser::no_comments);

	ptree & logmsg = propTree.get_child("Log");
	std::list<ptree const *> children; 
	SelectPTreeChildren("LogMessage", logmsg, children);
	
	std::for_each(children.begin(), children.end(),
		[] (ptree const *pt)
		{
			cout << "Child: " << pt->get_value<std::string>() << endl;
		}
	);

	std::string message = propTree.get<std::string>("Log.<xmlattr>.name");

	std::cout << "Message: " << message << endl;
	char str[256] = "TEST|TOK| HERE | what.. |";
	std::list<char *> tokens;
	Accevo::GetStringTokenList(str, "| \r\n\t", tokens);
	std::for_each(tokens.begin(), tokens.end(),
		[] (char *token)
		{
			std::cout << "token: " << token << endl;
		}
	);

	DirectXHelperTest dxTest;
	if(dxTest.Test())
	{
		cout << "Test succeeded!" << endl;
	}
	else
	{
		cout << "Test failed..." << endl;
	}

	std::cin.ignore(1000, '\n');
	return 0;
}