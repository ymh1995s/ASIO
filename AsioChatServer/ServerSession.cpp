﻿#include "ServerSession.h"
#include "ChatServer.h"

Session::Session(int nSessionID, boost::asio::io_context& io_context, ChatServer* pServer)
	: m_Socket(io_context)
	, m_nSessionID(nSessionID)
	, m_pServer(pServer)
{
}

Session::~Session()
{
	while (m_SendDataQueue.empty() == false)
	{
		delete[] m_SendDataQueue.front();
		m_SendDataQueue.pop_front();
	}
}

void Session::Init()
{
	m_nPacketBufferMark = 0;
}

void Session::PostReceive()
{
	// async_read_some() : 수신한 만큼 처리 (TCP 특성상 잘려서 올 수 있음)
	// async_read() : 내부적으로 async_read_some()을 반복 호출해서 지정된 버퍼 크기를 다 채울 때까지 대기
		// 고정 길이 패킷에 유용하지만, 부분 데이터도 빨리 처리해야 하는 경우엔 비효율적일 수 있음
		// 단, 헤더+바디로 이미 길이가 정해져 있으면 사용할 순 있겠지
	m_Socket.async_read_some
	(
		boost::asio::buffer(m_ReceiveBuffer),
		boost::bind(&Session::handle_receive, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)
	);
}

void Session::PostSend(const bool bImmediately, const int nSize, char* pData)
{
	char* pSendData = nullptr;

	if (bImmediately == false)
	{
		pSendData = new char[nSize];
		memcpy(pSendData, pData, nSize);

		m_SendDataQueue.push_back(pSendData);
	}
	else
	{
		pSendData = pData;
	}

	if (bImmediately == false && m_SendDataQueue.size() > 1)
	{
		return;
	}

	boost::asio::async_write(m_Socket, boost::asio::buffer(pSendData, nSize),
		boost::bind(&Session::handle_write, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void Session::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
	delete[] m_SendDataQueue.front();
	m_SendDataQueue.pop_front();

	if (m_SendDataQueue.empty() == false)
	{
		char* pData = m_SendDataQueue.front();

		PACKET_HEADER* pHeader = (PACKET_HEADER*)pData;

		PostSend(true, pHeader->nSize, pData);
	}
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

		m_pServer->CloseSession(m_nSessionID);
	}
	else
	{
		memcpy(&m_PacketBuffer[m_nPacketBufferMark], m_ReceiveBuffer.data(), bytes_transferred);

		int nPacketData = m_nPacketBufferMark + bytes_transferred;
		int nReadData = 0;

		while (nPacketData > 0)
		{
			if (nPacketData < sizeof(PACKET_HEADER))
			{
				break;
			}

			PACKET_HEADER* pHeader = (PACKET_HEADER*)&m_PacketBuffer[nReadData];

			if (pHeader->nSize <= nPacketData)
			{
				m_pServer->ProcessPacket(m_nSessionID, &m_PacketBuffer[nReadData]);

				nPacketData -= pHeader->nSize;
				nReadData += pHeader->nSize;
			}
			else
			{
				break;
			}
		}

		if (nPacketData > 0)
		{
			char TempBuffer[MAX_RECEIVE_BUFFER_LEN] = { 0, };
			memcpy(&TempBuffer[0], &m_PacketBuffer[nReadData], nPacketData);
			memcpy(&m_PacketBuffer[0], &TempBuffer[0], nPacketData);
		}

		m_nPacketBufferMark = nPacketData;

		PostReceive();
	}
}
