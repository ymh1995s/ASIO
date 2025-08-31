#include "SessionManager.h"

SessionManager::SessionManager(boost::asio::io_context& io_context, shared_ptr<Server> pServer)
	: ioContext(io_context), serverSession(pServer)
{
}

SessionManager::~SessionManager()
{

}

shared_ptr<Session> SessionManager::CreateSession()
{
	int thisSessionNo = sessionNo.fetch_add(1);
	shared_ptr<Session> pSession = make_shared<Session>(ioContext, serverSession, thisSessionNo);
	// 여기서 바로 등록하니까 브로드캐스트에서 문제 발생
	//sessionList[thisSessionNo] = pSession;
	return pSession;
}

void SessionManager::RegisterSession(shared_ptr<Session> pSession)
{
	sessionList[pSession->GetSessionID()] = pSession;
}

bool SessionManager::CloseSession(int no)
{
	sessionList.erase(no);
	return true;
}

shared_ptr<Session> SessionManager::GetSession(int no)
{
	shared_ptr<Session> pSession = sessionList[no];
	return pSession;
}

void SessionManager::Broadcast(const char* buffer, int nSize)
{
	for (auto& s : sessionList)
	{
		s.second->PostSend(buffer, nSize);
	}
}
