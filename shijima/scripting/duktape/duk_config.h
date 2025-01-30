#if !defined(DUK_CONFIG_H_INCLUDED)
#include "_duk_config.h"

#ifdef SHIJIMA_DUK_STATIC_BUILD
#undef DUK_EXTERNAL
#define DUK_EXTERNAL
#undef DUK_EXTERNAL_DECL
#define DUK_EXTERNAL_DECL extern
#endif
#endif
