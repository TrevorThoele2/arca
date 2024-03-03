#pragma once

#include <Arca/Command.h>

struct BasicCommand
{
    int value = 0;
};

namespace Arca
{
    template<>
    struct Traits<BasicCommand>
    {
        static const ObjectType objectType = ObjectType::Command;
        static TypeName TypeName() { return "BasicCommand"; }
    };
}