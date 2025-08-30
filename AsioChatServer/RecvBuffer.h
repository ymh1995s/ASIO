#pragma once
class RecvBuffer
{
public:
	RecvBuffer();

	char* GetBuffer() { return buffer; }
	int GetBufferSize() { return 1024; } // TODO 하드코딩 삭제

	void Read();
	void Write();

private:
	char buffer[1024]; // or 동적할당?
};
