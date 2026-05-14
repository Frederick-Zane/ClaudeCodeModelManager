#include "commands/remove.h"
#include <iostream>

namespace cmd {
    int remove(AppConfig& config, const std::string& name) {
        // Try model first
        if (config.get_model(name)) {
            if (config.current_model_name() == name) {
                config.set_current_model("");
                if (!config.save()) return EXIT_FAILURE;
            }
            if (!config.remove_model(name)) return EXIT_FAILURE;
            std::cout << "Removed model: " << name << std::endl;
            return EXIT_SUCCESS;
        }
        // Then provider
        if (config.get_provider(name)) {
            if (!config.remove_provider(name)) return EXIT_FAILURE;
            std::cout << "Removed provider: " << name << std::endl;
            return EXIT_SUCCESS;
        }
        std::cerr << "ERROR: not found: " << name << std::endl;
        return EXIT_FAILURE;
    }
}
