#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <string>
#include <map>
#include <vector>

#include "Session.h"

using namespace std;
class Session;

class SessionManager
{
public:
	SessionManager(boost::asio::io_context& io_context, Server* pServer);
	~SessionManager();

	Session* CreateSession();
	bool CloseSession(int no);
	Session* GetSession(int no);

private:
	boost::asio::io_context& m_ioContext;
	Server* serverSession;
	atomic<int> sessionNo;
	map<int, Session*> clientList;
};