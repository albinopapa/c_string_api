#pragma once

#include "defines.h"
#include "customerror.h"
#include <ctype.h>

typedef struct _iterator _iterator;
typedef struct _container _container;
typedef struct container container;

typedef bool( *default_construct )( void* );
typedef bool( *deep_copy_fn )( const void* src, void* const dest );
typedef void( *destroy )( const void* ptr );
typedef struct iterator iterator;

struct iterator
{
	void( *advance )( iterator this );
	void( *previous )( iterator this );
	void( *move_by )( iterator this, int offset );

	bool( *is_equal )( iterator this, iterator other );
	ptrdiff_t( *distance )( iterator this, iterator other );

	void*( *get )( iterator this );
	void( *set )( iterator this, const void* value );

	_iterator* it;
};

void it_destroy( iterator* this );

struct container
{
	bool( *begin )( container this, iterator* it );
	bool( *end )( container this, iterator* it );

	// getters
	_Bool( *at_get )( const container this, const size_t idx, void* value );
	_Bool( *empty )( const container this );
	size_t( *size )( const container this );
	size_t( *capacity )( const container thsi );
	const void* const( *data )( const container this );

	// utilities
	_Bool( *copy )( const container this, container* other );

	// container properties
	void( *clear )( container this );
	void( *pop_back )( container this );
	_Bool( *push_back )( container this, const void* value );
	_Bool( *reserve )( container this, const size_t size );
	_Bool( *resize )( container this, const size_t size );

	// setter
	_Bool( *at_set )( container this, size_t idx, const void* value );
	_Bool( *insert )( container this, size_t offset, const void* value );

	_container* pdata;
};
bool cont_default_construct( container* this, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor );
bool cont_reserve_construct( container* this, const size_t size, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor );
bool cont_size_construct( container* this, const size_t size, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor );
bool cont_destroy( container* this );


#ifndef swap
#define swap(type,a,b){ type c = (*a);(*a) = (*b);(*b) = c;}
#endif