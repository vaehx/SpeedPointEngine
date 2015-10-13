#pragma once
#include "util.h"
#include <string>
#include <vector>
#include <fstream>
#include <Abstract\SAPI.h>


using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;

#ifndef SP_NMSPACE_BEG
#define SP_NMSPACE_BEG namespace SpeedPoint {
#define SP_NMSPACE_END }
#endif

#ifndef IS_VALID_PTR
#define IS_VALID_PTR(x) ((x))
#endif

SP_NMSPACE_BEG

// DO NOT CHANGE THE ORDER!
enum S_API ESMBIllumModel
{
	eSMBILLUM_BLINN_PHONG = 0,
	eSMBILLUM_COOK_TORRANCE = 1,
	eSMBILLUM_HELPER = 2
};

struct SSMBMaterial
{
	string name;
	string textureMap;
	string normalMap;
	string roughnessMap;
	float roughness;
	Vec3f emissive;
	Vec3f diffuse;
	ESMBIllumModel illumModel;

	// ...

};

enum  ESMBReadResult
{
	eSMBREAD_OK,
	eSMBREAD_EOF,
	eSMBREAD_ENDOFBLOCK,
	eSMBREAD_ENDOFLINE,

	// The given character was found immediately after the start position
	eSMBREAD_EMPTY,

	// The read chunk was detected invalid and was successfully skipped
	eSMBREAD_SKIPPED
};

class  CSMBLoader
{
private:
	unsigned int m_LineCounter;
	ifstream m_Stream;

public:
	void ParseError(const char* msg) const;
	bool IsEOF() const;
	char ReadChar();
	ESMBReadResult ReadUntilNext(char until, string *pOut = 0, const vector<char>* pSkip = 0, bool checkBrackets = false, int startBracketCounter = 0);
	ESMBReadResult ReadUntilNext(const vector<char>& until, char* pFoundChar = 0, string *pOut = 0, vector<char>* pSkip = 0, bool checkBrackets = false, int startBracketCounter = 0);
	ESMBReadResult ReadUntilClosingBracket(int counterStart = 0);
	ESMBReadResult GetNextChunkHeader(vector<string> &chunkHeaderParams);
	ESMBReadResult ReadNextProperty(string& name, string& value);

	static bool ParseString(const string& i, string& o);
	static bool ParseVec3(const string& i, Vec3f& o);
	static string TrimString(const string& i);

	void ReadSMBFile(const char* filename, std::vector<SSMBMaterial>& materials);
};

class  CSMBWriter
{
private:
	ofstream m_Stream;
	unsigned int m_nOpenChunks;

	inline void WriteIndent();
	inline void WritePropertyLineIntrnl(const string& name, const string& value);

public:
	static string SerializeVec3(float x, float y, float z);

	void ParseError(const char* msg) const;
	bool Prepare(const char* filename);
	void Close();

	// header: space-separated list of chunk header parameters (the chunk brackets are added by the function)
	void OpenChunk(const string& header);

	void CloseChunk();

	void WritePropertyLine(const string& name, const string& strvalue);
	void WritePropertyLine(const string& name, unsigned int a);
	void WritePropertyLine(const string& name, float a);
	void WritePropertyLine(const string& name, float x, float y, float z);
};



enum ESMBChunkType
{
	eSMBCHUNK_MATERIAL
};

struct  ISMBChunk
{
public:
	inline virtual ESMBChunkType GetType() const = 0;

	virtual bool CheckParameters(const vector<string>& params) const = 0;
	virtual bool SetProperty(const string& name, const string& value) = 0;
	virtual void Create(const string& name) = 0;
};

class  CSMBTestMaterialChunk : public ISMBChunk
{
public:
	CSMBTestMaterialChunk();

	inline virtual ESMBChunkType GetType() const { return eSMBCHUNK_MATERIAL; };

	virtual bool CheckParameters(const vector<string>& params) const;
	virtual bool SetProperty(const string& name, const string& value);
	virtual void Create(const string& name);

	static void Write(CSMBWriter* pWriter, const SSMBMaterial& material);
};

class  CSMBMaterialChunk : public ISMBChunk
{
private:
	SSMBMaterial m_Material;
public:
	CSMBMaterialChunk();

	inline virtual ESMBChunkType GetType() const { return eSMBCHUNK_MATERIAL; };

	virtual bool CheckParameters(const vector<string>& params) const;
	virtual bool SetProperty(const string& name, const string& value);
	virtual void Create(const string& name);

	inline SSMBMaterial GetMaterial() const { return m_Material; }

	static void Write(CSMBWriter* pWriter, const SSMBMaterial& material);
};


SP_NMSPACE_END