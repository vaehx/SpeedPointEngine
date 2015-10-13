#include "SPMFile.h"
#include <Util\SVertex.h>

SP_NMSPACE_BEG

namespace SPM
{
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

	void CSPMLoader::ReadModelMetaChunk(SModelMeta& modelMeta)
	{
		ReadStringUntilFirstZero(modelMeta.name);
		ReadUShort(modelMeta.nSubsets);

#ifdef _DEBUG
		CLog::Log(S_DEBUG, "[SPMFile] name = %s", modelMeta.name.c_str());
		CLog::Log(S_DEBUG, "[SPMFile] nSubsets = %d", modelMeta.nSubsets);
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
		CLog::Log(S_DEBUG, "[SPMFile] Read %d vertices", modelMeta.nVertices);
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
				ReadStringUntilFirstZero(subset.materialName);				
				break;

			case SPM_CHUNK_SUBSET_INDICES:
				ReadUInt(subset.nIndices);
				subset.pIndices = new u32[subset.nIndices];

				for (u64 iIndex = 0; iIndex < subset.nIndices; ++iIndex)
				{
					ReadUInt(subset.pIndices[iIndex]);
				}

#ifdef _DEBUG
				CLog::Log(S_DEBUG, "[SPMFile] Read %d indices", subset.nIndices);
#endif
				break;

			default:
				CLog::Log(S_ERROR, "[SPMFile] Invalid chunk type: 0x%04X", chunkId);
				m_Stream.ignore(chunkLength);
				break;
			}			
		}
	}



	bool CSPMLoader::Load(const char* filename)
	{
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

		// Read the version
		ReadUShort(m_FileVersion);

		// Read chunks
		while (!m_Stream.eof())
		{
			u16 chunkId;
			u64 chunkLength;
			ReadChunkHeader(chunkId, chunkLength);

			switch (chunkId)
			{
			case SPM_CHUNK_MODEL_META:
				ReadModelMetaChunk(m_Model);
				break;

			case SPM_CHUNK_VERTICES:
				ReadVertexChunk(m_Model);
				break;

			case SPM_CHUNK_SUBSET:
				if (m_Model.nSubsets == 0 || !IS_VALID_PTR(m_Model.pSubsets))
				{
					CLog::Log(S_ERROR, "[SPMFile] Failed load subset chunk: nSubsets = 0 or subset array not initialized!");
					break;
				}

				if (m_Model.nLoadedSubsets == m_Model.nSubsets)
				{
					CLog::Log(S_ERROR, "[SPMFile] There are more subset-chunks than expected in the file!");
					break;
				}

				ReadSubsetChunk(m_Model.pSubsets[m_Model.nLoadedSubsets], chunkLength);
				break;

			default:
				CLog::Log(S_ERROR, "[SPMFile] Invalid main chunk type: 0x%04X", chunkId);
				m_Stream.ignore(chunkLength);
				break;
			}
		}

		m_Stream.close();
	}
}

SP_NMSPACE_END