#pragma once

#include <string>

#include <Inscription/CompositeScribeCategory.h>
#include <Inscription/BinaryFormat.h>

namespace Arca
{
    using TypeName = std::string;

    struct Type
    {
        TypeName name{};
        bool isConst = false;

        Type() = default;
        explicit Type(TypeName name);
        Type(TypeName name, bool isConst);
        Type(const Type& arg) = default;
        Type(Type&& arg) noexcept = default;

        Type& operator=(const Type& arg) = default;
        Type& operator=(Type&& arg) = default;

        bool operator==(const Type& arg) const;
        bool operator!=(const Type& arg) const;
    };
}

namespace Chroma
{
    std::string ToString(const Arca::Type& type);
}

namespace std
{
    template<>
    struct hash<Arca::Type>
    {
        using argument_type = Arca::Type;
        using result_type = std::size_t;

        result_type operator()(const argument_type& arg) const noexcept
        {
            const result_type first(std::hash<Arca::TypeName>()(arg.name));
            const result_type second(std::hash<bool>()(arg.isConst));
            return first ^ (second << 1);
        }
    };
}

namespace Inscription
{
    template<>
    class Scribe<Arca::Type> final
    {
    public:
        using ObjectT = Arca::Type;
    public:
        void Scriven(ObjectT& object, Format::Binary& format);
        void Scriven(ObjectT& object, Format::Json& format);
    };

    template<class Format>
    struct ScribeTraits<Arca::Type, Format> final
    {
        using Category = CompositeScribeCategory<Arca::Type>;
    };
}