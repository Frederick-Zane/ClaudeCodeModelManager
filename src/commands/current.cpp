#include "commands/current.h"
#include <iostream>

namespace cmd {
    int current(const AppConfig& config) {
        const auto* m = config.get_model(config.current_model_name());
        if (!m) {
            std::cerr << "ERROR: no current model set" << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Current model: " << m->name << std::endl;
        std::cout << "Provider: " << m->provider << std::endl;
        return EXIT_SUCCESS;
    }
}
