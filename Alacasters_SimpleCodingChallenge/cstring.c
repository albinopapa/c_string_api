#include "cstring.h"
#include "customerror.h"
#include "defines.h"
#include "memory.h"
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
void cs_destroy( _cstring* this );
cstring cs_copy( const cstring this );
void cs_push_back( cstring this, const char c );
void cs_pop_back( cstring this );
void cs_reserve( cstring this, const size_t size );
cstring cs_substr( const cstring this, size_t offset, size_t length );
size_t cs_length( const cstring this );
int cs_empty( const cstring this );
void cs_clear( cstring this );
void cs_fill( cstring this, const size_t size, const char c );
char cs_get( const cstring this, size_t idx );
char const* const cs_data( const cstring this );
size_t cs_find( const cstring this, size_t offset, const char c );
size_t cs_find_string( const cstring this, size_t offset, const char* str );
void cs_grow_to( cstring this, const size_t size );

int cs_isInitialized( cstring this )
{
	return ( ( this.at == cs_get ) && ( this._string != nullptr ) ) ? 1 : 0;
}


// Public definitions
void cs_destroy_cstring( cstring* this )
{	
	ResultCode result = Result_Ok;
	if( ( this == nullptr ) )
	{
		result = Result_Bad_Pointer;
	}
	else if( cs_isInitialized( *this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		cs_destroy( this->_string );

		if( ( result = err_get_result() ) == Result_Ok )
		{
			this->at = nullptr;
			this->copy = nullptr;
			this->fill = nullptr;
			this->find = nullptr;
			this->find_string = nullptr;
			this->pop_back = nullptr;
			this->push_back = nullptr;
			this->reserve = nullptr;
			this->size = nullptr;
			this->substr = nullptr;
			this->_string = nullptr;
			this->empty = nullptr;
			this->clear = nullptr;
			SafeDelete( &this );
		}
	}

	err_set_result( result );
}
cstring cs_default_construct()
{
	ResultCode result = Result_Ok;
	cstring self = { 0 };

	self.copy = cs_copy;
	self.push_back = cs_push_back;
	self.pop_back = cs_pop_back;
	self.reserve = cs_reserve;
	self.substr = cs_substr;
	self.at = cs_get;
	self.str = cs_data;
	self.find = cs_find;
	self.find_string = cs_find_string;
	self.size = cs_length;
	self.empty = cs_empty;
	self.clear = cs_clear;
	self.fill = cs_fill;

	_cstring* _string = ( _cstring* )malloc( sizeof( _cstring ) );
	if( _string == nullptr )
	{
		result = Result_Bad_Alloc;
	}
	else
	{
		_string->buffer = ( char* )malloc( _string->capacity );
		if( _string->buffer == nullptr )
		{
			result = Result_Bad_Alloc;
		}
		_string->buffer[ 0 ] = 0;
		_string->length = 0;
		_string->capacity = 1;
		_string->isConstructed = 1;

		self._string = _string;
	}

	err_set_result( result );
	return self;
}
cstring cs_size_construct( const size_t size )
{
	ResultCode result = Result_Ok;
	cstring this = cs_default_construct();

	if( ( result = err_get_result() ) == Result_Ok )
	{
		cs_reserve( this, size );
	}

	err_set_result( result );
	return this;
}
cstring cs_string_construct( const char* str )
{
	ResultCode result = Result_Ok;
	cstring this = { 0 };

	if( ( str == nullptr ) )
	{
		result = Result_Bad_Pointer;
	}
	else
	{		
		const size_t len = strlen( str );
		this = cs_size_construct( len );

		for( size_t i = 0; i < len; ++i )
		{
			if( ( result = err_get_result() ) != Result_Ok )
			{
				break;
			}

			cs_push_back( this, str[ i ] );
		}
	}

	err_set_result( result );

	return this;
}

// Private definitions
size_t cs_length( const cstring this )
{
	ResultCode result = Result_Ok;
	size_t retVal = -1;

	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		retVal = this._string->length;
	}

	err_set_result( result );

	return retVal;
}
char cs_get( const cstring this, size_t idx )
{
	ResultCode result = Result_Ok;
	char retVal = -1;
	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else if( idx >= this._string->length )
	{
		result = Result_Index_Out_Of_Range;
	}
	else
	{
		retVal = this._string->buffer[ idx ];
	}

	err_set_result( result );
	return retVal;
}
char const* const cs_data( const cstring this )
{
	ResultCode result = Result_Ok;
	char* data = nullptr;

	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else 
	{
		data = this._string->buffer;
	}

	err_set_result( result );
	return data;
}
void cs_destroy( _cstring* this )
{
	ResultCode result = Result_Ok;
	if( this == nullptr )
	{
		result = Result_Bad_Pointer;
	}
	else
	{
		SafeDelete( &this->buffer );
		this->length = 0;
		this->capacity = 0;
	}

	err_set_result( result );
}
cstring cs_copy( const cstring this )
{
	ResultCode result;
	cstring other = { 0 };

	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		other = cs_size_construct( cs_length( this ) );
		for( size_t i = 0; i < cs_length( this ); ++i )
		{
			if( ( result = err_get_result() ) != Result_Ok )
			{
				break;
			}

			cs_push_back( other, cs_get( this, i ) );
		}
	}

	err_set_result( result );
	return other;
}
void cs_push_back( cstring this, const char c )
{
	ResultCode result = Result_Ok;
	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		const size_t capacity = this._string->capacity;

		if( cs_length( this ) + 1 >= capacity )
		{
			cs_grow_to( this, ( ( capacity * 3 ) / 2 ) + 3 );
		}

		if( ( result = err_get_result() ) == Result_Ok )
		{
			this._string->buffer[ this._string->length++ ] = c;
		}
	}

	err_set_result( result );
}
void cs_pop_back( cstring this )
{
	ResultCode result = Result_Ok;

	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		if( cs_empty( this ) == false )
		{
			this._string->buffer[ this._string->length-- ] = 0;
		}
	}

	err_set_result( result );
}
void cs_reserve( cstring this, const size_t size )
{
	ResultCode result = Result_Ok;

	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		if( this._string->capacity < size )
		{
			cs_grow_to( this, size );
			result = err_get_result();
		}
	}

	err_set_result( result );
}
cstring cs_substr( const cstring this, size_t offset, size_t length )
{
	ResultCode result = Result_Ok;
	cstring other = { 0 };

	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		offset = offset > cs_length( this ) ? cs_length( this ) : offset;
		length = offset + length > cs_length( this ) ? cs_length( this ) - offset : length;

		other = cs_size_construct( length );

		for( size_t i = offset; i < offset + length; ++i )
		{
			if( ( result = err_get_result() ) != Result_Ok )
			{
				break;
			}

			cs_push_back( other, cs_get( this, i ) );
		}
	}

	err_set_result( result );
	return other;
}
void cs_grow_to( cstring this, const size_t size )
{
	ResultCode result = Result_Ok;
	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		if( this._string->capacity < size )
		{
			char* buffer = ( char* )malloc( size );
			if( buffer == nullptr )
			{
				result = Result_Bad_Alloc;
			}
			else
			{
				memset( buffer, 0, size );
				if( cs_length( this ) > 0 )
				{
					memcpy( buffer, this._string->buffer, size );
				}

				SafeDelete( &this._string->buffer );
				this._string->buffer = buffer;
				this._string->capacity = size;
			}
		}
	}

	err_set_result( result );
}
int cs_empty( const cstring this )
{
	ResultCode result = Result_Ok;
	int retVal = error;

	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		retVal = cs_length( this ) > 0 ? false : true;
	}

	err_set_result( result );
	return retVal;
}
void cs_clear( cstring this )
{
	ResultCode result = Result_Ok;
	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		while( cs_empty( this ) == false )
		{
			if( ( result = err_get_result() ) != Result_Ok )
			{
				break;
			}
			cs_pop_back( this );
		}
	}

	err_set_result( result );
}
void cs_fill( cstring this, const size_t size, const char c )
{
	ResultCode result = Result_Ok;

	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		void* dst = ( void* )this._string->buffer;
		const size_t setsize = size <= cs_length( this ) ? size : cs_length( this );

		memset( dst, c, setsize );
	}

	err_set_result( result );
}
size_t cs_find( const cstring this, size_t offset, const char c )
{
	size_t findIdx = -1;
	ResultCode result = Result_Ok;

	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		for( size_t i = offset; i < cs_length( this ); ++i )
		{
			if( cs_get( this, i ) == c )
			{
				findIdx = i;
				break;
			}
		}
	}

	err_set_result( result );
	return findIdx;
}
size_t cs_find_string( const cstring this, size_t offset, const char* str )
{
	ResultCode result = Result_Ok;
	size_t retVal = -1;

	if( cs_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else if( str == nullptr )
	{
		result = Result_Null_Parameter;
	}
	else
	{
		const size_t srcLen = strlen( str );
		size_t firstMatch = cs_find( this, offset, *str );

		if( firstMatch != -1 )
		{			
			for( size_t i = firstMatch, j = 0; i < cs_length( this ) && j < srcLen; ++i, ++j )
			{
				if( cs_get( this, i ) != str[ j ] )
				{
					retVal = i;
					break;
				}
			}
		}
	}	

	err_set_result( result );
	return retVal;
}
