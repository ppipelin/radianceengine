#include "include.h"

#include "boardParser.h"
#include "sio_client.h"

#include <mutex>
#include <future>
#include <thread>
#include <chrono>
#include <condition_variable>

/*******************************************************************************
	* SOCKET RELATED
	******************************************************************************/

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

sio::socket::ptr current_socket;

/*******************************************************************************
	* PROGRAM RELATED
	******************************************************************************/

BoardParser bParserConnect = BoardParser();
bool isWhite = true; //white

void drop(sio::event &e)
{
	// std::cout << "from: " << e.get_messages()[0]->get_string() << std::endl;
	// std::cout << "to: " << e.get_messages()[1]->get_string() << std::endl;
	if (e.get_messages()[0]->get_string() == e.get_messages()[1]->get_string())
		return;
	bParserConnect.movePiece(Board::toTiles(e.get_messages()[0]->get_string()), Board::toTiles(e.get_messages()[1]->get_string()));
	std::cout << bParserConnect.boardParsed()->board()[Board::toTiles(e.get_messages()[0]->get_string())]->str() << " from " << bParserConnect.boardParsed()->board()[Board::toTiles(e.get_messages()[0]->get_string())]->tile() << std::endl;
	std::cout << bParserConnect.boardParsed()->board()[Board::toTiles(e.get_messages()[1]->get_string())]->str() << " to " << bParserConnect.boardParsed()->board()[Board::toTiles(e.get_messages()[1]->get_string())]->tile() << std::endl;
	isWhite = !isWhite;
	// (*bParserConnect.boardParsed())[Board::toTiles(e.get_messages()[]->get_string())];
}

void dragStart(sio::event &e)
{
	std::cout << "dragStart: " << e.get_message()->get_string() << std::endl;
	std::vector<cMove> v;
	Piece *piece = (*bParserConnect.boardParsed())[Board::toTiles(e.get_message()->get_string())];
	piece->canMove(*bParserConnect.boardParsed(), v);
	// std::cout << "v0: " << v[0] << std::endl;
	std::cout << "sending: " << utils::to_string(v) << std::endl;
	current_socket->emit("cpp_moveset", utils::to_string(v));
}

bool cya = false;
void triggerExit(sio::event &)
{
	cya = true;
}

void bind_events()
{
	current_socket->on("dropToCPP", [&](sio::event &e) {return drop(e);});
	current_socket->on("dragStartToCPP", [&](sio::event &e) {return dragStart(e);});
	current_socket->on("triggerExit", &triggerExit);
}

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

	_lock.lock();
	if (false)
	{
		_cond.wait(_lock);
	}
	_lock.unlock();

	bind_events();

	using namespace std::chrono_literals;
	while (!cya)
	{
		// auto a2 = std::async(std::launch::deferred, dragStart, x, "world!");
		// std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::this_thread::sleep_for(10s);
	}

	h.sync_close();
	h.clear_con_listeners();

	return 0;
}
