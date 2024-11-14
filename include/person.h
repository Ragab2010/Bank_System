#pragma once
#include <string>
#include <vector>
#include <algorithm>

using AccountId_t = int;

class Person {
public:
    Person(int personId, std::string name, std::string nationalIdCard, AccountId_t accountId);

    // Setters
    void setPersonId(int personId);
    void setName(std::string name);
    void setNumOfAcc(int num);

    // Getters
    int getPersonId() const;
    std::string getName() const;
    int getNumberOfAccounts() const; // Fixed name
    std::string getNationalIdCard() const;

    // Increment number of accounts
    void incrementNumAcc(AccountId_t accountId) ;

    // Decrement number of accounts
    void decrementNumAcc(AccountId_t accountId);

    // Accessor for account IDs
    const std::vector<AccountId_t>& getAccountIdVector()const;
private:
    int mPersonId;
    std::string mName;
    std::string mNationalIdCard;
    int mNumOfAcc;
    std::vector<AccountId_t> mAccountIds;  // Store account IDs
};
