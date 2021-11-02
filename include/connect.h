#include "include.h"

#include "sio_client.h"

#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex _lock;

std::condition_variable_any _cond;
bool connect_finish = false;

class Connection_listener
{
	sio::client &handler;

public:
	Connection_listener(sio::client &h) : handler(h)
	{}

	void on_connected()
	{
		_lock.lock();
		_cond.notify_all();
		connect_finish = true;
		std::cout << "-> on_connected" << std::endl;
		_lock.unlock();
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

int participants = -1;

sio::socket::ptr current_socket;

void bind_events()
{}

int connect()
{
	sio::client h;
	Connection_listener l(h);

	h.set_open_listener(std::bind(&Connection_listener::on_connected, &l));
	h.set_close_listener(std::bind(&Connection_listener::on_close, &l, std::placeholders::_1));
	h.set_fail_listener(std::bind(&Connection_listener::on_fail, &l));
	h.connect("http://127.0.0.1:3000");
	_lock.lock();
	if (!connect_finish)
	{
		_cond.wait(_lock);
	}
	_lock.unlock();
	current_socket = h.socket();

	current_socket->emit("cpp_chat");

	_lock.lock();
	if (false)
	{
		_cond.wait(_lock);
	}
	_lock.unlock();
	for (std::string line; std::getline(std::cin, line);)
	{
		current_socket->emit("cpp_move", line);
		if (line == "exit")
		{
			break;
		}
	}

	bind_events();

	h.sync_close();
	h.clear_con_listeners();
	return 0;
}
