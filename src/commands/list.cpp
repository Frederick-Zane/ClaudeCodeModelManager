#include "commands/list.h"
#include <iostream>

namespace cmd {
    int list(const AppConfig& config) {
        const auto& current = config.current_model_name();
        for (const auto& m : config.models()) {
            if (m.name == current) {
                std::cout << "* " << m.name << std::endl;
            } else {
                std::cout << "  " << m.name << std::endl;
            }
        }
        return EXIT_SUCCESS;
    }
}
