#pragma once
#include <queue>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

using namespace std;

class Server;

class Session
{
public:
	Session(boost::asio::io_context& io_context, shared_ptr<Server> pServer, int sessionID);
	~Session();

	void Init();

	void PostReceive();
	void PostSend(char* buffer, int nSize);

	// Heleper
	int GetSessionID() { return m_sessionID; };
	boost::asio::ip::tcp::socket& Socket() { return m_Socket; }

private:
	void HandleReceive(const boost::system::error_code& error, int bytes_transferred);
	void HandleSend(const boost::system::error_code& error, int bytes_transferred);

	void Send();
	void Broadcase();

private:
	boost::asio::ip::tcp::socket m_Socket;
	shared_ptr<Server> m_pServer;
	int m_sessionID;

public:
	char m_recvBuffer[1024];
	queue<pair<char*, int>> m_sendQueue; // TODO 스마트포인터로 관리 
	atomic<bool> m_isSending = false;
		
private:
	std::mutex m_lock;
	bool m_bClosed = false;
};