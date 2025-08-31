#pragma once
#include "vector"
using namespace std;

class SendBuffer
	// TODO TLS나 메모리풀링 같은 최적화 기법 적용
{
public:
	SendBuffer() {};
	SendBuffer(int size);

	void CopyData(const void* data, size_t len);

	char* GetBuffer() { return buffer.data(); }
	int GetWriteSize() { return writeSize; }
	int GetCapacity() { return buffer.size(); }

private:
    vector<char> buffer;
    int writeSize;
};