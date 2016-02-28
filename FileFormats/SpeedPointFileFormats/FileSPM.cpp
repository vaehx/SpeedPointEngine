#include "FileSPM.h"
#include <cstdarg>
#include <Util\SVertex.h>
#include <Abstract\BoundBox.h>
#include <sstream>

SP_NMSPACE_BEG

using std::stringstream;

//////////////////////////////////////////////////////////////////////////////////////////////////////

void PrepareLogFmt(stringstream& ss, const char* fmt, const unsigned int indent)
{
	ss << "[SPMFile] ";
	for (unsigned int i = 0; i < indent; ++i)
		ss << "  ";

	ss << fmt;
}

void CSPMLogged::DebugLog(const char* fmt, ...)
{
	if (!m_bDebug)
		return;

	stringstream ss;
	PrepareLogFmt(ss, fmt, m_DebugIndent);

	va_list args;
	va_start(args, fmt);
	CLog::LogVargs(S_DEBUG, ss.str().c_str(), args);
	va_end(args);
}

void CSPMLogged::ErrorLog(const char* fmt, ...)
{
	stringstream ss;
	PrepareLogFmt(ss, fmt, m_DebugIndent);

	va_list args;
	va_start(args, fmt);
	CLog::LogVargs(S_ERROR, ss.str().c_str(), args);
	va_end(args);
}

void CSPMLogged::IncreaseDbgIndent()
{
	m_DebugIndent++;
}

void CSPMLogged::DecreaseDbgIndent()
{
	if (m_DebugIndent > 0)
		m_DebugIndent--;
}







//////////////////////////////////////////////////////////////////////////////////////////////////////




void CSPMLoader::ReadUShort(u16& u)
{
	if (!m_Stream.is_open()) return;
	m_Stream.read((char*)(&u), 2);
}

void CSPMLoader::ReadUInt(u32& u)
{
	if (!m_Stream.is_open()) return;
	m_Stream.read((char*)(&u), 4);
}

void CSPMLoader::ReadULong(u64& u)
{
	if (!m_Stream.is_open()) return;
	m_Stream.read((char*)(&u), 8);
}

void CSPMLoader::ReadFloat(f32& f)
{
	if (!m_Stream.is_open()) return;
	m_Stream.read((char*)(&f), 4);
}

void CSPMLoader::ReadStringUntilFirstZero(string& s)
{
	if (!m_Stream.is_open())
		return;

	char c;
	while (true)
	{
		m_Stream.read(&c, 1);
		if (c == '\0')
			break;

		s += c;
	}
}

void CSPMLoader::IgnoreBytes(u16 numBytes)
{
	if (!m_Stream.is_open())
		return;

	m_Stream.ignore(numBytes);
}



void CSPMLoader::ReadChunkHeader(u16 &id, u64 &length)
{
	ReadUShort(id);
	ReadULong(length);
}

void CSPMLoader::ReadModelChunk(SModelMeta& modelMeta, const u64& chunkLn)
{
	u64 readLength = 0;

	while (readLength < chunkLn)
	{
		u16 chunkId;
		u64 chunkLength;
		ReadChunkHeader(chunkId, chunkLength);

		switch (chunkId)
		{
		case SPM_CHUNK_MODEL_META:
			DebugLog("Reading SPM_CHUNK_MODEL_META (length = %llu)...", chunkLength);
			IncreaseDbgIndent();

			ReadModelMetaChunk(modelMeta);

			DecreaseDbgIndent();
			break;

		case SPM_CHUNK_VERTICES:
			DebugLog("Reading SPM_CHUNK_VERTICES (length = %llu)...", chunkLength);
			IncreaseDbgIndent();

			ReadVertexChunk(modelMeta);

			DecreaseDbgIndent();
			break;

		case SPM_CHUNK_SUBSET:
			DebugLog("Reading SPM_CHUNK_SUBSET (length = %llu)...", chunkLength);
			IncreaseDbgIndent();

			if (modelMeta.nSubsets == 0 || !IS_VALID_PTR(modelMeta.pSubsets))
			{
				ErrorLog("Failed load subset chunk: nSubsets = 0 or subset array not initialized!");
				break;
			}

			if (modelMeta.nLoadedSubsets == modelMeta.nSubsets)
			{
				ErrorLog("There are more subset-chunks than expected in the file!");
				break;
			}

			ReadSubsetChunk(modelMeta.pSubsets[modelMeta.nLoadedSubsets], chunkLength);
			modelMeta.nLoadedSubsets++;

			DecreaseDbgIndent();
			break;

		default:
			ErrorLog("Invalid chunk type (length=%d): 0x%04X", chunkLength, chunkId);			
			m_Stream.ignore(chunkLength);
			break;
		}

		readLength += chunkLength;
	}
}

void CSPMLoader::ReadModelMetaChunk(SModelMeta& modelMeta)
{
	ReadStringUntilFirstZero(modelMeta.name);

	modelMeta.nLoadedSubsets = 0;
	ReadUShort(modelMeta.nSubsets);

#ifdef _DEBUG
	DebugLog("modelMeta.name = \"%s\"", modelMeta.name.c_str());
	DebugLog("modelMeta.nSubsets = %d", modelMeta.nSubsets);
#endif

	if (modelMeta.nSubsets == 0)
		return;

	// Create the subset array
	modelMeta.pSubsets = new SSubset[modelMeta.nSubsets];
}

void CSPMLoader::ReadVertexChunk(SModelMeta& modelMeta)
{
	ReadUInt(modelMeta.nVertices);
	modelMeta.pVertices = new SVertex[modelMeta.nVertices];

	for (u64 iVtx = 0; iVtx < modelMeta.nVertices; ++iVtx)
	{
		ReadFloat(modelMeta.pVertices[iVtx].x);
		ReadFloat(modelMeta.pVertices[iVtx].y);
		ReadFloat(modelMeta.pVertices[iVtx].z);
		ReadFloat(modelMeta.pVertices[iVtx].nx);
		ReadFloat(modelMeta.pVertices[iVtx].ny);
		ReadFloat(modelMeta.pVertices[iVtx].nz);
		ReadFloat(modelMeta.pVertices[iVtx].textureCoords[0].u);
		ReadFloat(modelMeta.pVertices[iVtx].textureCoords[0].v);
	}

#ifdef _DEBUG
	DebugLog("Read %d vertices", modelMeta.nVertices);
#endif
}

void CSPMLoader::ReadSubsetChunk(SSubset& subset, const u64& chunkLn)
{
	u64 readLength = 0;

	while (readLength < chunkLn)
	{
		u16 chunkId;
		u64 chunkLength;
		ReadChunkHeader(chunkId, chunkLength);
		readLength += chunkLength;

		switch (chunkId)
		{
		case SPM_CHUNK_SUBSET_META:
			DebugLog("Reading SPM_CHUNK_SUBSET_META (length = %llu)...", chunkLength);
			IncreaseDbgIndent();

			ReadStringUntilFirstZero(subset.materialName);

			DebugLog("subset.materialName = \"%s\"", subset.materialName.c_str());
			
			DecreaseDbgIndent();
			break;

		case SPM_CHUNK_SUBSET_INDICES:
			DebugLog("Reading SPM_CHUNK_SUBSET_INDICES (length = %llu)...", chunkLength);
			IncreaseDbgIndent();

			ReadUInt(subset.nIndices);
			DebugLog("subset.nIndices = %u", subset.nIndices);

			subset.pIndices = new u32[subset.nIndices];

			for (u64 iIndex = 0; iIndex < subset.nIndices; ++iIndex)
			{
				ReadUInt(subset.pIndices[iIndex]);
			}

#ifdef _DEBUG
			DebugLog("Read %d indices", subset.nIndices);
#endif

			DecreaseDbgIndent();
			break;

		default:
			ErrorLog("Invalid chunk type (length = %llu): 0x%04X", chunkLength, chunkId);
			m_Stream.ignore(chunkLength);
			break;
		}
	}
}


void CSPMLoader::ReadPhysicsInfoChunk(SPhysicsInfo& physicsInfo, u64 chunkSz)
{
	u16 shapeType;
	ReadUShort(shapeType);
	chunkSz -= 2;
	physicsInfo.shape.shapeType = (ESPMCollisionShapeType)shapeType;

	DebugLog("physicsInfo.shape.shapeType = %d", shapeType);

	for (int i = 0; i < 4; ++i)
	{
		ReadFloat(physicsInfo.shape.v[i].x);
		ReadFloat(physicsInfo.shape.v[i].y);
		ReadFloat(physicsInfo.shape.v[i].z);
	}

	chunkSz -= 4 * 3 * sizeof(float);

	u64 readLength = 0;
	while (readLength < chunkSz)
	{
		if (chunkSz - readLength < SPM_CHUNK_HEADER_SIZE)
		{
			IgnoreBytes(chunkSz - readLength);
			return;
		}

		u16 chunkId;
		u64 chunkLength;
		ReadChunkHeader(chunkId, chunkLength);
		readLength += chunkLength;

		switch (chunkId)
		{
		case SPM_CHUNK_SHAPE_MESH:
		{
			DebugLog("Reading SPM_CHUNK_SHAPE_MESH (0x%04X, length = %llu)...", SPM_CHUNK_SHAPE_MESH, chunkLength);
			IncreaseDbgIndent();

			SSPMCollisionMesh& mesh = physicsInfo.shape.mesh;
			ReadUInt(mesh.nVertices);
			ReadUInt(mesh.nIndices);

			mesh.pVertices = new Vec3f[mesh.nVertices];
			for (u32 iVtx = 0; iVtx < mesh.nVertices; ++iVtx)
			{
				ReadFloat(mesh.pVertices[iVtx].x);
				ReadFloat(mesh.pVertices[iVtx].y);
				ReadFloat(mesh.pVertices[iVtx].z);
			}

			mesh.pIndices = new u32[mesh.nIndices];
			for (u32 iIdx = 0; iIdx < mesh.nIndices; ++iIdx)
			{
				ReadUInt(mesh.pIndices[iIdx]);
			}

			DecreaseDbgIndent();
			break;
		}
		default:
			ErrorLog("Invalid chunk type (length = %llu): 0x%04X", chunkLength, chunkId);
			IgnoreBytes(chunkLength);
			break;
		}
	}
}



bool CSPMLoader::Load(const char* filename, bool debug/*=false*/)
{
	m_bDebug = debug;

	m_Stream.open(filename, ifstream::binary | ifstream::in);
	if (!m_Stream.is_open())
	{
		ErrorLog("Failed load SPM file %s", filename);
		return false;
	}

	if (m_Stream.eof())
	{
		ErrorLog("Empty SPM file %s", filename);
		return false;
	}

	m_Stream.seekg(0, m_Stream.end);
	u64 fileSz = m_Stream.tellg();
	m_Stream.seekg(0, m_Stream.beg);
	
	DebugLog("Reading filename='%s' (fileSz = %llu)...", filename, fileSz);

	// Read the version
	ReadUShort(m_FileVersion);
	DebugLog("Version = %u", m_FileVersion);

	// Read objects
	u64 readLength = sizeof(m_FileVersion);
	while (readLength < fileSz)
	{
		u16 chunkId;
		u64 chunkLength;
		ReadChunkHeader(chunkId, chunkLength);
		readLength += chunkLength;

		if (readLength > fileSz)
		{
			ErrorLog("Incomplete chunk (0x%04X, length = %llu, only %llu bytes there)!", chunkId, chunkLength, chunkLength - (readLength - fileSz));
			continue;
		}

		switch (chunkId)
		{
		case SPM_CHUNK_MODEL:
			DebugLog("Reading SPM_CHUNK_MODEL 0x%04X (length = %llu)...", SPM_CHUNK_MODEL, chunkLength);
			IncreaseDbgIndent();

			m_Models.push_back(SModelMeta());
			ReadModelChunk(m_Models.back(), chunkLength);

			DecreaseDbgIndent();
			break;

		case SPM_CHUNK_PHYSICS_INFO:
			DebugLog("Reading CHUNK_PHYSICS_INFO 0x%04X (length = %llu)...", SPM_CHUNK_PHYSICS_INFO, chunkLength);
			IncreaseDbgIndent();

			ReadPhysicsInfoChunk(m_PhysicsInfo, chunkLength);

			DecreaseDbgIndent();
			break;

		default:
			ErrorLog("Invalid main chunk type (length = %llu): 0x%04X", chunkLength, chunkId);
			break;
		}

		DebugLog("readLength = %llu (0x%04X)", readLength, readLength);
		DebugLog("Remaining bytes: %llu", fileSz - readLength);
	}

	m_Stream.close();

	return true;
}

const vector<SModelMeta>& CSPMLoader::GetModels() const
{
	return m_Models;
}

const SPhysicsInfo& CSPMLoader::GetPhysicsInfo() const
{
	return m_PhysicsInfo;
}




////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////










void CSPMWriter::WriteUShort(u16 u)
{
	m_Stream.write((const char*)&u, sizeof(u));
}

void CSPMWriter::WriteUInt(u32 u)
{
	m_Stream.write((const char*)&u, sizeof(u));
}

void CSPMWriter::WriteULong(u64 u)
{	
	m_Stream.write((const char*)&u, sizeof(u));
}

void CSPMWriter::WriteFloat(f32 f)
{
	//Log("Writing float: %04X (%f)", f, f);
	m_Stream.write((const char*)&f, sizeof(f));
}

// Appends a terminating zero
void CSPMWriter::WriteString(const string& s)
{	
	const char* cs = s.c_str();

	// This algorithm writes string only to the first occurence of \0,
	// otherwise the read would fail.
	unsigned int cnt = 0;
	while (cnt < s.length())
	{
		if (cs[cnt] == '\0')
			break;

		cnt++;
	}

	m_Stream.write(cs, cnt + 1);
	DebugLog("Writing string '%s' (ln = %u)", cs, cnt + 1);
}

void CSPMWriter::WriteChunkHeader(u16 id, u64 length)
{
	WriteUShort(id);
	WriteULong(length);
	DebugLog("Write Chunk Header (id = 0x%04X, length = %llu)", id, length);
}

u64 CSPMWriter::DetermineModelMetaChunkLength(const SModelMeta& modelMeta)
{
	u64 sz = 0;
	sz += modelMeta.name.length() + 1;
	sz += 2; // num_subsets
	return sz;
}

u64 CSPMWriter::DetermineVertexChunkLength(const SModelMeta& modelMeta)
{
	return modelMeta.nVertices * (3 * sizeof(float) + 3 * sizeof(float) + 2 * sizeof(float));	
}

u64 CSPMWriter::DetermineSubsetChunkLength(const SSubset& subset, u64* pSubsetMetaLn /*=0*/, u64* pIndicesLn /*=0*/)
{
	u64 subsetMetaChnkSz = subset.materialName.length() + 1; // +1 for trailing 0
	u64 subsetIndicesChnkSz = subset.nIndices * 4;

	if (pSubsetMetaLn)
		*pSubsetMetaLn = subsetMetaChnkSz;

	if (pIndicesLn)
		*pIndicesLn = subsetIndicesChnkSz;

	return SPM_CHUNK_HEADER_SIZE + subsetMetaChnkSz + SPM_CHUNK_HEADER_SIZE + subsetIndicesChnkSz;
}



void CSPMWriter::WriteModelChunk(const SModelMeta& model)
{
	u64 chunkSize = 0;
	chunkSize += SPM_CHUNK_HEADER_SIZE + DetermineModelMetaChunkLength(model);
	chunkSize += SPM_CHUNK_HEADER_SIZE + DetermineVertexChunkLength(model);

	for (u16 iSubset = 0; iSubset < model.nLoadedSubsets; ++iSubset)
		chunkSize += SPM_CHUNK_HEADER_SIZE + DetermineSubsetChunkLength(model.pSubsets[iSubset]);

	WriteChunkHeader(SPM_CHUNK_MODEL, chunkSize);
	DebugLog("Writing model '%s'  (chunkSize=%llu)", model.name.c_str(), chunkSize);
	IncreaseDbgIndent();

	WriteModelMetaChunk(model);
	WriteVertexChunk(model);

	for (u16 iSubset = 0; iSubset < model.nLoadedSubsets; ++iSubset)
	{		
		WriteSubsetChunk(model.pSubsets[iSubset]);
	}

	DecreaseDbgIndent();
}

void CSPMWriter::WriteModelMetaChunk(const SModelMeta& modelMeta)
{
	u64 chunkSize = DetermineModelMetaChunkLength(modelMeta);

	WriteChunkHeader(SPM_CHUNK_MODEL_META, chunkSize);
	DebugLog("Writing Model-Meta Chunk (SPM_CHUNK_MODEL_META)  chunkSize = %llu...", chunkSize);
	IncreaseDbgIndent();

	DebugLog("model.name = '%s'", modelMeta.name.c_str());
	WriteString(modelMeta.name);

	DebugLog("model.nLoadedSubsets = %d", modelMeta.nLoadedSubsets);
	WriteUShort(modelMeta.nLoadedSubsets);

	DecreaseDbgIndent();
}

void CSPMWriter::WriteSubsetChunk(const SSubset& subset)
{	
	u64 subsetMetaChnkSz, subsetIndicesChnkSz;
	DetermineSubsetChunkLength(subset, &subsetMetaChnkSz, &subsetIndicesChnkSz);
	
	WriteChunkHeader(SPM_CHUNK_SUBSET, subsetMetaChnkSz + subsetIndicesChnkSz);
	DebugLog("Writing CHUNK_SUBSET (chunkSize = %llu)", subsetMetaChnkSz + subsetIndicesChnkSz);
	IncreaseDbgIndent();

	// Meta:	
	WriteChunkHeader(SPM_CHUNK_SUBSET_META, subsetMetaChnkSz);
	DebugLog("Writing CHUNK_SUBSET_META (chunkSize=%llu)", subsetMetaChnkSz);
	IncreaseDbgIndent();

	WriteString(subset.materialName);
	DebugLog("subset.materialName = '%s'", subset.materialName.c_str());

	DecreaseDbgIndent();


	// Indices:
	WriteChunkHeader(SPM_CHUNK_SUBSET_INDICES, subsetIndicesChnkSz);
	DebugLog("Writing CHUNK_SUBSET_INDICES (subsetIndicesChnkSz = %llu)", subsetIndicesChnkSz);
	IncreaseDbgIndent();

	WriteUInt(subset.nIndices);
	DebugLog("subset.nIndices = %lu", subset.nIndices);

	DebugLog("Writing %lu indices...", subset.nIndices);
	for (u64 iIndex = 0; iIndex < subset.nIndices; ++iIndex)
	{
		WriteUInt(subset.pIndices[iIndex]);
	}

	DecreaseDbgIndent();

	DecreaseDbgIndent();
}

void CSPMWriter::WriteVertexChunk(const SModelMeta& modelMeta)
{
	u64 subsetVertsChnkSz = modelMeta.nVertices * (3 * sizeof(float) + 3 * sizeof(float) + 2 * sizeof(float));
	
	WriteChunkHeader(SPM_CHUNK_VERTICES, subsetVertsChnkSz);
	DebugLog("Writing CHUNK_VERTICES (chunkSz = %llu)", subsetVertsChnkSz);
	IncreaseDbgIndent();

	AABB extents;

	WriteUInt(modelMeta.nVertices);
	for (u64 iVert = 0; iVert < modelMeta.nVertices; ++iVert)
	{
		WriteFloat(modelMeta.pVertices[iVert].x);
		WriteFloat(modelMeta.pVertices[iVert].y);
		WriteFloat(modelMeta.pVertices[iVert].z);
		WriteFloat(modelMeta.pVertices[iVert].nx);
		WriteFloat(modelMeta.pVertices[iVert].ny);
		WriteFloat(modelMeta.pVertices[iVert].nz);
		WriteFloat(modelMeta.pVertices[iVert].textureCoords[0].u);
		WriteFloat(modelMeta.pVertices[iVert].textureCoords[0].v);

		extents.AddPoint(Vec3f(modelMeta.pVertices[iVert].x, modelMeta.pVertices[iVert].y, modelMeta.pVertices[iVert].z));
	}

	CLog::Log(S_INFO, "Vertex-extents: min=(%.2f, %.2f, %.2f) max=(%.2f, %.2f, %.2f)",
		extents.vMin.x, extents.vMin.y, extents.vMin.z,
		extents.vMax.x, extents.vMax.y, extents.vMax.z);

	DecreaseDbgIndent();
}


void CSPMWriter::WritePhysicsInfoChunk(const SPhysicsInfo& physicsInfo)
{
	u64 chunkSz = 2 + 4 * 3 * sizeof(float); // shapeType + 4 * Vec3f

	u64 meshChunkSz = 0;
	if (physicsInfo.shape.shapeType == eCOLSHAPE_MESH)
	{
		meshChunkSz += 4 + 4; // nVertices + nIndices  (both u32)
		meshChunkSz += physicsInfo.shape.mesh.nVertices * 3 * sizeof(float) // Vec3f
			+ physicsInfo.shape.mesh.nIndices * 4; // u32

		chunkSz += SPM_CHUNK_HEADER_SIZE;
		chunkSz += meshChunkSz;
	}
	
	WriteChunkHeader(SPM_CHUNK_PHYSICS_INFO, chunkSz);
	DebugLog("Writing CHUNK_PHYSICS_INFO (chunkSz = %llu)", chunkSz);
	IncreaseDbgIndent();

	WriteUShort(physicsInfo.shape.shapeType);
	
	for (int i = 0; i < 4; ++i)
	{
		WriteFloat(physicsInfo.shape.v[i].x);
		WriteFloat(physicsInfo.shape.v[i].y);
		WriteFloat(physicsInfo.shape.v[i].z);
	}

	if (physicsInfo.shape.shapeType == eCOLSHAPE_MESH)
	{
		WriteChunkHeader(SPM_CHUNK_SHAPE_MESH, meshChunkSz);
		DebugLog("Writing CHUNK_SHAPE_MESH (chunkSz = %llu)", meshChunkSz);
		IncreaseDbgIndent();

		const SSPMCollisionMesh& mesh = physicsInfo.shape.mesh;

		WriteUInt(mesh.nVertices);
		WriteUInt(mesh.nIndices);

		for (u32 iVtx = 0; iVtx < mesh.nVertices; ++iVtx)
		{
			WriteFloat(mesh.pVertices[iVtx].x);
			WriteFloat(mesh.pVertices[iVtx].y);
			WriteFloat(mesh.pVertices[iVtx].z);
		}

		for (u32 iIdx = 0; iIdx < mesh.nIndices; ++iIdx)
		{
			WriteUInt(mesh.pIndices[iIdx]);
		}

		DecreaseDbgIndent();
	}

	DecreaseDbgIndent();
}


bool CSPMWriter::Write(const char* filename, const vector<SModelMeta>& models, const SPhysicsInfo& physicsInfo /*= SPhysicsInfo()*/)
{
	m_Stream.open(filename, ofstream::out | ofstream::trunc | ofstream::binary);
	if (!m_Stream.is_open())
	{
		ErrorLog("Failed open stream for file %s", filename);
		return false;
	}

	// Write file version	
	WriteUShort(SPM_CURRENT_VERSION);
	DebugLog("Version = 0x%04X", SPM_CURRENT_VERSION);

	for (auto itModel = models.begin(); itModel != models.end(); itModel++)
	{
		WriteModelChunk(*itModel);
	}

	WritePhysicsInfoChunk(physicsInfo);

	m_Stream.close();

	return true;
}

SP_NMSPACE_END
