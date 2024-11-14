#pragma once
#include <iostream>
#include <optional>
#include <tuple>
#include <string>
#include <limits>

using Menu_t = int;
enum MenuOption {
    CREATEACCOUNT = 1,
    DELETACCOUNT,
    BALANCEINQUIRY,
    SHOWPERSONACCOUNTS,
    SHOWACCOUNTDETAILS,
    DEPOSIT,
    WITHDRAWAL,
    EXIT
};

struct MenuReturn{
    std::string        personName;
    std::string        nationalCardId;
    std::optional<int> accountId;
    std::optional<int> amountOfMoney;
};

class cli {
public:
    // Show menu and return the selected option
    Menu_t showMenu();
    // Read the menu number from the user input
    Menu_t readMenuNumberFromInput() ;
    // Create Account and return details as a tuple, or std::nullopt if failed
    std::optional<MenuReturn> Menu_createAccount() ;
    // Handle Deposit
    std::optional< MenuReturn > Menu_deposit() ;
    // Handle Withdrawal
    std::optional<MenuReturn> Menu_withdrawal() ;
    // Balance Inquiry for an Account
    std::optional<MenuReturn> Menu_balanceInquiry();
    // Handle SHOWPERSONACCOUNTS 
    std::optional<MenuReturn> Menu_showPersonAccounts();
    // Handle SHOWACCOUNTDETAILS 
    std::optional<MenuReturn> Menu_showAccountDetails();
    // Delete Account
    std::optional<MenuReturn> Menu_deleteAccount();
private:
    // Read a full line of input (for strings)
    std::string readString();
    // Read an integer (for balance, etc.)
    int readInteger();
};
