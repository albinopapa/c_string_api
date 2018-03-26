#pragma once

#include <ctype.h>

typedef struct _sstream _sstream;
typedef struct stringstream stringstream;

typedef enum
{
	SS_SEEK_BEG = 0,
	SS_SEEK_CUR = 1,
	SS_SEEK_END = 2
}seekpos;

typedef struct stringstream
{
	_Bool(*getchar)( stringstream this, char* pc );
	_Bool( *extract )( stringstream this, cstring* output );
	_Bool( *string )( stringstream this, cstring* output );
	size_t( *tellg )( stringstream this );
	size_t( *tellp )( stringstream this );

	_Bool( *putchar )( stringstream this, const char c );
	_Bool( *insert )( stringstream this, const char* str );
	_Bool( *insert_cstring )( stringstream this, const cstring str );
	_Bool( *eof )( const stringstream this );
	_Bool(*seekg)( stringstream this, int offset, seekpos position );
	_Bool(*seekp)( stringstream this, int offset, seekpos position );

	_sstream* stream;
}stringstream;

_Bool ss_construct( stringstream* this );
void ss_destroy( stringstream* this );
