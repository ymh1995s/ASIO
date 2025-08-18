#include "Server.h"
#include "SessionManager.h"

Server::Server(boost::asio::io_context& io_context)
	: m_ioContext(io_context),
	m_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 7777))
{
	Start(); // Start PostAccept
}

Server::~Server()
{

}

void Server::Start()
{
	std::cout << "서버 시작....." << std::endl;
	CreateSessionManager();
	PostAccept();
}

void Server::CreateSessionManager()
{
	sessionManager = new SessionManager(m_ioContext, this);
}

bool Server::PostAccept()
{
	// async_accept(io_context, 접속이 완료됐을 때 콜백 함수)
	m_acceptor.async_accept(m_ioContext,
		// bind(bind할 함수, bind할 함수의 주인, bind할 함수의 파라미터 1, 파라미터2...)
		boost::bind(&Server::handle_accept,
			this,
			boost::asio::placeholders::error)
	);

	return true;
}

void Server::handle_accept(const boost::system::error_code& error)
{
	if (!error)
	{
		Session* pSession = sessionManager->CreateSession();

		std::cout << "클라이언트 접속 성공. SessionID: "<< pSession->GetSessionID()<<endl;

		pSession->Init();
		pSession->PostReceive();

		PostAccept();
	}
	else
	{
		std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
	}
}