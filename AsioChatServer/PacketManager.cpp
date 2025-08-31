#include "PacketManager.h"

PacketManager::PacketManager()
{
    recvBuffer = make_shared<RecvBuffer>();
}
