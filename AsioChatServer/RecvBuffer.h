#pragma once
#include "vector"
#include <iostream>
using namespace std;

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int bufferSize);
	~RecvBuffer();

	void			Clean(); // ★ 매번 이 함수를 통해 Copy 없이 Buffer 재사용
	bool			OnRead(int numOfBytes); // 데이터를 읽은 후 커서를 이동
	bool			OnWrite(int numOfBytes); // 데이터를 쓴 후 커서를 이동

	char* ReadPos() { return _buffer.data() + _readPos; } // 현재 읽을 위치를 반환
	char* WritePos() { return _buffer.data() + _writePos; } // 현재 쓸 위치를 반환
	int			DataSize() { return _writePos - _readPos; } // 현재 읽을 수 있는 데이터의 크기를 반환
	int			FreeSize() { return _capacity - _writePos; } // 현재 버퍼에 남아 있는 여유 공간의 크기를 반환

private:
	int			_capacity = 0; // 버퍼의 총 용량
	int			_bufferSize = 0; // 한 블록의 크기
	int			_readPos = 0; // 읽기 커서의 위치
	int			_writePos = 0; // 쓰기 커서의 위치
	vector<char>	_buffer; // 실제 데이터를 저장되는 버퍼
};