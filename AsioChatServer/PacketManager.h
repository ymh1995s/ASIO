#pragma once
#include "Session.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"

class PacketManager
{
public:
	PacketManager();

	char* GetRecvBuffer() { return recvBuffer->GetBuffer(); };
	char* GetSendBuffer() { return sendBuffer->GetBuffer(); };
	int GetRecvBufferSize() { return 1024; }; // TODO : 하드코딩 삭제
	int GetSendBufferSize() { return 1024; }; // TODO : 하드코딩 삭제

private:
	shared_ptr<RecvBuffer> recvBuffer;
	shared_ptr<SendBuffer> sendBuffer;
};

