#include "Mat44.h"

SP_NMSPACE_BEG

const Mat44 Mat44::Identity = Mat44(
	1.0f, 0, 0, 0,
	0, 1.0f, 0, 0,
	0, 0, 1.0f, 0,
	0, 0, 0, 1.0f
);

SP_NMSPACE_END
