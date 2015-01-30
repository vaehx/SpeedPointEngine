#include <stdio.h>


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
	unsigned int startsz = 4;


	unsigned int w = 0, d = 8;
	unsigned int sz = startsz;

	unsigned int xacc = CountSideParts(w, startsz);
	unsigned int zacc = CountSideParts(d, startsz);

	printf("========================================\n");
	unsigned int nIndices = (xacc * zacc) * 6;
	unsigned int nVertices = (w * d > 0) ? (xacc + 1) * (zacc + 1) : 0;
	printf("Need generate %dx%d (%d) quads, which means %d indices (6 per quad) and %d vertices\n", xacc, zacc, xacc * zacc, nIndices, nVertices);





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