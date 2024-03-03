#include "CuratorProvider.h"

namespace Arca
{
    CuratorProviderBase::~CuratorProviderBase() = default;

    auto CuratorProviderBase::Provide(Reliquary& reliquary) -> Provided
    {
        return
        {
            ProvideImpl(reliquary),
            typeHandle
        };
    }

    CuratorProviderBase::CuratorProviderBase(TypeHandle typeHandle) :
        typeHandle(std::move(typeHandle))
    {}

    ExternalCuratorProvider::ExternalCuratorProvider(Curator* external, TypeHandle typeHandle) :
        CuratorProviderBase(std::move(typeHandle)), external(external)
    {}

    std::unique_ptr<CuratorProviderBase> ExternalCuratorProvider::Clone() const
    {
        return std::make_unique<ExternalCuratorProvider>(*this);
    }

    auto ExternalCuratorProvider::ProvideImpl(Reliquary& reliquary) -> ProvidedCurator
    {
        return external;
    }
}