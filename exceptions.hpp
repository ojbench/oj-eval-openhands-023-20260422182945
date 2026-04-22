#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP
#include <exception>
#include <string>
namespace sjtu {
class exception : public std::exception {
protected:
    std::string detail;
public:
    exception() = default;
    exception(const exception&) = default;
    const char* what() const noexcept override { return detail.c_str(); }
};
class index_out_of_bound : public exception {};
class runtime_error : public exception {};
class invalid_iterator : public exception {};
class container_is_empty : public exception {};
}
#endif
