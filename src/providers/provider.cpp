#include "providers/provider.h"
#include <iostream>

namespace provider {
    void print_provider_info(const ProviderConfig& provider) {
        std::cout << "Provider: " << provider.name << std::endl;
        std::cout << "URL: " << provider.base_url << std::endl;
    }
}
