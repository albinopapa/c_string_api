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
	void( *putchar )( stringstream this, const char c );
	void(*insert)( stringstream this, const char* str );
	void(*insert_cstring )( stringstream this, const cstring str );
	char(*getchar)( stringstream this );
	cstring(*extract)( stringstream this );
	cstring(*string )( stringstream this );
	void(*seekg)( stringstream this, int offset, seekpos position );
	void(*seekp)( stringstream this, int offset, seekpos position );
	size_t( *tellg )( stringstream this );
	size_t( *tellp )( stringstream this );

	_sstream* stream;
}stringstream;

stringstream ss_construct();
void ss_destroy( stringstream this );
