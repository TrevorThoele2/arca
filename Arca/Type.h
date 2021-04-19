#pragma once

#include <string>

#include <Inscription/CompositeScribeCategory.h>
#include <Inscription/BinaryArchive.h>

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

namespace Inscription
{
    template<>
    class Scribe<Arca::Type> final
    {
    public:
        using ObjectT = Arca::Type;
    public:
        void Scriven(ObjectT& object, Archive::Binary& archive);
        void Scriven(ObjectT& object, Archive::Json& archive);
    };

    template<class Archive>
    struct ScribeTraits<Arca::Type, Archive> final
    {
        using Category = CompositeScribeCategory<Arca::Type>;
    };
}