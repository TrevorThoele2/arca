#include "CuratorHandle.h"

#include <Inscription/MemoryScribe.h>
#include <utility>

namespace Arca
{
    CuratorHandle::CuratorHandle(Type type) : type(std::move(type))
    {}

    CuratorHandle::~CuratorHandle() = default;

    CuratorHandle& CuratorHandle::operator=(const CuratorHandle& arg)
    {
        return *this;
    }

    CuratorHandle& CuratorHandle::operator=(CuratorHandle&& arg) noexcept
    {
        return *this;
    }

    OwnedCuratorHandle::OwnedCuratorHandle(std::unique_ptr<Curator>&& ptr, Type type) :
        CuratorHandle(std::move(type)), ptr(std::move(ptr)), type(typeid(ptr.get()))
    {}

    Curator* OwnedCuratorHandle::Get()
    {
        return ptr.get();
    }

    std::type_index OwnedCuratorHandle::TypeIndex()
    {
        return type;
    }

    bool OwnedCuratorHandle::IsOwning() const
    {
        return true;
    }

    UnownedCuratorHandle::UnownedCuratorHandle(Curator* ptr, Type type) :
        CuratorHandle(std::move(type)), ptr(ptr), type(typeid(ptr))
    {}

    Curator* UnownedCuratorHandle::Get()
    {
        return ptr;
    }

    std::type_index UnownedCuratorHandle::TypeIndex()
    {
        return type;
    }

    bool UnownedCuratorHandle::IsOwning() const
    {
        return false;
    }
}