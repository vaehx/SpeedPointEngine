#include "FileSPM.h"
#include <cstdarg>

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
	m_Stream.write((const char*)&f, sizeof(f));
}

// Appends a terminating zero
void CSPMWriter::WriteString(const string& s)
{
	m_Stream.write(s.c_str(), s.length());
	m_Stream.put('\0');
}

void CSPMWriter::WriteChunkHeader(u16 id, u64 length)
{
	WriteUShort(id);
	WriteULong(length);
}

void CSPMWriter::WriteModelMetaChunk(const SModelMeta& modelMeta)
{
	u64 chunkSize = 0;
	chunkSize += modelMeta.name.length() + 1;
	chunkSize += 2; // num_subsets

	WriteChunkHeader(SPM_CHUNK_MODEL_META, chunkSize);

	WriteString(modelMeta.name);
	WriteUShort(modelMeta.nLoadedSubsets);
}

void CSPMWriter::WriteSubsetChunk(const SSubset& subset)
{	
	u64 subsetMetaChnkSz = subset.materialName.length() + 1;	
	u64 subsetIndicesChnkSz = subset.nIndices * 8;	

	WriteChunkHeader(SPM_CHUNK_SUBSET, subsetMetaChnkSz + subsetIndicesChnkSz);

	// Meta:
	WriteChunkHeader(SPM_CHUNK_SUBSET_META, subsetMetaChnkSz);
	WriteString(subset.materialName);		

	// Indices:
	WriteChunkHeader(SPM_CHUNK_SUBSET_INDICES, subsetIndicesChnkSz);
	WriteUInt(subset.nIndices);
	for (u64 iIndex = 0; iIndex < subset.nIndices; ++iIndex)
	{
		WriteUInt(subset.pIndices[iIndex]);
	}
}

void CSPMWriter::WriteVertexChunk(const SModelMeta& modelMeta)
{
	u64 subsetVertsChnkSz = modelMeta.nVertices * (8 + 12 + 12 + 8);

	WriteChunkHeader(SPM_CHUNK_VERTICES, subsetVertsChnkSz);
	
	WriteUInt(modelMeta.nVertices);
	for (u64 iVert = 0; iVert < modelMeta.nVertices; ++iVert)
	{
		WriteFloat(modelMeta.pVertices[iVert].x);
		WriteFloat(modelMeta.pVertices[iVert].y);
		WriteFloat(modelMeta.pVertices[iVert].z);
		WriteFloat(modelMeta.pVertices[iVert].nx);
		WriteFloat(modelMeta.pVertices[iVert].ny);
		WriteFloat(modelMeta.pVertices[iVert].nz);
		WriteFloat(modelMeta.pVertices[iVert].u);
		WriteFloat(modelMeta.pVertices[iVert].v);
	}
}



bool CSPMWriter::Write(const char* filename, const SModelMeta& model)
{
	m_Stream.open(filename, ofstream::out | ofstream::trunc | ofstream::binary);
	if (!m_Stream.is_open())
	{
		Log("Failed open stream for file %s", filename);
		return false;
	}

	// Write file version
	WriteUShort(SPM_CURRENT_VERSION);
	
	WriteModelMetaChunk(model);
	WriteVertexChunk(model);

	for (u16 iSubset = 0; iSubset < model.nLoadedSubsets; ++iSubset)
	{
		WriteSubsetChunk(model.pSubsets[iSubset]);
	}

	m_Stream.close();

	return true;
}