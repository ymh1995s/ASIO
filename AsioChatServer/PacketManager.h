#pragma once
#include "Session.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include "Protocol.h"

class PacketManager
{
public:
    PacketManager() : recvBuffer(std::make_shared<RecvBuffer>(1024)) {}

    char* GetWritePtr() { return recvBuffer->ReadPos() + recvBuffer->DataSize(); }
    int GetWritableSize() { return recvBuffer->FreeSize(); }

    void HasWritten(int size) { recvBuffer->OnWrite(size); }

    // 조립된 패킷을 콜백으로 전달
    int ProcessPackets(std::function<void(const char*, int)> onPacket)
    {
        int processed = 0;

        while (true)
        {
            // 1. 최소한 헤더만큼 도착했는지 확인
            if (recvBuffer->DataSize() < sizeof(PacketHeader))
                break;

            // 2. 헤더 읽기
            PacketHeader header;
            memcpy(&header, recvBuffer->ReadPos(), sizeof(PacketHeader));

            // 3. 전체 패킷 도착 여부 확인
            if (recvBuffer->DataSize() < header.size)
                break;

            // 4. 패킷 완성 → 콜백 전달
            onPacket(recvBuffer->ReadPos(), header.size);

            // 5. 읽기 커서 이동
            recvBuffer->OnRead(header.size);

            processed++;
        }

        // 6. 버퍼 정리 (앞으로 땡기기)
        recvBuffer->Clean();
        return processed;
    }


private:
	shared_ptr<RecvBuffer> recvBuffer;
};

