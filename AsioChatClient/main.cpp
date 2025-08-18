﻿#include <SDKDDKVer.h>
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>

const char SERVER_IP[] = "127.0.0.1";


int main()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));
	boost::asio::io_context io_context;

	// boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(SERVER_IP), PORT_NUMBER); // deprecated
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(SERVER_IP), 7777);

	boost::system::error_code connect_error;
	boost::asio::ip::tcp::socket socket(io_context);

	socket.connect(endpoint, connect_error);

	if (connect_error)
	{
		std::cout << "연결 실패. error No: " << connect_error.value() << ", Message: " << connect_error.message() << std::endl;
		getchar();
		return 0;
	}
	else
	{
		std::cout << "서버에 연결 성공" << std::endl;
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	for (int i = 0; i < 7; ++i)
	{
		char szMessage[128] = { 0, };
		sprintf_s(szMessage, 128 - 1, "%d - Send Message", i);
		int nMsgLen = strnlen_s(szMessage, 128 - 1);

		boost::system::error_code ignored_error;
		socket.write_some(boost::asio::buffer(szMessage, nMsgLen), ignored_error);

		std::cout << "서버에 보낸 메시지: " << szMessage << std::endl;


		std::array<char, 128> buf;

		// buf.assign(0); deprecated
		buf.fill(0);

		boost::system::error_code error;

		size_t len = socket.read_some(boost::asio::buffer(buf), error);

		if (error)
		{
			if (error == boost::asio::error::eof)
			{
				std::cout << "서버와 연결이 끊어졌습니다" << std::endl;
			}
			else
			{
				std::cout << "error No: " << error.value() << " error Message: " << error.message().c_str() << std::endl;
			}

			break;
		}

		std::cout << "서버로부터 받은 메시지: " << &buf[0] << std::endl;
	}

	if (socket.is_open())
	{
		socket.close();
	}

	getchar();

	return 0;
}