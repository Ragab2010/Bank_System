#include "serialization.h"

void Serialization::saveToFile(const std::string& filename, const Bank& bank) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for saving");
    }

    const auto& database = bank.getDatabase();

    // Save the number of entries
    size_t dbSize = database.size();
    outFile.write(reinterpret_cast<const char*>(&dbSize), sizeof(dbSize));

    // Save each entry
    for (const auto& [accountId, pair] : database) {
        const auto& person = pair.first;
        const auto& account = pair.second;

        // Write Account ID
        outFile.write(reinterpret_cast<const char*>(&accountId), sizeof(accountId));

        // Write Person data
        int personId = person->getPersonId();
        outFile.write(reinterpret_cast<const char*>(&personId), sizeof(personId));

        std::string name = person->getName();
        size_t nameSize = name.size();
        outFile.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
        outFile.write(name.data(), nameSize);

        std::string nationalIdCard = person->getNationalIdCard();
        size_t idCardSize = nationalIdCard.size();
        outFile.write(reinterpret_cast<const char*>(&idCardSize), sizeof(idCardSize));
        outFile.write(nationalIdCard.data(), idCardSize);

        // int numOfAcc = person->getNumberOfAccounts();
        // outFile.write(reinterpret_cast<const char*>(&numOfAcc), sizeof(numOfAcc));

        // Write Account data
        int accountBalance = account->getBalance();
        outFile.write(reinterpret_cast<const char*>(&accountBalance), sizeof(accountBalance));

        time_t creationTime = account->getTimeOfAccCreation();
        outFile.write(reinterpret_cast<const char*>(&creationTime), sizeof(creationTime));
    }
}

void Serialization::loadFromFile(const std::string& filename, Bank& bank) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Failed to open file for loading");
    }

    size_t dbSize;
    inFile.read(reinterpret_cast<char*>(&dbSize), sizeof(dbSize));

    for (size_t i = 0; i < dbSize; ++i) {
        // Read Account ID
        AccountId_t accountId;
        inFile.read(reinterpret_cast<char*>(&accountId), sizeof(accountId));

        // Read Person data
        int personId;
        inFile.read(reinterpret_cast<char*>(&personId), sizeof(personId));

        size_t nameSize;
        inFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        std::string name(nameSize, ' ');
        inFile.read(&name[0], nameSize);

        size_t idCardSize;
        inFile.read(reinterpret_cast<char*>(&idCardSize), sizeof(idCardSize));
        std::string nationalIdCard(idCardSize, ' ');
        inFile.read(&nationalIdCard[0], idCardSize);

        // int numOfAcc;
        // inFile.read(reinterpret_cast<char*>(&numOfAcc), sizeof(numOfAcc));
        //person->setNumOfAcc(numOfAcc);

        // Check if there is a person with the same name or national ID
        auto PersonIter = bank.findPersonByNameOrId(name, nationalIdCard);

        // auto person= std::make_shared<Person>(personId, name, nationalIdCard, accountId);
        std::shared_ptr<Person> person;

        // If a person with the same name or national ID exists
        if (PersonIter !=nullptr) {
            // Increment the number of accounts for the found person
            PersonIter->incrementNumAcc(accountId); 
            person =PersonIter;
        } else {
        // Create a new person and associate with the account
        // person = std::make_shared<Person>(personId, std::move(name), std::move(nationalIdCard) , accountId);
        person = ObjectFactory::createPerson(personId, std::move(name), std::move(nationalIdCard) , accountId);
        // std::cout<<"new mUniquePersonId:"<<mUniquePersonId<<std::endl;
        }

        // Read Account data
        int accountBalance;
        inFile.read(reinterpret_cast<char*>(&accountBalance), sizeof(accountBalance));

        time_t creationTime;
        inFile.read(reinterpret_cast<char*>(&creationTime), sizeof(creationTime));

        // auto account = std::make_shared<Account>(accountId, accountBalance);
        auto account = ObjectFactory::createAccount(accountId, accountBalance);
        account->setLastOperationDetails(std::chrono::system_clock::from_time_t(creationTime), TypeOfOperation::CREATION);

        // Add to Bank's database
        bank.addToDatabase(accountId, person, account);
    }
}
 