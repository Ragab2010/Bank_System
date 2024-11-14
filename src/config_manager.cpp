#include "config_manager.h"

// Constructor accepts config file path and loads config
ConfigManager::ConfigManager(const std::string& configFilePath)
    : mConfigFilePath(configFilePath) {
    //loadConfig();
}

// Getter for config
const nlohmann::json&  ConfigManager::getConfig() const {
    return mConfig;
}

// Saves the current configuration to the file
void  ConfigManager::saveConfig() {
    std::ofstream file(mConfigFilePath);
    if (!file) {
        throw std::runtime_error("Failed to open config file for writing.");
    }
    file << mConfig.dump(4); // Pretty print JSON with 4 spaces
}
// Helper function to load data based on storage type
void  ConfigManager::loadData(const std::string& storageType, const nlohmann::json& config, Bank& bank) {
    try {
        if (storageType == "json") {
            Serialization::loadFromFile(config["persistence"]["json_file_path"], bank);
            std::cout << "Data loaded successfully from JSON file.\n";
        } else if (storageType == "sqlite") {
            SQLiteManager sqliteManager(config["persistence"]["sqlite_file_path"]);
            sqliteManager.loadFromDatabase(bank);
            std::cout << "Data loaded successfully from SQLite database.\n";
        } else {
            throw std::runtime_error("Unsupported storage type: " + storageType);
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error loading data: " << e.what() << "\n";
    }
}

    // Helper function to save data based on storage type
void  ConfigManager::saveData(const std::string& storageType, const nlohmann::json& config, Bank& bank) {
    try {
        if (storageType == "json") {
            Serialization::saveToFile(config["persistence"]["json_file_path"], bank);
        } else if (storageType == "sqlite") {
            SQLiteManager sqliteManager(config["persistence"]["sqlite_file_path"]);
            sqliteManager.saveToDatabase(bank);
        } else {
            throw std::runtime_error("Unsupported storage type: " + storageType);
        }
        std::cout << "Data saved successfully.\n";
    } catch (const std::runtime_error& e) {
        std::cerr << "Error saving data: " << e.what() << "\n";
    }
}
// Loads configuration from the file or creates a default if the file doesn't exist
void  ConfigManager::loadConfig() {
    std::ifstream file(mConfigFilePath);
    if (file) {
        try {
            file >> mConfig;  // Try reading the JSON file
        } catch (const std::exception& e) {
            throw std::runtime_error("Error parsing config file: " + std::string(e.what()));
        }
    } else {
        // File doesn't exist, create default config
        createDefaultConfig();
        saveConfig();  // Save default config to file
    }
}
// Creates a default configuration if the file does not exist
void  ConfigManager::createDefaultConfig() {
    mConfig = {
        {"persistence", {
            {"storage_type", "json"},
            {"json_file_path", "bank_data.json"},
            {"sqlite_file_path", "bank_data.sqlite"}
        }},
        {"logging", {
            {"log_level", "INFO"},
            {"log_file_path", "system.log"},
            {"console_logging", true}
        }},
        {"system", {
            {"max_concurrent_accounts", 1000},
            {"default_account_balance", 0},
            {"unique_id_initialization", {
                {"account_id_start", 1},
                {"person_id_start", 1}
            }}
        }},
        {"backup", {
            {"enabled", true},
            {"interval_minutes", 60},
            {"backup_path", "backups/"}
        }}
    };
}