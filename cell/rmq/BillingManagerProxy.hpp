#pragma once
#include <map>
#include <string>
#include <chrono>
#include "../interfaces.hpp"

#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>
#include <boost/lexical_cast.hpp>

namespace rmq {
class BillingManagerProxy : public IBillingManager
{
    public:

    explicit BillingManagerProxy(const std::string& address,boost::asio::io_service& svc):
        handler_(svc),
        connection_(&handler_, AMQP::Address(address)),
        channel_(&connection_),
        svc_(svc) 
    {

    }

    virtual void callStarted(const std::string& phone)
    {
        channel_.publish("biller", "biller", "callStarted:"+phone);
    }

    virtual void callEnded(const std::string& phone, const call_duration& dur)
    {
        channel_.publish("biller", "biller", "callEnded:"+phone+":"+boost::lexical_cast<std::string>(std::chrono::duration_cast<std::chrono::seconds>(dur).count()));
    }

    private:

    AMQP::LibBoostAsioHandler handler_;
    AMQP::TcpConnection connection_; 
    AMQP::TcpChannel channel_;
    boost::asio::io_service& svc_;

};

}
