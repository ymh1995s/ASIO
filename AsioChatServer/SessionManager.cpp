#include "SessionManager.h"

SessionManager::SessionManager(boost::asio::io_context& io_context, Server* pServer)
	: m_ioContext(io_context), serverSession(pServer)
{
}

SessionManager::~SessionManager()
{
	for (auto i : clientList)
	{
		delete i.second;
	}
}

Session* SessionManager::CreateSession()
{
	int thisSessionNo = sessionNo.fetch_add(1);
	Session* pSession = new Session(m_ioContext, serverSession, thisSessionNo);
	clientList[thisSessionNo] = pSession;
	return pSession;
}

bool SessionManager::CloseSession(int no)
{
	if (clientList[no] == nullptr) return true;

	delete clientList[no];
	clientList[no] = nullptr;
	return true;
}

Session* SessionManager::GetSession(int no)
{
	Session* pSession = clientList[no];
	return pSession;
}