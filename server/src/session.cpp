#include "session.hpp"
#include "sessions_manager.hpp"
#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/string.hpp>
#include <string>

void session::run()
{
	net::dispatch(ws_.get_executor(), beast::bind_front_handler(
		&session::on_run, shared_from_this()
	)
	);
}

void session::on_run()
{
	//Sets a timeout for cases when client is dead or slow
		ws_.set_option(
			websocket::stream_base::timeout::suggested(
				beast::role_type::server
			)
		);

		//Just changes info in the responce message during handshake
		ws_.set_option(websocket::stream_base::decorator(
			[](websocket::response_type& res)
			{
				res.set(http::field::server,
					std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
			}
		));

		//Accept WebSocket handshake
		ws_.async_accept(
			beast::bind_front_handler(
				&session::on_accept,
				shared_from_this()
			)
		);
}

void session::on_accept(beast::error_code ec)
{
	if (ec)
		return fail(ec, "accept");
	do_read();
}

void session::do_read()
{
	ws_.async_read(
		read_buffer_, beast::bind_front_handler(
			&session::on_read,
			shared_from_this()
		)
	);
}

void session::on_read(
	beast::error_code ec,
	std::size_t bytes_transferd)
{
	//Skips if message is empty
	boost::ignore_unused(bytes_transferd);

	if (ec == websocket::error::closed)
	{
		if(this->session_id != -1)
		{
			manager_->remove_session(session_id);
		}
		return;
	}

	if (ec)
	{
		if(this->session_id != -1)
		{
			manager_->remove_session(session_id);
			std::cout << session_id << " was removed" << std::endl; 
		}
		return fail(ec, "read");
	}


	//Transforming received data from Json
	std::string raw_string = beast::buffers_to_string(read_buffer_.data());

	read_buffer_.consume(bytes_transferd);

	if(this->session_id != -1)
		manager_->on_data(this->session_id, raw_string);
	else
		manager_->on_auth_attempt(shared_from_this(), raw_string);
	do_read();
}

void session::send(std::string message)
{
	std::cout << "send" << std::endl;
	bool writing = !write_queue_.empty();
	write_queue_.push_back(message);
	if(!writing)
		do_write();
}

void session::do_write()
{
	std::cout << "do_write" << std::endl;
	ws_.async_write(boost::asio::buffer(write_queue_.front()), 
		boost::beast::bind_front_handler(&session::on_write, shared_from_this()));
}

void session::on_write(beast::error_code ec, std::size_t bytes_transferred)
{
	if(!ec)
	{
		write_queue_.pop_front();
		if(!write_queue_.empty())
			do_write();
	}
}

void session::on_close(beast::error_code ec)
{
	if (ec)
		return fail(ec, "close");
}

void session::set_session_id(const int64_t session_id)
{
	this->session_id = session_id;
	std::cout << "Session id was set to: " <<  this->session_id << std::endl;
}

void session::reset_session_id()
{
	this->session_id = -1;
	std::cout << "Session id was reset to: " <<  this->session_id << std::endl;
}