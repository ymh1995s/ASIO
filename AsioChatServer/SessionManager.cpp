#include "SessionManager.h"

SessionManager::SessionManager(boost::asio::io_context& io_context, shared_ptr<Server> pServer)
	: m_ioContext(io_context), serverSession(pServer)
{
}

SessionManager::~SessionManager()
{

}

shared_ptr<Session> SessionManager::CreateSession()
{
	int thisSessionNo = sessionNo.fetch_add(1);
	shared_ptr<Session> pSession = make_shared<Session>(m_ioContext, serverSession, thisSessionNo);
	sessionList[thisSessionNo] = pSession;
	return pSession;
}

bool SessionManager::CloseSession(int no)
{
	sessionList[no] = nullptr;
	return true;
}

shared_ptr<Session> SessionManager::GetSession(int no)
{
	shared_ptr<Session> pSession = sessionList[no];
	return pSession;
}