#include "Session.h"
#include "Server.h"
#include "SessionManager.h"
#include "PacketManager.h"
#include "SendBuffer.h"

Session::Session(boost::asio::io_context& io_context, shared_ptr<Server> pServer, int sessionID)
	: socket(io_context)
	, server(pServer)
	, sessionID(sessionID)
{
}

Session::~Session()
{
	cout << sessionID << "번 클라이언트는 안전하게 해제 되었습니다.(스마트 포인터 정상 작동)" << endl;
}

void Session::Init()
{
	packetManager = make_shared<PacketManager>();
	PostReceive();
}

void Session::PostReceive()
{
	// async_read_some() : 수신한 만큼 처리 (TCP 특성상 잘려서 올 수 있음)
	// async_read() : 내부적으로 async_read_some()을 반복 호출해서 지정된 버퍼 크기를 다 채울 때까지 대기
		// 고정 길이 패킷에 유용하지만, 부분 데이터도 빨리 처리해야 하는 경우엔 비효율적일 수 있음
		// 단, 헤더+바디로 이미 길이가 정해져 있으면 사용할 순 있겠지
	// IOCP의 Async_Receive 등록과 같은 기능
	socket.async_read_some
	(
		boost::asio::buffer(packetManager->GetWritePtr(), packetManager->GetWritableSize()),
		boost::bind(&Session::HandleReceive, 
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void Session::PostSend(const char* data, int size)
{
	auto sendBuffer = make_shared<SendBuffer>(size);
	sendBuffer->CopyData(data, size);

	{
		std::lock_guard<mutex> _lock(lock);

		sendQueue.push(sendBuffer);

		// 이미 보내는 중이면, 큐에 넣기만 한다.
		// TODO : 큐에 넣는 곳에 병목이 생기진 않을까?
		if (isSending)
			return;

		isSending = true;
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
		std::lock_guard<mutex> _lock(lock);

		if (sendQueue.empty())
		{
			isSending = false;
			return;
		}

		while (!sendQueue.empty())
		{
			shared_ptr<SendBuffer>& front = sendQueue.front();
			buffers.push_back(boost::asio::buffer(front->GetBuffer(), front->GetWriteSize()));
			sendQueue.pop();
		}
	}

	boost::asio::async_write(socket, buffers,
		boost::bind(&Session::HandleSend, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
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

		if (auto s = server.lock())
		{
			s->sessionManager->CloseSession(sessionID);
		}
		else
		{
			cout << " Weak Ptr이 null 반환 [서버 소멸] \n";
		}
	}
}


void Session::HandleReceive(const boost::system::error_code& error, int bytes_transferred)
{
	if (isClosed) return; // 이미 종료된 세션이면 무시
	if (!error)
	{
		cout << " 데이터 수신 \n";

		packetManager->HasWritten(bytes_transferred);

		// 2. 패킷 조립 시도
		int packetCount = packetManager->ProcessPackets(
			[this](const char* data, int size) 
			{
				if (auto s = server.lock())
				{
					s->sessionManager->Broadcast(data, size);
				}
			}
		);

		//if (auto s = server.lock())
		//{
		//	s->sessionManager->Broadcast(packetManager->GetRecvBuffer(), bytes_transferred);
		//}
		//else
		//{
		//	cout << " Weak Ptr이 null 반환 [서버 소멸] \n";
		//}

		PostReceive();
	}
	else
	{
		isClosed = true;  // 한 번만 처리
		if (error == boost::asio::error::eof)
		{
			std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;
		}
		else
		{
			std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
		}

		if (auto s = server.lock())
		{
			s->sessionManager->CloseSession(sessionID);
		}
		else
		{
			cout << " Weak Ptr이 null 반환 [서버 소멸] \n";
		}
	}
}