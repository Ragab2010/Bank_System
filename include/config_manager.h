#pragma once
#include <string>
#include <fstream>
#include <nlohmann/json.hpp> // Include your JSON library
#include <stdexcept>  // For runtime_error
#include "serialization.h"
#include "sqlite.h"

class ConfigManager {
public:
    // Constructor accepts config file path and loads config
    ConfigManager(const std::string& configFilePath);

    // Getter for config
    const nlohmann::json& getConfig() const;

    // Saves the current configuration to the file
    void saveConfig();
// Helper function to load data based on storage type
    void loadData(const std::string& storageType, const nlohmann::json& config, Bank& bank);

        // Helper function to save data based on storage type
    void saveData(const std::string& storageType, const nlohmann::json& config, Bank& bank);
    // Loads configuration from the file or creates a default if the file doesn't exist
    void loadConfig();

private:
    std::string mConfigFilePath;
    nlohmann::json mConfig;


    // Creates a default configuration if the file does not exist
    void createDefaultConfig();
};
