///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2018 Pascal Rosenkranz, All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QHull.h"
#include <set>
#include <queue>

using std::set;
using std::queue;

SP_NMSPACE_BEG

inline int modi(int a, int b)
{
	return (a % b + b) % b;
}

#ifdef QHULL_DEBUG
void VisualizeTriangle(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, const SColor& color = SColor::Turqouise())
{
	Helpers::Register(VisualizeLine(p1, p2, color));
	Helpers::Register(VisualizeLine(p1, p3, color));
	Helpers::Register(VisualizeLine(p2, p3, color));
}
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INVALID_ID UINT_MAX

struct QH2Face
{
	static unsigned int idCtr;

	unsigned int id; // assigned to points - if INVALID_ID, this face must be ignored
	unsigned int p[3]; // points of this face, indices to source poly point array
	Vec3f n; // normal of this face
	unsigned int neighbor[3]; // index into faces stack pointing to the neighbors of this face

	QH2Face()
		: id(INVALID_ID)
	{
		for (int i = 0; i < 3; ++i)
		{
			p[i] = INVALID_ID;
			neighbor[i] = INVALID_ID;
		}
	}
};

struct QH2HorizonEdge
{
	unsigned int face; // existing face index
	unsigned int neighbor; // neighbor of the existing face that was removed (idx into face->neighbor)
	unsigned int p[2];

	QH2HorizonEdge(unsigned int _face, unsigned int _neighbor, unsigned int p1, unsigned int p2)
		: face(_face), neighbor(_neighbor)
	{
		p[0] = p1;
		p[1] = p2;
	}

	QH2HorizonEdge()
		: QH2HorizonEdge(INVALID_ID, INVALID_ID, INVALID_ID, INVALID_ID)
	{
	}
};

unsigned int QH2Face::idCtr = 0;




#ifdef QHULL_DEBUG
void VisualizeFace2(const QH2Face& face, const Vec3f* poly, const SColor& color = SColor::Turqouise(), bool inset = false)
{
	Vec3f pt[] = { poly[face.p[0]], poly[face.p[1]], poly[face.p[2]] };

	// Inset points to center a little bit
	if (inset) {
		Vec3f c = (pt[0] + pt[1] + pt[2]) / 3.0f;
		for (int i = 0; i < 3; ++i)
			pt[i] += (c - pt[i]) * 0.02f;
	}

	VisualizeTriangle(pt[0], pt[1], pt[2], color);

	//Vec3f p[3];
	//for (int j = 0; j < 3; ++j)
	//	p[j] = points[face->ipt[j]];

	//Vec3f c = (p[0] + p[1] + p[2]) / 3.f;

	//for (int i = 0; i < 3; ++i)
	//	VisualizeVector(c, p[i] - c, color);
}

void VisualizeFaceNormal2(const QH2Face& face, const Vec3f* poly, const SColor& color = SColor::Turqouise(), float length = 1.0f)
{
	Vec3f pt[] = { poly[face.p[0]], poly[face.p[1]], poly[face.p[2]] };
	Vec3f c = (pt[0] + pt[1] + pt[2]) / 3.0f;
	VisualizeVector(c, face.n * length, color);
}

void VisualizeFaceCrisscross2(const QH2Face& face, const Vec3f* poly, const SColor& color = SColor::Red())
{
	Vec3f p[3];
	for (int j = 0; j < 3; ++j)
		p[j] = poly[face.p[j]];

	Vec3f c = (p[0] + p[1] + p[2]) / 3.f;
	for (int i = 0; i < 3; ++i)
		VisualizeVector(c, p[i] - c, color);
}

void VisualizeAllFaceNormals2(const Vec3f* poly, const vector<QH2Face>& faces, const SColor& color = SColor::White(), float length = 1.0f)
{
	for (auto face = faces.begin(); face != faces.end(); ++face)
		if (face->id != INVALID_ID)
			VisualizeFaceNormal2(*face, poly, color, length);
}

void VisualizeEdge2(const Vec3f* poly, const QH2HorizonEdge& edge, const SColor& color = SColor::Turqouise())
{
	VisualizeCylinder(poly[edge.p[0]], poly[edge.p[1]], 0.01f, color);
}

void VisualizeFaceIds(const Vec3f* poly, const unsigned int* faceIds, unsigned int npolyverts, const vector<QH2Face>& faces)
{
	for (auto& face : faces)
	{
		if (face.id == INVALID_ID)
			continue;

		SColor color;
		color.r = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 100.f)) * 0.01f;
		color.g = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 100.f)) * 0.01f;
		color.b = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 100.f)) * 0.01f;

		VisualizeFace2(face, poly, color, true);
		for (unsigned int i = 0; i < npolyverts; ++i)
		{
			if (faceIds[i] == face.id)
				VisualizePoint(poly[i], color);
		}
	}
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unsigned int _qh2_DebugFace = 0;
unsigned int _qh2_iFace = 0; // 0 = first simplex only

bool _qh2debug()
{
	return _qh2_iFace + 1 == _qh2_DebugFace;
}
void _qh2debug_set_inspect_face(unsigned int face) { _qh2_DebugFace = face; }
unsigned int _qh2debug_get_inspect_face() { return _qh2_DebugFace; }
void _qh2debug_reset()
{
	_qh2_iFace = 0;
}
void _qh2debug_step()
{
	_qh2_iFace++;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#endif



void QuickHull2_CreateFirstSimplex(
	const Vec3f* poly,
	const unsigned int npolyverts,
	unsigned int* faceIds, // assigns faceId to each point in poly
	vector<QH2Face>& faces)
{
	QH2Face face0;

	// Determine extreme points on each axis
	unsigned int ep[3][2];
	unsigned int* epp = (unsigned int*)ep; // flat extreme points array
	ep[0][0] = ep[1][0] = ep[2][0] = INVALID_ID; // min
	ep[0][1] = ep[1][1] = ep[2][1] = INVALID_ID; // max
	for (unsigned int i = 0; i < npolyverts; ++i)
		for (unsigned int j = 0; j < 3; ++j)
		{
			if (ep[j][0] == INVALID_ID || poly[i][j] < poly[ep[j][0]][j]) ep[j][0] = i;
			if (ep[j][1] == INVALID_ID || poly[i][j] > poly[ep[j][1]][j]) ep[j][1] = i;
		}

	// Determine two extreme points farthest away
	float distmax = -FLT_MAX, dist;
	for (int i = 0; i < 6; ++i)
		for (int j = 0; j < 6; ++j)
		{
			dist = (poly[epp[i]] - poly[epp[j]]).LengthSq();
			if (dist > distmax)
			{
				distmax = dist;
				face0.p[0] = epp[i];
				face0.p[1] = epp[j];
			}
		}

	// Determine point farthest away from line through first points
	Vec3f lndir = Vec3Normalize(poly[face0.p[1]] - poly[face0.p[0]]);
	distmax = -FLT_MAX;
	for (int i = 0; i < 6; ++i)
	{
		// Distance to base line from basept[0] to basept[1]
		const Vec3f& p = poly[epp[i]];
		dist = (p - (poly[face0.p[0]] + Vec3Dot(p - poly[face0.p[0]], lndir) * lndir)).LengthSq();
		if (dist > distmax)
		{
			distmax = dist;
			face0.p[2] = epp[i];
		}
	}

	// Determine point farthest away from first face
	// We have to remember on which side the farthest point was, so we
	// can align the face normal so that it points away from that point
	unsigned int farthestPt = INVALID_ID;
	Vec3f facept = poly[face0.p[0]]; // a point on the first face
	Vec3f n = Vec3Normalize((poly[face0.p[1]] - poly[face0.p[0]]) ^ (poly[face0.p[2]] - poly[face0.p[0]]));
	distmax = -FLT_MAX;
	int nsign = 1;
	for (int i = 0; i < npolyverts; ++i)
	{
		dist = Vec3Dot(poly[i] - facept, n);
		if (fabsf(dist) > distmax)
		{
			distmax = fabsf(dist);
			nsign = (dist < 0 ? 1 : -1); // normal must point away from farthestPt
			farthestPt = i;
		}
	}

	// Add first simplex faces
	face0.id = QH2Face::idCtr++;
	face0.n = n * (float)nsign;

	for (int i = 0; i < 3; ++i)
		face0.neighbor[i] = i + 1;

	faces.push_back(face0);

	for (int i = 0; i < 3; ++i)
	{
		QH2Face face;
		face.id = QH2Face::idCtr++;

		face.p[0] = face0.p[i];
		face.p[1] = farthestPt;
		face.p[2] = face0.p[(i + 1) % 3];

		face.neighbor[0] = modi(i - 1, 3) + 1;
		face.neighbor[1] = modi(i + 1, 3) + 1;
		face.neighbor[2] = 0; // face0

		face.n = Vec3Normalize((poly[face.p[1]] - poly[face.p[0]]) ^ (poly[face.p[2]] - poly[face.p[0]]));
		if (Vec3Dot(poly[face0.p[(i + 2) % 3]] - poly[face.p[0]], face.n) >= 0.f)
			face.n *= -1.f;

		faces.push_back(face);
	}

	// Assign points to first faces
	// We can stop if we assigned all points
	unsigned int numAssignedPoints = 0;
	for (auto& face = faces.begin(); face != faces.end() && numAssignedPoints < npolyverts; ++face)
		for (unsigned int i = 0; i < npolyverts && numAssignedPoints < npolyverts; ++i)
		{
			if (faceIds[i] != INVALID_ID)
				continue; // already assigned

			const Vec3f& p = poly[i];
			if (Vec3Dot(p - poly[face->p[0]], face->n) > 0.001f)
			{
				faceIds[i] = face->id;
				numAssignedPoints++;
			}
		}
}

unsigned int QuickHull2_FindFarthestPoint(
	const Vec3f* poly,
	const unsigned int npolyverts,
	const unsigned int* faceIds,
	const QH2Face& face)
{
	unsigned int farthestPt = INVALID_ID;
	float dist, distMax = -FLT_MAX;
	for (unsigned int i = 0; i < npolyverts; ++i)
	{
		if (faceIds[i] != face.id)
			continue;

		const Vec3f& p = poly[i];
		dist = Vec3Dot(p - poly[face.p[0]], face.n);
		if (dist > 0.001f && dist > distMax)
		{
			distMax = dist;
			farthestPt = i;
		}
	}

	return farthestPt;
}

// Returns count of removed faces
// Implements a region-growing algorithm.
// horizonFaces are those faces, that were NOT removed and close to a removed face.
// This method will remove faces and unset ALL neighbors on removed face and neighbor on existing face.
unsigned int QuickHull2_RemoveFacesVisibleFromPoint(
	const Vec3f* poly,
	vector<QH2Face>& faces,
	unsigned int* faceIds,
	unsigned int npolyverts,
	const Vec3f& pt,
	const unsigned int startFace,
	vector<unsigned int>& horizonFaces)
{
	unsigned int startFaceId = faces.at(startFace).id;

	// Find visible faces from that point
	unsigned int numRemoved = 0;
	set<unsigned int> visitedFaces;
	queue<unsigned int> openFaces;
	openFaces.push(startFace);
	while (!openFaces.empty())
	{
		unsigned int iface = openFaces.front();
		QH2Face& face = faces.at(iface);
		openFaces.pop();
		visitedFaces.insert(iface);

		if (Vec3Dot(pt - poly[face.p[0]], face.n) > 0.001f)
		{
			// Unassign points from this face
			// Here, we assign all points of all removed faces to the startFace.
			// Later, these points will be actually unassigned or reassigned to new faces
			for (unsigned int i = 0; i < npolyverts; ++i)
			{
				if (faceIds[i] == face.id)
					faceIds[i] = startFaceId;
			}

			face.id = INVALID_ID;
			numRemoved++;

			// Scan neighbors of this face
			for (int i = 0; i < 3; ++i)
			{
				if (face.neighbor[i] == INVALID_ID)
					continue;

				QH2Face& neighbor = faces.at(face.neighbor[i]);
				for (int j = 0; j < 3; ++j)
					if (neighbor.neighbor[j] == iface)
						neighbor.neighbor[j] = INVALID_ID;

				// Only add those neighbors to queue that we haven't visited yet
				if (visitedFaces.find(face.neighbor[i]) == visitedFaces.end())
					openFaces.push(face.neighbor[i]);

				face.neighbor[i] = INVALID_ID;
			}
		}
		else
		{
			horizonFaces.push_back(iface);
		}
	}

	return numRemoved;
}

// Returns a list of ordered horizon loop points. The edges between those
// points are the edges of the horizon edge loop.
void QuickHull2_DetermineHorizonEdgeLoop(
	const Vec3f* poly,
	const vector<QH2Face>& faces,
	const vector<unsigned int>& horizonFaces,
	vector<QH2HorizonEdge>& horizon)
{
	vector<QH2HorizonEdge> edges; // unsorted
	for (auto itHorizonFace = horizonFaces.begin(); itHorizonFace != horizonFaces.end(); ++itHorizonFace)
	{
		const QH2Face& hf = faces.at(*itHorizonFace);

		// Note: It can't happen that we add the same edge more than once
		for (int i = 0; i < 3; ++i)
		{
			if (hf.neighbor[i] == INVALID_ID)
			{
				QH2HorizonEdge edge;
				edge.face = *itHorizonFace;
				edge.neighbor = i;
				edge.p[0] = hf.p[i];
				edge.p[1] = hf.p[(i + 1) % 3];

#ifdef QHULL_DEBUG
				if (_qh2debug())
				{
					VisualizeEdge2(poly, edge);
				}
#endif

				edges.push_back(edge);
			}
		}
	}

	// Determine sorted loop over edges
	unsigned int start = edges.begin()->p[0], cur = edges.begin()->p[1], from = start;
	horizon.push_back(QH2HorizonEdge(edges.begin()->face, edges.begin()->neighbor, start, cur));
	while (cur != start)
	{
		bool foundNext = false;
		for (auto& he = edges.begin(); he != edges.end() && !foundNext; ++he)
			for (int i = 0; i < 2; ++i)
			{
				if (he->p[i] == cur && he->p[(i + 1) % 2] != from)
				{
					from = cur;
					cur = he->p[(i + 1) % 2];
					horizon.push_back(QH2HorizonEdge(he->face, he->neighbor, from, cur));
					foundNext = true;
					break;
				}
			}

		assert_trace(foundNext, "Could not find next edge in horizon edge loop!");
	}
}

void QuickHull2_CreateNewFaces(
	const Vec3f* poly,
	unsigned int* faceIds,
	unsigned int npolyverts,
	vector<QH2Face>& faces,
	unsigned int extendedFaceIndex,
	unsigned int extendedFaceId,
	const unsigned int iFarthestPt,
	const vector<QH2HorizonEdge>& horizon)
{
	const QH2Face& extendedFace = faces.at(extendedFaceIndex);
	const Vec3f extendedFaceCenter = (poly[extendedFace.p[0]] + poly[extendedFace.p[1]] + poly[extendedFace.p[2]]) / 3.0f;

	unsigned int firstFace = INVALID_ID;
	unsigned int prevFace = INVALID_ID;
	for (auto& he = horizon.begin(); he != horizon.end(); ++he)
	{
		QH2Face newFace;
		unsigned int nextFace = faces.size();

		newFace.id = QH2Face::idCtr++;
		newFace.p[0] = he->p[0];
		newFace.p[1] = he->p[1];
		newFace.p[2] = iFarthestPt;

		newFace.n = Vec3Normalize((poly[newFace.p[1]] - poly[newFace.p[0]]) ^ (poly[newFace.p[2]] - poly[newFace.p[0]]));

		// The normal of the new face must not point into the extended face plane
		// In other words, the centroid of the extended face must be on the backside of all new faces
		if (Vec3Dot(extendedFaceCenter - poly[newFace.p[0]], newFace.n) > 0.001f)
			newFace.n *= -1.0f;

		newFace.neighbor[0] = he->face;
		newFace.neighbor[1] = INVALID_ID; // will be set by the next face
		newFace.neighbor[2] = prevFace;

		faces.at(he->face).neighbor[he->neighbor] = nextFace;

		if (prevFace != INVALID_ID)
			faces.at(prevFace).neighbor[1] = nextFace;

		prevFace = nextFace;
		if (firstFace == INVALID_ID)
			firstFace = nextFace;

		faces.push_back(newFace);

#ifdef QHULL_DEBUG
		SColor debugColor;
		if (_qh2debug())
		{
			debugColor.r = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 100.f)) * 0.01f;
			debugColor.g = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 100.f)) * 0.01f;
			debugColor.b = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 100.f)) * 0.01f;

			//VisualizeFace2(newFace, poly, debugColor, true);
		}
#endif

		// Reassign points to this new face
		for (unsigned int i = 0; i < npolyverts; ++i)
		{
			if (faceIds[i] == extendedFaceId && Vec3Dot(poly[i] - poly[newFace.p[0]], newFace.n) > 0.001f)
			{
				faceIds[i] = newFace.id;
#ifdef QHULL_DEBUG
				if (_qh2debug())
				{
					//VisualizePoint(poly[i], debugColor);
				}
#endif
			}
		}
	}

	faces.at(prevFace).neighbor[1] = firstFace;
	faces.at(firstFace).neighbor[2] = prevFace;

	// Unassign all points that are still assigned to the extended face
	for (unsigned int i = 0; i < npolyverts; ++i)
		if (faceIds[i] == extendedFaceId)
			faceIds[i] = INVALID_ID;
}

void QuickHull2(
	const Vec3f* poly,
	const unsigned int npolyverts,
	Vec3f** phull,
	unsigned int* pnhullverts,
	unsigned int** phullindices/* = 0*/,
	unsigned int* pnfaces/* = 0*/)
{
#ifdef QHULL_DEBUG
	_qh2debug_reset();
#endif

	if (!phull)
		return;

	if (!pnhullverts)
	{
		CLog::Log(S_ERROR, "pnhullverts is an invalid ptr");
		return;
	}

	if (!poly || npolyverts == 0)
	{
		CLog::Log(S_ERROR, "QuickHull2: Invalid polygon given");
		return;
	}

	if (npolyverts == UINT_MAX)
		CLog::Log(S_WARN, "QuickHull2: UINT_MAX polygon verts given - this is likely not correct");


	unsigned int* faceIds = new unsigned int[npolyverts];
	for (unsigned int i = 0; i < npolyverts; ++i)
		faceIds[i] = INVALID_ID;

	vector<QH2Face> faces;

	// Determine and add first simplex
	QuickHull2_CreateFirstSimplex(poly, npolyverts, faceIds, faces);

	// Iterate through faces stack
	vector<unsigned int> horizonFaces;
	vector<QH2HorizonEdge> horizon;
	for (unsigned int iface = 0;; ++iface)
	{
#ifdef QHULL_DEBUG
		if (_qh2_DebugFace == 0)
			break;
#endif

		if (iface >= faces.size())
			break;

#ifdef QHULL_DEBUG
		if (_qh2_iFace + 1  > _qh2_DebugFace)
			break;
#endif

		const QH2Face& face = faces.at(iface);
		unsigned int extendedFaceId = face.id;
		if (face.id == INVALID_ID)
			continue; // face has been removed

					  // Find farthest point in assigned point set
		unsigned int iFarthestPt = QuickHull2_FindFarthestPoint(poly, npolyverts, faceIds, face);
		if (iFarthestPt == INVALID_ID) {
#ifdef QHULL_DEBUG
			if (_qh2debug())
				CLog::Log(S_DEBUG, "No farthest point");

			_qh2debug_step();
#endif
			continue; // this face can't be extended
		}

#ifdef QHULL_DEBUG
		if (_qh2debug())
		{
			VisualizePoint(poly[iFarthestPt], SColor::Purple());
		}
#endif

		// Find visible faces from that point and remove them
		horizonFaces.clear();
		unsigned int numRemoved = QuickHull2_RemoveFacesVisibleFromPoint(poly, faces, faceIds, npolyverts, poly[iFarthestPt], iface, horizonFaces);
		if (horizonFaces.empty()) {
#ifdef QHULL_DEBUG
			if (_qh2debug())
				CLog::Log(S_DEBUG, "horizonFaces is empty() (numRemoved = %d)", numRemoved);

			_qh2debug_step();
#endif
			continue;
		}

		// Determine horizon edge loop of set of removed faces
		horizon.clear();
		QuickHull2_DetermineHorizonEdgeLoop(poly, faces, horizonFaces, horizon);
		assert_trace(!horizon.empty(), "Horizon loop was empty, but we removed %d faces!", numRemoved);

		// Create new faces connecting edges of horizon edge loop with farthest point
		QuickHull2_CreateNewFaces(poly, faceIds, npolyverts, faces, iface, extendedFaceId, iFarthestPt, horizon);

#ifdef QHULL_DEBUG
		if (_qh2debug())
		{
			VisualizeAllFaceNormals2(poly, faces, SColor::White(), 0.1f);
		}

		_qh2debug_step();
#endif
	}


	//	VisualizeFaceIds(poly, faceIds, npolyverts, faces);


	// Convert to output format

	// TODO: Actually reduce vertices down

	unsigned int numHullFaces = 0;
	for (auto& face = faces.begin(); face != faces.end(); ++face)
	{
		if (face->id != INVALID_ID)
			++numHullFaces;
	}

	if (pnfaces)
		*pnfaces = numHullFaces;

	*phull = new Vec3f[(*pnhullverts = numHullFaces * 3)];
	if (phullindices)
		*phullindices = new unsigned int[numHullFaces * 3];

	unsigned int numAddedVerts = 0, numAddedIndices = 0;
	for (auto& face = faces.begin(); face != faces.end(); ++face)
	{
		if (face->id == INVALID_ID)
			continue;

		// Make sure the points are sorted to the face points into the correct direction
		int ipoints[] = { 0, 1, 2 };
		Vec3f tn = Vec3Normalize((poly[face->p[1]] - poly[face->p[0]]) ^ (poly[face->p[2]] - poly[face->p[0]]));
		if (Vec3Dot(tn, face->n) < 0)
		{
			// Reverse points order to get reverse normal
			ipoints[0] = 2;
			ipoints[2] = 0;
		}

		for (int i = 0; i < 3; ++i)
		{
			(*phull)[numAddedVerts] = poly[face->p[ipoints[i]]];
			if (phullindices)
				(*phullindices)[numAddedIndices] = numAddedVerts;
			++numAddedVerts;
			++numAddedIndices;
		}
	}
}


SP_NMSPACE_END
