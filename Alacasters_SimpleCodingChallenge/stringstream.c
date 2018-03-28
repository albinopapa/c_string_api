#include "cstring.h"
#include "customerror.h"
#include "defines.h"
#include "memory.h"
#include "stringstream.h"
#include <stdlib.h>
#include <string.h>

typedef struct _sstream
{
	size_t readPos, writePos, alloc_size, str_size;
	char* buffer;
}_sstream;

// Private forward declarations
bool ss_putchar( stringstream this, const char c );
bool ss_insert( stringstream this, const char* str );
bool ss_insert_cstring( stringstream this, const cstring str );
bool ss_eof( const stringstream this );

bool ss_getchar( stringstream this, char* pc );
bool ss_extract( stringstream this, cstring* output );
bool ss_string( stringstream this, cstring* output );

bool ss_seek( size_t* ptr, size_t maxSize, int offset, seekpos position );
bool ss_seekg( stringstream this, int offset, seekpos position );
bool ss_seekp( stringstream this, int offset, seekpos position );
size_t ss_tellg( stringstream this );
size_t ss_tellp( stringstream this );

bool ss_resize( stringstream this, size_t newSize );
bool ss_isInitialized( stringstream this );

bool ss_construct( stringstream* this )
{
	err_set_result( Result_Ok );
	bool result = true;
	_sstream* stream = nullptr;
	const size_t alloc_size = 16;

	if( this == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		result = false;
	}
	if( result )
	{
		stream = stream = ( _sstream* )malloc( sizeof( _sstream ) );
		if( stream == nullptr )
		{
			err_set_result( Result_Bad_Alloc );
			result = false;
		}
	}
	if( result )
	{
		stream->buffer = ( char* )malloc( alloc_size );
		if( stream->buffer == nullptr )
		{
			free( stream );
			err_set_result( Result_Bad_Alloc );
			result = false;
		}
	}
	if( result )
	{
		stream->alloc_size = alloc_size;
		stream->readPos = 0;
		stream->str_size = 0;
		stream->writePos = 0;

		stringstream self;
		self.extract = ss_extract;
		self.getchar = ss_getchar;
		self.insert = ss_insert;
		self.insert_cstring = ss_insert_cstring;
		self.putchar = ss_putchar;
		self.tellg = ss_tellg;
		self.tellp = ss_tellp;
		self.seekg = ss_seekg;
		self.seekp = ss_seekp;
		self.string = ss_string;
		self.eof = ss_eof;
		self.stream = stream;

		*this = self;
	}

	return result;
}
void ss_destroy( stringstream* this )
{
	err_set_result( Result_Ok );

	bool result = true;

	if( ss_isInitialized( *this ) == false )
	{
		err_set_result( Result_Not_Initialized );
		result = false;
	}
	if( result )
	{
		this->extract = nullptr;
		this->getchar = nullptr;
		this->insert = nullptr;
		this->insert_cstring = nullptr;
		this->putchar = nullptr;
		this->seekg = nullptr;
		this->seekp = nullptr;
		this->string = nullptr;
		this->tellg = nullptr;
		this->tellp = nullptr;
		this->eof = nullptr;

		this->stream->alloc_size = 0;
		this->stream->readPos = 0;
		this->stream->str_size = 0;
		this->stream->writePos = 0;

		SafeDelete( &this->stream->buffer );
	}
}
bool ss_resize( stringstream this, size_t newSize )
{
	err_set_result( Result_Ok );
	char* buffer = nullptr;
	bool result = newSize >= this.stream->alloc_size;

	if( result )
	{
		buffer = ( char* )malloc( newSize );
		if( buffer == nullptr )
		{
			err_set_result( Result_Bad_Alloc );
			result = false;
		}
	}
	if( result )
	{
		memset( buffer, 0, newSize );
		memcpy( buffer, this.stream->buffer, this.stream->str_size );
		SafeDelete( &this.stream->buffer );
		this.stream->buffer = buffer;
		this.stream->alloc_size = newSize;
	}
	
	return result;
}
bool ss_putchar( stringstream this, const char c )
{
	err_set_result( Result_Ok );

	bool result = this.stream->str_size >= this.stream->alloc_size;

	if( result )
	{
		result = ss_resize( this, this.stream->alloc_size * 3 / 2 );
	}
	if( result )
	{
		this.stream->buffer[ this.stream->writePos++ ] = c;
		++this.stream->str_size;
	}

	return result;
}
bool ss_insert( stringstream this, const char* str )
{
	err_set_result( Result_Ok );
	bool result = true;

	if( str == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		result = false;
	}
	if( result )
	{
		const char *iter = str;
		while( *iter != '\0' && result == true )
		{
			result = ss_putchar( this, *iter );
		}
	}

	return result;
}
bool ss_insert_cstring( stringstream this, const cstring str )
{
	err_set_result( Result_Ok );
	bool result = true;

	for( size_t i = 0; i < str.size( &str ) && result == true; ++i )
	{
		char c = 0;
		result = str.at_get( &str, i, &c );
		if( result )
		{
			result = ss_putchar( this, c );
		}
	}

	return result;
}
bool ss_getchar( stringstream this, char* pc )
{
	err_set_result( Result_Ok );

	bool result = true;
	if( pc == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		result = false;
	}
	if( result )
	{
		result = ss_eof( this );
	}
	if( result == false && err_get_result() == Result_Ok )
	{
		*pc = this.stream->buffer[ this.stream->readPos++ ];
	}

	return result;
}
bool ss_extract( stringstream this, cstring* output )
{
	err_set_result( Result_Ok );

	cstring out = { 0 };
	bool result = true;

	if( output == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		result = false;
	}
	if( result )
	{
		result = cs_reserve_construct( &out, this.stream->str_size );
	}
	if( result )
	{
		for( char c = 0; c != -1 && !isspace( c ) && result == true; )
		{
			result = ss_getchar( this, &c );
			if( result )
			{
				result = out.push_back( &out, c );
			}
		}
	}

	// ss_getchar can return false if a char was not retrieved like at the end of stream
	// but still set result code to Result_Ok since this is not an error.
	// In order to not confuse other error checking, 
	// result is set to true again if this is the case
	result = ( err_get_result() == Result_Ok );

	if( result )
	{
		if( output->at_get != nullptr )
		{
			cs_destroy_cstring( output );
		}
		*output = out;
	}

	return result;
}
bool ss_string( stringstream this, cstring* output )
{
	err_set_result( Result_Ok );
	bool result = true;
	cstring out = { 0 };

	if( output == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		result = false;
	}
	if( result )
	{
		result = cs_reserve_construct( &out, this.stream->str_size );
	}
	if( result )
	{
		for( size_t i = 0; i < this.stream->str_size && result == true; ++i )
		{
			result = out.push_back( &out, this.stream->buffer[ i ] );
		}
	}
	if( result )
	{
		if( output->empty( output ) == false )
		{
			cs_destroy_cstring( output );
		}
		*output = out;
	}

	return result;
}
bool ss_seek( size_t* ptr, size_t maxSize, int offset, seekpos position )
{
	err_set_result( Result_Invalid_Parameter );
	bool result = false;

	const int newvalue = ( int )*ptr + offset;
	if( position == SS_SEEK_BEG )
	{
		if( offset >= 0 && newvalue < ( int )maxSize )
		{
			result = true;
		}
	}
	else if( position == SS_SEEK_CUR )
	{
		if( newvalue >= 0 && newvalue < (int)maxSize )
		{
			result = true;
		}
	}
	else
	{
		if( offset < 0 && newvalue >= 0 )
		{
			result = true;
		}
	}

	if( result )
	{
		err_set_result( Result_Ok );
		( int )*ptr = newvalue;
	}

	return result;
}
bool ss_seekg( stringstream this, int offset, seekpos position )
{
	err_set_result( Result_Ok );
	return ss_seek( &this.stream->readPos, this.stream->str_size, offset, position );
}
bool ss_seekp( stringstream this, int offset, seekpos position )
{
	err_set_result( Result_Ok );
	return ss_seek( &this.stream->writePos, this.stream->str_size, offset, position );
}
size_t ss_tellg( stringstream this )
{
	err_set_result( Result_Ok );
	return this.stream->readPos;
}
size_t ss_tellp( stringstream this )
{
	err_set_result( Result_Ok );
	return this.stream->writePos;
}
bool ss_eof( const stringstream this )
{
	err_set_result( Result_Ok );
	return this.stream->readPos >= this.stream->str_size;
}
bool ss_isInitialized( stringstream this )
{
	err_set_result( Result_Ok );
	return ( this.extract == ss_extract && this.stream != nullptr );
}
