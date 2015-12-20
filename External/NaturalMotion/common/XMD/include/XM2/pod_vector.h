
#include "XM2/vector_iterators.h"
#ifndef XM2_POD_VECTOR_INCLUDED
#define XM2_POD_VECTOR_INCLUDED
#define NMTL_TEMPLATE_EXPORT XM2EXPORT
#define NMTL_NAMESPACE_OVERRIDE XM2
#define NMTL_NO_SENTRY
#include "nmtl/pod_vector.h"
#undef NMTL_NO_SENTRY
#undef NMTL_NAMESPACE_OVERRIDE
#undef NMTL_TEMPLATE_EXPORT
#endif
