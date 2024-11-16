

#include "sqlite.h"


SQLiteManager::SQLiteManager(const std::string& dbFilePath)
    : databaseFilePath(dbFilePath) {
    createTablesIfNotExist();
}

void SQLiteManager::createTablesIfNotExist() {
    sqlite3* db;
    char* errorMessage = nullptr;

    // Open the SQLite database
    if (sqlite3_open(databaseFilePath.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
    }

    // SQL commands to create tables if they do not exist
    const char* createTablesSQL = R"(
        CREATE TABLE IF NOT EXISTS Persons (
            PersonId INTEGER PRIMARY KEY,
            Name TEXT NOT NULL,
            NationalIdCard TEXT UNIQUE NOT NULL,
            NumOfAccounts INTEGER --DEFAULT 1  -- Added field for number of accounts
        );

        CREATE TABLE IF NOT EXISTS Accounts (
            AccountId INTEGER PRIMARY KEY,
            PersonId INTEGER NOT NULL,
            Balance INTEGER NOT NULL,
            TimeOfCreation INTEGER NOT NULL,  -- Changed to INTEGER (to store std::time_t)
            LastOperationTime INTEGER,        -- Changed to INTEGER (to store std::time_t)
            LastOperationType INTEGER,
            FOREIGN KEY(PersonId) REFERENCES Persons(PersonId)
        );
    )";

    // Execute the SQL commands
    if (sqlite3_exec(db, createTablesSQL, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        std::string errorMsg = "Failed to create tables: " + std::string(errorMessage);
        sqlite3_free(errorMessage);
        sqlite3_close(db);
        throw std::runtime_error(errorMsg);
    }

    sqlite3_close(db);
}

void SQLiteManager::loadFromDatabase(Bank& bank) {
    sqlite3* db;
    sqlite3_stmt* stmt_persons;
    sqlite3_stmt* stmt_accounts;

    try {
        // Open database connection
        if (sqlite3_open(databaseFilePath.c_str(), &db) != SQLITE_OK) {
            throw std::runtime_error("Failed to open database");
        }

        // Query to select all data from Persons table
        const char* selectPersonsSQL = R"(
            SELECT PersonId, Name, NationalIdCard, NumOfAccounts FROM Persons;
        )";

        // Query to select all data from Accounts table
        const char* selectAccountsSQL = R"(
            SELECT AccountId, PersonId, Balance, TimeOfCreation, LastOperationTime, LastOperationType FROM Accounts;
        )";

        // Prepare SELECT statements
        if (sqlite3_prepare_v2(db, selectPersonsSQL, -1, &stmt_persons, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement for Persons");
        }
        if (sqlite3_prepare_v2(db, selectAccountsSQL, -1, &stmt_accounts, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement for Accounts");
        }

        // Temporary map to associate PersonId with shared_ptr<Person>
        std::unordered_map<int, std::shared_ptr<Person>> personMap;

        // Load all Persons
        while (sqlite3_step(stmt_persons) == SQLITE_ROW) {
            int personId = sqlite3_column_int(stmt_persons, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt_persons, 1));
            std::string nationalIdCard = reinterpret_cast<const char*>(sqlite3_column_text(stmt_persons, 2));
            int numOfAccounts = sqlite3_column_int(stmt_persons, 3);

            // auto person = std::make_shared<Person>(personId, name, nationalIdCard);
            auto person = ObjectFactory::createPerson(personId, name, nationalIdCard , std::nullopt);
            person->setNumOfAcc(numOfAccounts);
            personMap[personId] = person;
        }
        // for(auto [personId , sharedPersonPtr] :personMap){
        //     std::cout<<"personId:"<<personId<<"  :name:"<<sharedPersonPtr->getName()<<std::endl;
        // }
        // std::cout<<"account------------------------------------------"<<std::endl;
        // Load all Accounts and reconstruct mDatabase
        while (sqlite3_step(stmt_accounts) == SQLITE_ROW) {
            int accountId = sqlite3_column_int(stmt_accounts, 0);
            int personId = sqlite3_column_int(stmt_accounts, 1);
            int balance = sqlite3_column_int(stmt_accounts, 2);
            std::time_t timeOfCreation = sqlite3_column_int64(stmt_accounts, 3);
            std::time_t lastOperationTime = sqlite3_column_int64(stmt_accounts, 4);
            int lastOperationTypeInt = sqlite3_column_int(stmt_accounts, 5);

            // Reconstruct the account
            auto account = std::make_shared<Account>(accountId, balance);
            account->setTimeOfCreation(std::chrono::system_clock::from_time_t(timeOfCreation));
            account->setLastOperationDetails(
                std::chrono::system_clock::from_time_t(lastOperationTime),
                static_cast<TypeOfOperation>(lastOperationTypeInt)
            );

            // Link the account with the corresponding person
            auto personIt = personMap.find(personId);
            if (personIt == personMap.end()) {
                throw std::runtime_error("Person ID not found in personMap.");
            }
            
            auto person = personIt->second;
            // std::cout<<"find name:"<<person->getName()<<std::endl;
            //add accountId to the AccountIdVector , this means that this account relate for this person
            person->addAccountIdVector(accountId);
            // Update the Bank's database
            //bank.getDatabase()[accountId] = {person, account};
            bank.addToDatabase(accountId, person, account);
            // std::cout<<"accountId:"<<accountId<<std::endl;
            // std::cout<<"account ID:"<<account->getAccountId()<<std::endl;
            // std::cout<<"person name:"<<person->getName()<<std::endl;
            // std::cout<<"------------------------------------------"<<std::endl;


        }

        // Finalize prepared statements
        sqlite3_finalize(stmt_persons);
        sqlite3_finalize(stmt_accounts);
        std::cout<<"the last line at Load Sqlite3"<<std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    sqlite3_close(db);
}

void SQLiteManager::saveToDatabase(Bank& bank) {
    sqlite3* db;
    sqlite3_stmt* stmt_person = nullptr;
    sqlite3_stmt* stmt_account = nullptr;

    try {
        // Open database connection
        if (sqlite3_open(databaseFilePath.c_str(), &db) != SQLITE_OK) {
            throw std::runtime_error("Failed to open database");
        }

        // Start transaction for better performance and atomicity
        char* errorMsg = nullptr;
        if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errorMsg) != SQLITE_OK) {
            throw std::runtime_error("Failed to begin transaction: " + std::string(errorMsg));
        }

        // Clear the tables
        const char* clearPersonsSQL = "DELETE FROM Persons;";
        const char* clearAccountsSQL = "DELETE FROM Accounts;";

        if (sqlite3_exec(db, clearPersonsSQL, nullptr, nullptr, &errorMsg) != SQLITE_OK) {
            throw std::runtime_error("Failed to clear Persons table: " + std::string(errorMsg));
        }
        if (sqlite3_exec(db, clearAccountsSQL, nullptr, nullptr, &errorMsg) != SQLITE_OK) {
            throw std::runtime_error("Failed to clear Accounts table: " + std::string(errorMsg));
        }

        // Prepare INSERT statement for Persons table
        const char* insertPersonSQL = R"(
            INSERT INTO Persons (PersonId, Name, NationalIdCard, NumOfAccounts)
            VALUES (?, ?, ?, ?);
        )";
        const char* insertAccountSQL = R"(
            INSERT INTO Accounts (AccountId, PersonId, Balance, TimeOfCreation, LastOperationTime, LastOperationType)
            VALUES (?, ?, ?, ?, ?, ?);
        )";

        if (sqlite3_prepare_v2(db, insertPersonSQL, -1, &stmt_person, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement for Persons");
        }
        if (sqlite3_prepare_v2(db, insertAccountSQL, -1, &stmt_account, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement for Accounts");
        }

        // Insert data into the database
        for (const auto& [accountId, personAccountPair] : bank.getDatabase()) {
            const auto& [person, account] = personAccountPair;

            // Insert Person
            sqlite3_bind_int(stmt_person, 1, person->getPersonId());
            sqlite3_bind_text(stmt_person, 2, person->getName().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt_person, 3, person->getNationalIdCard().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt_person, 4, person->getNumberOfAccounts());
            if(person->getSqliteFlage()==false){
                if (sqlite3_step(stmt_person) != SQLITE_DONE) {
                    throw std::runtime_error("Failed to execute INSERT statement for Persons: " + std::string(sqlite3_errmsg(db)));
                }
                person->setSqliteFlage(true);
            }

            sqlite3_reset(stmt_person); // Reset the statement for the next binding

            // Insert Account
            sqlite3_bind_int(stmt_account, 1, account->getAccountId());
            sqlite3_bind_int(stmt_account, 2, person->getPersonId());
            sqlite3_bind_int(stmt_account, 3, account->getBalance());

            // Handle time fields
            std::time_t timeOfCreation = account->getTimeOfAccCreation();
            sqlite3_bind_int64(stmt_account, 4, timeOfCreation);

            auto [lastOperationTime, lastOperationType] = account->getLastOperationDetails();
            sqlite3_bind_int64(stmt_account, 5, to_time_t(lastOperationTime));
            sqlite3_bind_int(stmt_account, 6, static_cast<int>(lastOperationType));

            if (sqlite3_step(stmt_account) != SQLITE_DONE) {
                throw std::runtime_error("Failed to execute INSERT statement for Accounts: " + std::string(sqlite3_errmsg(db)));
            }
            //to make only one person if there is a lot of account relate to one person 
            person->decrementNumAcc(account->getAccountId());

            sqlite3_reset(stmt_account); // Reset the statement for the next binding
        }

        // Commit the transaction
        if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errorMsg) != SQLITE_OK) {
            throw std::runtime_error("Failed to commit transaction: " + std::string(errorMsg));
        }

        std::cout << "Data saved successfully to SQLite database." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        if (db) sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr); // Rollback on error
    }

    // Cleanup
    if (stmt_person) sqlite3_finalize(stmt_person);
    if (stmt_account) sqlite3_finalize(stmt_account);
    if (db) sqlite3_close(db);
}
