#pragma once
#include "vector"
#include <iostream>
using namespace std;

class RecvBuffer
{
public:
	RecvBuffer();
	RecvBuffer(int size);
	~RecvBuffer() { cout << "RecvBuffer 안전하게 해제되었습니다\n"; }

	char* GetBuffer() { return buffer.data(); }
	int GetBufferSize() { return 1024; } // TODO 하드코딩 삭제

private:
	vector<char> buffer;
	int writeSize;
};
