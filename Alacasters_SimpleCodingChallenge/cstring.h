#pragma once

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct _cstring _cstring;
typedef struct cstring cstring;

typedef struct cstring
{
	cstring( *copy )( const cstring this );
	void( *push_back )( cstring this, const char c );
	void( *pop_back )( cstring this );
	void( *reserve )( cstring this, const size_t size );
	cstring( *substr )( const cstring this, size_t offset, size_t length );
	char( *at )( const cstring this, const size_t idx );
	int( *empty )( const cstring this );
	size_t( *size )( const cstring this );
	void( *clear )( cstring this );
	char const* const( *str )( const cstring this );
	size_t( *find )( const cstring this, size_t offset, const char c );
	size_t( *find_string )( const cstring this, size_t offset, const char* str );
	void( *fill )( cstring this, const size_t size, const char c );

	_cstring* _string;
}cstring;

cstring cs_default_construct();
cstring cs_size_construct( const size_t size );
cstring cs_string_construct( const char* str );
void cs_destroy_cstring( cstring* this );

