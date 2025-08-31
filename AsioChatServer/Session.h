#pragma once
#include <queue>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

using namespace std;

class Server;

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::io_context& io_context, shared_ptr<Server> pServer, int sessionID);
	~Session();

	void Init();

	void PostReceive();
	void PostSend(char* buffer, int size);

	// Heleper
	int GetSessionID() { return sessionID; };
	boost::asio::ip::tcp::socket& Socket() { return socket; }

private:
	void HandleReceive(const boost::system::error_code& error, int bytes_transferred);
	void HandleSend(const boost::system::error_code& error, int bytes_transferred);

public:
	void Send();

private:
	boost::asio::ip::tcp::socket socket;
	weak_ptr<Server> server;
	shared_ptr<class PacketManager> packetManager;

	int sessionID;

public:
	//char m_recvBuffer[1024];
	std::queue<shared_ptr<class SendBuffer>> sendQueue;
	atomic<bool> isSending = false;
		
private:
	std::mutex lock;
	bool isClosed = false;
};