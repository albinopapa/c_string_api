#pragma once

typedef enum ResultCode
{
	Result_Ok,
	Result_Bad_Alloc,
	Result_Bad_Pointer,
	Result_Not_Initialized,
	Result_Index_Out_Of_Range,
	Result_Null_Parameter
} ResultCode;


ResultCode err_get_result();
void err_set_result( ResultCode result );

