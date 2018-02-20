#pragma once
#include <map>
#include <string>
#include <chrono>
#include "../interfaces.hpp"

#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>
#include <boost/algorithm/string/classification.hpp> 
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

namespace rmq {
class BillingManagerAdapter 
{
    public:

    explicit BillingManagerAdapter(const std::string& address,IBillingManager& impl, boost::asio::io_service& svc):
        handler_(svc),
        connection_(&handler_, AMQP::Address(address)),
        channel_(&connection_),
        impl_(impl), svc_(svc) 
    {

    }

    virtual void callStarted(const std::string& phone)
    {
        svc_.post([this, phone](){
                impl_.callStarted(phone);
        });
    }

    virtual void callEnded(const std::string& phone, const call_duration& dur)
    {
        svc_.post([this, phone, dur](){
                impl_.callEnded(phone, dur);
        });
    }

    void activeRMQ() {

        channel_.declareExchange("biller", AMQP::topic).onSuccess([](){
                std::cout << "declared exchange " << std::endl;
                });

        channel_.declareQueue("biller").onSuccess([](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
                std::cout << "declared queue " << name << std::endl;
                });

        channel_.bindQueue("biller", "biller", "biller" ).onSuccess([](){
                std::cout << "binded to queue " << std::endl;
                });

        channel_.consume("biller", AMQP::noack).onReceived(
            [this](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
                svc_.post([this, &message]() {
                    handleMessage_(std::string(message.body(), message.body()+message.bodySize()));
                });
        });
    }
    private:
    void handleMessage_(const std::string& message) {
        //std::cout << "Got message: " << message << std::endl;
        std::vector<std::string> words;
        boost::split(words, message, boost::is_any_of(":"), boost::token_compress_on);

        if(words.empty()) return;
        if(words.front() == "callStarted" && words.size() == 2){
            callStarted(words.back());
        }
        if(words.front() == "callEnded" && words.size() == 3){
            callEnded(words[1], std::chrono::seconds(boost::lexical_cast<int>(words[2])));
        }
    }

    AMQP::LibBoostAsioHandler handler_;
    AMQP::TcpConnection connection_; 
    AMQP::TcpChannel channel_;

    IBillingManager& impl_;
    boost::asio::io_service& svc_;

};

}
