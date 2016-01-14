//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2016, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	File3DS.h
// Created:	12/22/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <stdio.h>
#include <stdarg.h>
#include <fstream>
#include <vector>
#include <string>

using std::string;

#define CHNK3DS_MAIN3DS 0x4D4D
#define CHNK3DS_EDIT3DS 0x3D3D
#define CHNK3DS_MESH_VERSION 0x3D3E

#define CHNK3DS_M3D_VERSION 0x0002
#define CHNK3DS_COLOR_F 0x0010
#define CHNK3DS_COLOR_24 0x0011
#define CHNK3DS_LIN_COLOR_24 0x0012
#define CHNK3DS_LIN_COLOR_F 0x0013
#define CHNK3DS_INT_PERCENTAGE 0x0030
#define CHNK3DS_FLOAT_PERCENTAGE 0x0031
#define CHNK3DS_MASTER_SCALE 0x0100

#define CHNK3DS_EDIT_MATERIAL 0xAFFF
#define CHNK3DS_MAT_NAME 0xA000
#define CHNK3DS_MAT_AMBIENT 0xA010
#define CHNK3DS_MAT_DIFFUSE 0xA020
#define CHNK3DS_MAT_SPECULAR 0xA030
#define CHNK3DS_MAT_SHININESS 0xA040
#define CHNK3DS_MAT_SHINESTRENGTH 0xA041
#define CHNK3DS_MAT_SHIN2PCT CHNK3DS_MAT_SHINESTRENGTH
#define CHNK3DS_MAT_TRANSPARENCY 0xA050
#define CHNK3DS_MAT_XPFALL 0xA052
#define CHNK3DS_MAT_REFBLUR 0xA053
#define CHNK3DS_MAT_SELF_ILPCT 0xA084
#define CHNK3DS_MAT_WIRE_SIZE 0xA087
#define CHNK3DS_MAT_XPFALLIN 0xA08A
#define CHNK3DS_MAT_SHADING 0xA100
#define CHNK3DS_MAT_TEXMAP 0xA200
#define CHNK3DS_MAT_SPECMAP 0xA204
#define CHNK3DS_MAT_OPACMAP 0xA210
//#define CHNK3DS_MAT_REFLMAP 0xA220
#define CHNK3DS_MAT_BUMPMAP 0xA230

#define CHNK3DS_MAT_MAPNAME 0xA300
#define CHNK3DS_MAT_MAP_TILING 0xA351
#define CHNK3DS_MAT_MAP_USCALE 0xA354
#define CHNK3DS_MAT_MAP_VSCALE 0xA356
#define CHNK3DS_MAT_MAP_UOFFSET 0xA358
#define CHNK3DS_MAT_MAP_VOFFSET 0xA35A

#define CHNK3DS_PIVOT 0xB013

#define CHNK3DS_NAMED_OBJECT 0x4000
#define CHNK3DS_N_TRI_OBJECT 0x4100
#define CHNK3DS_POINT_ARRAY 0x4110
#define CHNK3DS_FACE_ARRAY 0x4120
#define CHNK3DS_MESH_MATERIAL 0x4130
#define CHNK3DS_MESH_MATRIX 0x4160
#define CHNK3DS_SMOOTH_GROUP 0x4150
#define CHNK3DS_UV_ARRAY 0x4140


struct  S3DSColorF
{
	float r, g, b;

	S3DSColorF() {}
	S3DSColorF(const S3DSColorF& c)
		: r(c.r), g(c.g), b(c.b) {}
};

struct  S3DSMaterialMap
{
	string name;
	unsigned short tiling;
	float uscale, vscale;
	float uoffset, voffset;

	S3DSMaterialMap()
		: tiling(0),
		uscale(1.0f), vscale(1.0f),
		uoffset(0), voffset(0) {}

	S3DSMaterialMap(const S3DSMaterialMap& mm)
		: name(mm.name),
		tiling(mm.tiling),
		uscale(mm.uscale), vscale(mm.vscale),
		uoffset(mm.uoffset), voffset(mm.voffset)
	{		
	}

	~S3DSMaterialMap()
	{		
	}
};

struct  S3DSMaterial
{
	string material_name;
	S3DSColorF ambient;
	S3DSColorF diffuse;
	S3DSColorF specular;
	S3DSMaterialMap texturemap;
	S3DSMaterialMap bumpmap;		// = normalmap
	float shininess;
	float shine_strength;
	float transparency;
	float xpfall;
	float refblur;
	float self_illum;
	unsigned short shading;

	S3DSMaterial() {}
	S3DSMaterial(const S3DSMaterial& mat)
		: material_name(mat.material_name),
		ambient(mat.ambient),
		diffuse(mat.diffuse),
		specular(mat.specular),
		texturemap(mat.texturemap),
		bumpmap(mat.bumpmap),
		shininess(0.0f),
		shine_strength(0.0f)
	{		
	}

	~S3DSMaterial()
	{		
	}
};

struct  S3DSVertex
{
	float x, y, z;
	float nx, ny, nz; // normal
	float tx, ty, tz; // tangent
	float u, v;

	S3DSVertex() {}
	S3DSVertex(const S3DSVertex& v)
		: x(v.x), y(v.y), z(v.z),
		nx(v.nx), ny(v.ny), nz(v.nz),
		tx(v.tx), ty(v.ty), tz(v.tz),
		u(v.u), v(v.v) {}
};

struct  S3DSMatrix3
{
	float m11, m12, m13;
	float m21, m22, m23;
	float m31, m32, m33;

	S3DSMatrix3()
		: m11(1.0f), m12(0), m13(0),
		m21(0), m22(1.0f), m23(0),
		m31(0), m32(0), m33(1.0f) {}

	S3DSMatrix3(const S3DSMatrix3& m)
		: m11(m.m11), m12(m.m12), m13(m.m13),
		m21(m.m21), m22(m.m22), m23(m.m23),
		m31(m.m31), m32(m.m32), m33(m.m33) {}
};

struct  S3DSVector3
{
	float x, y, z;

	S3DSVector3() : x(0), y(0), z(0) {}
	S3DSVector3(const S3DSVector3& v) : x(v.x), y(v.y), z(v.z) {}
};

struct  S3DSFace
{
	unsigned short v1, v2, v3;
	unsigned short flag; // 0: CA-visibility, 1: BC-visibility, 2: AB-visibility, 3: use mapping

	S3DSFace() {}
	S3DSFace(const S3DSFace& f)
		: v1(f.v1), v2(f.v2), v3(f.v3), flag(f.flag) {}
};

struct S3DSMeshMaterial
{
	string name; // should accord to one S3DSMaterial that was read
	unsigned short nAffectedFaces;
	unsigned short* pAffectedFaces;	// indices of the faces
	S3DSMeshMaterial()
		: nAffectedFaces(0),
		pAffectedFaces(0) {}

	S3DSMeshMaterial(const S3DSMeshMaterial& mm)
		: name(mm.name)
	{
		nAffectedFaces = mm.nAffectedFaces;
		pAffectedFaces = 0;
		if (IS_VALID_PTR(mm.pAffectedFaces) && nAffectedFaces > 0)
		{
			pAffectedFaces = new unsigned short[nAffectedFaces];
			memcpy(pAffectedFaces, mm.pAffectedFaces, sizeof(unsigned short) * nAffectedFaces);
		}
	}

	~S3DSMeshMaterial()
	{		
		if (IS_VALID_PTR(pAffectedFaces))
			delete[] pAffectedFaces;
		
		nAffectedFaces = 0;
		pAffectedFaces = 0;
	}
};

struct  S3DSNamedObject
{
	std::vector<S3DSMeshMaterial> meshMaterials;

	string name;

	S3DSMatrix3 meshMatrix;
	S3DSVector3 pivot;

	unsigned short nVertices;
	S3DSVertex* pVertices;

	unsigned short nFaces;
	S3DSFace* pFaces;

	S3DSNamedObject()
		: pVertices(0),
		nVertices(0),
		pFaces(0),
		nFaces(0) {}

	S3DSNamedObject(const S3DSNamedObject& no)
		: name(no.name),
		meshMatrix(no.meshMatrix),
		pivot(no.pivot),
		nVertices(no.nVertices),
		nFaces(no.nFaces)
	{
		for (auto itMeshMat = no.meshMaterials.begin(); itMeshMat != no.meshMaterials.end(); itMeshMat++)
			meshMaterials.push_back(*itMeshMat);

		pVertices = 0;
		pFaces = 0;

		if (IS_VALID_PTR(no.pVertices) && nVertices > 0)
		{
			pVertices = new S3DSVertex[nVertices];
			memcpy(pVertices, no.pVertices, nVertices * sizeof(S3DSVertex));
		}

		if (IS_VALID_PTR(no.pFaces) && nFaces > 0)
		{
			pFaces = new S3DSFace[nFaces];
			memcpy(pFaces, no.pFaces, nFaces * sizeof(S3DSFace));
		}
	}

	~S3DSNamedObject()
	{
		if (IS_VALID_PTR(pVertices))
			delete[] pVertices;

		if (IS_VALID_PTR(pFaces))
			delete[] pFaces;

		pVertices = 0;
		nVertices = 0;
		pFaces = 0;
		nFaces = 0;
	}
};



class  File3DS
{
private:
	std::ifstream& file;
	unsigned int curChnkLvl;
	unsigned long version;
	unsigned long mesh_version;
	float master_scale;

public:
	std::vector<S3DSMaterial> materials;
	std::vector<S3DSNamedObject> objects;

	File3DS(std::ifstream& f) : file(f), curChnkLvl(0) {}

	enum { CHUNK_HEADER_SIZE = 6 };

	struct Chunk {
		unsigned long byte_pos;
		unsigned short id;
		unsigned long byte_size;
	} curChnk, mainChnk;

	void Read();

	void ReadUShort(unsigned short& s);
	void ReadULong(unsigned long& l);
	void ReadFloat(float& f);
	void ReadColor(S3DSColorF& color);
	void ReadVector(float& x, float& y, float& z);
	void ReadPercentage(float& f);

	// length - number of char's to read
	void ReadString(string& dst, unsigned long length);

	// Reads a string to the first occurence of a terminating zero.
	// *dst must not be created yet. It will be simply overriden
	void ReadStringToFirstZero(string& s);

	void ReadEditChunk();
	void ReadMaterialChunk(S3DSMaterial& mat);
	void ReadMaterialMapChunk(S3DSMaterialMap& matmap);
	void ReadNamedObjectChunk(S3DSNamedObject& obj);
	void ReadTriangledObjectChunk(S3DSNamedObject& obj);
	void ReadChunkHeader(Chunk& chnk);
	void ReadDummyChunk();

	void ReadMeshMaterial(S3DSNamedObject& obj);

	void DbgMsg(const char* fmt, ...) const;
};