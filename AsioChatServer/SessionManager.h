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
	SessionManager(boost::asio::io_context& io_context, shared_ptr<Server> pServer);
	~SessionManager();

	shared_ptr<Session> CreateSession();
	bool CloseSession(int no);
	shared_ptr<Session> GetSession(int no);

private:
	boost::asio::io_context& m_ioContext;
	shared_ptr<Server> serverSession;
	atomic<int> sessionNo; // 이번 세션에게 부여할 번호

public:
	map<int, shared_ptr<Session>> sessionList;
};