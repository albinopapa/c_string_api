#pragma once

#ifdef __cplusplus
#undef nullptr
#undef bool
#undef true
#undef false
#else
#define nullptr NULL
#define bool _Bool
#define true 1
#define false 0
#define error -1
#endif

