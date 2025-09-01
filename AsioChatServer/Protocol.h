#pragma once
using namespace std;

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
	static vector<char> MakeChatPacket(const std::string& msg)
	{
		PacketHeader header;
		header.size = sizeof(PacketHeader) + msg.size();
		header.id = 1; // 1 = 채팅

		std::vector<char> buffer(header.size);
		memcpy(buffer.data(), &header, sizeof(PacketHeader));
		memcpy(buffer.data() + sizeof(PacketHeader), msg.data(), msg.size());

		return buffer;
	}
};