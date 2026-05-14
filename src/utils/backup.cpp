#include "utils/backup.h"
#include "utils/paths.h"
#include "utils/fs.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>

namespace backup {
    bool create(const std::filesystem::path& source) {
        if (!std::filesystem::exists(source)) return true;

        auto dir = paths::backup_dir();
        if (!fs_util::ensure_dir(dir)) return false;

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        localtime_r(&time, &tm);

        std::ostringstream oss;
        oss << "claude-config-"
            << std::put_time(&tm, "%Y%m%d-%H%M%S")
            << ".json";

        auto dest = dir / oss.str();
        std::error_code ec;
        std::filesystem::copy_file(source, dest, ec);
        if (ec) {
            std::cerr << "ERROR: backup failed: " << ec.message() << std::endl;
            return false;
        }

        rotate(dir);
        return true;
    }

    bool rotate(const std::filesystem::path& backup_dir, size_t max_count) {
        std::vector<std::filesystem::path> backups;
        for (const auto& entry : std::filesystem::directory_iterator(backup_dir)) {
            if (entry.is_regular_file()) {
                backups.push_back(entry.path());
            }
        }

        if (backups.size() <= max_count) return true;

        std::sort(backups.begin(), backups.end());
        size_t to_remove = backups.size() - max_count;
        for (size_t i = 0; i < to_remove; ++i) {
            std::filesystem::remove(backups[i]);
        }
        return true;
    }
}
