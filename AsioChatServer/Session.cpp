#include "Session.h"
#include "Server.h"
#include "SessionManager.h"

Session::Session(boost::asio::io_context& io_context, shared_ptr<Server> pServer, int sessionID)
	: m_Socket(io_context)
	, m_pServer(pServer)
	, m_sessionID(sessionID)
{
}

Session::~Session()
{
	cout << m_sessionID << "번 클라이언트는 안전하게 해제 되었습니다.(스마트 포인터 정상 작동)" << endl;
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
		boost::bind(&Session::HandleReceive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void Session::PostSend(char* buffer, int nSize)
{
	{
		std::lock_guard<mutex> lock(m_lock);

		m_sendQueue.push(make_pair(buffer, nSize));

		// 이미 보내는 중이면, 큐에 넣기만 한다.
		// TODO : 큐에 넣는 곳에 병목이 생기진 않을까?
		if (m_isSending)
			return;

		m_isSending = true;
	}
	

	Send();
}

void Session::Send()
{
	// conster buffer
		// ASIO에서 제공하는 읽기용 버퍼
		// 내부적으로 데이터 주소와 길이 정보를 가지고 있음
		// 이 자료구조를 사용하여 scatter gather 사용할 예정
	vector<boost::asio::const_buffer> buffers;

	// scateer - gather : 큐에있는 모든 데이터를 모아서 한 번에 보냄
	{
		std::lock_guard<mutex> lock(m_lock);

		if (m_sendQueue.empty())
		{
			m_isSending = false;
			return;
		}

		while (!m_sendQueue.empty())
		{
			auto& front = m_sendQueue.front();
			buffers.push_back(boost::asio::buffer(front.first, front.second));
			m_sendQueue.pop();
		}
	}

	boost::asio::async_write(m_Socket, buffers,
		boost::bind(&Session::HandleSend, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void Session::Broadcase()
{
	for (auto s : m_pServer->sessionManager->sessionList)
	{

	}
}

void Session::HandleSend(const boost::system::error_code& error, int bytes_transferred)
{
	if (!error)
	{
		Send();
	}
	else
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
}


void Session::HandleReceive(const boost::system::error_code& error, int bytes_transferred)
{
	if (m_bClosed) return; // 이미 종료된 세션이면 무시
	if (!error)
	{
		// TODO : 현재는 m_recvBuffer가 있으나 Buffer클래스에서 조립해서 사용하게 할거임
		cout << " 무언가 수신했습니다 \n";

		// TODO 조립
		PostSend(m_recvBuffer, bytes_transferred);

		PostReceive();
	}
	else
	{
		m_bClosed = true;  // 한 번만 처리
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
}