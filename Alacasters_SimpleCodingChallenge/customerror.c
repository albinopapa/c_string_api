#include "customerror.h"

ResultCode g_result;

ResultCode err_get_result()
{
	return g_result;
}
void err_set_result( ResultCode result )
{
	g_result = result;
}

