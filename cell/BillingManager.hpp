#pragma once
#include <map>
#include <string>
#include <chrono>
#include "interfaces.hpp"

class BillingManager : public IBillingManager
{
    public:

    explicit BillingManager(boost::asio::io_service& svc):svc_(svc) {

    }

    void connectTo(ICallManager& manager) {
        manager_ = &manager;
    }

    virtual void callStarted(const std::string& phone)
    {
        std::cout << "BillingManager::callStarted(" << phone << ")" << std::endl;
        svc_.post([this, phone](){
            if(manager_)   manager_->updateBalance(phone, recalculateBalance_(phone, call_duration(0)));
        });
    }

    virtual void callEnded(const std::string& phone, const call_duration& dur)
    {
        std::cout << "BillingManager::callEnded(" << phone << ")" << std::endl;
        svc_.post([this, phone, dur](){
           if(manager_)    manager_->updateBalance(phone, recalculateBalance_(phone, dur));
        });
    }

    void setBalance(const std::string& phone, const call_duration& balance) {

        accounts_[phone] = balance;
    }

    private:

    call_duration recalculateBalance_(const std::string& phone, call_duration dur) {
        auto account = accounts_.find(phone);
        if(account == accounts_.end()){
            return call_duration(0);
        }
        account->second -= dur;
        std::cout << "New balance for: " << phone << " = " << std::chrono::duration_cast<std::chrono::seconds>(account->second).count() << std::endl;
        return account->second;
    }
    
    std::map<std::string, call_duration> accounts_;
    ICallManager* manager_ = nullptr;
    boost::asio::io_service& svc_;
};
