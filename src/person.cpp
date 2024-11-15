#include"person.h"

Person::Person(int personId, std::string name, std::string nationalIdCard, AccountId_t accountId) 
    : mPersonId{personId}, mName{std::move(name)}, mNationalIdCard{std::move(nationalIdCard)}, mNumOfAcc{0}  , sqliteFlage{}{
    mAccountIds.push_back(accountId);
    mNumOfAcc++;
}

    Person::Person(int personId, std::string name, std::string nationalIdCard)
            : mPersonId{personId}, mName{std::move(name)}, mNationalIdCard{std::move(nationalIdCard)}, mNumOfAcc{},sqliteFlage{}{ }
// Setters
void Person::setPersonId(int personId) { mPersonId = personId; }
void Person::setName(std::string name) { mName = std::move(name); }
void Person::setNumOfAcc(int num) { mNumOfAcc = num; }

// Getters
int Person::getPersonId() const { return mPersonId; }
std::string Person::getName() const { return mName; }
int Person::getNumberOfAccounts() const { return mNumOfAcc; } // Fixed name
std::string Person::getNationalIdCard() const { return mNationalIdCard; }

// Increment number of accounts
void Person::incrementNumAcc(AccountId_t accountId) {
    mAccountIds.push_back(accountId);
    mNumOfAcc++;
}

// Decrement number of accounts
void Person::decrementNumAcc(AccountId_t accountId) {
    mAccountIds.erase(std::remove(std::begin(mAccountIds), std::end(mAccountIds), accountId), std::end(mAccountIds));
    mNumOfAcc--;
}

// Accessor for account IDs
const std::vector<AccountId_t>& Person::getAccountIdVector() const {
    return mAccountIds;
}

// set account IDs into AccountIdVector
void Person::addAccountIdVector(AccountId_t accountId)  {
    mAccountIds.push_back(accountId);
}

bool Person::getSqliteFlage()const{
    return sqliteFlage;
}
void Person::setSqliteFlage(bool flage){
    sqliteFlage=flage;
}