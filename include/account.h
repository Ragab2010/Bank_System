#pragma once
#include <string>
#include <chrono>

enum class TypeOfOperation { CREATION=1, CHECK_BALANCE, DEPOSIT, WITHDRAWAL };

class Account {
public:
    Account(int accountId, int balance = 0);

    // Setters
    void setAccountId(int id) ;
    void setBalance(int balance) ;
    void setLastOperationDetails(std::chrono::system_clock::time_point timestamp, TypeOfOperation operationType);
    void setTimeOfCreation(std::chrono::system_clock::time_point timeOfCreation);
    // Getters
    int getAccountId() const;
    time_t getTimeOfAccCreation() const;
    // std::chrono::system_clock::time_point getTimeOfAccCreation() const { return mTimeOfCreation; }
    std::pair<std::chrono::system_clock::time_point, TypeOfOperation> getLastOperationDetails() const;

    int getBalance();

    // Operations
    int deposit(int amount);

    int withdrawal(int amount);
private:
    void updateLastOperation(TypeOfOperation operation);

    int mAccountId;
    int mBalance;
    std::chrono::system_clock::time_point mTimeOfCreation;
    std::pair<std::chrono::system_clock::time_point, TypeOfOperation> mLastOperationDetails;
};
