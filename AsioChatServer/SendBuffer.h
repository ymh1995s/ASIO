#pragma once
#include "vector"
#include <iostream>
using namespace std;

class SendBuffer
	// TODO TLS나 메모리풀링 같은 최적화 기법 적용
{
public:
	SendBuffer() {};
	SendBuffer(int size);
	~SendBuffer() { cout << "SendBuffer가 안전하게 해제되었습니다 [스마트 포인터] \n"; };


	void CopyData(const void* data, int len);

	char* GetBuffer() { return buffer.data(); }
	int GetWriteSize() { return writeSize; }
	int GetBufferSize() { return buffer.size(); }

private:
    vector<char> buffer;
    int writeSize;
};