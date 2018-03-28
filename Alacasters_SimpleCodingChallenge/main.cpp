#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cstring.h"
#include "customerror.h"
#include "defines.h"
#include "memory.h"
#include "stringstream.h"
#include "utility.h"

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

bool PrintTransformed( const cstring input, const size_t numColumns, const size_t numRows );

bool WordEncrypter()
{	
	cstring input = { 0 };
	cstring transformed = { 0 };
	cstring output = { 0 };
	size_t numColumns = 0;
	size_t numRows = 0;

	bool result = cs_default_construct( &input );

	if( result == true )
	{
		result = GetUserInput( input );
	}
	if( result == true )
	{
		result = WordCounter( input, &numColumns );
	}
	if( result == true )
	{
		result = MaxWordLength( input, &numRows );
	}
	if( result == true )
	{
		result = cs_default_construct( &transformed );
	}
	if( result == true )
	{
		result = Transform( input, transformed, numColumns, numRows );
	}
	if( result == true )
	{
		result = cs_reserve_construct( &output, numColumns * numRows );
	}
	if( result == true )
	{
		result = Encode( transformed, output, numColumns, numRows );
	}
	if( result == true )
	{
		printf( "%c", '\n' );
		result = PrintTransformed( transformed, numColumns, numRows );
	}
	if( result == true )
	{
		printf( "%c", '\n' );
		printf( "%s", output.str( &output ) );
	}

	cs_destroy_cstring( &output );
	cs_destroy_cstring( &transformed );
	cs_destroy_cstring( &input );

	return result;
}



void func()
{
	const size_t numElements = 10;
	container cont_a = { 0 };
	container cont_b = { 0 };
	cstring ins_temp = { 0 };
	char six = 0;

	bool result = cont_size_construct(
		&cont_a, 
		numElements,
		sizeof( cstring ), 
		cs_default_construct,
		cs_copy, 
		cs_destroy_cstring );
	if( result )
	{
		for( size_t i = 0; i < numElements && result == true; ++i )
		{
			if( i != 6 )
			{
				char num[ 2 ];
				_ultoa( i, num, 10 );
				cstring temp = { 0 };

				result = cs_default_construct( &temp );
				if( result )
				{
					result = temp.push_back( &temp, num[ 0 ] );
				}
				if( result )
				{
					result = cont_a.push_back( &cont_a, &temp );
				}

				cs_destroy_cstring( &temp );
			}
		}
	}
	if( result )
	{
		six = '6';
		result = cs_size_construct( &ins_temp, 1, six );
	}
	if( result )
	{
		result = cont_a.insert( &cont_a, 6, &ins_temp );
		cs_destroy_cstring( &ins_temp );
	}
	if( result )
	{
		result = cont_a.copy( &cont_a, &cont_b );
	}
	if( result )
	{
		printf( "%c", '\n' );
		iterator it = cont_a.begin( &cont_a ), end = cont_a.end( &cont_a );
		for( ; it.is_equal( it, end ) == false && result == true; it = it.advance( it ) )
		{
			cstring* temp = ( cstring* )it.get( it );
			printf( "%s", temp->str( temp ) );
		}
	}

	cont_a.clear( &cont_a );

	cont_destroy( &cont_b );
	cont_destroy( &cont_a );
}

int main()
{
	func();
	return 0;
}

bool GetUserInput( cstring input )
{
	char c = 0;
	while( ( c = getchar() ) != '\n' )
	{
		if( input.push_back( &input, c ) == false )
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

		if( temp.empty( &temp ) == false )
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
	err_set_result( Result_Ok );

	size_t curWordLen = 0,maxWordLen = 0;
	*wordLength = -1;
	stringstream ss = { 0 };

	bool result = ss_construct( &ss );
	if( result )
	{
		result = ss.insert_cstring( ss, input );
	}
	if( result )
	{
		do
		{
			cstring temp = { 0 };
			result = ss.extract( ss, &temp );

			if( result == false && err_get_result() == Result_Ok )
			{
				if( ( curWordLen = temp.size( &temp ) ) > 0 )
				{
					maxWordLen = curWordLen > maxWordLen ? curWordLen : maxWordLen;
				}
			}

			cs_destroy_cstring( &temp );
		} while( ss.eof( ss ) == false );
	}
	
	ss_destroy( &ss );

	*wordLength = maxWordLen;
	return true;
}

bool Transform( const cstring input, cstring output, const size_t numColumns, const size_t numRows )
{
	err_set_result( Result_Ok );

	size_t wordCounter = 0;
	stringstream ss = { 0 };
	cstring temp = { 0 };

	bool result = ss_construct( &ss );

	if( result )
	{
		result = ss.insert_cstring( ss, input );
	}
	if( result )
	{
		result = ss.extract( ss, &temp );
	}
	if( result )
	{
		if( output.size( &output ) < numColumns * numRows )
		{
			result = output.resize( &output, numColumns * numRows );
		}
	}
	if( result )
	{
		while( result )
		{
			const size_t str_len = temp.size( &temp );

			for( size_t i = 0; i < str_len; ++i )
			{
				char c = 0;
				temp.at_get( &temp, i, &c );

				const size_t idx = wordCounter + ( i * numColumns );
				output.at_set( &output, idx, c );
			}

			++wordCounter;

			cs_destroy_cstring( &temp );
			result = ss.extract( ss, &temp );
		}
	}

	cs_destroy_cstring( &temp );
	ss_destroy( &ss );

	return result;
}

bool Encode( const cstring input, cstring output, const size_t numColumns, const size_t numRows )
{	
	bool result = true;
	err_set_result( Result_Ok );

	for( size_t j = 0, idx = 0; j < numRows; ++j, ++idx )
	{
		const size_t rowOffset = j * numColumns;
		for( size_t i = 0; i < numColumns; ++i )
		{
			char c = 0;
			if( result )
			{
				result = input.at_get( &input, i + rowOffset, &c ) && result;
			}
			if( result && c != '\0' )
			{
				result = output.push_back( &output, c );
			}
		}
		if( result )
		{
			result = output.push_back( &output, ' ' );
		}
	}

	return true;
}

bool PrintTransformed( const cstring input, const size_t numColumns, const size_t numRows )
{
	bool result = true;
	for( size_t j = 0; j < numRows; ++j )
	{
		for( size_t i = 0; i < numColumns; ++i )
		{
			const size_t idx = i + ( j * numColumns );
			char c = 0;
			input.at_get( &input, idx, &c );

			printf( "%c", c == 0 ? ' ' : c );
			printf( "%s", ", " );
		}

		printf( "%c", '\n' );
	}

	return result;
}