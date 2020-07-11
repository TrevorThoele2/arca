#pragma once

#include <string>

#include <Inscription/CompositeScribe.h>
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
    class Scribe<Arca::Type, BinaryArchive> final :
        public CompositeScribe<Arca::Type, BinaryArchive>
    {
    public:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}