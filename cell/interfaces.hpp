#pragma once

#include <string>
#include <chrono>
#include <boost/asio/steady_timer.hpp>

typedef std::chrono::time_point<std::chrono::system_clock> call_time;
typedef std::chrono::duration<long, std::ratio<1l, 1000000000l> > call_duration;

class IBillingManager
{
    public:

        virtual void callStarted(const std::string& phone) =0;
        virtual void callEnded(const std::string& phone, const call_duration& dur) =0;

};


class ICallManager
{
    public:

        class Call {

            public:
                explicit Call(const std::string& phone, ICallManager& m, boost::asio::io_service& context):phone_(phone), manager_(m), stop_timer_(context),context_(context){ }

                void initiate(long secs_to_wait) {
                    context_.post([this](){
                        manager_.handleIncommingCall(phone_);
                    });

                    stop_timer_.expires_from_now(std::chrono::seconds(secs_to_wait));
                    stop_timer_.async_wait([this](const boost::system::error_code& error){
                        manager_.handleCallDisconnection(phone_);
                    });
                }

            protected:
                std::string phone_;
                ICallManager& manager_;
                boost::asio::steady_timer stop_timer_;
                boost::asio::io_service& context_;
        };

        virtual void updateBalance(const std::string& phone,  call_duration left) =0;
        virtual void pulse() =0;

    private:

        virtual void handleIncommingCall(const std::string& phone) =0;
        virtual void handleCallDisconnection(const std::string& phone) =0;
};


