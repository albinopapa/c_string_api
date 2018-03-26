#include "cstring.h"
#include "defines.h"
#include "memory.h"
#include "utility.h"
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

struct _cstring
{
	size_t length, capacity;
	int isConstructed;
	char* buffer;
};

// Private forward declarations
_Bool cs_get( const cstring this, const size_t idx, char* c );
_Bool cs_empty( const cstring this );
size_t cs_length( const cstring this );
const char* const* cs_data( const cstring this );

_Bool cs_copy( const cstring this, cstring other );
_Bool cs_find( const cstring this, size_t offset, const char c, size_t* foundAt );
_Bool cs_substr( const cstring this, size_t offset, size_t length, cstring subString );

void cs_clear( cstring this );
void cs_fill( cstring this, const size_t size, const char c );
void cs_pop_back( cstring this );
_Bool cs_push_back( cstring this, const char c );
_Bool cs_reserve( cstring this, const size_t size );
_Bool cs_resize( cstring this, const size_t size );

_Bool cs_set( cstring this, size_t idx, const char c );
_Bool cs_insert( cstring this, size_t offset, const char c );
_Bool cs_insert_string( cstring this, size_t offset, const char* str );
_Bool cs_insert_cstring( cstring this, size_t offset, const cstring other );

void cs_destroy( _cstring* this );
_Bool cs_grow_to( cstring this, const size_t size );

_Bool cs_isInitialized( cstring this );


// Public definitions
_Bool cs_default_construct( cstring* this )
{
	if( cs_isInitialized( *this ) == true )
	{
		cs_destroy_cstring( this );
	}

	if( this == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}

	_cstring* _string = ( _cstring* )malloc( sizeof( _cstring ) );
	if( _string == nullptr )
	{
		err_set_result( Result_Bad_Alloc );
		return false;
	}

	const size_t capacity = 1;
	_string->buffer = ( char* )malloc( capacity );
	if( _string->buffer == nullptr )
	{
		free( _string );
		err_set_result( Result_Bad_Alloc );
		return false;
	}
	_string->length = 0;
	_string->capacity = capacity;
	_string->isConstructed = 1;
	_string->buffer[ 0 ] = 0;


	cstring self = { 0 };
	self.at_get = cs_get;
	self.empty = cs_empty;
	self.size = cs_length;
	self.str = cs_data;

	self.copy = cs_copy;
	self.find = cs_find;
	self.substr = cs_substr;

	self.clear = cs_clear;
	self.fill = cs_fill;
	self.pop_back = cs_pop_back;
	self.push_back = cs_push_back;
	self.reserve = cs_reserve;
	self.resize = cs_resize;

	self.at_set = cs_set;
	self.insert = cs_insert;
	self.insert_cstring = cs_insert_cstring;
	self.insert_string = cs_insert_string;

	self._string = _string;

	*this = self;
	err_set_result( Result_Ok );

	return true;
}
_Bool cs_reserve_construct( cstring* this, const size_t size )
{
	if( cs_default_construct( this ) == false )
	{
		return false;
	}
	cs_reserve( *this, size );
	return true;
}
_Bool cs_size_construct( cstring* this, const size_t size, const char fillWith )
{
	if( cs_default_construct( this ) == false )
	{
		return false;
	}
	if( cs_resize( *this, size ) == false )
	{
		cs_destroy_cstring( this );
		return false;
	}

	cs_fill( *this, size, fillWith );
	return true;
}
_Bool cs_string_construct( cstring* this, const char* str )
{
	if( ( str == nullptr ) )
	{
		err_set_result( Result_Bad_Pointer );
		return false;
	}

	const size_t len = strlen( str );
	if( cs_reserve_construct( this, len ) == false )
	{
		return false;
	}

	for( size_t i = 0; i < len; ++i )
	{
		if( cs_push_back( *this, str[ i ] ) == false )
		{
			cs_destroy_cstring( this );
			return false;
		}
	}

	return true;
}
_Bool cs_destroy_cstring( cstring* this )
{
	err_set_result( Result_Ok );

	if( this == nullptr )
	{
		err_set_result( Result_Bad_Pointer );
		return false;
	}
	if( cs_isInitialized( *this ) == false )
	{
		err_set_result( Result_Not_Initialized );
		return false;
	}

	cs_destroy( this->_string );

	this->at_get = nullptr;
	this->at_set = nullptr;
	this->clear = nullptr;
	this->copy = nullptr;
	this->empty = nullptr;
	this->fill = nullptr;
	this->find = nullptr;
	this->insert = nullptr;
	this->insert_cstring = nullptr;
	this->insert_string = nullptr;
	this->pop_back = nullptr;
	this->push_back = nullptr;
	this->reserve = nullptr;
	this->resize = nullptr;
	this->_string = nullptr;
	this->size = nullptr;
	this->str = nullptr;
	this->substr = nullptr;
}

// Private definitions
_Bool cs_get( const cstring this, const size_t idx, char* c )
{
	if( idx >= this._string->length )
	{
		err_set_result( Result_Index_Out_Of_Range );
		return false;
	}

	err_set_result( Result_Ok );

	*c = this._string->buffer[ idx ];

	return true;
}
_Bool cs_empty( const cstring this )
{
	return cs_length( this ) > 0 ? false : true;
}
size_t cs_length( const cstring this )
{
	return this._string->length;
}
const char* const* cs_data( const cstring this )
{
	return this._string->buffer;
}

_Bool cs_copy( const cstring this, cstring other )
{
	if( cs_reserve_construct( &other, cs_length( this ) ) == false )
	{
		return false;
	}
	for( size_t i = 0; i < cs_length( this ); ++i )
	{
		char c = 0;
		if( cs_get( this, i, &c ) == false )
		{
			return false;
		}
		if( cs_push_back( other, c ) == false )
		{
			return false;
		}
	}
	
	return true;
}
_Bool cs_find( const cstring this, size_t offset, const char c, size_t* foundAt )
{
	for( size_t i = offset; i < cs_length( this ); ++i )
	{
		char _c = 0;
		cs_get( this, i, &_c );
		if( _c == c )
		{
			*foundAt = i;
			return true;
		}
	}

	return false;
}
_Bool cs_substr( const cstring this, size_t offset, size_t length, cstring subString )
{
	offset = offset > cs_length( this ) ? cs_length( this ) : offset;
	length = offset + length > cs_length( this ) ? cs_length( this ) - offset : length;

	if( cs_reserve_construct( &subString, length ) == false )
	{
		return false;
	}

	for( size_t i = offset; i < offset + length; ++i )
	{
		char c = 0;
		cs_get( this, i, &c );
		if( cs_push_back( subString, c ) == false )
		{
			return false;
		}
	}
	
	return true;
}

void cs_clear( cstring this )
{
	while( cs_empty( this ) == false )
	{
		cs_pop_back( this );
	}

	return true;
}
void cs_fill( cstring this, const size_t size, const char c )
{
	void* dst = ( void* )this._string->buffer;
	const size_t setsize = ( size <= cs_length( this ) ) ? size : cs_length( this );

	memset( dst, c, setsize );
	return true;
}
void cs_pop_back( cstring this )
{
	if( cs_empty( this ) == false )
	{
		this._string->buffer[ this._string->length-- ] = 0;		
	}
}
_Bool cs_push_back( cstring this, const char c )
{
	const size_t capacity = this._string->capacity;
	_Bool retVal = true;

	if( cs_length( this ) + 1 >= capacity )
	{
		retVal = cs_grow_to( this, ( ( capacity * 3 ) / 2 ) + 3 );
	}

	if(retVal == true)
	{
		this._string->buffer[ this._string->length++ ] = c;
	}

	return retVal;
}
_Bool cs_reserve( cstring this, const size_t size )
{
	_Bool retVal = true;
	if( this._string->capacity < size )
	{
		retVal = cs_grow_to( this, size );
	}
	
	return retVal;
}
_Bool cs_resize( cstring this, const size_t size )
{
	if( cs_reserve( this, size + 1 ) == true )
	{
		this._string->length = size;
		return true;
	}

	return false;
}

_Bool cs_set( cstring this, size_t idx, const char c )
{
	if( idx >= cs_length( this ) )
	{
		err_set_result( Result_Index_Out_Of_Range );
		return false;
	}

	this._string->buffer[ idx ] = c;
	return true;
}
_Bool cs_insert( cstring this, size_t offset, const char c )
{
	if( offset > cs_length( this ) )
	{
		err_set_result( Result_Invalid_Parameter );
		return false;
	}

	if( offset == cs_length( this ) )
	{
		cs_push_back( this, c );
		err_set_result( Result_Ok );
		return true;
	}

	if( offset < cs_length( this ) )
	{
		for( size_t i = offset, j = cs_length( this ) - 1; i < j; ++i, --j )
		{
			swap( char, &this._string->buffer[ i ], &this._string->buffer[ j ] );
		}
	}

	return true;
}
_Bool cs_insert_string( cstring this, size_t offset, const char* str )
{
	if( str == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}

	const size_t len = strlen( str );
	if( offset > cs_length( this ) )
	{
		err_set_result( Result_Invalid_Parameter );
		return false;
	}
	
	if( cs_reserve( this, cs_length( this ) + len ) == false )
	{
		return false;
	}

	if( offset == cs_length( this ) )
	{
		for( size_t i = 0; i < len; ++i )
		{
			cs_push_back( this, str[ i ] );
		}
	}
	else
	{
		// Copy chars from offset to end of string
		cstring temp = { 0 };
		if( cs_reserve_construct( &temp, cs_length( this ) + len ) == false )
		{
			return false;
		}

		// Copy 0 to offset to temp
		for( size_t i = 0; i < offset; ++i )
		{
			char c = 0;
			cs_get( this, i, &c );
			cs_push_back( temp, c );
		}

		// Copy string parameter to temp
		for( size_t i = 0; i < len; ++i )
		{
			cs_push_back( temp, str[ i ] );
		}

		// Copy remaining chars from this to temp
		for( size_t i = offset; i < cs_length( this ); ++i )
		{
			char c = 0;
			cs_get( this, i, &c );
			cs_push_back( temp, c );
		}

		// destroy this 
		cs_destroy_cstring( &this );

		// Assign temp to this, don't destroy temp
		this = temp;
	}

	return true;
}
_Bool cs_insert_cstring( cstring this, size_t offset, const cstring other )
{
	if( cs_isInitialized( other ) == false )
	{
		err_set_result( Result_Not_Initialized );
		return false;
	}

	const size_t len = cs_length( other );
	if( len > offset )
	{
		err_set_result( Result_Invalid_Parameter );
		return false;
	}
	
	if( cs_reserve( this, cs_length( this ) + len ) == false )
	{
		return false;
	}

	if( len == offset )
	{
		for( size_t i = 0; i < len; ++i )
		{
			char c = 0;
			cs_get( other, i, &c );
			cs_push_back( this, c );
		}
	}
	else
	{
		// Copy chars from offset to end of string
		cstring temp = { 0 };
		if( cs_reserve_construct( &temp, cs_length( this ) + len ) == false )
		{
			return false;
		}

		// Copy 0 to offset to temp
		for( size_t i = 0; i < offset; ++i )
		{
			char c = 0;
			cs_get( this, i, &c );
			cs_push_back( temp, c );
		}

		// Copy string parameter to temp
		for( size_t i = 0; i < len; ++i )
		{
			char c = 0;
			cs_get( other, i, &c );
			cs_push_back( temp, c );
		}

		// Copy remaining chars from this to temp
		for( size_t i = offset; i < cs_length( this ); ++i )
		{
			char c = 0;
			cs_get( this, i, &c );
			cs_push_back( temp, c );
		}

		// destroy this 
		cs_destroy_cstring( &this );

		// Assign temp to this, don't destroy temp
		this = temp;
	}
}

void cs_destroy( _cstring* this )
{
	SafeDelete( &this->buffer );
	this->length = 0;
	this->capacity = 0;
}
_Bool cs_grow_to( cstring this, const size_t size )
{
	if( this._string->capacity >= size )
	{
		err_set_result( Result_Ok );
		return true;
	}

	char* buffer = ( char* )malloc( size );
	if( buffer == nullptr )
	{
		err_set_result( Result_Bad_Alloc );
		return false;
	}

	memset( buffer, 0, size );
	if( cs_length( this ) > 0 )
	{
		memcpy( buffer, this._string->buffer, cs_length( this ) );
	}

	SafeDelete( &this._string->buffer );
	this._string->buffer = buffer;
	this._string->capacity = size;

	err_set_result( Result_Ok );
	return true;
}

_Bool cs_isInitialized( cstring this )
{
	return ( ( this.at_get == cs_get ) && ( this._string != nullptr ) ) ? 1 : 0;
}