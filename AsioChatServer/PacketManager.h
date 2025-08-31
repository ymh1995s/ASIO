#pragma once
#include "Session.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"

class PacketManager
{
public:
	PacketManager();

	char* GetRecvBuffer() { return recvBuffer->GetBuffer(); };
	int GetRecvBufferSize() { return recvBuffer->GetBufferSize(); }; // TODO : 하드코딩 삭제

private:
	shared_ptr<RecvBuffer> recvBuffer;
};

