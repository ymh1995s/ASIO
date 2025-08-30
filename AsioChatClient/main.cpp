#include <SDKDDKVer.h>
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>

using boost::asio::ip::tcp;

const char SERVER_IP[] = "127.0.0.1";
const int SERVER_PORT = 7777;

class ChatClient
{
public:
	ChatClient(boost::asio::io_context& io_context, tcp::endpoint& endpoint)
		: socket(io_context)
	{
		socket.async_connect(endpoint,
			// this => 값으로 캡쳐 
			[this](boost::system::error_code ec)
			{
				if (!ec)
				{
					std::cout << "서버 연결 성공\n";
					do_read();
				}
				else
				{
					std::cout << "연결 실패: " << ec.message() << "\n";
				}
			});
	}

	void send_message(std::shared_ptr<std::string> msg)
	{
		boost::asio::async_write(socket, boost::asio::buffer(*msg),
			[msg](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (ec)
					std::cout << "송신 오류: " << ec.message() << "\n";
			});
	}

private:
	void do_read()
	{
		socket.async_read_some(boost::asio::buffer(recvBuffer, 1024),
			[this](boost::system::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					std::cout << "[서버] " << std::string(recvBuffer, length) << "\n";
					do_read(); // 계속 읽기 등록
				}
				else
				{
					std::cout << "수신 오류: " << ec.message() << "\n";
				}
			});
	}

	tcp::socket socket;
	char recvBuffer[1024];
};


int main()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	boost::asio::io_context io_context;
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(SERVER_IP), 7777);

	ChatClient client(io_context, endpoint);

	// io_context.run()을 호출하면
	// 등록된 비동기 작업이 있는 동안 스레드가 블록되기 떄문에
	// 채팅은 별도 스레드에서 진행
	// 람다 캡쳐 : io_context와 client를 참조로 캡쳐
	std::thread input_thread([&]()
		{
			for (;;)
			{
				std::shared_ptr<std::string> line = std::make_shared<std::string>();
				std::getline(std::cin, *line);
				if (*line == "/quit") break;

				// shared_ptr를 람다 안으로 캡쳐해서 send_message 호출 시 생명주기 유지
				client.send_message(line);
			}
			io_context.stop();
		});

	// 메인 스레드에서 송수신 이벤트 처리
	io_context.run();

	input_thread.join();
	return 0;
}