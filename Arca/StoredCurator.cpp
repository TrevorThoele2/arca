#include "StoredCurator.h"

#include <Inscription/MemoryScribe.h>
#include <utility>

namespace Arca
{
    StoredCurator::StoredCurator(Type type) : type(std::move(type))
    {}

    StoredCurator::~StoredCurator() = default;

    StoredCurator& StoredCurator::operator=(const StoredCurator& arg)
    {
        return *this;
    }

    StoredCurator& StoredCurator::operator=(StoredCurator&& arg) noexcept
    {
        return *this;
    }

    OwnedStoredCurator::OwnedStoredCurator(std::unique_ptr<Curator>&& ptr, Type type) :
        StoredCurator(std::move(type)), ptr(std::move(ptr)), type(typeid(ptr.get()))
    {}

    Curator* OwnedStoredCurator::Get()
    {
        return ptr.get();
    }

    std::type_index OwnedStoredCurator::TypeIndex()
    {
        return type;
    }

    bool OwnedStoredCurator::IsOwning() const
    {
        return true;
    }

    UnownedStoredCurator::UnownedStoredCurator(Curator* ptr, Type type) :
        StoredCurator(std::move(type)), ptr(ptr), type(typeid(ptr))
    {}

    Curator* UnownedStoredCurator::Get()
    {
        return ptr;
    }

    std::type_index UnownedStoredCurator::TypeIndex()
    {
        return type;
    }

    bool UnownedStoredCurator::IsOwning() const
    {
        return false;
    }
}