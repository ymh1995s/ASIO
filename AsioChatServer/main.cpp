#include <SDKDDKVer.h>
#include "ChatServer.h"

int main()
{
	boost::asio::io_context io_context; // IOCP를 추상화한 ASIO 객체
	// accept, connect, rw 같은 비동기 함수 호출이 io_context에 등록된다. 
	// => 그러므로 서버나 세션에 생성자 파라미터로 넘겨줘야함
	// ex) ChatServer 클래스의 acceptor와 연결해야함
	// ex) Session 클래스의 비동기 입출력을 위해 

	ChatServer server(io_context, 100/*MAX Session Count Pooling*/);

	io_context.run(); // 내부적으로 GetQueuedCompletionStatus() 실행 (io_context에 등록된 핸들러들을 실행)

	while (true)
	{

	}

	return 0;
}