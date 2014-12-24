//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	File3DS.cpp
// Created:	12/22/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\File3DS.h>

SP_NMSPACE_BEG

// -----------------------------------------------------------------------------------------------------
S_API void File3DS::DbgMsg(const char* fmt, ...) const
{
	unsigned int i = 0;
	while (i < curChnkLvl)
	{
		printf("  ");
		i++;
	}

	va_list argptr;
	va_start(argptr, fmt);
	vprintf(fmt, argptr);
	va_end(argptr);

	printf("\n");
}


// -----------------------------------------------------------------------------------------------------
S_API void File3DS::ReadUShort(unsigned short& s)
{
	file.read((char*)(&s), 2);
}

// -----------------------------------------------------------------------------------------------------
S_API void File3DS::ReadULong(unsigned long& l)
{
	file.read((char*)(&l), 4);
}

// -----------------------------------------------------------------------------------------------------
S_API void File3DS::ReadString(char* dst, unsigned long length)
{
	if (!IS_VALID_PTR(dst))
		return;

	file.read(dst, length);
	if (dst[length - 1] != 0)
		dst[length - 1] = 0;
}

// -----------------------------------------------------------------------------------------------------
void File3DS::ReadStringToFirstZero(char** dst)
{
	if (!IS_VALID_PTR(dst))
		return;

	unsigned int charlimit = 256;

	char* tmp = new char[charlimit];
	unsigned int readLength = 0;
	char lastReadChar = 1;
	bool saveChar = true;
	while (lastReadChar != 0)
	{
		if (saveChar)
		{
			file.read(tmp + readLength, 1);
			lastReadChar = tmp[readLength];
			readLength++;
			if (readLength == charlimit)
				saveChar = false;
		}
		else
		{
			file.read(&lastReadChar, 1);
		}

		if (file.eof())
			break;
	}

	if (readLength < charlimit)
	{
		// shrink
		*dst = new char[readLength];
		memcpy(*dst, tmp, readLength);
		delete[] tmp;
	}
	else
	{
		*dst = tmp;
	}
}

// -----------------------------------------------------------------------------------------------------
void File3DS::ReadFloat(float& f)
{
	file.read((char*)(&f), 4);
}

// -----------------------------------------------------------------------------------------------------
void File3DS::ReadVector(float& x, float& y, float& z)
{
	ReadFloat(x);
	ReadFloat(y);
	ReadFloat(z);
}

// -----------------------------------------------------------------------------------------------------
void File3DS::ReadColor(S3DSColorF& color)
{
	unsigned short colorChnkId;
	unsigned long colorChnkSz;
	ReadUShort(colorChnkId);
	ReadULong(colorChnkSz);
	if (colorChnkId == CHNK3DS_COLOR_24)
	{
		char channels[3];
		file.read(channels, 3);

		color.r = ((float)(unsigned char)channels[0]) / 256.0f;
		color.g = (float)(unsigned char)channels[1] / 256.0f;
		color.b = (float)(unsigned char)channels[2] / 256.0f;
	}
	else if (colorChnkId == CHNK3DS_COLOR_F)
	{
		file.read((char*)(&color), 3 * 4);
	}
	else
	{
		DbgMsg("Unknown color chunk found of id 0x%04X", colorChnkId);
	}
}

// -----------------------------------------------------------------------------------------------------
void File3DS::ReadChunkHeader(Chunk& chnk)
{
	chnk.byte_pos = file.tellg();
	ReadUShort(chnk.id);
	ReadULong(chnk.byte_size);
}

// -----------------------------------------------------------------------------------------------------
void File3DS::ReadDummyChunk()
{
	if (curChnk.byte_pos == 0 || curChnk.byte_size == 0)
		return;

	file.seekg(curChnk.byte_size - CHUNK_HEADER_SIZE, std::ifstream::cur);
	//DbgMsg("Skipped %d bytes, now at %d", curChnk.byte_size - CHUNK_HEADER_SIZE, (unsigned int)file.tellg());
}


// ===========================================================================================================================
// ===========================================================================================================================
void File3DS::ReadTriangledObjectChunk(S3DSNamedObject& obj)
{
	curChnkLvl++;
	Chunk triChunk = curChnk;
	curChnk.byte_size = 0;

	while (curChnk.byte_pos + curChnk.byte_size < (triChunk.byte_pos + triChunk.byte_size))
	{
		ReadChunkHeader(curChnk);
		switch (curChnk.id)
		{
		case CHNK3DS_POINT_ARRAY:
			ReadUShort(obj.nVertices);
			if (obj.nVertices == 0)
			{
				DbgMsg("0 points to read");
				break;
			}
			obj.pVertices = new S3DSVertex[obj.nVertices];
			for (unsigned short iVtx = 0; iVtx < obj.nVertices; ++iVtx)
			{
				ReadFloat(obj.pVertices[iVtx].x);
				ReadFloat(obj.pVertices[iVtx].y);
				ReadFloat(obj.pVertices[iVtx].z);
			}
			DbgMsg("CHNK3DS_POINT_ARRAY: Read %d vertices", obj.nVertices);
			break;

		case CHNK3DS_FACE_ARRAY:
			ReadUShort(obj.nFaces);
			if (obj.nFaces == 0)
			{
				DbgMsg("0 faces to read");
				break;
			}
			if (obj.nVertices == 0)
				DbgMsg("Warning: Reading %d faces, but there were no vertices!", obj.nFaces);
			obj.pFaces = new S3DSFace[obj.nFaces];
			for (unsigned short iFace = 0; iFace < obj.nFaces; ++iFace)
			{
				ReadUShort(obj.pFaces[iFace].v1);
				ReadUShort(obj.pFaces[iFace].v2);
				ReadUShort(obj.pFaces[iFace].v3);
				ReadUShort(obj.pFaces[iFace].flag);
			}
			DbgMsg("CHNK3DS_FACE_ARRAY: Read %d faces", obj.nFaces);
			break;

		case CHNK3DS_MESH_MATERIAL:
		{
			obj.meshMaterials.push_back(S3DSMeshMaterial());
			S3DSMeshMaterial& meshMat = obj.meshMaterials.back();
			ReadStringToFirstZero(&meshMat.name);
			ReadUShort(meshMat.nAffectedFaces);
			if (meshMat.nAffectedFaces == 0)
			{
				DbgMsg("Warning: Found material that does not affect any face in this trimesh!");
				break;
			}
			meshMat.pAffectedFaces = new unsigned short[meshMat.nAffectedFaces];
			for (unsigned short iAffectedFace = 0; iAffectedFace < meshMat.nAffectedFaces; ++iAffectedFace)
				ReadUShort(meshMat.pAffectedFaces[iAffectedFace]);

			DbgMsg("CHNK3DS_MESH_MATERIAL #%d: %d faces are affected by material '%s'", obj.meshMaterials.size(), meshMat.nAffectedFaces, meshMat.name);
			break;
		}

		case CHNK3DS_MESH_MATRIX:
			ReadVector(obj.meshMatrix.m11, obj.meshMatrix.m12, obj.meshMatrix.m13);
			ReadVector(obj.meshMatrix.m21, obj.meshMatrix.m22, obj.meshMatrix.m23);
			ReadVector(obj.meshMatrix.m31, obj.meshMatrix.m32, obj.meshMatrix.m33);
			ReadVector(obj.pivot.x, obj.pivot.y, obj.pivot.z);

			DbgMsg("CHNK3DS_MESH_MTX: X(%1.2f, %1.2f, %1.2f) Y(%1.2f, %1.2f, %1.2f) Z(%1.2f, %1.2f, %1.2f)",
				obj.meshMatrix.m11, obj.meshMatrix.m12, obj.meshMatrix.m13,
				obj.meshMatrix.m21, obj.meshMatrix.m22, obj.meshMatrix.m23,
				obj.meshMatrix.m31, obj.meshMatrix.m32, obj.meshMatrix.m33);
			DbgMsg("  pivot = (%1.2f, %1.2f, %1.2f)", obj.pivot.x, obj.pivot.y, obj.pivot.z);
			break;

		default:
			DbgMsg("Found chunk 0x%04X, sz=%d, payload=%d",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);
			ReadDummyChunk();
			break;
		}
	}

	curChnk = triChunk;
	curChnkLvl--;
}


// ===========================================================================================================================
// ===========================================================================================================================
void File3DS::ReadNamedObjectChunk(S3DSNamedObject& obj)
{
	curChnkLvl++;

	Chunk objChunk = curChnk;
	curChnk.byte_size = 0;
	//curChnk.byte_pos = 0;

	// read name
	char* objName = new char[256];
	memset(objName, 0, 256);
	unsigned int charCnt = 0;
	char lastReadChar = 1;
	while (lastReadChar != 0)
	{
		file.read(objName + charCnt, 1);
		lastReadChar = objName[charCnt];
		charCnt++;
		if (charCnt == 256)
		{
			printf("Model name exceeded limit of 256 chars!");
			delete[] objName;
			return;
		}
	}

	DbgMsg("obj.name = \"%s\"", objName);
	delete[] objName;

	while (curChnk.byte_pos + curChnk.byte_size < (objChunk.byte_pos + objChunk.byte_size))
	{
		ReadChunkHeader(curChnk);
		switch (curChnk.id)
		{
		case CHNK3DS_N_TRI_OBJECT:
			DbgMsg("Reading CHNK3DS_N_TRI_OBJECT (0x%04X), sz=%d, payload=%d:",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);
			ReadTriangledObjectChunk(obj);
			break;
		default:
			DbgMsg("Found chunk 0x%04X, sz=%d, payload=%d",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);
			ReadDummyChunk();
			break;
		};
	}

	curChnk = objChunk;
	curChnkLvl--;
}

// ===========================================================================================================================
// ===========================================================================================================================
void File3DS::ReadMaterialMapChunk(S3DSMaterialMap& matmap)
{
	curChnkLvl++;

	Chunk matmapChunk = curChnk;
	curChnk.byte_size = 0;
	while (curChnk.byte_pos + curChnk.byte_size < (matmapChunk.byte_pos + matmapChunk.byte_size))
	{
		ReadChunkHeader(curChnk);
		switch (curChnk.id)
		{
		case CHNK3DS_MAT_MAPNAME:
		{
			unsigned long matmapNameLn = curChnk.byte_size - CHUNK_HEADER_SIZE;
			matmap.name = new char[matmapNameLn];
			ReadString(matmap.name, matmapNameLn);
			DbgMsg("matmap.name = \"%s\"", matmap.name);
			break;
		}
		case CHNK3DS_MAT_MAP_TILING:
			//CheckChunkSize(curChnk, 2);
			ReadUShort(matmap.tiling);
			DbgMsg("matmap.tiling = %d", matmap.tiling);
			break;
		case CHNK3DS_MAT_MAP_USCALE:
			//CheckChunkSize(curChnk, 4);
			ReadFloat(matmap.uscale);
			DbgMsg("matmap.uscale = %1.2f", matmap.uscale);
			break;
		case CHNK3DS_MAT_MAP_VSCALE:
			//CheckChunkSize(curChnk, 4);
			ReadFloat(matmap.vscale);
			DbgMsg("matmap.vscale = %1.2f", matmap.vscale);
			break;
		case CHNK3DS_MAT_MAP_UOFFSET:
			//CheckChunkSize(curChnk, 4);
			ReadFloat(matmap.uoffset);
			DbgMsg("matmap.uoffset = %1.2f", matmap.uoffset);
			break;
		case CHNK3DS_MAT_MAP_VOFFSET:
			//CheckChunkSize(curChnk, 4);
			ReadFloat(matmap.voffset);
			DbgMsg("matmap.voffset = %1.2f", matmap.voffset);
			break;
		default:
			ReadDummyChunk();
			break;
		}
	}

	curChnk = matmapChunk;
	curChnkLvl--;
}



// ===========================================================================================================================
// ===========================================================================================================================
void File3DS::ReadMaterialChunk(S3DSMaterial& mat)
{
	curChnkLvl++;

	Chunk matChnk = curChnk;
	curChnk.byte_size = 0;
	while (curChnk.byte_pos + curChnk.byte_size < (matChnk.byte_pos + matChnk.byte_size))
	{
		ReadChunkHeader(curChnk);
		switch (curChnk.id)
		{
		case CHNK3DS_MAT_NAME:
		{
			unsigned long matNameLn = curChnk.byte_size - CHUNK_HEADER_SIZE;
			mat.material_name = new char[matNameLn];
			ReadString(mat.material_name, matNameLn);
			DbgMsg("material.name = \"%s\"", mat.material_name);
			break;
		}
		case CHNK3DS_MAT_AMBIENT:
			ReadColor(mat.ambient);
			DbgMsg("material.ambient = (%d, %d, %d)",
				(unsigned int)(mat.ambient.r * 256.0f), (unsigned int)(mat.ambient.g * 256.0f), (unsigned int)(mat.ambient.b * 256.0f));
			break;
		case CHNK3DS_MAT_DIFFUSE:
			ReadColor(mat.diffuse);
			DbgMsg("material.diffuse = (%d, %d, %d)",
				(unsigned int)(mat.diffuse.r * 256.0f), (unsigned int)(mat.diffuse.g * 256.0f), (unsigned int)(mat.diffuse.b * 256.0f));
			break;
		case CHNK3DS_MAT_SPECULAR:
			ReadColor(mat.specular);
			DbgMsg("material.specular = (%d, %d, %d)",
				(unsigned int)(mat.specular.r * 256.0f), (unsigned int)(mat.specular.g * 256.0f), (unsigned int)(mat.specular.b * 256.0f));
			break;
		case CHNK3DS_MAT_TEXMAP:
			DbgMsg("Reading CHNK3DS_MAT_TEXMAP (0x%04X), sz=%d, payload=%d:",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);
			ReadMaterialMapChunk(mat.texturemap);
			break;
		case CHNK3DS_MAT_BUMPMAP:
			DbgMsg("Reading CHNK3DS_MAT_BUMPMAP (0x%04X), sz=%d, payload=%d:",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);
			ReadMaterialMapChunk(mat.bumpmap);
			break;
		default:
			DbgMsg("Found chunk 0x%04X, sz=%d, payload=%d",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);
			ReadDummyChunk();
			break;
		}
	}

	curChnk = matChnk;
	curChnkLvl--;
}




// ===========================================================================================================================
// ===========================================================================================================================
// Read 0x3D3D
void File3DS::ReadEditChunk()
{
	curChnkLvl++;

	Chunk editChunk = curChnk;
	curChnk.byte_size = 0;
	curChnk.byte_pos = 0;
	while (curChnk.byte_pos + curChnk.byte_size < (editChunk.byte_pos + editChunk.byte_size))
	{
		ReadChunkHeader(curChnk);
		switch (curChnk.id)
		{
		case CHNK3DS_EDIT_MATERIAL:
			materials.push_back(S3DSMaterial());
			DbgMsg("Reading CHNK3DS_EDIT_MATERIAL (0x%04X), sz=%d, payload=%d:",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);

			ReadMaterialChunk(materials.back());
			break;
		case CHNK3DS_NAMED_OBJECT:
		{
			objects.push_back(S3DSNamedObject());
			DbgMsg("Reading CHNK3DS_NAMED_OBJECT (0x%04X), sz=%d, payload=%d:",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);
			ReadNamedObjectChunk(objects.back());
			break;
		}
		default:
			DbgMsg("Found chunk 0x%04x, sz=%d, payload=%d",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);
			ReadDummyChunk();
			break;
		};
	}

	curChnk = editChunk;
	curChnkLvl--;
}





// ===========================================================================================================================
// ===========================================================================================================================
void File3DS::Read()
{
	if (!file.good())
	{
		printf("File not a valid 3ds file!");
		return;
	}

	file.seekg(0, file.end);
	unsigned int fileSz = file.tellg();
	DbgMsg("File Size = %d", fileSz);
	file.seekg(0, file.beg);

	// read main chunk header
	ReadChunkHeader(mainChnk);
	curChnk.byte_pos = file.tellg();
	curChnk.byte_size = 0;
	DbgMsg("Found main chunk: 0x%04x, size=%d", mainChnk.id, mainChnk.byte_size);
	curChnkLvl++;

	// read version chunk
	Chunk versionChunk;
	ReadChunkHeader(versionChunk);
	if (versionChunk.byte_size == CHUNK_HEADER_SIZE + 4)
		ReadULong(version);
	else
	{
		unsigned short tmpVer;
		ReadUShort(tmpVer);
		version = (unsigned long)tmpVer;
	}

	DbgMsg("Read model version: %d", version);

	// read further model chunks
	while (file.good() && !file.eof() && (curChnk.byte_pos + curChnk.byte_size) < mainChnk.byte_size)
	{
		ReadChunkHeader(curChnk);
		switch (curChnk.id)
		{
		case CHNK3DS_EDIT3DS:
			DbgMsg("Reading CHNK3DS_EDIT3DS (0x%04X), sz=%d, payload=%d:",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);
			ReadEditChunk();
			break;
		default:
			DbgMsg("Found chunk 0x%04x, sz=%d, payload=%d",
				curChnk.id, curChnk.byte_size, curChnk.byte_size - CHUNK_HEADER_SIZE);
			ReadDummyChunk();
			break;
		}
		//curChnk.byte_pos = file.tellg();
	}
}


SP_NMSPACE_END