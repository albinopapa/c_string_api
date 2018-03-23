#pragma once

#include <ctype.h>

// Memory
void** AllocateArray2D( size_t ArrayCount, size_t ElementSize );
void SafeDelete( void** ptr );
void SafeDeleteArray( void** ptr, size_t count );

