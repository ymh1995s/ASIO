#pragma once
#include <deque>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

using namespace std;

class Server;

class Session
{
public:
	Session(boost::asio::io_context& io_context, Server* pServer, int sessionID);
	~Session();

	void Init();
	void PostReceive();
	int GetSessionID() { return m_sessionID; };

	boost::asio::ip::tcp::socket& Socket() { return m_Socket; }

private:
	void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);

private:
	boost::asio::ip::tcp::socket m_Socket;
	Server* m_pServer;
	int m_sessionID;

public:
	int m_recvBuffer[1024];
};