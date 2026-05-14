#include "utils/atomic_write.h"
#include "utils/guard.h"
#include "utils/fs.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>

namespace atomic_write {

static bool do_write(const std::filesystem::path& target, const std::string& content) {
    auto tmp = target;
    tmp += ".tmp";

    {
        std::ofstream ofs(tmp, std::ios::binary | std::ios::trunc);
        if (!ofs) {
            std::cerr << "ERROR: cannot open temp file: " << tmp << std::endl;
            return false;
        }
        ofs.write(content.data(), static_cast<std::streamsize>(content.size()));
        if (!ofs.flush()) {
            std::cerr << "ERROR: cannot flush temp file" << std::endl;
            return false;
        }
        ofs.close();
    }

    int fd = ::open(tmp.c_str(), O_RDONLY);
    if (fd >= 0) {
        ::fsync(fd);
        ::close(fd);
    }

    std::error_code ec;
    std::filesystem::rename(tmp, target, ec);
    if (ec) {
        std::cerr << "ERROR: atomic rename failed: " << ec.message() << std::endl;
        std::filesystem::remove(tmp);
        return false;
    }

    return true;
}

bool write(const std::filesystem::path& target, const std::string& content) {
    if (guard::is_protected(target)) return false;
    return do_write(target, content);
}

bool write_unchecked(const std::filesystem::path& target, const std::string& content) {
    return do_write(target, content);
}

} // namespace atomic_write
