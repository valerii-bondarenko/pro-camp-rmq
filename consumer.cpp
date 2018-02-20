#include <iostream>
#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>


int main(int argc, char* argv[])
{
    boost::asio::io_service io;
    boost::asio::io_service::work w(io);

    AMQP::LibBoostAsioHandler handler(io);
    AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://guest:guest@localhost/"));
    AMQP::TcpChannel channel(&connection);
    

    std::string queue = argv[1];
    std::string routing_key = argv[2];

    channel.declareExchange("my-exchange", AMQP::direct).onSuccess([](){
            std::cout << "declared exchange " << std::endl;
            });

    channel.declareQueue(queue).onSuccess([](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
            std::cout << "declared queue " << name << std::endl;
            });

    channel.bindQueue("my-exchange", queue, routing_key).onSuccess([](){
            std::cout << "binded to queue " << std::endl;
            });

    channel.consume(queue, AMQP::noack).onReceived(
            [](const AMQP::Message &message,
                uint64_t deliveryTag,
                bool redelivered)
            {
            std::cout <<" [x] Received from "<< message.exchange() << ":" << std::string(message.body(), message.body()+message.bodySize()) << std::endl;
            });
    
    io.run();
}
