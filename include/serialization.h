#pragma once

#include <fstream>
#include <memory>
#include <unordered_map>
#include <utility>
#include <string>
#include "bank.h"

class Serialization {
public:
    static void saveToFile(const std::string& filename, const Bank& bank) ;
    static void loadFromFile(const std::string& filename, Bank& bank);
};
