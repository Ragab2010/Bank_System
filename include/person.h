#pragma once
#include <string>
#include <vector>
#include <algorithm>

using AccountId_t = int;

class Person {
public:
    Person(int personId, std::string name, std::string nationalIdCard, AccountId_t accountId);
    //this constructor to create instance from sqlite database
    Person(int personId, std::string name, std::string nationalIdCard);

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
    // add account IDs into AccountIdVector
    void addAccountIdVector(AccountId_t accountId );

    bool getSqliteFlage()const;
    void setSqliteFlage(bool flage);

private:
    int mPersonId;
    std::string mName;
    std::string mNationalIdCard;
    int mNumOfAcc;
    bool sqliteFlage;
    std::vector<AccountId_t> mAccountIds;  // Store account IDs

};
