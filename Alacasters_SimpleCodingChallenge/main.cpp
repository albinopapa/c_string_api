#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cstring.h"
#include "defines.h"
#include "memory.h"
#include "stringstream.h"
#include "customerror.h"

#define MAX_LEN 255

// Input
bool GetUserInput( cstring input );

// String utilities
bool WordCounter( const cstring input, size_t* numWords );
bool MaxWordLength( const cstring input, size_t* wordLength );


// Message codec
bool Encode( const cstring input, cstring output, const size_t numColumns, const size_t numRows );
bool Transform( const cstring input, cstring output, const size_t numColumns, const size_t numRows );
void Decode( const cstring input, cstring output );

int main() 
{	
	//take input
	cstring input = { 0 };
	if( cs_default_construct( &input ) == false )
	{
		return -1;
	}

	if( GetUserInput( input ) == false )
	{
		cs_destroy_cstring( &input );
		return -1;
	}

	size_t numColumns = 0, numRows = 0; 
	if( WordCounter( input, &numColumns ) == false )
	{
		cs_destroy_cstring( &input );
		return -1;
	}
	if( MaxWordLength( input, &numRows ) == false )
	{
		cs_destroy_cstring( &input );
		return -1;
	}

	printf( "%c", '\n' );
	cstring transformed = { 0 };
	if( cs_default_construct( &transformed ) == false )
	{
		cs_destroy_cstring( &input );
		return -1;
	}
	if( Transform( input, transformed, numColumns, numRows ) == false )
	{
		cs_destroy_cstring( &input );
		cs_destroy_cstring( &transformed );
		return -1;
	}

	for( size_t j = 0; j < numRows; ++j )
	{
		for( size_t i = 0; i < numColumns; ++i )
		{
			const size_t idx = i + ( j*numColumns );
			const char c = 0;
			if( transformed.at_get( transformed, idx, &c ) == false )
			{
				if( err_get_result() != Result_Ok )
				{
					cs_destroy_cstring( &input );
					cs_destroy_cstring( &transformed );
					return -1;
				}
			}

			printf( "%c", c == 0 ? ' ' : c );
			printf( "%s", ", " );
		}

		printf( "%c", '\n' );
	}

	printf( "%c", '\n' );
	//encode string one into string two without modifying string one
	cstring output = { 0 };
	if( cs_reserve_construct( &output, numColumns * numRows ) == false )
	{
		cs_destroy_cstring( &input );
		cs_destroy_cstring( &transformed );
		return -1;
	}
	if( Encode( transformed, output, numColumns, numRows ) == false )
	{
		if( err_get_result() != Result_Ok )
		{
			cs_destroy_cstring( &input );
			cs_destroy_cstring( &transformed );
			return -1;
		}
	}

	printf( "%s", output.str( output ) );

	cs_destroy_cstring( &output );
	cs_destroy_cstring( &transformed );
	cs_destroy_cstring( &input );

	return 0;
}

bool GetUserInput( cstring input )
{
	char c = 0;
	while( ( c = getchar() ) != '\n' )
	{
		if( input.push_back( input, c ) == false )
		{
			return false;
		}
	}
	
	return true;
}

bool WordCounter( const cstring input, size_t* numWords )
{
	size_t wordCount = 0;
	*numWords = -1;

	stringstream ss = { 0 };
	if( ss_construct( &ss ) == false )
	{
		return false;
	}

	if( ss.insert_cstring( ss, input ) == false )
	{
		ss_destroy( &ss );
		return false;
	}

	while( ss.eof( ss ) == false )
	{
		cstring temp = { 0 };
		if( ss.extract( ss, &temp ) == false )
		{
			ss_destroy( &ss );
			return false;
		}

		if( temp.empty( temp ) == false )
		{
			++wordCount;
		}

		cs_destroy_cstring( &temp );
	}

	*numWords = wordCount;

	ss_destroy( &ss );
	err_set_result( Result_Ok );
	return true;
}

bool MaxWordLength( const cstring input, size_t* wordLength )
{
	size_t curWordLen = 0,maxWordLen = 0;	
	*wordLength = -1;

	stringstream ss = { 0 };
	if( ss_construct( &ss ) == false )
	{
		return false;
	}

	if( ss.insert_cstring( ss, input ) == false )
	{
		ss_destroy( &ss );
		return false;
	}

	do
	{
		cstring temp = { 0 };
		if( ss.extract( ss, &temp ) == false )
		{
			if(err_get_result() != Result_Ok)
			{
				ss_destroy( &ss );
				return false;
			}
		}

		if( ( curWordLen = temp.size( temp ) ) > 0 )
		{
			maxWordLen = curWordLen > maxWordLen ? curWordLen : maxWordLen;
		}

		cs_destroy_cstring( &temp );
	} while( ss.eof( ss ) == false );
	
	ss_destroy( &ss );

	err_set_result( Result_Ok );
	*wordLength = maxWordLen;
	return true;
}

bool Transform( const cstring input, cstring output, const size_t numColumns, const size_t numRows )
{
	size_t wordCounter = 0;

	// Transformation
	stringstream ss = { 0 };
	if( ss_construct( &ss ) == false )
	{
		return false;
	}
	if( ss.insert_cstring( ss, input ) == false )
	{
		ss_destroy( &ss );
		return false;
	}

	cstring temp = { 0 };
	if( ss.extract( ss, &temp ) == false )
	{
		if( err_get_result() != Result_Ok )
		{
			ss_destroy( &ss );
			return false;
		}
	}

	if( output.size( output ) < numColumns * numRows )
	{
		if( output.resize( output, numColumns * numRows ) == false )
		{
			ss_destroy( &ss );
			return false;
		}
	}
	while( temp.empty( temp ) == false )
	{
		const size_t str_len = temp.size( temp );

		for( size_t i = 0; i < str_len; ++i )
		{
			char c = 0;
			temp.at_get( temp, i, &c );

			const size_t idx = wordCounter + ( i * numColumns );
			output.at_set( output, idx, c );
		}

		++wordCounter;

		cs_destroy_cstring( &temp );
		if( ss.extract( ss, &temp ) == false )
		{
			if( err_get_result() != Result_Ok )
			{
				ss_destroy( &ss );
				return false;
			}
		}
	}

	cs_destroy_cstring( &temp );
	ss_destroy( &ss );

	err_set_result( Result_Ok );
	return true;
}

bool Encode( const cstring input, cstring output, const size_t numColumns, const size_t numRows )
{	
	// Output	
	for( size_t j = 0, idx = 0; j < numRows; ++j, ++idx )
	{
		const size_t rowOffset = j * numColumns;
		for( size_t i = 0; i < numColumns; ++i )
		{
			char c = 0;
			if( input.at_get( input, i + rowOffset, &c ) == false )
			{
				return false;
			}
			if( c != '\0' )
			{
				if( output.push_back( output, c ) == false )
				{
					return false;
				}
			}
		}

		if( output.push_back( output, ' ' ) == false )
		{
			return false;
		}
	}

	err_set_result( Result_Ok );
	return true;
}
