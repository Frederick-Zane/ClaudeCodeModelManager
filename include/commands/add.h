#pragma once
#include "config/app_config.h"

namespace cmd {
    int add_model(AppConfig& config, const std::string& name, const std::string& provider, const std::string& model);
    int add_provider(AppConfig& config, const std::string& name, const std::string& base_url, const std::string& auth_token);
}
