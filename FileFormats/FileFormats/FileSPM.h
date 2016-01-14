#pragma once

#include <Abstract\SAPI.h>
#include <SPrerequisites.h>
#include <string>
#include <fstream>

using std::string;
using std::ifstream;
using std::ofstream;

#define SPM_CURRENT_VERSION 0x0001

#define SPM_CHUNK_MODEL_META 0x1000
#define SPM_CHUNK_VERTICES 0x2000
#define SPM_CHUNK_SUBSET 0x3000
#define SPM_CHUNK_SUBSET_META 0x3100
#define SPM_CHUNK_SUBSET_INDICES 0x3200

SP_NMSPACE_BEG

struct SVertex;

struct SSubset
{
	string materialName; // specification/name

	u32 nIndices;
	u32* pIndices;

	SSubset()
		: nIndices(0),
		pIndices(0)
	{
	}
};

struct SModelMeta
{
	string name;

	u16 nSubsets;
	u16 nLoadedSubsets; // actual loaded subsets
	SSubset* pSubsets;

	u32 nVertices;
	SVertex* pVertices;

	SModelMeta()
		: nVertices(0),
		pVertices(0),
		nSubsets(0),
		pSubsets(0)
	{
	}

	SModelMeta(const SModelMeta& o)
		: name(o.name),
		nSubsets(o.nSubsets),
		nLoadedSubsets(o.nLoadedSubsets),
		pSubsets(o.pSubsets),
		nVertices(o.nVertices),
		pVertices(o.pVertices)
	{
	}
};

class CSPMWriter
{
private:
	ofstream m_Stream;

	inline void WriteUShort(u16 u);
	inline void WriteUInt(u32 u);
	inline void WriteULong(u64 u);
	inline void WriteFloat(f32 f);

	// Appends a terminating zero
	inline void WriteString(const string& s);

	inline void WriteChunkHeader(u16 id, u64 length);
	inline void WriteModelMetaChunk(const SModelMeta& modelMeta);
	inline void WriteVertexChunk(const SModelMeta& modelMeta);
	inline void WriteSubsetChunk(const SSubset& subset);

	inline void Log(const char* msg, ...) const;

public:
	// Returns success
	bool Write(const char* filename, const SModelMeta& model);
};

// Loads a .spm file, which contains subset, vertex and index information of the model.
// The materials are stored with their names in the subset info. To associate the real materials with
// the resource pointers, you can use IMaterialManager::FindMaterial().
// The loading pipeline is: Load Textures --> Load Materials --> Load Models --> Associate model's subsets with their material
class CSPMLoader
{
private:
	u16 m_FileVersion;
	SModelMeta m_Model;
	ifstream m_Stream;

	bool m_bDebug;

	inline void ReadUShort(u16& u);
	inline void ReadUInt(u32& u);
	inline void ReadULong(u64& u);
	inline void ReadFloat(f32& f);
	inline void ReadStringUntilFirstZero(string& s);

	inline void ReadChunkHeader(u16 &id, u64 &length);
	inline void ReadModelMetaChunk(SModelMeta& modelMeta);
	inline void ReadVertexChunk(SModelMeta& modelMeta);
	inline void ReadSubsetChunk(SSubset& subset, const u64& chunkLn);

public:
	// Returns success
	bool Load(const char* filename, bool bDebug = false);

	SModelMeta& GetModelMeta();
};

SP_NMSPACE_END
