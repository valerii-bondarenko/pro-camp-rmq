
#include <iostream>
#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>


int main(int argc, char* argv[])
{
    std::string key = argv[1];
    boost::asio::io_service io;

    AMQP::LibBoostAsioHandler handler(io);
    AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://guest:guest@localhost/"));
    AMQP::TcpChannel channel(&connection);
    channel.onReady([&io](){
            std::cout << "Ready" << std::endl;
            });
    channel.publish("my-exchange", key, "Hello world!");
    io.run();
 
}
