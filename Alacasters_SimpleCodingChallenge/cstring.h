#pragma once

#include "customerror.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


typedef struct _cstring _cstring;
typedef struct cstring cstring;

typedef struct cstring
{
	// getters
	_Bool( *at_get )( const cstring this, const size_t idx, char* c );
	_Bool( *empty )( const cstring this );
	size_t( *size )( const cstring this );
	const char* const*( *str )( const cstring this );

	// utilities
	_Bool( *copy )( const cstring this, cstring other );
	_Bool( *find )( const cstring this, size_t offset, const char c, size_t* foundAt );
	_Bool( *substr )( const cstring this, size_t offset, size_t length, cstring subString );

	// container properties
	void( *clear )( cstring this );
	void( *fill )( cstring this, const size_t size, const char c );
	void( *pop_back )( cstring this );
	_Bool( *push_back )( cstring this, const char c );
	_Bool( *reserve )( cstring this, const size_t size );
	_Bool( *resize )( cstring this, const size_t size );

	// setter
	_Bool( *at_set )( cstring this, size_t idx, const char c );
	_Bool( *insert )( cstring this, size_t offset, const char c );
	_Bool( *insert_string )( cstring this, size_t offset, const char* str );
	_Bool( *insert_cstring )( cstring this, size_t offset, const cstring other );

	_cstring* _string;
}cstring;

_Bool cs_default_construct(cstring* this);
_Bool cs_reserve_construct( cstring* this, const size_t size );
_Bool cs_size_construct( cstring* this, const size_t size, const char fillWith );
_Bool cs_string_construct( cstring* this, const char* str );
_Bool cs_destroy_cstring( cstring* this );

