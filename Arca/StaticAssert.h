#pragma once

#include "TypeValidation.h"

#define STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(type)                               \
static_assert(::Arca::IsCurator<type>::value, "This must be derived from Curator")