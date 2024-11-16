#pragma once

#include "person.h"
#include "account.h"
#include "factory.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <optional>

using AccountId_t = int ;
using Id_t = int ;
using AccountId_t = int ;
using Name_t = std::string ;
using Database_t = std::unordered_map<AccountId_t, std::pair<std::shared_ptr<Person>, std::shared_ptr<Account>>>;



struct AccountDetails {
    AccountId_t accountId;
    Name_t personName;
    std::chrono::system_clock::time_point lastOperationTime;
    TypeOfOperation operationType;
};

//Singleton pattern 
class Bank{
public:
    static Bank& getInstance();

    int getDatabaseSize() const ;

    void addToDatabase(AccountId_t accountId, std::shared_ptr<Person> person, std::shared_ptr<Account> account) ;
    const Database_t& getDatabase() const ;
    void updateUniqueAccountId();

    int createAccount( Name_t personName, std::string nationalIdCard, int balance = 0);
    bool deleteAccount(AccountId_t accountId) ;
    int balanceInquiryByAccountId(AccountId_t accountId);
    std::tuple<AccountId_t ,Name_t , std::pair< time_t ,std::string> , time_t > 
    showAccountDetails(AccountId_t accountId);
    int withdrawal(AccountId_t accountId , int withdrawalMoney);
    int deposit(AccountId_t accountId , int depositMoney);
    std::vector<std::shared_ptr<Account>> showPersonAccounts( Name_t personName);
    
    std::pair< time_t ,std::string> convertLastOperationToTimeAndString( std::pair<std::chrono::system_clock::time_point ,TypeOfOperation >  lastOperationDetails);

    void addAccountFromDatabase(AccountId_t accountId, const std::string& ownerName, int balance,
                            const std::chrono::system_clock::time_point& creationTime,
                            const std::pair<std::chrono::system_clock::time_point, TypeOfOperation>& lastOperationDetails);

    std::vector<std::shared_ptr<Account>> getAccounts() const;
    std::shared_ptr<Person> findPersonByNameOrId(const Name_t& personName, const std::string& nationalIdCard);
private:

    std::shared_ptr<Account> findAccountByName(const Name_t& personName);

    std::shared_ptr<Account> findAccountById(AccountId_t accountId) ;
    std::string trim(const std::string str);
    //constructor to make Back singleton
    Bank();
    /*this for store account and pointer to Person  by AccountId,
        why pointer/reference  for Person ? for normalize system ,
        if there more than account for one person so we store only reference for it 
        to minimize the storage data  as (primary/foreign keys in databases)
    */
    std::unordered_map<AccountId_t ,std::pair<std::shared_ptr<Person>  , std::shared_ptr<Account>> > mDatabase;

    /*this table for store the Person instances*/
    std::vector<std::shared_ptr<Person>> mPersonTable;

    int mNumberOfAcc;
    int mUniqueAccountId;
    int mUniquePersonId;

    // std::unordered_map<Person , std::unordered_map<AccountId_t , Account>> mDatabase;
    // std::unordered_map<AccountId , Account> mAccountTable;
    // std::unordered_map<PersonId  , Person > mPersonTable;
};