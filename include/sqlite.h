#pragma once
#include "bank.h"
#include <string>
#include <chrono>
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>

class SQLiteManager {
public:
    // Constructor to initialize database file path
    SQLiteManager(const std::string& dbFilePath);

    // Load data from SQLite database into the Bank instance
    void loadFromDatabase(Bank& bank);

    // Save data from the Bank instance to SQLite database
    void saveToDatabase(Bank& bank);

private:
    std::string databaseFilePath;

    // Helper functions
    void createTablesIfNotExist();

    // Converts system_clock::time_point to std::time_t
    static std::time_t to_time_t(const std::chrono::system_clock::time_point& tp) {
        return std::chrono::system_clock::to_time_t(tp);
    }

    // Converts std::time_t to system_clock::time_point
    static std::chrono::system_clock::time_point from_time_t(std::time_t t) {
        return std::chrono::system_clock::from_time_t(t);
    }
};
