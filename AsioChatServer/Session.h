#pragma once
class Server;

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::io_context& io_context, std::shared_ptr<Server> pServer, int sessionID);
	~Session();

	void Init();

	void PostReceive();
	void PostSend(const char* buffer, int size);

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
	std::weak_ptr<Server> server;
	std::shared_ptr<class PacketManager> packetManager;

	int sessionID;

public:
	//char m_recvBuffer[1024];
	std::queue<std::shared_ptr<class SendBuffer>> sendQueue;
	std::atomic<bool> isSending = false;
		
private:
	std::mutex lock;
	bool isClosed = false;
};