#include "PacketManager.h"

PacketManager::PacketManager()
{
    recvBuffer = make_shared<RecvBuffer>();
    sendBuffer = make_shared<SendBuffer>();
}
