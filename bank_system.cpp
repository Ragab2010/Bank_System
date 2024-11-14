#include <iostream>
#include "bank.h"
#include "cli.h"
#include "serialization.h" 
#include "config_manager.h" 
#include "sqlite.h"



int main() {
    cli menu;
    Bank& universalBank = Bank::getInstance();
    Menu_t SelNumber = 0;
    std::string storageType;
    nlohmann::json config; // Declare config here
    ConfigManager configManager("config.json");// Initialize ConfigManager and get storage type

    try {
        configManager.loadConfig();
        config = configManager.getConfig(); // Assign to the declared variable
        storageType = config["persistence"]["storage_type"];
        std::cout << "Storage type: " << storageType << std::endl;

        // Load data based on storage type
        configManager.loadData(storageType, config, universalBank);
    } catch (const std::exception& e) {
        std::cerr << "Error initializing configuration or loading data: " << e.what() << std::endl;
        return 1;
    }

    // Load data from file
    // try {
    //     Serialization::loadFromFile("bank_data.json", universalBank);
    //     std::cout << "Data loaded successfully from file.\n";
    // } catch (const std::runtime_error& e) {
    //     std::cerr << "No existing data found or failed to load: " << e.what() << "\n";
    // }


    //updateIds
    universalBank.updateUniqueAccountId();
    while (SelNumber != MenuOption::EXIT) {
        SelNumber = menu.showMenu();
        try{
            switch (SelNumber) {
            case MenuOption::CREATEACCOUNT: {
                auto createAccountData = menu.Menu_createAccount();
                if (createAccountData.has_value()) {
                    auto& data = createAccountData.value();
                    int accountId = universalBank.createAccount(
                        std::move(data.personName),
                        std::move(data.nationalCardId),
                        data.amountOfMoney.value_or(0)
                    );
                    std::cout << "The Account ID is: " << accountId << "\n";
                } else {
                    std::cout << "Failed to create account. Please try again.\n";
                }
                break;
            }
            case MenuOption::DELETACCOUNT: {
                auto deleteAccountData = menu.Menu_deleteAccount();
                if (deleteAccountData.has_value()) {
                    int accountId = deleteAccountData.value().accountId.value_or(-1);
                    bool success = universalBank.deleteAccount(accountId);
                    if (success) {
                        std::cout << "Account ID " << accountId << " deleted successfully.\n";
                    } else {
                        std::cout << "Failed to delete account. Account may not exist.\n";
                    }
                }
                break;
            }
            case MenuOption::BALANCEINQUIRY: {
                auto balanceInquiryData = menu.Menu_balanceInquiry();
                if (balanceInquiryData.has_value()) {
                    int accountId = balanceInquiryData.value().accountId.value_or(-1);
                    auto balance = universalBank.balanceInquiryByAccountId(accountId);
                    std::cout << "The balance of Account ID " << accountId << " is: " << balance << "\n";
                }
                break;
            }
            case MenuOption::SHOWPERSONACCOUNTS: {
                std::string personName;
                auto showPersonAccountsData = menu.Menu_showPersonAccounts();
                if (showPersonAccountsData.has_value()) {
                    personName = showPersonAccountsData.value().personName;
                    if (personName.empty()) {
                        personName = ""; 
                    }
                    auto accounts = universalBank.showPersonAccounts(personName);
                    if (!accounts.empty()) {
                        std::cout << "Accounts for " << personName << ":\n";
                        for (const auto& account : accounts) {
                            std::cout << "  Account ID: " << account->getAccountId() 
                                    << ", Balance: " << account->getBalance() << "\n";
                        }
                    }
                } else {
                    std::cout << "No accounts found for " << personName << ".\n";
                }
                break;
            }
            case MenuOption::SHOWACCOUNTDETAILS: {
                auto showAccountDetailsData = menu.Menu_showAccountDetails();
                if (showAccountDetailsData.has_value()) {
                    int accountId = showAccountDetailsData.value().accountId.value_or(-1);
                    auto [id, owner, lastOperationDetails ,timeOfAccountCreation] = universalBank.showAccountDetails(accountId);
                    auto [lastOperationTime, operationType] = lastOperationDetails;
                    std::cout << "Account ID: " << id << "\n";
                    std::cout << "Owner: " << owner << "\n";
                    std::cout << "Last Operation Time: " << std::ctime(&lastOperationTime);
                    std::cout << "Last Operation Type: " <<operationType << "\n";
                    std::cout << "Account Creation Time: " <<std::ctime(&timeOfAccountCreation) << "\n";
                }
                break;
            }
            case MenuOption::DEPOSIT: {
                auto depositData = menu.Menu_deposit();
                if (depositData.has_value()) {
                    int accountId = depositData.value().accountId.value_or(-1);
                    int amount = depositData.value().amountOfMoney.value_or(0);
                    if (universalBank.deposit(accountId, amount)) {
                        std::cout << "Deposited " << amount << " to Account ID " << accountId << ".\n";
                    } else {
                        std::cout << "Deposit failed. Account not found.\n";
                    }
                }
                break;
            }
            case MenuOption::WITHDRAWAL: {
                auto withdrawalData = menu.Menu_withdrawal();
                if (withdrawalData.has_value()) {
                    int accountId = withdrawalData.value().accountId.value_or(-1);
                    int amount = withdrawalData.value().amountOfMoney.value_or(0);
                    if (universalBank.withdrawal(accountId, amount)) {
                        std::cout << "Withdrew " << amount << " from Account ID " << accountId << ".\n";
                    } else {
                        std::cout << "Withdrawal failed. Insufficient balance or account not found.\n";
                    }
                }
                break;
            }
            case MenuOption::EXIT: {
                // Serialization::saveToFile("bank_data.json", universalBank);
                // std::cout << "Data saved. Exiting...\n";
                configManager.saveData(storageType, config, universalBank);
                break;
            }
            default:
                std::cout << "Invalid option. Please try again.\n";
                break;
            }
        }catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
    return 0;
}
