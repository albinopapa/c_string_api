# c_string_api
Some hopefully helpful utilities for manipulating strings in C.

Written in C using Visual Studio 2017.  

The APIs are cstring and stringstream and are loosely based on the C++ std::string and std::stringstream.

Here's how to use the cstring API.
```c
int main( int argc, char* argv[] )
{
  cstring str = {0};
  _Bool result = cs_default_construct(&str);
  if(result == false)
  {
    // Optionally, you can check the global result code.
    if( err_get_result() != Result_Ok)
      return -1;
  }
  
  result = str.insert_string( str, "Hello, world!" );  
  if(result == false)
  {
    cs_destroy_cstring(&str);
    return -1;
  }
  
  printf( "%s", str.str(str) );
  
  return 0;
}
```
The global result code list is limited:
- Result_Ok,
- Result_Bad_Alloc,
- Result_Bad_Pointer,
- Result_Not_Initialized,
- Result_Index_Out_Of_Range,
- Result_Null_Parameter,
- Result_Invalid_Parameter

These APIs are mostly pass by value with the exception of construct and destroy functions.  See the main.cpp file for a demo of the entire API.
