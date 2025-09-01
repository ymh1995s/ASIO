#include "pch.h"

#include "SendBuffer.h"
SendBuffer::SendBuffer(int size = 1024)
    : buffer(size), writeSize(0)
{
}

void SendBuffer::CopyData(const void* data, int len)
{
    std::memcpy(buffer.data(), data, len);
    writeSize = len;
}