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
	if( this == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}

	_sstream* stream = stream = ( _sstream* )malloc( sizeof( _sstream ) );
	if( stream == nullptr )
	{
		err_set_result( Result_Bad_Alloc );
		return false;
	}

	const size_t alloc_size = 16;
	stream->buffer = ( char* )malloc( alloc_size );
	if( stream->buffer == nullptr )
	{
		free( stream );
		err_set_result( Result_Bad_Alloc );
		return false;
	}

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

	err_set_result( Result_Ok );
	return true;
}
void ss_destroy( stringstream* this )
{
	if( ss_isInitialized( *this ) == false )
	{
		err_set_result( Result_Not_Initialized );
		return;
	}

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

	err_set_result( Result_Ok );
}
bool ss_resize( stringstream this, size_t newSize )
{
	if( newSize < this.stream->alloc_size )
	{
		err_set_result( Result_Ok );
		return true;
	}

	char* buffer = ( char* )malloc( newSize );
	if( buffer == nullptr )
	{
		err_set_result( Result_Bad_Alloc );
		return false;
	}

	memset( buffer, 0, newSize );
	memcpy( buffer, this.stream->buffer, this.stream->str_size );
	SafeDelete( &this.stream->buffer );
	this.stream->buffer = buffer;
	this.stream->alloc_size = newSize;

	err_set_result( Result_Ok );
	return true;
}
bool ss_putchar( stringstream this, const char c )
{
	if( this.stream->str_size >= this.stream->alloc_size )
	{
		if( ss_resize( this, this.stream->alloc_size * 3 / 2 ) == false )
		{
			return false;
		}
	}

	this.stream->buffer[ this.stream->writePos++ ] = c;
	++this.stream->str_size;

	err_set_result( Result_Ok );
	return true;
}
bool ss_insert( stringstream this, const char* str )
{
	if( str == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}

	const char *iter = str;
	while( *iter != '\0' )
	{
		if( ss_putchar( this, *iter ) == false )
		{
			return false;
		}
	}

	err_set_result( Result_Ok );
	return true;
}
bool ss_insert_cstring( stringstream this, const cstring str )
{
	for( size_t i = 0; i < str.size( str ); ++i )
	{
		char c = 0;
		if( str.at_get( str, i, &c ) == false )
		{
			return false;
		}
		if( ss_putchar( this, c ) == false )
		{
			return false;
		}
	}

	err_set_result( Result_Ok );
	return true;
}
bool ss_getchar( stringstream this, char* pc )
{
	if( pc == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}

	if( ss_eof( this ) == true )
	{
		err_set_result( Result_Ok );
		return false;
	}

	*pc = this.stream->buffer[ this.stream->readPos++ ];

	err_set_result( Result_Ok );
	return true;
}
bool ss_extract( stringstream this, cstring* output )
{
	if( output == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}

	cstring out = { 0 };
	if( cs_reserve_construct( &out, this.stream->str_size ) == false )
	{
		return false;
	}

	for( char c = 0; c != -1 && !isspace( c ); )
	{
		if( ss_getchar( this, &c ) == false )
		{
			if( err_get_result() != Result_Ok )
			{
				cs_destroy_cstring( &out );
				return false;
			}
			break;
		}
		if( out.push_back( out, c ) == false )
		{
			cs_destroy_cstring( &out );
			return false;
		}
	}

	if(output->at_get != nullptr)	
	{
		cs_destroy_cstring( output );
	}
	*output = out;

	err_set_result( Result_Ok );
	return true;
}
bool ss_string( stringstream this, cstring* output )
{
	if( output == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}

	cstring out = { 0 };
	if( cs_reserve_construct( &out, this.stream->str_size ) == false )
	{
		return false;
	}

	for( size_t i = 0; i < this.stream->str_size; ++i )
	{
		if( out.push_back( out, this.stream->buffer[ i ] ) == false )
		{
			cs_destroy_cstring( &out );
			return false;
		}
	}

	if( output->empty( *output ) == false )
	{
		cs_destroy_cstring( output );
	}

	*output = out;
	err_set_result( Result_Ok );
	return true;
}
bool ss_seek( size_t* ptr, size_t maxSize, int offset, seekpos position )
{
	if( position == SS_SEEK_BEG )
	{
		if( offset >= 0 && ( size_t )offset + *ptr < maxSize )
		{
			*ptr += ( size_t )offset;
			err_set_result( Result_Ok );
			return true;
		}
	}
	else if( position == SS_SEEK_CUR )
	{
		if( ( int )*ptr + offset >= 0 && ( int )*ptr + offset < maxSize )
		{
			( int )*ptr += offset;
			err_set_result( Result_Ok );
			return true;
		}
	}
	else
	{
		if( offset < 0 && ( int )*ptr + offset >= 0 )
		{
			( int )*ptr += offset;
			err_set_result( Result_Ok );
			return true;
		}
	}

	err_set_result( Result_Invalid_Parameter );
	return false;
}
bool ss_seekg( stringstream this, int offset, seekpos position )
{
	return ss_seek( &this.stream->readPos, this.stream->str_size, offset, position );
}
bool ss_seekp( stringstream this, int offset, seekpos position )
{
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
	return this.stream->readPos >= this.stream->str_size;
}
bool ss_isInitialized( stringstream this )
{
	return ( this.extract == ss_extract && this.stream != nullptr );
}
