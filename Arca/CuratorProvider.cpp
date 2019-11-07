#include "CuratorProvider.h"

namespace Arca
{
    CuratorProviderBase::~CuratorProviderBase() = default;

    auto CuratorProviderBase::Provide(Reliquary& reliquary) -> Provided
    {
        return
        {
            ProvideImpl(reliquary),
            typeDescription
        };
    }

    CuratorProviderBase::CuratorProviderBase(CuratorTypeDescription typeDescription) :
        typeDescription(std::move(typeDescription))
    {}

    ExternalCuratorProvider::ExternalCuratorProvider(Curator* external, CuratorTypeDescription typeDescription) :
        CuratorProviderBase(std::move(typeDescription)), external(external)
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