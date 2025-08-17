#pragma once 

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <string>
#include <vector>


#include "ServerSession.h"
#include "Protocol.h"

class ChatServer
{
public:
	ChatServer(boost::asio::io_context& io_context, int maxSessionCount);
	~ChatServer();

	void Init(const int nMaxSessionCount);
	void Start();
	void CloseSession(const int nSessionID);
	void ProcessPacket(const int nSessionID, const char* pData);

private:
	bool PostAccept();
	void handle_accept(Session* pSession, const boost::system::error_code& error);
	
public:
	boost::asio::io_context& m_ioContext; // 비동기 입출력을 위해 acceptor와 session에게 전달 
	boost::asio::ip::tcp::acceptor m_acceptor; // 리스너 클래스 역할 (IOCP 서버의 Listener.h/cpp)
	bool m_bIsAccepting;
	std::vector< Session* > m_SessionList;
	std::deque< int > m_SessionQueue;
};