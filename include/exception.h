#pragma once

#include <string>

namespace lfz {

class Exception {
public:
    Exception() noexcept;
    Exception(const Exception &e) noexcept;
    Exception &operator= (const Exception &e) noexcept;
    virtual ~Exception();

    virtual const char *what() const noexcept;

private:
    std::string msg_;
};

} // namespace lfz
