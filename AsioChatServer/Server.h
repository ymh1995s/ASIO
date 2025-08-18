#pragma once 

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <string>
#include <vector>


#include "Session.h"

class Server
{
public:
	Server(boost::asio::io_context& io_context);
	~Server();

	void Start();
	void CreateSessionManager();

private:
	// acceptor를 서버가 갖고 있게 하고 싶기 때문에 여기서 처리하는게 맞다.
	bool PostAccept();
	void handle_accept(const boost::system::error_code& error);
	
public:
	boost::asio::io_context& m_ioContext; // 비동기 입출력을 위해 acceptor와 session에게 전달 
	boost::asio::ip::tcp::acceptor m_acceptor; // 리스너 클래스 역할 (IOCP 서버의 Listener.h/cpp)
	class SessionManager* sessionManager; // TODO 스마트포인터로 변경
};