#pragma once

#include "session.h"
#include "sessions_manager.h"
#include "message.h"

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
		buffer_, beast::bind_front_handler(
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
		return;

	if (ec)
		return fail(ec, "read");
	//Transforming received data from Json
	std::string string_data = beast::buffers_to_string(buffer_.data());
	message msg(json::parse(string_data));
	//Depending on message type handles connection or forwarding
	switch (msg.get_type())
	{
		case message::message_type::Connect:
			sessions_manager::instance().add_session(msg.get_uuid_from(), 
				shared_from_this());
			std::cout << "User: " << msg.get_uuid_from() << " connected!" << std::endl;
			break;
		case message::message_type::Forward:
			std::cout << "Sending message to: " << msg.get_uuid_to();
			break;
	}
	//TODO: Here's logic for sending message to client
	ws_.text(ws_.got_text());
	ws_.async_write(
		buffer_.data(),
		beast::bind_front_handler(
			&session::on_write,
			shared_from_this()));

}

void session::on_write(
	beast::error_code ec,
	std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if (ec)
		return fail(ec, "write");

	// Clear the buffer
	buffer_.consume(buffer_.size());

	// Do another read
	do_read();
}