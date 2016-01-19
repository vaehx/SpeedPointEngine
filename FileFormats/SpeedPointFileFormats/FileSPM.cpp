#include "FileSPM.h"
#include <cstdarg>
#include <Util\SVertex.h>
#include <Abstract\BoundBox.h>

SP_NMSPACE_BEG

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
			if (m_bDebug) CLog::Log(S_DEBUG, "[SPMFile] Reading SPM_CHUNK_MODEL_META...");
			ReadModelMetaChunk(modelMeta);
			break;

		case SPM_CHUNK_VERTICES:
			if (m_bDebug) CLog::Log(S_DEBUG, "[SPMFile] Reading SPM_CHUNK_VERTICES...");
			ReadVertexChunk(modelMeta);
			break;

		case SPM_CHUNK_SUBSET:
			if (m_bDebug) CLog::Log(S_DEBUG, "[SPMFile] Reading SPM_CHUNK_SUBSET...");
			if (modelMeta.nSubsets == 0 || !IS_VALID_PTR(modelMeta.pSubsets))
			{
				CLog::Log(S_ERROR, "[SPMFile] Failed load subset chunk: nSubsets = 0 or subset array not initialized!");
				break;
			}

			if (modelMeta.nLoadedSubsets == modelMeta.nSubsets)
			{
				CLog::Log(S_ERROR, "[SPMFile] There are more subset-chunks than expected in the file!");
				break;
			}

			ReadSubsetChunk(modelMeta.pSubsets[modelMeta.nLoadedSubsets], chunkLength);
			modelMeta.nLoadedSubsets++;
			break;

		default:
			CLog::Log(S_ERROR, "[SPMFile] Invalid chunk type (length=%d): 0x%04X", chunkLength, chunkId);			
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
	if (m_bDebug) CLog::Log(S_DEBUG, "[SPMFile] name = %s", modelMeta.name.c_str());
	if (m_bDebug) CLog::Log(S_DEBUG, "[SPMFile] nSubsets = %d", modelMeta.nSubsets);
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
	if (m_bDebug) CLog::Log(S_DEBUG, "[SPMFile] Read %d vertices", modelMeta.nVertices);
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
			if (m_bDebug) CLog::Log(S_DEBUG, "[SPMFILE] Reading SPM_CHUNK_SUBSET_META...");
			ReadStringUntilFirstZero(subset.materialName);
			break;

		case SPM_CHUNK_SUBSET_INDICES:
			if (m_bDebug) CLog::Log(S_DEBUG, "[SPMFILE] Reading SPM_CHUNK_SUBSET_INDICES...");
			ReadUInt(subset.nIndices);
			subset.pIndices = new u32[subset.nIndices];

			for (u64 iIndex = 0; iIndex < subset.nIndices; ++iIndex)
			{
				ReadUInt(subset.pIndices[iIndex]);
			}

#ifdef _DEBUG
			if (m_bDebug) CLog::Log(S_DEBUG, "[SPMFile] Read %d indices", subset.nIndices);
#endif
			break;

		default:
			CLog::Log(S_ERROR, "[SPMFile] Invalid chunk type (length=%d): 0x%04X", chunkLength, chunkId);
			m_Stream.ignore(chunkLength);
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
		CLog::Log(S_ERROR, "Failed load SPM file %s", filename);
		return false;
	}

	if (m_Stream.eof())
	{
		CLog::Log(S_ERROR, "Empty SPM file %s", filename);
		return false;
	}

	m_Stream.seekg(0, m_Stream.end);
	u64 fileSz = m_Stream.tellg();
	m_Stream.seekg(0, m_Stream.beg);
	
	CLog::Log(S_DEBUG, "[SPMFile] Reading %s (fileSz = %llu)...", filename, fileSz);

	// Read the version
	ReadUShort(m_FileVersion);

	// Read objects
	u64 readLength = 0;
	while (readLength < fileSz)
	{
		u16 chunkId;
		u64 chunkLength;
		ReadChunkHeader(chunkId, chunkLength);
		readLength += chunkLength;

		switch (chunkId)
		{
		case SPM_CHUNK_MODEL:
		{
			if (m_bDebug) CLog::Log(S_DEBUG, "[SPMFILE] Reading SPM_CHUNK_MODEL ...");
			m_Models.push_back(SModelMeta());
			ReadModelChunk(m_Models.back(), chunkLength);
			break;
		}
		default:
			CLog::Log(S_ERROR, "[SPMFile] Invalid main chunk type: 0x%04X", chunkId);
			break;
		}
	}

	m_Stream.close();

	return true;
}

vector<SModelMeta>& CSPMLoader::GetModels()
{
	return m_Models;
}




////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////










void CSPMWriter::Log(const char* msg, ...) const
{
	printf("[SPM] ");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);

	printf("\n");
}

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
}

void CSPMWriter::WriteChunkHeader(u16 id, u64 length)
{
	WriteUShort(id);
	WriteULong(length);
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
	u64 subsetMetaChnkSz = subset.materialName.length() + 1;
	u64 subsetIndicesChnkSz = subset.nIndices * 4;

	if (pSubsetMetaLn)
		*pSubsetMetaLn = subsetMetaChnkSz;

	if (pIndicesLn)
		*pIndicesLn = subsetMetaChnkSz;

	return subsetMetaChnkSz + subsetIndicesChnkSz;
}



void CSPMWriter::WriteModelChunk(const SModelMeta& model)
{
	u64 chunkSize = 0;
	chunkSize += DetermineModelMetaChunkLength(model);
	chunkSize += DetermineVertexChunkLength(model);

	for (u16 iSubset = 0; iSubset < model.nLoadedSubsets; ++iSubset)
		chunkSize += DetermineSubsetChunkLength(model.pSubsets[iSubset]);

	WriteChunkHeader(SPM_CHUNK_MODEL, chunkSize);

	WriteModelMetaChunk(model);
	WriteVertexChunk(model);

	for (u16 iSubset = 0; iSubset < model.nLoadedSubsets; ++iSubset)
	{
		Log("WriteSubsetChunk...");
		WriteSubsetChunk(model.pSubsets[iSubset]);
	}
}

void CSPMWriter::WriteModelMetaChunk(const SModelMeta& modelMeta)
{
	u64 chunkSize = DetermineModelMetaChunkLength(modelMeta);

	WriteChunkHeader(SPM_CHUNK_MODEL_META, chunkSize);

	Log("  model.name = %s", modelMeta.name.c_str());
	WriteString(modelMeta.name);

	Log("  model.nLoadedSubsets = %d", modelMeta.nLoadedSubsets);
	WriteUShort(modelMeta.nLoadedSubsets);
}

void CSPMWriter::WriteSubsetChunk(const SSubset& subset)
{	
	u64 subsetMetaChnkSz, subsetIndicesChnkSz;
	DetermineSubsetChunkLength(subset, &subsetMetaChnkSz, &subsetIndicesChnkSz);

	Log("Writing CHUNK_SUBSET (chunkSize=%llu", subsetMetaChnkSz + subsetIndicesChnkSz);
	WriteChunkHeader(SPM_CHUNK_SUBSET, subsetMetaChnkSz + subsetIndicesChnkSz);

	// Meta:
	Log("  Writing CHUNK_SUBSET_META (chunkSize=%llu)", subsetMetaChnkSz);
	WriteChunkHeader(SPM_CHUNK_SUBSET_META, subsetMetaChnkSz);
	WriteString(subset.materialName);		

	// Indices:
	Log("  Writing CHUNK_SUBSET_INDICES (subsetIndicesChnkSz=%llu)", subsetIndicesChnkSz);
	WriteChunkHeader(SPM_CHUNK_SUBSET_INDICES, subsetIndicesChnkSz);
	Log("    Writing %lu indices...", subset.nIndices);
	WriteUInt(subset.nIndices);
	for (u64 iIndex = 0; iIndex < subset.nIndices; ++iIndex)
	{
		WriteUInt(subset.pIndices[iIndex]);
	}
}

void CSPMWriter::WriteVertexChunk(const SModelMeta& modelMeta)
{
	u64 subsetVertsChnkSz = modelMeta.nVertices * (3 * sizeof(float) + 3 * sizeof(float) + 2 * sizeof(float));

	Log("Writing CHUNK_VERTICES (chunkSz=%llu)", subsetVertsChnkSz);
	WriteChunkHeader(SPM_CHUNK_VERTICES, subsetVertsChnkSz);

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

	Log("*** Model-extents: min=(%.2f, %.2f, %.2f) max=(%.2f, %.2f, %.2f) ***",
		extents.vMin.x, extents.vMin.y, extents.vMin.z,
		extents.vMax.x, extents.vMax.y, extents.vMax.z);
}



bool CSPMWriter::Write(const char* filename, const vector<SModelMeta>& models)
{
	m_Stream.open(filename, ofstream::out | ofstream::trunc | ofstream::binary);
	if (!m_Stream.is_open())
	{
		Log("Failed open stream for file %s", filename);
		return false;
	}

	// Write file version
	WriteUShort(SPM_CURRENT_VERSION);

	for (auto itModel = models.begin(); itModel != models.end(); itModel++)
		WriteModelChunk(*itModel);

	m_Stream.close();

	return true;
}

SP_NMSPACE_END
