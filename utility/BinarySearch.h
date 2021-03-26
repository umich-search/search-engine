#include "Vector.h"
template<typename T>
int binarySearch(vector<T> v,T x)
{
	int start = 0;
	int end = v.size() - 1;
	while(start<=end)
	{
		int mid = (start+end)/2;
		if(v[mid]==x)	
			return mid;
		else if(v[mid]<x)	
			start = mid + 1;
		else	
			end = mid - 1;
	}
	return -1;
}
