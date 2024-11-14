#include "cli.h"


// Show menu and return the selected option
Menu_t cli::showMenu() {
    std::cout << "\n------------------------------\n";
    std::cout << "Welcome to Universal Bank System\n";
    std::cout << "Please enter the number for your choice:\n";
    std::cout   << "1: Create Account\n"
                << "2: Delete Account\n"
                << "3: Balance Inquiry\n"
                << "4: Show Person Accounts\n"
                << "5: Show Account Details\n"
                << "6: Deposit\n"
                << "7: Withdrawal\n"
                << "8: Exit\n"; 
    return readMenuNumberFromInput();
}

// Read the menu number from the user input
Menu_t cli::readMenuNumberFromInput() {
    std::string number;
    std::getline(std::cin>>std::ws, number); // Read full line to handle accidental spaces
    try {
        return std::stoi(number); // Convert to integer
    } catch (const std::invalid_argument& e) {
        std::cout << "Invalid input! Please enter a number from 1 to 8.\n";
        return 0; // Invalid choice, retry
    }
}

// Create Account and return details as a tuple, or std::nullopt if failed
std::optional<MenuReturn> cli::Menu_createAccount() {
    std::string personName;
    std::string nationalCardId;
    int amount;

    std::cout << "Enter your Name: ";
    personName = readString();
    //std::cout <<personName<<std::endl;
    std::cout << "Enter your National Card Id: ";
    nationalCardId = readString();
    //std::cout <<nationalCardId<<std::endl;
    std::cout << "Deposit money (or enter 0 if no deposit): ";
    amount = readInteger();
    // std::cout <<amount<<std::endl;

    // Validate input
    if (!personName.empty() && nationalCardId.size() >= 4 && amount >= 0) {
        return MenuReturn{personName , nationalCardId, std::nullopt , amount};
    } else {
        std::cout   << "Invalid input. Please ensure your name is not empty, "
                    << "your card ID is 14 characters, and the balance is non-negative.\n";
    }
    return std::nullopt;  // Return empty optional on failure
}

// Handle Deposit
std::optional< MenuReturn > cli::Menu_deposit() {
    std::cout << "Enter Account ID  for Deposit: ";
    int accountId= readInteger();

    std::cout << "Enter deposit amount: ";
    int amount = readInteger();
    
    if (amount > 0) {
        // Perform deposit logic here (e.g., call to Bank class or database)
        std::cout << "Deposited " << amount << " to account " << accountId << ".\n";
        return MenuReturn{"" , "", accountId , amount}; // Return account and deposit info
        
    } else {
        std::cout << "Invalid deposit amount.\n";
        return std::nullopt;    // Return empty optional on failure
    }
}

// Handle Withdrawal
std::optional<MenuReturn> cli::Menu_withdrawal() {
    std::cout << "Enter Account ID  for Withdrawal: ";
    int accountId= readInteger();

    std::cout << "Enter withdrawal amount: ";
    int amount = readInteger();
    
    if (amount > 0) {
        // Perform withdrawal logic here (e.g., call to Bank class or database)
        std::cout << "Withdrew " << amount << " from account " << accountId << ".\n";
        return MenuReturn{"" , "", accountId , amount}; // Return account and withdrawal info
    } else {
        std::cout << "Invalid withdrawal amount.\n";
        return std::nullopt;   // Return empty optional on failure
    }
}

// Balance Inquiry for an Account
std::optional<MenuReturn> cli::Menu_balanceInquiry() {
    std::cout << "Enter Account ID  for Balance Inquiry: ";
    int accountId= readInteger();


    std::cout << "Account ID : " << accountId<<"\n";
    if (accountId > 0){
        return MenuReturn{"" , "", accountId , std::nullopt};;
    }
    return std::nullopt; 
}


// Handle SHOWPERSONACCOUNTS 
std::optional<MenuReturn> cli::Menu_showPersonAccounts() {
    std::cout << "Enter the person's name to display their accounts: ";
    std::string personName = readString();


    std::cout << "Person Name : " << personName<<"\n";
    if (!personName.empty()){
        return MenuReturn{personName , "", std::nullopt , std::nullopt};;
    }
    return std::nullopt; 
}
// Handle SHOWACCOUNTDETAILS 
std::optional<MenuReturn> cli::Menu_showAccountDetails() {
    std::cout << "Enter Account ID  for Balance Inquiry: ";
    int accountId= readInteger();


    std::cout << "Account ID : " << accountId<<"\n";
    if (accountId > 0){
        return MenuReturn{"" , "", accountId , std::nullopt};;
    }
    return std::nullopt; 
}

// Delete Account
std::optional<MenuReturn> cli::Menu_deleteAccount() {
    std::cout << "Enter Account ID to delete the account: ";
    int accountId= readInteger();

    // Simulate account deletion (actual logic would likely query a Bank class)
    std::cout << "Account ID : " << accountId<<"\n";
    if (accountId > 0){
        return MenuReturn{"" , "", accountId , std::nullopt};
    }
    return std::nullopt;
}

// Read a full line of input (for strings)
std::string cli::readString() {
    std::string line;
    std::getline(std::cin>>std::ws, line); // Read entire line (including spaces)
    return line;
}

// Read an integer (for balance, etc.)
int cli::readInteger() {
    int value;
    std::cin>>std::ws;
    std::cin >> value;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input! Please enter a valid number.\n";
        return -1;  // Return -1 on failure
    }

    return value;
}