#include "Server.h"
#include "SessionManager.h"

Server::Server(boost::asio::io_context& io_context)
	: ioContext(io_context),
	acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 7777))
{
}

Server::~Server()
{
}

void Server::Init()
{
	std::cout << "서버 시작....." << std::endl;
	CreateSessionManager();
	PostAccept();
}

void Server::CreateSessionManager()
{
	// shared_from_this()는 이미 관리되고 있는 객체, 즉 생성자 안에서는 호출할 수 없다.
	sessionManager = make_shared<SessionManager>(ioContext, shared_from_this());
}

bool Server::PostAccept()
{
	// ASIO는 수립됐을 때 할당 받을 세션이 미리 준비돼 있어야만 한다.
	shared_ptr<Session> pSession = sessionManager->CreateSession();

	// async_accept(클라에 할당할 소켓 클래스, 접속이 완료됐을 때 콜백 함수)
	acceptor.async_accept(pSession->Socket(),
		// bind(bind할 함수, bind할 함수의 주인, bind할 함수의 파라미터 1, 파라미터2...)
		boost::bind(&Server::handle_accept,
			this,
			pSession,
			boost::asio::placeholders::error)
	);

	return true;
}

void Server::handle_accept(shared_ptr<Session> pSession, const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout << "클라이언트 접속 성공. SessionID: "<< pSession->GetSessionID()<<endl;

		sessionManager->RegisterSession(pSession);
		pSession->Init();

		PostAccept();
	}
	else
	{
		std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
	}
}