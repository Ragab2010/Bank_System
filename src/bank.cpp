

#include "bank.h"

Bank& Bank::getInstance() {
    static Bank instance;
    return instance;
}

int Bank::getDatabaseSize() const {
    return mDatabase.size();
}

void Bank::addToDatabase(AccountId_t accountId, std::shared_ptr<Person> person, std::shared_ptr<Account> account) {
    mDatabase[accountId] = {person, account};
}

const Database_t& Bank::getDatabase() const {
    return mDatabase;
}

void Bank::updateUniqueAccountId() {
    mUniqueAccountId = 0;
    mUniquePersonId = 0;

    for (const auto& [accountId, data] : mDatabase) {
        // Update the unique account ID to the highest account ID
        mUniqueAccountId = std::max(mUniqueAccountId, accountId);

        // Update the unique person ID to the highest person ID
        if (data.first) { // Ensure the person pointer is valid
            mUniquePersonId = std::max(mUniquePersonId, data.first->getPersonId());
        }
    }

    // Increment IDs so the next one to be assigned is unique
    // ++mUniqueAccountId;
    // ++mUniquePersonId;
    // std::cout<<"updated mUniqueAccountId:"<<mUniqueAccountId<<std::endl;
    // std::cout<<"updated mUniquePersonId:"<<mUniquePersonId<<std::endl;

}

int Bank::createAccount( Name_t personName, std::string nationalIdCard, int balance ) {
    personName = trim(personName);
    // Create account
    auto sharedPtrAccount = std::make_shared<Account>(++mUniqueAccountId, balance);
    // std::cout<<"mUniqueAccountId:"<<mUniqueAccountId<<std::endl;
    if (!sharedPtrAccount) {
        --mUniqueAccountId;
        throw std::runtime_error("Cannot create account");
    }

    // Check if there is a person with the same name or national ID
    auto PersonIter = findPersonByNameOrId(personName, nationalIdCard);


    // If a person with the same name or national ID exists
    if (PersonIter !=nullptr) {
        // Increment the number of accounts for the found person
        PersonIter->incrementNumAcc(mUniqueAccountId);  // Dereference the shared_ptr to access the Person object
        mDatabase[mUniqueAccountId] = {PersonIter, sharedPtrAccount};
        // std::cout<<"found mUniquePersonId:"<<PersonIter->getPersonId()<<std::endl;
    } else {
        // Create a new person and associate with the account
        auto sharedPtrPerson = std::make_shared<Person>(++mUniquePersonId, std::move(personName), std::move(nationalIdCard) , mUniqueAccountId);
        // std::cout<<"new mUniquePersonId:"<<mUniquePersonId<<std::endl;
        if (sharedPtrPerson == nullptr) {
            --mUniquePersonId;
            throw std::runtime_error("Cannot create person");
        }
        mDatabase[mUniqueAccountId] = {sharedPtrPerson, sharedPtrAccount};
        mPersonTable.push_back(sharedPtrPerson);  // Add new person to the person table
    }
    //the  mTimeOfCreation and mLastOperationDetails are set by constructor
    // Increment the account count
    mNumberOfAcc++;
    return mUniqueAccountId;
}

bool Bank::deleteAccount(AccountId_t accountId) {
    // Find the account by ID
    auto pairValueIter = mDatabase.find(accountId); 
    if (pairValueIter == mDatabase.end()) {
        throw std::runtime_error("there is no Account by this Id_number");
    }
    // Access the Person pointer and call decrementNumAcc
    if (pairValueIter->second.first) { // Check if shared_ptr<Person> is valid
        pairValueIter->second.first->decrementNumAcc(accountId);
    }

    // Reset the Account pointer
    if (pairValueIter->second.second) { // Check if shared_ptr<Account> is valid
        pairValueIter->second.second.reset();
    }

    // Reset the Person pointer if necessary
    if (pairValueIter->second.first) { // Check if shared_ptr<Person> is valid
        pairValueIter->second.first.reset();
    }

    // Erase the map entry
    mDatabase.erase(pairValueIter);
    return true;
}
int Bank::balanceInquiryByAccountId(AccountId_t accountId) {
        // Find account by account ID
    auto pairValueIter = mDatabase.find(accountId);
    if (pairValueIter == mDatabase.end()){
        throw std::runtime_error("There is no Account by this Id_number");
    } 
    pairValueIter->second.second->setLastOperationDetails( std::chrono::system_clock::now() ,TypeOfOperation::CREATION );
    return pairValueIter->second.second->getBalance();
}
std::vector<std::shared_ptr<Account>> Bank::showPersonAccounts( Name_t personName){
    personName= trim(personName);
    auto personSharedPtr = findPersonByNameOrId(personName , "");
    if(personSharedPtr == nullptr){
        throw std::runtime_error("there is no Person found by this Name");
    }
    auto refAccountIdVector =  personSharedPtr->getAccountIdVector();
    std::vector<std::shared_ptr<Account>> accounts;
    for(auto & accountId :refAccountIdVector ){
        auto iter = findAccountById(accountId);
        if(iter != nullptr)
            accounts.push_back(iter);
    }
    return accounts;
}


std::tuple<AccountId_t ,Name_t , std::pair< time_t ,std::string> , time_t > 
Bank::showAccountDetails(AccountId_t accountId){
    auto accountIdIter = mDatabase.find(accountId);
    if(accountIdIter == mDatabase.end()){
        throw std::runtime_error("there is no Account by this Id_number");
    }
    std::string personName    = accountIdIter->second.first->getName();
    auto lastOperationDetails =  convertLastOperationToTimeAndString(accountIdIter->second.second->getLastOperationDetails());
    auto timeOfAccountCreation= accountIdIter->second.second->getTimeOfAccCreation();
    return {accountId ,personName , lastOperationDetails  , timeOfAccountCreation};
}

int Bank::deposit(AccountId_t accountId , int depositMoney){
    auto accountSharedPtr = findAccountById(accountId);
    if(accountSharedPtr == nullptr){
        throw std::runtime_error("there is no Account by this Id_number");
    }
    accountSharedPtr->setLastOperationDetails( std::chrono::system_clock::now() ,TypeOfOperation::DEPOSIT );
    // return (balance + depositMony)
    return accountSharedPtr->deposit(depositMoney);
}

int Bank::withdrawal(AccountId_t accountId , int withdrawalMoney){
    auto accountSharedPtr = findAccountById(accountId);
    if(accountSharedPtr == nullptr){
        throw std::runtime_error("there is no Account by this Id_number");
    }
    if(accountSharedPtr->getBalance() < withdrawalMoney){
        throw std::runtime_error("there is no enough money to withdrawal");
    }
    accountSharedPtr->setLastOperationDetails( std::chrono::system_clock::now() ,TypeOfOperation::WITHDRAWAL );
    // return (balance - withdrawalMoney)
    return accountSharedPtr->withdrawal(withdrawalMoney);
}


std::pair< time_t ,std::string> Bank::convertLastOperationToTimeAndString( std::pair<std::chrono::system_clock::time_point ,TypeOfOperation >  lastOperationDetails){
    std::string lastOperation;
    switch (lastOperationDetails.second){
    case TypeOfOperation::CREATION: lastOperation="CREATION";break;
    case TypeOfOperation::CHECK_BALANCE: lastOperation="CHECK_BALANCE";break;
    case TypeOfOperation::DEPOSIT: lastOperation="DEPOSIT";break;
    case TypeOfOperation::WITHDRAWAL: lastOperation="WITHDRAWAL";break;
    }
    return {std::chrono::system_clock::to_time_t(lastOperationDetails.first) , lastOperation};
}


void Bank::addAccountFromDatabase(AccountId_t accountId, const std::string& ownerName, int balance,
                        const std::chrono::system_clock::time_point& creationTime,
                        const std::pair<std::chrono::system_clock::time_point, TypeOfOperation>& lastOperationDetails) {
// Create a new Account
auto account = std::make_shared<Account>(accountId, balance);
account->setLastOperationDetails(lastOperationDetails.first, lastOperationDetails.second);

// Check if the person already exists
auto person = findPersonByNameOrId(ownerName, "");
if (!person) {
    person = std::make_shared<Person>(++mUniquePersonId, ownerName, "", accountId);
    mPersonTable.push_back(person);
}

// Add to the database
mDatabase[accountId] = {person, account};
}

std::vector<std::shared_ptr<Account>> Bank::getAccounts() const {
std::vector<std::shared_ptr<Account>> accounts;
for (const auto& entry : mDatabase) {
    accounts.push_back(entry.second.second);  // Add account (second element of the pair)
}
return accounts;
}



std::shared_ptr<Person> Bank::findPersonByNameOrId(const Name_t& personName, const std::string& nationalIdCard)
{
    for(auto &[accountId, pairValue] : mDatabase) {
        if ((pairValue.first->getName() == personName) || 
            (pairValue.first->getNationalIdCard() == nationalIdCard)) {
            return pairValue.first; // Return the shared_ptr to the Person object
        }
    }
    return nullptr; // Return nullptr if no match is found
}
std::shared_ptr<Account> Bank::findAccountByName(const Name_t& personName){
    for(auto &[accountId, pairValue] : mDatabase) {
        if ((pairValue.first->getName() == personName) ){
            return pairValue.second; // Return the shared_ptr to the Person object
        }
    }
    return nullptr; // Return nullptr if no match is found
}

std::shared_ptr<Account> Bank::findAccountById(AccountId_t accountId) {
    // Search for the account by its ID in the account database (unordered_map)
    auto iter = mDatabase.find(accountId);  // mDatabase is assumed to store the account objects

    if (iter != mDatabase.end()) {
        // Account found, return the shared pointer to the account
        return iter->second.second;  // second is the shared_ptr<Account> stored in the pair
    }

    // Account not found, return nullptr
    return nullptr;
}
//constructor to make Back singleton
Bank::Bank():mDatabase{} ,mPersonTable{} ,mNumberOfAcc{} , mUniqueAccountId{} , mUniquePersonId{}{}

std::string Bank::trim(const std::string str) {
    // Find the first non-whitespace character
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        return ""; // If there's no non-whitespace character, return empty string
    }

    // Find the last non-whitespace character
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    
    // Return the substring that represents the trimmed string
    return std::move(str.substr(start, end - start + 1));
}