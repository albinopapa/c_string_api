#include "utility.h"
#include "memory.h"
#include <string.h>
#include <stdlib.h>

struct _container
{
	char* pBuffer;
	size_t capacity, size, elemSize;
	default_construct constructor;
	deep_copy_fn copy_construct;
	destroy destructor;
};

// Forward declarations for iterator
iterator it_construct( const container *cont, void* addr );
iterator it_advance( iterator this );
iterator it_previous( iterator this );
iterator it_move_by( iterator this, int offset );

bool it_is_equal( iterator this, iterator other );
ptrdiff_t it_distance( iterator this, iterator other );

void* it_get( iterator this );
void it_set( iterator this, const void* value );


// container iterator 
iterator cont_begin( const container* this );
iterator cont_end( const container* this );

// getters
_Bool cont_at_get( const container* this, const size_t idx, void* value );
_Bool cont_empty( const container* this );
size_t cont_size( const container* this );
size_t cont_capacity( const container* this );
const void* cont_data( const container* this );

// utilities
_Bool cont_copy( const container* this, container* other );
size_t cont_elem_size( const container* this );
size_t cont_calc_addr( const container* this, const size_t idx );
void* cont_get_element( const container* this, const size_t idx );

// container properties
void cont_clear( container* this );
void cont_pop_back( container* this );
_Bool cont_push_back( container* this, const void* value );
_Bool cont_reserve( container* this, const size_t size );
_Bool cont_resize( container* this, const size_t size );

// setter
_Bool cont_at_set( container* this, size_t idx, const void* value );
_Bool cont_insert( container* this, size_t offset, const void* value );



// Private defintions for iterator
iterator it_construct( const container *cont, void* addr )
{
	iterator it;
	it.advance = it_advance;
	it.distance = it_distance;
	it.get = it_get;
	it.is_equal = it_is_equal;
	it.move_by = it_move_by;
	it.previous = it_previous;
	it.set = it_set;
	it.cur = addr;
	it.cont = cont;

	return it;
}
iterator it_advance( iterator this )
{
	if( this.cur != this.cont->end( this.cont ).cur )
	{
		this.cur += cont_elem_size( this.cont );
	}
	return this;
}
iterator it_previous( iterator this )
{
	if( this.cur != this.cont->begin( this.cont ).cur )
	{
		this.cur -= cont_elem_size( this.cont );
	}
	return this;
}
iterator it_move_by( iterator this, int offset )
{
	const int _offset = ( offset * cont_elem_size( this.cont ) );

	if( this.cur + _offset >= this.cont->begin( this.cont ).cur &&
		this.cur + _offset < this.cont->end( this.cont ).cur )
	{
		this.cur += _offset;
	}
	return this;
}

bool it_is_equal( iterator this, iterator other )
{
	return this.cur == other.cur;
}
ptrdiff_t it_distance( iterator this, iterator other )
{
	return ( other.cur - this.cur ) / cont_elem_size( this.cont );
}

void* it_get( iterator this )
{
	return ( void* )this.cur;
}
void it_set( iterator this, const void* value )
{
	if( this.cur != this.cont->end( this.cont ).cur )
	{
		this.cont->pdata->copy_construct( value, it_get( this ), cont_elem_size( this.cont ) );
	}
}



bool cont_default_construct( container* this, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor )
{
	_container* pdata = nullptr;
	bool result = true;
	ResultCode rescode = Result_Ok;

	if( this == nullptr )
	{
		rescode = Result_Null_Parameter;
		result = false;
	}
	if( result )
	{
		pdata = ( _container* )malloc( sizeof( _container ) );
		if( pdata == nullptr )
		{
			rescode = Result_Bad_Alloc;
			result = false;
		}
	}
	if( result )
	{
		pdata->pBuffer = ( char* )malloc( elementSize * 3 );
		if( pdata->pBuffer == nullptr )
		{
			rescode = Result_Bad_Alloc;
			SafeDelete( &pdata );
			result = false;
		}
	}
	if( result )
	{
		pdata->constructor = constructor;
		pdata->destructor = destructor;
		pdata->copy_construct = copy_construct;
		pdata->capacity = 3;
		pdata->elemSize = elementSize;
		pdata->size = 0;

		container self = { 0 };
		self.at_get = cont_at_get;
		self.at_set = cont_at_set;
		self.clear = cont_clear;
		self.copy = cont_copy;
		self.data = cont_data;
		self.empty = cont_empty;
		self.insert = cont_insert;
		self.pop_back = cont_pop_back;
		self.push_back = cont_push_back;
		self.reserve = cont_reserve;
		self.resize = cont_resize;
		self.size = cont_size;
		self.pdata = pdata;
		self.capacity = cont_capacity;
		self.begin = cont_begin;
		self.end = cont_end;

		*this = self;

	}

	err_set_result( rescode );
	return result;
}
bool cont_reserve_construct( container* this, const size_t size, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor )
{
	bool result = true;
	ResultCode rescode = Result_Ok;
	result = cont_default_construct( this, elementSize, constructor, copy_construct, destructor );
	
	if( result )
	{
		result = cont_reserve( this, size );
	}
	if( result == false )
	{
		cont_destroy( this );
		rescode = err_get_result();
		result = false;
	}

	err_set_result( rescode );
	return true;
}
bool cont_size_construct( container* this, const size_t size, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor )
{
	ResultCode rescode = Result_Ok;
	bool result = cont_default_construct( this, elementSize, constructor, copy_construct, destructor );
	if( result )
	{
		result = cont_resize( this, size );
	}
	if( result )
	{
		iterator it = cont_begin( this );
		const iterator end = cont_end( this );

		bool constructed = true;
		for( ; it.is_equal( it, end ) == false && constructed == true; it = it.advance( it ) )
		{
			constructed = this->pdata->constructor( it.get( it ), cont_elem_size( this ) );
		}
		result = constructed;
	}

	if( result == false )
	{
		cont_destroy( this );
		rescode = err_get_result();
	}

	err_set_result( rescode );
	return result;
}
bool cont_destroy( container* this )
{
	if( this == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return true;
	}

	cont_clear( this );

	this->at_get = nullptr;
	this->at_set = nullptr;
	this->clear = nullptr;
	this->copy = nullptr;
	this->data = nullptr;
	this->empty = nullptr;
	this->insert = nullptr;
	this->pop_back = nullptr;
	this->push_back = nullptr;
	this->reserve = nullptr;
	this->resize = nullptr;
	this->size = nullptr;
	this->begin = nullptr;
	this->end = nullptr;
	this->capacity = nullptr;

	SafeDelete( &this->pdata->pBuffer );
	SafeDelete( &this->pdata );

	err_set_result( Result_Ok );
	return true;
}


// container iterators
iterator cont_begin( const container* this )
{
	return it_construct( this, this->pdata->pBuffer );
}
iterator cont_end( const container* this )
{
	return it_construct( this, cont_get_element( this, cont_size( this ) ) );
}

// getters
_Bool cont_at_get( const container* this, const size_t idx, void* value )
{
	if( idx > cont_size( this ) )
	{
		err_set_result( Result_Index_Out_Of_Range );
		return false;
	}

	if( value == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}

	iterator it = cont_begin( this );
	it.move_by( it, idx );
	this->pdata->copy_construct( it.get( it ), value, cont_elem_size( this ) );

	err_set_result( Result_Ok );
	return true;
}
_Bool cont_empty( const container* this )
{
	return cont_size( this ) == 0;
}
size_t cont_size( const container* this )
{
	return this->pdata->size;
}
size_t cont_capacity( const container* this )
{
	return this->pdata->capacity;
}
const void* cont_data( const container* this )
{
	return this->pdata->pBuffer;
}

// utilities
_Bool cont_copy( const container* this, container* other )
{
	if( other == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}
	if( other->at_get != nullptr )
	{
		cont_destroy( other );
	}

	container out = { 0 };
	if( cont_size_construct(
		&out,
		cont_size( this ),
		cont_elem_size( this ),
		this->pdata->constructor,
		this->pdata->copy_construct,
		this->pdata->destructor ) == false )
	{
		return false;
	}

	for( iterator this_it = cont_begin( this ),
		copy_it = cont_begin( &out ),
		end_it = cont_end( this );
		this_it.is_equal( this_it, end_it ) == false;
		this_it = this_it.advance( this_it ),
		copy_it = copy_it.advance( copy_it ) )
	{
		copy_it.set( copy_it, this_it.get( this_it ) );
	}

	*other = out;

	err_set_result( Result_Ok );
	return true;
}
size_t cont_elem_size( const container* this )
{
	return this->pdata->elemSize;
}
size_t cont_calc_addr( const container* this, const size_t idx )
{
	return idx * cont_elem_size( this );
}
void* cont_get_element( const container* this, const size_t idx )
{
	return &this->pdata->pBuffer[ cont_calc_addr( this, idx ) ];
}

// container properties
void cont_clear( container* this )
{
	while( cont_empty( this ) == false )
	{
		cont_pop_back( this );
	}
}
void cont_pop_back( container* this )
{
	if( cont_empty( this ) == true )
	{
		return;
	}

	iterator it = cont_end( this );
	it.previous( it );

	this->pdata->destructor( it.get( it ), cont_elem_size( this ) );

	--this->pdata->size;
}
_Bool cont_push_back( container* this, const void* value )
{
	if( cont_size( this ) >= cont_capacity( this ) )
	{
		const size_t newSize = ( ( cont_size( this ) + 1 ) * 3 ) / 2;
		if( cont_reserve( this, newSize ) == false )
		{
			return false;
		}
	}

	iterator it = cont_end( this );

	const size_t elemSize = cont_elem_size( this );
	//this->pdata->constructor( it.get(it), elemSize );
	this->pdata->copy_construct( value, it.get( it ), elemSize );

	++this->pdata->size;

	err_set_result( Result_Ok );
	return true;
}
_Bool cont_reserve( container* this, const size_t size )
{
	if( this->pdata->capacity >= size )
	{
		err_set_result( Result_Ok );
		return true;
	}

	const size_t newSize = size * this->pdata->elemSize;
	char* pBuffer = ( char* )malloc( newSize );

	if( pBuffer == nullptr )
	{
		err_set_result( Result_Bad_Alloc );
		return false;
	}

	const iterator beg = cont_begin( this );
	const iterator end = cont_end( this );

	for( iterator it = beg; it.is_equal( it, end ) == false; it = it.advance( it ) )
	{
		const size_t diff = it.distance( beg, it );
		const size_t idx = cont_calc_addr( this, diff );
		this->pdata->copy_construct( it.get( it ), &pBuffer[ idx ], cont_elem_size( this ) );
	}

	SafeDelete( &this->pdata->pBuffer );

	this->pdata->pBuffer = pBuffer;
	this->pdata->capacity = size;

	err_set_result( Result_Ok );
	return true;

}
_Bool cont_resize( container* this, const size_t size )
{
	if( cont_reserve( this, size + 1 ) == true )
	{
		this->pdata->size = size;
		return true;
	}

	return false;
}

// setter
_Bool cont_at_set( container* this, size_t idx, const void* value )
{
	iterator it = cont_begin( this );
	it.move_by( it, idx );
	it.set( it, value );
	return true;
}
_Bool cont_insert( container* this, size_t offset, const void* value )
{
	container temp = { 0 };
	if( cont_size_construct(
		&temp,
		cont_size( this ) + 1,
		cont_elem_size( this ),
		this->pdata->constructor,
		this->pdata->copy_construct,
		this->pdata->destructor
	) == false )
	{
		return false;
	}

	iterator this_it = cont_begin( &this );
	iterator temp_it = cont_begin( &temp );

	iterator end_it = cont_begin( &this );
	end_it = end_it.move_by( end_it, offset );

	for( ; this_it.is_equal( this_it, end_it ) == false;
		this_it = this_it.advance( this_it ), temp_it = temp_it.advance( temp_it ) )
	{
		if( this->pdata->copy_construct(
			this_it.get( this_it ),
			temp_it.get( temp_it ),
			cont_elem_size( this ) ) == false )
		{
			cont_destroy( &temp );
			return false;
		}
	}

	temp_it.set( temp_it, value );
	temp_it = temp_it.advance( temp_it );

	end_it = this->end( &this );

	for( ; this_it.is_equal( this_it, end_it ) == false;
		this_it = this_it.advance( this_it ), temp_it = temp_it.advance( temp_it ) )
	{
		if( this->pdata->copy_construct(
			this_it.get( this_it ),
			temp_it.get( temp_it ),
			cont_elem_size( this ) ) == false )
		{
			cont_destroy( &temp );
			return false;
		}
	}

	SafeDelete( &this->pdata->pBuffer );
	*this->pdata = *temp.pdata;
	SafeDelete( &temp.pdata );

	err_set_result( Result_Ok );
	return true;
}
