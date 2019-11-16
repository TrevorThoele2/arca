#include "CuratorHandle.h"

#include <Inscription/MemoryScribe.h>
#include <utility>

namespace Arca
{
    CuratorHandle::CuratorHandle(TypeHandle typeHandle) : typeHandle(std::move(typeHandle))
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

    OwnedCuratorHandle::OwnedCuratorHandle(std::unique_ptr<Curator>&& ptr, TypeHandle typeHandle) :
        CuratorHandle(std::move(typeHandle)), ptr(std::move(ptr)), type(typeid(ptr.get()))
    {}

    Curator* OwnedCuratorHandle::Get()
    {
        return ptr.get();
    }

    std::type_index OwnedCuratorHandle::Type()
    {
        return type;
    }

    bool OwnedCuratorHandle::IsOwning() const
    {
        return true;
    }

    UnownedCuratorHandle::UnownedCuratorHandle(Curator* ptr, TypeHandle typeHandle) :
        CuratorHandle(std::move(typeHandle)), ptr(ptr), type(typeid(ptr))
    {}

    Curator* UnownedCuratorHandle::Get()
    {
        return ptr;
    }

    std::type_index UnownedCuratorHandle::Type()
    {
        return type;
    }

    bool UnownedCuratorHandle::IsOwning() const
    {
        return false;
    }
}