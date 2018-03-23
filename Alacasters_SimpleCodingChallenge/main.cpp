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
cstring GetUserInput();

// String utilities
int EndOfString( const char c );
int NewLine( const char c );
size_t SkipWhiteSpace( const cstring input );
cstring GetWord( const cstring input );
size_t WordCounter( const cstring input );
size_t MaxWordLength( const cstring input );


// Message codec
cstring Encode( const cstring input );
void Transform( const cstring input, size_t in_len, char** out_array, size_t column );
void Decode( const cstring input, cstring out_str );

int main() 
{
	ResultCode result = Result_Ok;

	//take input
	cstring input = GetUserInput();	

	if( ( result = err_get_result() ) == Result_Ok )
	{
		//encode string one into string two without modifying string one
		cstring output = Encode( input );
		
		if( ( result = err_get_result() ) == Result_Ok )
		{
			printf( "%s", output.str( output ) );
		}

		cs_destroy_cstring( &output );
	}
	cs_destroy_cstring( &input );

	return 0;
}

cstring GetUserInput()
{
	ResultCode result = Result_Ok;
	cstring input = cs_default_construct();
	
	char c = 0;
	while( ( c = getchar() ) != '\n' )
	{
		if( ( result = err_get_result() ) != Result_Ok )
		{
			break;
		}

		input.push_back( input, c );
	}

	err_set_result( result );
	return input;
}

void Transform( const cstring input, size_t in_len, char** out_array, size_t column )
{
	ResultCode result = Result_Ok;
	if( out_array == nullptr || *out_array == nullptr)
	{
		result = Result_Bad_Pointer;
	}
	else
	{
		for( size_t i = 0; i < in_len; ++i )
		{
			if( ( result = err_get_result() ) != Result_Ok )
			{
				break;
			}
			out_array[ i ][ column ] = input.at( input, i );
		}

		if( ( result = err_get_result() ) == Result_Ok )
		{
			out_array[ in_len ][ column ] = '\0';
		}
	}

	err_set_result( result );
}

cstring Encode( const cstring input )
{
	const size_t wordCount = WordCounter( input );
	const size_t maxWordLen = MaxWordLength( input );
	char** ppBuffer = ( char** )AllocateArray2D( maxWordLen + 1, wordCount );

	size_t wordCounter = 0;
	size_t wordLen = -1;
	stringstream ss;
	ss = ss_construct();
	ss.insert_cstring( ss, input );
	
	// Transformation
	while( wordLen != 0 )
	{
		cstring temp = ss.extract( ss );
		wordLen = temp.size( temp );

		if( wordLen != 0 )
		{
			Transform( temp, temp.size( temp ), ppBuffer, wordCounter );

			++wordCounter;
		}

		cs_destroy_cstring( &temp );
	}
	
	cstring output = cs_default_construct();
	// Output	
	for( size_t j = 0, idx = 0; j < maxWordLen; ++j, ++idx )
	{
		char prev = ' ';
		for( size_t i = 0; i < wordCount; ++i, prev = ppBuffer[ j ][ i ] )
		{
			if( isalpha( ppBuffer[ j ][ i ] ) && !isspace( prev ) )
			{
				output.push_back(output, ppBuffer[ j ][ i ] );
			}			
		}

		output.push_back( output, ' ' );
	}

	// Clean up
	SafeDeleteArray( ppBuffer, wordCount );

	return output;
}

int EndOfString( const char c )
{
	return c == '\0';
}

int NewLine( const char c )
{
	return c == '\n';
}

size_t SkipWhiteSpace( const cstring input )
{
	size_t count = 0;
	while( isspace( input.at( input, count ) ) )
	{
		++count;
	}

	return count;
}

cstring GetWord( const cstring input )
{	
	stringstream ss = ss_construct();
	ss.insert_cstring( ss, input );

	cstring output = ss.extract( ss );

	ss_destroy( ss );

	return output;	
}

size_t MaxWordLength( const cstring input )
{
	size_t curWordLen = 0,maxWordLen = 0;
	size_t pos = 0;

	do
	{
		pos = input.find( input, pos, ' ' ) + 1;
		
		if( pos == -1 ) break;

		cstring temp = input.substr( input, pos, -1 );
		curWordLen = temp.size( temp );

		maxWordLen = curWordLen > maxWordLen ? curWordLen : maxWordLen;
		cs_destroy_cstring( &temp );

	} while( pos != -1 );
	
	return maxWordLen;
}

size_t WordCounter( const cstring input )
{
	ResultCode result = Result_Ok;
	size_t wordCount = 0;

	stringstream ss = ss_construct();
	if( ( result = err_get_result() ) == Result_Ok )
	{
		ss.insert_cstring( ss, input );
		if( ( result = err_get_result() ) == Result_Ok )
		{			
			while( ( result = err_get_result() ) == Result_Ok )
			{
				cstring temp = ss.extract( ss );

				if( temp.size( temp ) > 0 )
				{
					++wordCount;
				}

				cs_destroy_cstring( &temp );
			}

			ss_destroy( ss );
		}
	}

	err_set_result( result );
	return wordCount;
}