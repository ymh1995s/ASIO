#include "Session.h"
#include "Server.h"
#include "SessionManager.h"

Session::Session(boost::asio::io_context& io_context, Server* pServer, int sessionID)
	: m_Socket(io_context)
	, m_pServer(pServer)
	, m_sessionID(sessionID)
{
	Init();
}

Session::~Session()
{

}

void Session::Init()
{
	PostReceive();
}

void Session::PostReceive()
{
	// async_read_some() : 수신한 만큼 처리 (TCP 특성상 잘려서 올 수 있음)
	// async_read() : 내부적으로 async_read_some()을 반복 호출해서 지정된 버퍼 크기를 다 채울 때까지 대기
		// 고정 길이 패킷에 유용하지만, 부분 데이터도 빨리 처리해야 하는 경우엔 비효율적일 수 있음
		// 단, 헤더+바디로 이미 길이가 정해져 있으면 사용할 순 있겠지
	// IOCP의 Async_Receive 등록과 같은 기능
	m_Socket.async_read_some
	(
		boost::asio::buffer(m_recvBuffer),
		boost::bind(&Session::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void Session::handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		if (error == boost::asio::error::eof)
		{
			std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;
		}
		else
		{
			std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
		}

		m_pServer->sessionManager->CloseSession(m_sessionID);
	}
	else
	{
		cout << " 무언가 수신했습니다 \n";

		PostReceive();
	}
}
