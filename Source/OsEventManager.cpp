#include <assert.h>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <exception>
#include <limits.h>
#include "xml/Writer.h"
#include "xml/Parser.h"
#include "StdStream.h"
#include "OsEventManager.h"

using namespace Framework;
using namespace boost;
using namespace std;

COsEventManager::COsEventManager()
{
	m_nCurrentTime		= 0;
	m_sRecordPath		= "";
	m_Events.reserve(RESERVE);
}

COsEventManager::~COsEventManager()
{

}

void COsEventManager::Begin(const char* sExecutableName)
{
	for(unsigned int i = 0; i < UINT_MAX; i++)
	{
		string sRecordPath;
		sRecordPath += string("./") + sExecutableName + "_" + lexical_cast<string>(i) + ".xml";
		
		if(!filesystem::exists(filesystem::path(sRecordPath, filesystem::native)))
		{
			m_sRecordPath = sRecordPath;
			return;
		}
	}

	throw exception();
}

void COsEventManager::InsertEvent(COsEvent Event)
{
	Event.nTime = m_nCurrentTime++;

	m_Events.push_back(Event);

	if(m_Events.size() >= RESERVE)
	{
		Flush();
	}
}

void COsEventManager::Flush()
{
	if(m_Events.size() == 0) return;
	if(m_sRecordPath.length() == 0) return;

	Xml::CNode* pRootNode;
	Xml::CNode* pEventsNode;

	try
	{
		pRootNode = Xml::CParser::ParseDocument(&CStdStream(fopen(m_sRecordPath.c_str(), "rb")));
		pEventsNode = pRootNode->Search("Events");
		if(pEventsNode == NULL) throw exception();
	}
	catch(...)
	{
		pRootNode	= new Xml::CNode();
		pEventsNode	= new Xml::CNode("Events", true);
		pRootNode->InsertNode(pEventsNode);
	}

	for(EventListType::const_iterator itEvent(m_Events.begin());
		itEvent != m_Events.end(); itEvent++)
	{
		Xml::CNode* pEventNode;
		
		pEventNode = new Xml::CNode("Event", true);
		pEventNode->InsertAttribute(Xml::AttributeType("Address",		lexical_cast<string>((*itEvent).nAddress)));
		pEventNode->InsertAttribute(Xml::AttributeType("ThreadId",		lexical_cast<string>((*itEvent).nThreadId)));
		pEventNode->InsertAttribute(Xml::AttributeType("EventType",		lexical_cast<string>((*itEvent).nEventType)));
		pEventNode->InsertAttribute(Xml::AttributeType("Time",			lexical_cast<string>((*itEvent).nTime)));
		pEventNode->InsertAttribute(Xml::AttributeType("Description",	(*itEvent).sDescription));

		pEventsNode->InsertNode(pEventNode);
	}

	Xml::CWriter::WriteDocument(&CStdStream(fopen(m_sRecordPath.c_str(), "wb")), pRootNode);

	m_Events.clear();

	delete pRootNode;
}

Xml::CNode* COsEventManager::GetEvents()
{
	if(m_sRecordPath.length() == 0) return NULL;

	Flush();

	try
	{
		return Xml::CParser::ParseDocument(&CStdStream(fopen(m_sRecordPath.c_str(), "rb")));
	}
	catch(...)
	{
		return NULL;
	}
}
