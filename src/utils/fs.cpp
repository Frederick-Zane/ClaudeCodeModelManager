#include "utils/fs.h"
#include <iostream>

namespace fs_util {
    bool ensure_dir(const std::filesystem::path& dir) {
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec) {
            std::cerr << "ERROR: cannot create dir: " << dir << ": " << ec.message() << std::endl;
            return false;
        }
        return true;
    }
}
