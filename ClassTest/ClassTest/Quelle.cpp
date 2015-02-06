#include <stdio.h>
#include "ChunkedObjectPool.h"

unsigned int CountSideParts(unsigned int w, unsigned int startsz)
{
	unsigned int sz = startsz;
	unsigned int accumulator = 0;
	while (w > 0)
	{
		while ((int)w - (int)sz < 0)
			sz = (sz > 1) ? (unsigned int)((double)sz * 0.5) : 0;

		if (sz == 0)
			break;

		accumulator++;
		w -= sz;
	}

	return accumulator;
}

void main()
{
	

	RunChunkedObjectPoolTest();
	RunMapObjectPoolTest();


	printf("Finished.");


	// ---------------	
	getchar();	
}



/*


o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|                                               |                                               |
|                                               |                                               |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|                                               |                                               |
|                                               |                                               |
o                                               o                                               o
|                                               |                                               |
|                                               |                                               |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o
|                                               |                                               |
|                                               |                                               |
o                                               o                                               o
|                                               |                                               |
|                                               |                                               |
o                                               o                                               o
|                                               |                                               |
|                                               |                                               |
o                                               o                                               o
|                                               |                                               |
|                                               |                                               |
o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o-----o



*/