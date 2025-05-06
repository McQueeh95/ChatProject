#pragma once

#include "session.h"
#include "sessions_manager.h"

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
	std::cout << "Received: " << beast::buffers_to_string(buffer_.data()) << std::endl;
	//Skips if message is empty
	boost::ignore_unused(bytes_transferd);

	if (ec == websocket::error::closed)
		return;

	if (ec)
		return fail(ec, "read");
	//Transforming received data from Json
	std::string string_data = beast::buffers_to_string(buffer_.data());
	message msg(json::parse(string_data));
	std::cout << "Receieved JSON id: " << msg.get_type() << std::endl;
	//Depending on message type handles connection or forwarding or disconnecting
	switch(msg.get_type())
	{
		//Add session by uuid
		case message::message_type::Connect:
			handle_connect(msg);
			break;
		//Remove session from hashmap close ws 
		case message::message_type::Disconnect:
			handle_disconnect(msg);
			break;
		//Get uuid of receiver, get receiver write in his ws and send it to read state again
		case message::message_type::Forward:
			handle_forward(msg);
			break;
		
	}
	//TODO: Here's logic for sending message to client
	/*ws_.text(ws_.got_text());
	ws_.async_write(
		buffer_.data(),
		beast::bind_front_handler(
			&session::on_write,
			shared_from_this()));*/

}

void session::on_write(
	beast::error_code ec,
	std::size_t bytes_transferred)
{
	std::cout << "on_write called" << std::endl;
	boost::ignore_unused(bytes_transferred);
	if (ec)
		return fail(ec, "write");
	// Clear the buffer
	buffer_.consume(buffer_.size());
	// Do another read
	do_read();
}



//Fix sending messages, it sends but after crashes
//It's curently reading probably cuz of that crash
//void session::send_message(const std::string &message, std::function<void()> on_sent)
//{
//	std::shared_ptr<std::string> msg_shared = std::make_shared<std::string>(message);
//	ws_.text(ws_.got_text());
//	ws_.async_write(
//		boost::asio::buffer(*msg_shared),
//		[self = shared_from_this(), msg_shared, on_sent](beast::error_code ec, std::size_t bytes_transferred)
//		{
//			boost::ignore_unused(bytes_transferred);
//			if (ec)
//				return fail(ec, "write");
//			if (on_sent)
//				on_sent();
//		}
//	);
//}

void session::send_message(const std::string& message, std::function<void()> on_sent)
{
	if (is_writing == true)
	{
		message_queue_.push(message);
		return;
	}

	is_writing = true;

	write_buffer_.clear();
	
	beast::ostream(write_buffer_) << message;

	ws_.async_write(
		write_buffer_.data(),
		[self = shared_from_this(), on_sent](beast::error_code ec, std::size_t bytes_transferred)
		{
			boost::ignore_unused(bytes_transferred);
			if (ec)
				return fail(ec, "write");

			if (on_sent) 
				on_sent();

			self->is_writing = false;
			if (!self->message_queue_.empty())
			{
				std::string next_message = self->message_queue_.front();
				self->message_queue_.pop();
				self->send_message(next_message, [self]()
					{
						self->do_read();
					});
			}
		}
	);
}

void session::on_close(beast::error_code ec)
{
	if (ec)
		return fail(ec, "close");
}

void session::handle_connect(const message& msg)
{
	sessions_manager::instance().add_session(msg.get_uuid_from(),
		shared_from_this());
	std::cout << "User: " << msg.get_uuid_from() << " connected!" << std::endl;
	buffer_.consume(buffer_.size());
	do_read();
}

void session::handle_disconnect(const message& msg)
{
	std::cout << "User: " << msg.get_uuid_from() << " requested for disconecting" << std::endl;
	sessions_manager::instance().remove_session(msg.get_uuid_from());
	ws_.async_close(websocket::close_code::normal,
		beast::bind_front_handler(&session::on_close, shared_from_this()));
}

void session::handle_forward(const message& msg)
{
	std::cout << "Sending message to: " << msg.get_uuid_to() << std::endl;
	std::shared_ptr<session> receiver = sessions_manager::instance().get_client(msg.get_uuid_to());
	std::string string_from_buffer = beast::buffers_to_string(buffer_.data());
	buffer_.consume(buffer_.size());
	receiver->send_message(string_from_buffer, [self = shared_from_this()]()
		{
			self->do_read();
		});
}

