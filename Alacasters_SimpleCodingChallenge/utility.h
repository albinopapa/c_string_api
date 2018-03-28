#pragma once

#include "defines.h"
#include "customerror.h"
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

typedef struct _container _container;
typedef struct container container;

typedef struct container_func_params
{
	const void* this;
	void* other;
	size_t this_size;
}container_func_params;

typedef bool( *default_construct )( container_func_params params );
typedef bool( *deep_copy_fn )( container_func_params params );
typedef void( *destroy )( container_func_params params );
typedef struct iterator iterator;

bool trivially_constructable( container_func_params params );
bool trivially_copyable( container_func_params params );
void trivially_destructable( container_func_params params );

inline bool trivially_constructable( container_func_params params )
{
	if( params.this == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}
	if( params.this_size == 0 )
	{
		err_set_result( Result_Invalid_Parameter );
		return false;
	}

	memset( params.this, 0, params.this_size );

	err_set_result( Result_Ok );
	return true;
}
inline bool trivially_copyable( container_func_params params )
{
	if( params.this == nullptr || params.other == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}
	if( params.this_size == 0 )
	{
		err_set_result( Result_Invalid_Parameter );
		return false;
	}

	memcpy( params.other, params.this, params.this_size );

	return true;
}
inline void trivially_destructable( container_func_params params ){}

struct iterator
{
	iterator( *advance )( iterator this );
	iterator( *previous )( iterator this );
	iterator( *move_by )( iterator this, int offset );

	bool( *is_equal )( iterator this, iterator other );
	ptrdiff_t( *distance )( iterator this, iterator other );

	void*( *get )( iterator this );
	void( *set )( iterator this, const void* value );

	const container* cont;
	char* cur;
};

struct container
{
	iterator( *begin )( const container* this );
	iterator( *end )( const container* this );

	// getters
	_Bool( *at_get )( const container* this, const size_t idx, void* value );
	_Bool( *empty )( const container* this );
	size_t( *size )( const container* this );
	size_t( *capacity )( const container* this );
	const void* const( *data )( const container* this );

	// utilities
	_Bool( *copy )( const container* this, container* other );

	// container properties
	void( *clear )( container* this );
	void( *pop_back )( container* this );
	_Bool( *push_back )( container* this, const void* value );
	_Bool( *reserve )( container* this, const size_t size );
	_Bool( *resize )( container* this, const size_t size );

	// setter
	_Bool( *at_set )( container* this, size_t idx, const void* value );
	_Bool( *insert )( container* this, size_t offset, const void* value );

	_container* pdata;
};

bool cont_default_construct( container* this, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor );
bool cont_reserve_construct( container* this, const size_t size, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor );
bool cont_size_construct( container* this, const size_t size, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor );
bool cont_destroy( container* this );


#ifndef swap
#define swap(type,a,b){ type c = (*a);(*a) = (*b);(*b) = c;}
#endif