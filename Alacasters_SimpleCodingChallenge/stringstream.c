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

bool ss_isInitialized( stringstream this );

// Private forward declarations
void ss_resize( stringstream this, size_t newSize );
void ss_putchar( stringstream this, const char c );
void ss_insert( stringstream this, const char* str );
void ss_insert_cstring( stringstream this, const cstring str );
char ss_getchar( stringstream this );
cstring ss_extract( stringstream this );
cstring ss_string( stringstream this );
void ss_seek( size_t* ptr, size_t maxSize, int offset, seekpos position );
void ss_seekg( stringstream this, int offset, seekpos position );
void ss_seekp( stringstream this, int offset, seekpos position );
size_t ss_tellg( stringstream this );
size_t ss_tellp( stringstream this );

bool ss_isInitialized( stringstream this )
{
	return ( this.extract == ss_extract && this.stream != nullptr );
}
stringstream ss_construct()
{
	ResultCode result = Result_Ok;

	stringstream this;
	this.extract = ss_extract;
	this.getchar = ss_getchar;
	this.insert = ss_insert;
	this.insert_cstring = ss_insert_cstring;
	this.putchar = ss_putchar;
	this.seekg = ss_seekg;
	this.seekp = ss_seekp;
	this.string = ss_string;
	
	this.stream = ( _sstream* )malloc( sizeof( _sstream ) );
	if( !this.stream )
	{
		result = Result_Bad_Alloc;
	}
	else
	{
		this.stream->alloc_size = 15;
		this.stream->buffer = ( char* )malloc( this.stream->alloc_size );
		this.stream->readPos = 0;
		this.stream->str_size = 0;
		this.stream->writePos = 0;
	}

	err_set_result( result );
	return this;
}
void ss_destroy( stringstream this )
{
	ResultCode result = Result_Ok;
	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		this.stream->alloc_size = 0;
		this.stream->readPos = 0;
		this.stream->str_size = 0;
		this.stream->writePos = 0;

		SafeDelete( &this.stream->buffer );
	}

	err_set_result( result );
}
void ss_resize( stringstream this, size_t newSize )
{
	ResultCode result = Result_Ok;

	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		char* temp = ( char* )malloc( newSize );
		if( temp == nullptr )
		{
			result = Result_Bad_Alloc;
		}
		else
		{
			memset( temp, 0, newSize );
			memcpy( temp, this.stream->buffer, this.stream->str_size );
			SafeDelete( &this.stream->buffer );
			this.stream->buffer = temp;
			this.stream->alloc_size = newSize;
		}
	}

	err_set_result( result );
}
void ss_putchar( stringstream this, const char c )
{
	ResultCode result = Result_Ok;
	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		if( this.stream->str_size >= this.stream->alloc_size )
		{
			ss_resize( this, this.stream->alloc_size * 3 / 2 );
		}
		if( err_get_result() == Result_Ok )
		{
			this.stream->buffer[ this.stream->writePos++ ] = c;
			++this.stream->str_size;
		}
	}

	err_set_result( result );
}
void ss_insert( stringstream this, const char* str )
{
	ResultCode result = Result_Ok;
	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		if( str == nullptr )
		{
			result = Result_Null_Parameter;
		}
		else
		{
			const char *iter = str;
			while( *iter != '\0' )
			{
				ss_putchar( this, *iter );
				if( ( result = err_get_result() ) != Result_Ok )
				{
					break;
				}
			}
		}
	}

	err_set_result( result );
}
void ss_insert_cstring( stringstream this, const cstring str )
{	
	ResultCode result = Result_Ok;
	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		for( size_t i = 0; i < str.size( str ); ++i )
		{
			if( ( result = err_get_result() != Result_Ok ) )
			{
				break;
			}
			ss_putchar( this, str.at( str, i ) );
		}
	}

	err_set_result( result );
}
char ss_getchar( stringstream this )
{
	ResultCode result = Result_Ok;
	char gcResult = -1;

	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		if( this.stream->readPos < this.stream->str_size )
		{
			gcResult = this.stream->buffer[ this.stream->readPos++ ];
		}
	}

	return gcResult;
}
cstring ss_extract( stringstream this )
{
	ResultCode result = Result_Ok;
	cstring output;
	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		if( (result = err_get_result() == Result_Ok))
		{
			output = cs_size_construct( this.stream->str_size );

			for( char c = ss_getchar( this ); c != -1 && !isspace( c ); c = ss_getchar( this ) )
			{
				if( ( result = err_get_result() ) != Result_Ok )
				{
					break;
				}
				output.push_back( output, c );
			}
		}
	}
	
	err_set_result( result );

	return output;
}
cstring ss_string( stringstream this )
{
	ResultCode result = Result_Ok;
	cstring output;
	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		output = cs_size_construct( this.stream->str_size );

		for( size_t i = 0; i < this.stream->str_size; ++i )
		{
			if( ( result = err_get_result() ) != Result_Ok )
			{
				break;
			}
			output.push_back( output, this.stream->buffer[ i ] );
		}
	}

	err_set_result( result );
	return output;
}
void ss_seek( size_t* ptr, size_t maxSize, int offset, seekpos position )
{
	switch( position )
	{
		case SS_SEEK_BEG:
			if( offset <= 0 )
			{
				*ptr = 0;
			}
			else if( offset > (int)maxSize )
			{
				*ptr = maxSize;
			}
			else
			{
				*ptr += offset;
			}
			break;
		case SS_SEEK_CUR:
			if( offset < 0 )
			{
				if( ( int )*ptr + offset < 0 )
				{
					*ptr = 0;
				}
				else
				{
					*ptr += offset;
				}
			}
			else if( offset > 0 )
			{
				if( *ptr + offset > maxSize )
				{
					*ptr = maxSize;
				}
				else
				{
					*ptr += maxSize;
				}
			}
			break;
		case SS_SEEK_END:
			if( offset >= 0 )
			{
				*ptr = maxSize;
			}
			else
			{
				if( ( int )*ptr + offset < 0 )
				{
					*ptr = 0;
				}
				else
				{
					*ptr += offset;
				}
			}
			break;
	}

}
void ss_seekg( stringstream this, int offset, seekpos position )
{
	ResultCode result = Result_Ok;
	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		ss_seek( &this.stream->readPos, this.stream->str_size, offset, position );
	}
	
	err_set_result( result );
}
void ss_seekp( stringstream this, int offset, seekpos position )
{
	ResultCode result = Result_Ok;
	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		ss_seek( &this.stream->writePos, this.stream->str_size, offset, position );
	}

	err_set_result( result );
}
size_t ss_tellg( stringstream this )
{
	ResultCode result = Result_Ok;
	size_t pos = -1;

	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		pos = this.stream->readPos;
	}

	err_set_result( result );

	return pos;
}
size_t ss_tellp( stringstream this )
{
	ResultCode result = Result_Ok;
	size_t pos = -1;

	if( ss_isInitialized( this ) == false )
	{
		result = Result_Not_Initialized;
	}
	else
	{
		pos = this.stream->writePos;
	}

	err_set_result( result );

	return pos;
}
