#ifndef _RWTOOLS_LIBRW_STREAMS_HPP_
#define _RWTOOLS_LIBRW_STREAMS_HPP_

#include <rw/filesystem.hpp>

#include <rw.h>

#include <memory>

enum class LibRWOpenMode : int {
    READ,
    WRITE,
};

template<typename T>
std::shared_ptr<T> LibRWSharedPtr(T* obj) {
    return std::shared_ptr<T>(obj, [](T* obj) {
        obj->destroy();
    });
}

class LibRWStreamFile : public rw::StreamFile {
public:
    LibRWStreamFile() = default;

    using rw::StreamFile::open;
    LibRWStreamFile* open(const rwfs::path& path, LibRWOpenMode openMode) {
        const char* mode;
        switch (openMode) {
        case LibRWOpenMode::READ:
            mode = "r";
            break;
        case LibRWOpenMode::WRITE:
            mode = "w";
            break;
        }
        //using rw::StreamFile::open;
        //open(path.c_str(), mode);
        rw::StreamFile::open(path.c_str(), mode);
        return this;
    }
    virtual ~LibRWStreamFile() {
        this->close();
    }
};

class LibRWStreamMemory : public rw::StreamMemory {
public:
    virtual ~LibRWStreamMemory() {
        this->close();
    }
};

#endif
