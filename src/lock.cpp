#include "lock.h"

ProcessLock::ProcessLock(const std::string& app_name ) 
    : lockfile_path("/tmp/" + app_name + ".lock"), fd(-1), is_locked(false) {}

ProcessLock::~ProcessLock() {
    release();
}

bool ProcessLock::acquire() {
    // Open the file with read/write permissions
    fd = open(lockfile_path.c_str(), O_RDWR | O_CREAT, 0600);
    if (fd == -1) {
        std::cerr << "Error opening lockfile: " << strerror(errno) << std::endl;
        return false;
    }

    // Try to acquire an exclusive lock
    if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
        if (errno == EWOULDBLOCK) {
            std::cerr << "Another instance is already running" << std::endl;
            close(fd);
            return false;
        }
        std::cerr << "Error acquiring lock: " << strerror(errno) << std::endl;
        close(fd);
        return false;
    }

    // Write PID to lockfile
    std::string pid = std::to_string(getpid()) + "\n";
    if (write(fd, pid.c_str(), pid.length()) == -1) {
        std::cerr << "Error writing PID to lockfile" << std::endl;
        release();
        return false;
    }

    is_locked = true;
    return true;
}

void ProcessLock::release() {
    if (fd != -1) {
        if (is_locked) {
            flock(fd, LOCK_UN);
            is_locked = false;
        }
        close(fd);
        fd = -1;
        unlink(lockfile_path.c_str());
    }
}

bool ProcessLock::isLocked() const {
    return is_locked;
}

