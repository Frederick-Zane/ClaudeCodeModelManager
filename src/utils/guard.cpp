#include "utils/guard.h"
#include <iostream>
#include <cstdlib>
#include <vector>

namespace guard {

static std::filesystem::path home_dir() {
    const char* h = std::getenv("HOME");
    return h ? std::filesystem::path(h) : std::filesystem::path{};
}

static std::vector<std::filesystem::path> protected_paths() {
    auto home = home_dir();
    if (home.empty()) return {};

    auto cwd = std::filesystem::current_path();

    return {
        home / ".claude" / "settings.json",
        home / ".claude" / "settings.local.json",
        home / ".claude.json",
        cwd / ".claude" / "settings.json",
        cwd / ".claude" / "settings.local.json",
    };
}

bool is_protected(const std::filesystem::path& target) {
    std::error_code ec;
    auto abs = std::filesystem::weakly_canonical(target, ec);
    if (ec) abs = std::filesystem::absolute(target);

    for (const auto& p : protected_paths()) {
        auto abs_p = std::filesystem::weakly_canonical(p, ec);
        if (ec) continue;
        if (abs == abs_p) {
            std::cerr << "ERROR: blocked: cannot write to live Claude Code config: "
                      << abs << std::endl;
            return true;
        }
    }
    return false;
}

} // namespace guard
