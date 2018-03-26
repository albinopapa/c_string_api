#pragma once

#ifndef SWAP
#define swap(type,a,b){ type c = (*a);(*a) = (*b);(*b) = c;}
#endif