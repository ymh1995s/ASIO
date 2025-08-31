#include <SDKDDKVer.h>
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>

using namespace std;

using boost::asio::ip::tcp;

const char SERVER_IP[] = "127.0.0.1";
const int SERVER_PORT = 7777;

struct PacketHeader
{
	int size;   
	int id;
};

struct ChatMessage
{
	PacketHeader header;
	string  body; 
};

class Packet
{
public:
	static vector<char> MakeChatPacket(const string& msg)
	{
		PacketHeader header;
		header.size = sizeof(PacketHeader) + msg.size();
		header.id = 1; // 1 = 채팅

		vector<char> buffer(header.size);
		memcpy(buffer.data(), &header, sizeof(PacketHeader));
		memcpy(buffer.data() + sizeof(PacketHeader), msg.data(), msg.size());

		return buffer;
	}
};

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

	//void send_message(std::shared_ptr<std::vector<char>> packet)
	//{
	//	boost::asio::async_write(socket, boost::asio::buffer(*packet),
	//		[packet](boost::system::error_code ec, std::size_t /*length*/)
	//		{
	//			if (ec)
	//				std::cout << "송신 오류: " << ec.message() << "\n";
	//		});
	//}

	// 패킷을 의도적으로 나눠보내 서버에서 패킷이 바로 가는지 확인하기
	void send_message(std::shared_ptr<vector<char>> packet)
	{
		int half_size = packet->size() / 2;

		// 첫 절반 전송
		boost::asio::async_write(socket, boost::asio::buffer(packet->data(), half_size),
			[packet](boost::system::error_code ec, size_t /*length*/)
			{
				if (ec)
					cout << "첫 번째 전송 오류: " << ec.message() << "\n";
			});

		this_thread::sleep_for(chrono::seconds(2));

		// 두 번째 절반 전송
		boost::asio::async_write(socket, boost::asio::buffer(packet->data() + half_size, packet->size() - half_size),
			[packet](boost::system::error_code ec, size_t /*length*/)
			{
				if (ec)
					cout << "두 번째 전송 오류: " << ec.message() << "\n";
			});
	}

private:
	void do_read()
	{
		socket.async_read_some(boost::asio::buffer(recvBuffer, 1024),
			[this](boost::system::error_code ec, size_t length)
			{
				if (!ec)
				{
					cout << "[서버] " << string(recvBuffer, length) << "\n";
					do_read(); // 계속 읽기 등록
				}
				else
				{
					cout << "수신 오류: " << ec.message() << "\n";
				}
			});
	}

	tcp::socket socket;
	char recvBuffer[1024];
};


int main()
{
	this_thread::sleep_for(chrono::seconds(1));
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
				std::string line;
				std::getline(std::cin, line);
				if (line == "/quit") break;

				vector<char> rawPacket = Packet::MakeChatPacket(line);
				shared_ptr<vector<char>> packet = make_shared<vector<char>>(rawPacket);
				client.send_message(packet);
			}
			io_context.stop();
		});

	// 메인 스레드에서 송수신 이벤트 처리
	io_context.run();

	input_thread.join();
	return 0;
}