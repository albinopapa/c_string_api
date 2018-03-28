#include "utility.h"
#include "memory.h"
#include <string.h>
#include <stdlib.h>

struct _iterator
{
	char* beg, *end, *cur;
	size_t elemSize;
	deep_copy_fn copy_constructor;
};

bool it_construct( iterator* this, const container cont )
{
	if( this == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}

	iterator self = { 0 };
	self.it = ( _iterator* )malloc( sizeof( _iterator ) );
	if( self.it == nullptr )
	{
		err_set_result( Result_Bad_Alloc );
		return false;
	}

	self.it->beg = cont.pdata->pBuffer;
	self.it->end = cont.pdata->pBuffer + ( cont_size( cont ) * cont_elem_size( cont ) );
	self.it->cur = self.it->beg;
	self.it->elemSize = cont_elem_size( cont );
	self.it->copy_constructor = cont.pdata->copy_construct;

	self.advance = it_advance;
	self.distance = it_distance;
	self.get = it_get;
	self.is_equal = it_is_equal;
	self.move_by = it_move_by;
	self.previous = it_previous;

	if( this->advance != nullptr )
	{
		it_destroy( this );
	}

	*this = self;

	err_set_result( Result_Ok );
	return true;
}
void it_destroy( iterator* this )
{
	if( this == nullptr )
	{
		return;
	}

	this->advance = nullptr;
	this->distance = nullptr;
	this->get = nullptr;
	this->is_equal = nullptr;
	this->move_by = nullptr;
	this->previous = nullptr;
	SafeDelete( &this->it );
}
void it_advance( iterator this );
void it_previous( iterator this );
void it_move_by( iterator this, int offset );

bool it_is_equal( iterator this, iterator other );
ptrdiff_t it_distance( iterator this, iterator other );

void* it_get( iterator this );
void it_set( iterator this, const void* value );

void it_advance( iterator this )
{
	if( this.it->cur != this.it->end )
	{
		this.it->cur += this.it->elemSize;
	}
}
void it_previous( iterator this )
{
	if( this.it->cur != this.it->beg )
	{
		this.it->cur -= this.it->elemSize;
	}
}
void it_move_by( iterator this, int offset )
{
	const int _offset = ( offset * this.it->elemSize );
	if( this.it->cur + _offset >= this.it->beg &&
		this.it->cur + _offset < this.it->end )
	{
		this.it->cur += _offset;
	}
}

bool it_is_equal( iterator this, iterator other )
{
	return this.it->cur == other.it->cur;
}
ptrdiff_t it_distance( iterator this, iterator other )
{
	return ( other.it->cur - this.it->cur ) / this.it->elemSize;
}

void* it_get( iterator this )
{
	return ( void* )this.it->cur;
}
void it_set( iterator this, const void* value )
{
	if(this.it->cur != this.it->end)
	{
		this.it->copy_constructor( value, it_get( this ) );
	}
}

struct _container
{
	char* pBuffer;
	size_t capacity, size, elemSize;
	default_construct constructor;
	deep_copy_fn copy_construct;
	destroy destructor;
};

// container iterator 
bool cont_begin( container this, iterator* it );
bool cont_end( container this, iterator* it );

// getters
_Bool cont_at_get( const container this, const size_t idx, void* value );
_Bool cont_empty( const container this );
size_t cont_size( const container this );
size_t cont_capacity( const container this );
const void* cont_data( const container this );

// utilities
_Bool cont_copy( const container this, container* other );
size_t cont_elem_size( const container this );
size_t cont_calc_index( const container this, const size_t idx );
void* cont_get_element( const container this, const size_t idx );

// container properties
void cont_clear( container this );
void cont_pop_back( container this );
_Bool cont_push_back( container this, const void* value );
_Bool cont_reserve( container this, const size_t size );
_Bool cont_resize( container this, const size_t size );

// setter
_Bool cont_at_set( container this, size_t idx, const void* value );
_Bool cont_insert( container this, size_t offset, const void* value );

bool cont_default_construct( container* this, size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor )
{
	if( this == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}

	_container* pdata = ( _container* )malloc( sizeof( _container ) );
	if( pdata == nullptr )
	{
		err_set_result( Result_Bad_Alloc );
		return false;
	}

	pdata->constructor = constructor;
	pdata->destructor = destructor;
	pdata->copy_construct = copy_construct;
	pdata->capacity = 3;
	pdata->elemSize = elementSize;
	pdata->size = 0;
	pdata->pBuffer = ( char* )malloc( elementSize * 3 );
	if( pdata->pBuffer == nullptr )
	{
		SafeDelete( &pdata );
		return false;
	}

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

	*this = self;

	err_set_result( Result_Ok );
	return true;
}
bool cont_reserve_construct( container* this, const size_t size, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor )
{
	if( cont_default_construct( this, elementSize, constructor, copy_construct, destructor ) == false )
	{
		return false;
	}

	if( cont_reserve( *this, size ) == false )
	{
		cont_destroy( this );
		return false;
	}

	err_set_result( Result_Ok );
	return true;
}
bool cont_size_construct( container* this, const size_t size, const size_t elementSize, default_construct constructor, deep_copy_fn copy_construct, destroy destructor )
{
	if( cont_default_construct( this, elementSize, constructor, copy_construct, destructor ) == false )
	{
		return false;
	}

	if( cont_resize( *this, size ) == false )
	{
		cont_destroy( this );
		return false;
	}

	iterator it = { 0 }, end = { 0 };
	cont_begin( *this, &it );
	cont_end( *this, &end );

	bool success = false;
	for( ; it.is_equal( it, end ) == false; it.advance( it ) )
	{
		if( ( success = this->pdata->constructor( it.get( it ) ) ) == false )
		{
			break;
		}
	}

	it_destroy( &it );
	it_destroy( &end );

	if( success == false )
	{
		cont_destroy( this );
		return false;
	}

	err_set_result( Result_Ok );
	return true;
}
bool cont_destroy( container* this )
{
	if( this == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return true;
	}

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

	for( size_t i = 0; i < cont_size( *this ); ++i )
	{
		this->pdata->destructor( cont_get_element( *this, i ) );
	}
	SafeDelete( &this->pdata );

	err_set_result( Result_Ok );
	return true;
}


// container iterators
bool cont_begin( container this, iterator* it )
{
	return it_construct( it, this );
}
bool cont_end( container this, iterator* it )
{
	if( it_construct( it, this ) == false )
	{
		return false;
	}

	it->it->cur = it->it->end;
	return true;
}

// getters
_Bool cont_at_get( const container this, const size_t idx, void* value )
{
	if( idx > cont_size( this ) )
	{
		err_set_result( Result_Index_Out_Of_Range );
		return false;
	}

	if( value != nullptr )
	{
		err_set_result( Result_Invalid_Parameter );
		return false;
	}

	iterator it = { 0 };
	if( cont_begin( this, &it ) == false )
	{
		return false;
	}

	it.move_by( it, idx );
	value = it.get( it );
	it_destroy( &it );

	err_set_result( Result_Ok );
	return true;
}
_Bool cont_empty( const container this )
{
	return cont_size( this ) == 0;
}
size_t cont_size( const container this )
{
	return this.pdata->size;
}
size_t cont_capacity( const container this )
{
	return this.pdata->capacity;
}
const void* cont_data( const container this )
{
	return this.pdata->pBuffer;
}

// utilities
_Bool cont_copy( const container this, container* other )
{
	if( other == nullptr )
	{
		err_set_result( Result_Null_Parameter );
		return false;
	}
	if( other->pdata->elemSize != this.pdata->elemSize )
	{
		err_set_result( Result_Invalid_Parameter );
		return false;
	}

	container out = { 0 };
	if( cont_resize_construct( 
		&out, 
		cont_size( this ), 
		cont_elem_size( this ), 
		this.pdata->constructor,
		this.pdata->copy_construct, 
		this.pdata->destructor ) == false )
	{
		return false;
	}

	{
		iterator this_it = { 0 }, copy_it = { 0 };
		cont_begin( this, &this_it );
		cont_begin( out, &copy_it );

		for( size_t i = 0; i < cont_size( this ); ++i, this_it.advance( this_it ), copy_it.advance( copy_it ) )
		{			
			copy_it.set( copy_it, this_it.get( this_it ) );
			this.pdata->copy_construct( this_it.get( this_it ), copy_it.get( copy_it ) );
		}

		it_destroy( &this_it );
		it_destroy( &copy_it );
	}

	if( cont_empty( *other ) == false )
	{
		cont_destroy( other );
	}

	*other = out;

	err_set_result( Result_Ok );
	return true;
}
size_t cont_elem_size( const container this )
{
	return this.pdata->elemSize;
}
size_t cont_calc_index( const container this, const size_t idx )
{
	return idx * this.pdata->elemSize;
}
void* cont_get_element( const container this, const size_t idx )
{
	return &this.pdata->pBuffer[ cont_calc_index( this, idx ) ];
}
// container properties
void cont_clear( container this )
{
	while( cont_empty( this ) == false )
	{
		cont_pop_back( this );
	}
}
void cont_pop_back( container this )
{
	if( cont_empty( this ) == true )
	{
		return;
	}

	iterator it = { 0 };
	cont_end( this, &it );
	it.previous( it );

	this.pdata->destructor( it.get( it ) );

	it_destroy( &it );
	--this.pdata->size;
}
_Bool cont_push_back( container this, const void* value )
{
	if( cont_size( this ) >= cont_capacity( this ) )
	{
		const size_t newSize = ( ( cont_size( this ) + 1 ) * 3 ) / 2;
		if( cont_reserve( this, newSize ) == false )
		{
			return false;
		}
	}

	iterator it = { 0 };
	cont_end( this, &it );
	
	this.pdata->copy_construct( value, it.get( it ) );

	it_destroy( &it );

	++this.pdata->size;

	err_set_result( Result_Ok );
	return true;
}
_Bool cont_reserve( container this, const size_t size )
{
	if( this.pdata->capacity >= size )
	{
		err_set_result( Result_Ok );
		return true;
	}

	const size_t newSize = size * this.pdata->elemSize;

	container newCont = { 0 };
	if( cont_default_construct(
		&newCont,
		cont_elem_size( this ),
		this.pdata->constructor,
		this.pdata->copy_construct,
		this.pdata->destructor ) == false )
	{
		return false;
	}

	newCont.pdata->pBuffer = ( char* )malloc( newSize );
	if( newCont.pdata->pBuffer == nullptr )
	{
		cont_destroy( &newCont );
		err_set_result( Result_Bad_Alloc );
		return false;
	}

	newCont.pdata->capacity = newSize;

	iterator it = { 0 };
	cont_begin( this, &it );

	for( size_t i = 0; i < cont_size( this ); ++i, it.advance( it ) )
	{
		newCont.push_back( newCont, it.get( it ) );
	}

	SafeDelete( &this.pdata->pBuffer );
	SafeDelete( &this.pdata );
	this = newCont;

	err_set_result( Result_Ok );
	return true;

}
_Bool cont_resize( container this, const size_t size )
{
	if( cs_reserve( this, size + 1 ) == true )
	{
		this.pdata->size = size;
		return true;
	}

	return false;
}

// setter
_Bool cont_at_set( container this, size_t idx, const void* value )
{
	iterator it = { 0 };
	cont_begin( this, &it );
	it.move_by( it, idx );
	it.set( it, value );
	it_destroy( &it );
}
_Bool cont_insert( container this, size_t offset, const void* value )
{
	container temp = { 0 };
	if( cont_size_construct(
		&temp,
		cont_size( this ) + 1,
		cont_elem_size( this ),
		this.pdata->constructor,
		this.pdata->copy_construct,
		this.pdata->destructor
	) == false )
	{
		return false;
	}

	iterator this_it = { 0 }, temp_it = { 0 };
	cont_begin( this, &this_it );
	cont_begin( temp, &temp_it );

	for( size_t i = 0; i < offset; ++i, this_it.advance( this_it ), temp_it.advance( temp_it ) )
	{
		this.pdata->copy_construct( this_it.get( this_it ), temp_it.get( temp_it ) );
	}

	temp_it.set( temp_it, value );
	temp_it.advance( temp_it );

	for( size_t i = offset; i < cont_size( this ); ++i, this_it.advance( this_it ), temp_it.advance( temp_it ) )
	{
		this.pdata->copy_construct( this_it.get( this_it ), temp_it.get( temp_it ) );
	}

	it_destroy( &this_it );
	it_destroy( &temp_it );

	SafeDelete( &this.pdata->pBuffer );
	SafeDelete( &this.pdata );

	this = temp;
}
