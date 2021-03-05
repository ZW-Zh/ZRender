#pragma once
#ifndef _GLOBAL_
#define _GLOBAL_
template <class T>
inline int getArrayLen(T& array)

{
	return sizeof(array) / sizeof(array[0]);

}

#endif // !_GLOBAL_

