#include <iostream>
#include <fstream>
#include <filesystem>
#include <system_error>
#include <string>
#include <cstring>  // for strerror
#include <cerrno>   // for errno
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

class ProcessLock {

public:
    ProcessLock(const std::string& app_name = "myapp");

    ~ProcessLock();

    bool acquire();

    void release();

    bool isLocked()const;
private:
    std::string lockfile_path;
    int fd;
    bool is_locked;
};
