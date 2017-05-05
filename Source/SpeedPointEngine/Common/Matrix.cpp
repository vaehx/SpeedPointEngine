#include "Matrix.h"

SP_NMSPACE_BEG

const SMatrix SMatrix::Identity = SMatrix(
	1.0f, 0, 0, 0,
	0, 1.0f, 0, 0,
	0, 0, 1.0f, 0,
	0, 0, 0, 1.0f
);

SP_NMSPACE_END
