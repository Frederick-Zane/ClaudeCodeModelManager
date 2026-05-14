#include "commands/add.h"
#include <iostream>

namespace cmd {
    int add_model(AppConfig& config, const std::string& name, const std::string& provider, const std::string& model) {
        if (config.get_model(name)) {
            std::cerr << "ERROR: model already exists: " << name << std::endl;
            return EXIT_FAILURE;
        }
        if (!config.get_provider(provider)) {
            std::cerr << "ERROR: provider not found: " << provider << std::endl;
            return EXIT_FAILURE;
        }
        if (!config.add_model(name, provider, model)) {
            std::cerr << "ERROR: failed to add model" << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Added model: " << name << std::endl;
        return EXIT_SUCCESS;
    }

    int add_provider(AppConfig& config, const std::string& name, const std::string& base_url, const std::string& auth_token) {
        if (config.get_provider(name)) {
            std::cerr << "ERROR: provider already exists: " << name << std::endl;
            return EXIT_FAILURE;
        }
        if (!config.add_provider(name, base_url, auth_token)) {
            std::cerr << "ERROR: failed to add provider" << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Added provider: " << name << std::endl;
        return EXIT_SUCCESS;
    }
}
