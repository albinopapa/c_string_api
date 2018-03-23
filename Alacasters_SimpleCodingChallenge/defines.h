#pragma once

#ifdef NULL
#define nullptr NULL
#else
#define nullptr (void*)0
#endif

#define bool _Bool
#define true 1
#define false 0
#define error -1