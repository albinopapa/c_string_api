#include "memory.h"
#include <stdlib.h>

void** AllocateArray2D( size_t ArrayCount, size_t ElementSize )
{
	void** ppVoid = ( void** )malloc( ArrayCount );
	for( size_t i = 0; i < ArrayCount; ++i )
	{
		ppVoid[ i ] = malloc( ElementSize );
	}

	return ppVoid;
}
void SafeDelete( void** ptr )
{
	if( *ptr )
		free( *ptr );
	*ptr = NULL;
}
void SafeDeleteArray( void ** ptr, size_t count )
{
	if( ptr )
	{
		for( size_t i = 0; i < count; ++i )
		{
			SafeDelete( ptr[ i ] );
		}

		free( ( ptr ) );
		ptr = NULL;
	}
}

