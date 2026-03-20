#include "session.h"
//#include "server_message.h"
#include "sessions_manager.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/string.hpp>

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
	if (is_reading)
		return;
	is_reading = true;
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
	is_reading = false;
	//Skips if message is empty
	boost::ignore_unused(bytes_transferd);

	if (ec == websocket::error::closed)
		return;

	if (ec)
		return fail(ec, "read");
	//Transforming received data from Json
	std::string string_data = beast::buffers_to_string(buffer_.data());
	boost::json::value msg_j = json::parse(string_data);
	message msg(json::parse(string_data));
	//server_protocol::message m_s = server_protocol::parse_message(json::parse(string_data));
	std::cout << "MSG: " << string_data << std::endl;
	std::cout << "MSG PARSED: " << msg.get_text() << " " << msg.get_time() << std::endl;
	//message msg1(string_data);
	//Depending on message type handles connection or forwarding or disconnecting
	switch(msg.get_type())
	{
		//Add session by uuid
		case message::message_type::Connect:
		{
			handle_connect(msg);
			//std::queue<std::string>& queue_to_send = sessions_manager::instance().get_undelieverd(msg.get_uuid_from());
			std::queue<message>& queue_to_send1 = sessions_manager::instance().get_undelieverd(msg.get_uuid_from());
			while (!queue_to_send1.empty())
			{
				message to_send(queue_to_send1.front());
				
				std::cout << "Sending unsent: " << to_send.get_text() << std::endl;
				queue_to_send1.pop();
				handle_forward(to_send, shared_from_this());
			}
			break;
		}
		//Remove session from hashmap close ws 
		case message::message_type::Disconnect:
		{
			handle_disconnect(msg);
			break;
		}
		//Get uuid of receiver, get receiver write in his ws and send it to read state again
		case message::message_type::Forward:
		case message::message_type::AddContact:
		case message::message_type::ContactAdded:
		case message::message_type::ContactRejected:
			handle_message_before_forwarding(msg, string_data);
			break;
	}

}

void session::do_write()
{
	if(!is_writing && !message_queue_.empty())
	{
		is_writing = true;
		ws_.async_write(boost::asio::buffer(message_queue_.front()), 
						boost::beast::bind_front_handler(
							&session::on_write, shared_from_this()));
	}
}

void session::on_write(
	beast::error_code ec,
	std::size_t bytes_transferred)
{
	if (ec)
		return fail(ec, "write");
	message_queue_.pop();
	is_writing = false;
	
	do_write();
}


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

void session::set_session_id(const int64_t session_id)
{
	this->session_id = session_id;
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

void session::handle_forward(const message& msg, std::shared_ptr<session> receiver)
{
	//Sending unreceived doesn't work because we sending str from buff but msg is in queue not in buff
	std::cout << "Sending message to: " << msg.get_uuid_to() << std::endl;
	std::cout << "Message from: " << msg.get_uuid_from() << std::endl;
	std::cout << "Time: " << msg.get_time() << std::endl;
	std::string string_from_buffer = beast::buffers_to_string(buffer_.data());
	if (string_from_buffer.empty())
	{
		string_from_buffer = json::serialize(msg.to_json());
		std::cout << "Sending message with time: " << string_from_buffer << std::endl;
	}
	buffer_.consume(buffer_.size());
	receiver->send_message(string_from_buffer, [self = shared_from_this()]()
		{
			self->do_read();
		});
	//receiver->send_message_test(string_from_buffer);
}

void session::handle_add_contact(const message& msg, std::shared_ptr<session> receiver)
{
	std::cout << "Sending request to: " << msg.get_uuid_to() <<  " nickname from " << msg.get_username() << std::endl;
	std::string string_from_buffer = beast::buffers_to_string(buffer_.data());
	buffer_.consume(buffer_.size());
	receiver->send_request(string_from_buffer);
}

void session::send_request(const std::string& msg)
{
	if (is_writing == true)
	{
		message_queue_.push(msg);
		return;
	}

	is_writing = true;

	write_buffer_.clear();

	beast::ostream(write_buffer_) << msg;

	ws_.async_write(
		write_buffer_.data(), beast::bind_front_handler(&session::on_write, shared_from_this()));
}

void session::handle_message_before_forwarding(const message& msg, const std::string& string_data)
{
	std::shared_ptr<session> receiver = sessions_manager::instance().get_client(msg.get_uuid_to());
	if (receiver == nullptr)
	{
		//sessions_manager::instance().add_message(msg.get_uuid_to(), string_data, msg);
		//sessions_manager::instance().add_message(msg);
		
		buffer_.consume(buffer_.size());
		do_read();
	}
	else
		handle_forward(msg, receiver);
}

/*void session::deleiver(const server_protocol::message& msg)
{
	boost::json::value to_send = server_protocol::serialize_message(msg);
	do_write();
}*/