#pragma once

#include <stdexcept>

namespace Arca
{
    class BatchNotSetup final : public std::runtime_error
    {
    public:
        BatchNotSetup();
    };
}