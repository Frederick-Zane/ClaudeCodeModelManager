#pragma once
#include <string>
#include <vector>

struct ModelConfig {
    std::string name;
    std::string provider;
    std::string model;
};

struct ProviderConfig {
    std::string name;
    std::string base_url;
    std::string auth_token;
};
