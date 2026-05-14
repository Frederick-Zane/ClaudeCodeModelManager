#include "commands/use.h"
#include "config/claude_config.h"
#include <iostream>

namespace cmd {
    int use(AppConfig& config, const std::string& model_name) {
        const auto* m = config.get_model(model_name);
        if (!m) {
            std::cerr << "ERROR: model not found: " << model_name << std::endl;
            return EXIT_FAILURE;
        }

        const auto* p = config.get_provider(m->provider);
        if (!p) {
            std::cerr << "ERROR: provider not found: " << m->provider << std::endl;
            return EXIT_FAILURE;
        }

        ClaudeCodeAdapter adapter;
        if (!adapter.load()) {
            std::cerr << "ERROR: Claude config missing" << std::endl;
            return EXIT_FAILURE;
        }

        if (!adapter.switch_model(*m, *p)) {
            std::cerr << "ERROR: failed to switch model" << std::endl;
            return EXIT_FAILURE;
        }

        config.set_current_model(model_name);
        if (!config.save()) {
            std::cerr << "ERROR: failed to save config" << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}
