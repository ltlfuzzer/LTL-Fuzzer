#include <exception.h>

namespace lfz {

Exception::Exception() noexcept
{
    this->msg_ = std::string("LFZ generic error");
}

Exception::Exception(const Exception &e) noexcept
    : msg_(e.msg_)
{
}

Exception &Exception::operator= (const Exception &e) noexcept
{
    this->msg_ = e.msg_;
    return *this;
}

Exception::~Exception()
{
}

const char *Exception::what() const noexcept
{
    return this->msg_.c_str();
}

} // namespace lfz
