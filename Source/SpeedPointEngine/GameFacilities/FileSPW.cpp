#include "FileSPW.h"
#include <Abstract\IEntity.h>
#include <Abstract\ISettings.h>
#include <Abstract\IGeometry.h>
#include <Abstract\IMaterial.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IPhysics.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\ITexture.h>
#include <Abstract\IScene.h>
#include <Abstract\SerializationTools.h>

SP_NMSPACE_BEG

S_API void CSPWLoader::LogError(const char* msg) const
{
	CLog::Log(S_ERROR, "SPW Load Error: %s", msg);
}



// If the delimiter was not found, left will be set to the whole input string
inline void split_at_first_occurence(const string& str, char delim, string& left, string& right)
{
	size_t occurence = str.find_first_of(delim);
	if (occurence == str.npos)
	{
		left = str;
		return;
	}

	left = str.substr(0, occurence);
	right = str.substr(occurence + 1);
}

inline vector<string> whitespace_explode(const string& str)
{
	vector<string> parts;
	size_t offs = 0;
	while (true)
	{
		size_t start = str.find_first_not_of(" \t", offs);
		if (start == str.npos)
			break;

		size_t end = str.find_first_of(" \t", start);
		parts.push_back(str.substr(start, end - start));
		if (end == str.npos)
			break;

		offs = end;
	}

	return parts;
}

S_API void CSPWLoader::ParseParams(const string& expr, map<string, string>& params)
{
	string key, val;
	vector<string> paramExpressions = whitespace_explode(expr);
	for (auto itParamExpr = paramExpressions.begin(); itParamExpr != paramExpressions.end(); ++itParamExpr)
	{
		split_at_first_occurence(*itParamExpr, '=', key, val);
		if (key.empty())
		{
			LogError("Invalid param");
			continue;
		}

		params[key] = val;
	}
}






S_API unsigned int CSPWLoader::ReadIndent()
{
	unsigned int indent = 0;
	char c;
	while (true)
	{
		c = m_Stream.get();
		if (c == ' ' || c == '\r')
			continue;
		else if (c == '\t')
			++indent;
		else
		{
			m_Stream.seekg(m_Stream.tellg() - std::streampos(1));
			return indent;
		}
	}
}

S_API void CSPWLoader::ReadToEOL(string& content)
{
	char buff[SPW_MAX_LINE_LENGTH];
	m_Stream.getline(buff, SPW_MAX_LINE_LENGTH);
	content = buff;
}


S_API void CSPWLoader::ReadAndParseBlockHeadline(string& blockType, string& nextBlockParamsExpr)
{
	string headline;
	ReadToEOL(headline);

	if (headline.empty())
		return;

	// Read block type
	size_t firstSpace = headline.find_first_of(" \t");
	size_t firstParam = headline.find_first_not_of(" \t", firstSpace + 1);
	if (firstSpace == string::npos || firstParam == string::npos)
	{
		blockType = headline; // may be empty
		return; // no params
	}

	blockType = headline.substr(0, firstSpace);
	nextBlockParamsExpr = headline.substr(firstParam);
}




// Skips lines until the next line with the given indent
// Assumes, that the current pointer is at the beginning of the next
// line after the parent block's header line.
S_API void CSPWLoader::SkipSubBlocks(unsigned int blockIndent, unsigned int& trailingIndent)
{
	unsigned int indent;
	char tmp[SPW_MAX_LINE_LENGTH];
	while (true)
	{
		indent = ReadIndent();
		if (indent <= blockIndent)
		{
			trailingIndent = indent;
			return;
		}

		m_Stream.getline(tmp, SPW_MAX_LINE_LENGTH);
	}
}








S_API void CSPWLoader::DeserializeComponent(const map<string, string>& params, IComponent* pComponent) const
{
	pComponent->Deserialize(params);
}

S_API void CSPWLoader::ReadAndParseEntityBlock(unsigned int blockIndent, IEntity* pEntity, const string& paramsExpr, unsigned int& trailingIndent)
{
	map<string, string> params;
	params["name"] = "\"entity\"";
	params["pos"] = "(0,0,0)";
	params["rot"] = "(0,0,0,1.0f)";
	params["scale"] = "(1.0f,1.0f,1.0f)";
	ParseParams(paramsExpr, params);

	pEntity->SetName(DeserializeString(params["name"]).c_str());
	pEntity->SetPos(DeserializeVector(params["pos"]));
	pEntity->SetRotation(DeserializeQuaternion(params["rot"]));
	pEntity->SetScale(DeserializeVector(params["scale"]));

	IGameEngine* pEngine = SpeedPointEnv::GetEngine();

	unsigned int nextBlockIndent = ReadIndent();
	while (nextBlockIndent > blockIndent)
	{
		string nextBlockType, nextBlockParamsExpr;
		ReadAndParseBlockHeadline(nextBlockType, nextBlockParamsExpr);

		map<string, string> nextBlockParams;
		ParseParams(nextBlockParamsExpr, nextBlockParams);

		unsigned int nextBlockTrailingIndent = 0;
		if (nextBlockType == "Renderable") DeserializeComponent(nextBlockParams, pEntity->AddComponent(pEngine->Get3DEngine()->CreateMesh()));
		else if (nextBlockType == "Physical") DeserializeComponent(nextBlockParams, pEntity->AddComponent(pEngine->GetPhysics()->CreatePhysObject()));
		else SkipSubBlocks(nextBlockIndent, nextBlockTrailingIndent);

		nextBlockIndent = ReadIndent() + nextBlockTrailingIndent;
	}

	trailingIndent = nextBlockIndent;
}





// file - relative to rootDir
ITexture* LoadRawTexture(const string& file, ETextureType type)
{
	IResourcePool* pResources = SpeedPointEnv::GetEngine()->GetResources();
	if (!pResources)
	{
		CLog::Log(S_ERROR, "Failed LoadRawTexture('%s'): ResourcePool not initialized", file.c_str());
		return 0;
	}

	std::ifstream stream;
	stream.open(SpeedPointEnv::GetEngine()->GetSettings()->Get().resources.rootDir + "\\" + file, stream.in | stream.binary);
	if (!stream.good())
	{
		CLog::Log(S_ERROR, "Failed LoadRawTexture('%s'): Cannot open file", file.c_str());
		return 0;
	}

	unsigned int sz[2];
	unsigned int bitsPerPixel;
	stream.read((char*)&sz[0], sizeof(unsigned int));
	stream.read((char*)&sz[1], sizeof(unsigned int));
	stream.read((char*)&bitsPerPixel, sizeof(unsigned int));

	ITexture* pTex;
	if (Failure(pResources->AddTexture(file, &pTex, sz[0], sz[1], 1, type, SColor())))
	{
		CLog::Log(S_ERROR, "Failed LoadRawTexture('%s'): Cannot AddTexture()", file.c_str());
		stream.close();
		return 0;
	}

	void* pPixels;
	unsigned int nPixels, nRowPitch;
	if (Success(pTex->Lock(&pPixels, &nPixels, &nRowPitch)))
	{
		unsigned int numReadPixels = 0;
		while (!stream.eof() && numReadPixels < nPixels)
		{
			unsigned int pixelsToRead = nRowPitch;
			if (numReadPixels + pixelsToRead > nPixels)
				pixelsToRead = (nPixels - numReadPixels);

			if (type == eTEXTURE_D32_FLOAT || type == eTEXTURE_R32_FLOAT)
				stream.read((char*)&((float*)pPixels)[numReadPixels], pixelsToRead * sizeof(float));
			else
				stream.read((char*)&((unsigned int*)pPixels)[numReadPixels], pixelsToRead * sizeof(unsigned int));

			numReadPixels += pixelsToRead;
		}

		pTex->Unlock();
	}
	else
	{
		CLog::Log(S_ERROR, "Failed LoadRawTexture('%s'): Cannot lock texture", file.c_str());
		stream.close();
		return 0;
	}

	stream.close();
	return pTex;
}


S_API void CSPWLoader::ReadAndParseTerrainLayerBlock(unsigned int blockIndent, ITerrain* pTerrain, const string& paramsExpr, unsigned int& trailingIndent)
{
	map<string, string> params;
	params["detailmap"] = "\"\"";
	params["alphamap"] = "\"\"";
	ParseParams(paramsExpr, params);

	IResourcePool* pResources = SpeedPointEnv::GetEngine()->GetResources();

	STerrainLayer layer;
	layer.pDetailMap = pResources->GetTexture(DeserializeString(params["detailmap"]));
	layer.pAlphaMask = LoadRawTexture(DeserializeString(params["alphamap"]), eTEXTURE_R8G8B8A8_UNORM);

	pTerrain->AddLayer(layer);


	SkipSubBlocks(blockIndent, trailingIndent);
}

S_API void CSPWLoader::ReadAndParseTerrainBlock(unsigned int blockIndent, const string& paramsExpr, unsigned int& trailingIndent)
{
	map<string, string> params;
	params["heightmap"] = "\"\"";
	params["colormap"] = "\"\"";
	params["segments"] = "512";
	params["chunkSegments"] = "16";
	params["size"] = "512.0f";
	params["baseHeight"] = "0.0f";
	params["chunkStepDist"] = "20.0f";
	params["lodLevels"] = "4";
	params["center"] = "true";
	ParseParams(paramsExpr, params);

	STerrainInfo terrain;
	terrain.segments		= DeserializeUInt(params["segments"]);
	terrain.chunkSegments	= DeserializeUInt(params["chunkSegments"]);
	terrain.size			= DeserializeFloat(params["size"]);
	terrain.baseHeight		= DeserializeFloat(params["baseHeight"]);
	terrain.fChunkStepDist	= DeserializeFloat(params["chunkStepDist"]);
	terrain.nLodLevels		= DeserializeUInt(params["lodLevels"]);
	terrain.center			= DeserializeBool(params["center"]);

	I3DEngine* p3DEngine = SpeedPointEnv::GetEngine()->Get3DEngine();
	IResourcePool* pResources = SpeedPointEnv::GetEngine()->GetResources();

	p3DEngine->ClearTerrain();
	ITerrain* pTerrain = p3DEngine->CreateTerrain(terrain);
	
	pTerrain->SetHeightScale(10.0f);
	pTerrain->CalculateProxyMesh(5);

	pTerrain->SetHeightmap(LoadRawTexture(DeserializeString(params["heightmap"]), eTEXTURE_R32_FLOAT));
	pTerrain->SetColorMap(LoadRawTexture(DeserializeString(params["colormap"]), eTEXTURE_R8G8B8A8_UNORM));


	unsigned int nextBlockIndent = ReadIndent();
	while (nextBlockIndent > blockIndent)
	{
		string nextBlockType, nextBlockParamsExpr;
		ReadAndParseBlockHeadline(nextBlockType, nextBlockParamsExpr);

		unsigned int nextBlockTrailingIndent = 0;
		if (nextBlockType == "Layer")
			ReadAndParseTerrainLayerBlock(nextBlockIndent, pTerrain, nextBlockParamsExpr, nextBlockTrailingIndent);
		else
			SkipSubBlocks(nextBlockIndent, nextBlockTrailingIndent);

		nextBlockIndent = ReadIndent() + nextBlockTrailingIndent;
	}

	trailingIndent = nextBlockIndent;
}







S_API void CSPWLoader::ReadAndParseWorldBlock(unsigned int blockIndent, IScene* pWorld, const string& paramsExpr, unsigned int& trailingIndent)
{
	map<string, string> params;
	params["name"] = "\"world\"";
	ParseParams(paramsExpr, params);

	pWorld->SetName(DeserializeString(params["name"]));

	unsigned int nextBlockIndent = ReadIndent();
	while (nextBlockIndent > blockIndent)
	{
		string nextBlockType, nextBlockParamsExpr;
		ReadAndParseBlockHeadline(nextBlockType, nextBlockParamsExpr);

		unsigned int nextBlockTrailingIndent = 0;
		if (nextBlockType == "Entity")
		{
			IEntity* pEntity = pWorld->SpawnEntity();
			ReadAndParseEntityBlock(nextBlockIndent, pEntity, nextBlockParamsExpr, nextBlockTrailingIndent);
			pWorld->AddObject(pEntity);
		}
		else if (nextBlockType == "Terrain")
		{
			ReadAndParseTerrainBlock(nextBlockIndent, nextBlockParamsExpr, nextBlockTrailingIndent);
		}
		else
		{
			SkipSubBlocks(nextBlockIndent, nextBlockTrailingIndent);
		}

		nextBlockIndent = ReadIndent() + nextBlockTrailingIndent;
	}

	trailingIndent = nextBlockIndent;
}




S_API void CSPWLoader::ReadAndParseFile()
{
	unsigned int baseIndent = ReadIndent();

	string blockType, blockParamsExpr;
	ReadAndParseBlockHeadline(blockType, blockParamsExpr);

	unsigned int blockTrailingIndent = 0;
	if (blockType == "World")
	{
		ReadAndParseWorldBlock(baseIndent, SpeedPointEnv::GetEngine()->GetScene(), blockParamsExpr, blockTrailingIndent);
	}

	// We only support 1 World as of now
	return;
}





S_API void CSPWLoader::Load(const string& file)
{
	m_Stream.open(file.c_str(), ifstream::in);
	if (m_Stream.good())
	{
		ReadAndParseFile();
		m_Stream.close();
	}
	else
	{
		LogError("Cannot open file");
	}
}

SP_NMSPACE_END
