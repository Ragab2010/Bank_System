#include "account.h"

Account::Account(int accountId, int balance)
    : mAccountId{accountId}, mBalance{balance} {
    mTimeOfCreation = std::chrono::system_clock::now();
    mLastOperationDetails = {std::chrono::system_clock::now(), TypeOfOperation::CREATION};
}

// Setters
void Account::setAccountId(int id) { mAccountId = id; }
void Account::setBalance(int balance) { mBalance = balance; }
void Account::setLastOperationDetails(std::chrono::system_clock::time_point timestamp, TypeOfOperation operationType) {
    mLastOperationDetails = {timestamp, operationType};
}
void Account::setTimeOfCreation(std::chrono::system_clock::time_point timeOfCreation){
    mTimeOfCreation=timeOfCreation;
}

// Getters
int Account::getAccountId() const { return mAccountId; }
time_t Account::getTimeOfAccCreation() const { return std::chrono::system_clock::to_time_t(mTimeOfCreation); }
// std::chrono::system_clock::time_point getTimeOfAccCreation() const { return mTimeOfCreation; }
std::pair<std::chrono::system_clock::time_point, TypeOfOperation> Account::getLastOperationDetails() const {
    return mLastOperationDetails;
}

int Account::getBalance() {
    updateLastOperation(TypeOfOperation::CHECK_BALANCE);
    return mBalance;
}

// Operations
int Account::deposit(int amount) {
    updateLastOperation(TypeOfOperation::DEPOSIT);
    mBalance += amount;
    return mBalance;
}

int Account::withdrawal(int amount) {
    updateLastOperation(TypeOfOperation::WITHDRAWAL);
    mBalance -= amount;
    return mBalance;
}

void Account::updateLastOperation(TypeOfOperation operation) {
    mLastOperationDetails = {std::chrono::system_clock::now(), operation};
}