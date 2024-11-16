#include <memory>
#include <optional>
#include "person.h"
#include "account.h"

class ObjectFactory {
public:
    static std::shared_ptr<Person> createPerson(
        int personId,
        const std::string personName,
        const std::string nationalIdCard,
        std::optional<AccountId_t> accountId) {
        if(accountId.has_value()){
            return std::make_shared<Person>(personId, std::move(personName), std::move(nationalIdCard) , accountId.value());
        }
        return std::make_shared<Person>(personId, std::move(personName), std::move(nationalIdCard) );
    }

    static std::shared_ptr<Account> createAccount(int accountId,int balance) {
        return std::make_shared<Account>(accountId, balance);
    }

};
