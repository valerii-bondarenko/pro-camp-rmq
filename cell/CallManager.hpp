#pragma once
#include <set>
#include "interfaces.hpp"

class CallManager : public ICallManager
{
    public:
        explicit CallManager(boost::asio::io_service& svc):biller_(nullptr), svc_(svc){}


        virtual void updateBalance(const std::string& phone, call_duration left) {
            if(left <= call_duration(0)){
                std::cout << "Need to drop call: " << phone << std::endl;
                to_drop_.insert(phone);
            }
        }

        virtual void pulse()
        {
            for(auto& x: to_drop_) {

            }
        }


        void connectTo(IBillingManager& biller) { biller_ = &biller; }

    private:
        virtual void handleIncommingCall(const std::string& phone)
        {
            std::cout << "Call start: " << phone << std::endl;
            activeCalls_[phone] =  std::chrono::system_clock::now();
            if(biller_) biller_->callStarted(phone);
        }

        virtual void handleCallDisconnection(const std::string& phone) {
            std::cout << "Call   end: " << phone << std::endl;
            auto call = activeCalls_.find(phone);
            if(call != activeCalls_.end()) {
                if(biller_) biller_->callEnded(phone, std::chrono::system_clock::now() - call->second);
                activeCalls_.erase(call);
            }
        }

        std::map<std::string, call_time> activeCalls_;
        std::set<std::string> to_drop_;
        IBillingManager* biller_;
        boost::asio::io_service& svc_;

};


