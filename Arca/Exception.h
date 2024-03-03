#pragma once

#include <string>
#include <stdexcept>

namespace Arca
{
    class Exception : public std::runtime_error
    {
    public:
        explicit Exception(const std::string& message);
    };
}
