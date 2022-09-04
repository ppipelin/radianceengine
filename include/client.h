#pragma once

#include "include.h"

#include "game.h"

#include "sio_client.h"

#include <mutex>
#include <condition_variable>

class ConnectionListener
{
	std::mutex &m_lock;
	std::condition_variable_any &m_cond;
	bool &m_connect_finish;

public:
	ConnectionListener(std::mutex &lock, std::condition_variable_any &cond, bool &connect_finish) : m_lock(lock), m_cond(cond), m_connect_finish(connect_finish)
	{}

	void on_connected()
	{
		m_lock.lock();
		m_cond.notify_all();
		m_connect_finish = true;
		std::cout << "-> on_connected" << std::endl;
		m_lock.unlock();
	}
	void on_close(sio::client::close_reason const &)
	{
		std::cout << "sio closed " << std::endl;
		exit(0);
	}

	void on_fail()
	{
		std::cout << "sio failed " << std::endl;
		exit(0);
	}
};

class Client
{
private:
	Game *m_game;
	sio::client m_handler;
	sio::socket::ptr m_currentSocket;

	std::mutex m_lock;
	std::condition_variable_any m_cond;
	bool m_connect_finish;
	bool m_cya;

public:
	Client(Game &game);
	Client(const Client &c);
	~Client();

	void drop(sio::event &e);
	void dragStart(sio::event &e);
	void triggerExit(sio::event &);

	void bindEvents();
	int connect();
};
