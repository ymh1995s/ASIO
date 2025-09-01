#pragma once

using namespace std;
class Session;

class SessionManager 
{
public:
	SessionManager(boost::asio::io_context& io_context, shared_ptr<Server> pServer);
	~SessionManager();

	shared_ptr<Session> CreateSession();
	void RegisterSession(shared_ptr<Session> pSession); // 실제 관리 대상으로 등록 (sessionList)
	bool CloseSession(int no);
	shared_ptr<Session> GetSession(int no);

public:
	// 세션 정보를 매니저가 들고 있으니까 여기서 브로드캐스트 하는게 맞겠지?
	void Broadcast(const char* buffer, int nSize);

private:
	boost::asio::io_context& ioContext;
	shared_ptr<Server> serverSession; // TODO : To WeakPtr?
	atomic<int> sessionNo; // 이번 세션에게 부여할 번호

public:
	// TODO :  RegisterSession, CloseSession, Broadcast 등에서의 스레드 세이프 필요
	map<int, shared_ptr<Session>> sessionList;
};