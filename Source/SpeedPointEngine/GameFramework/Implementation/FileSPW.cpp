#include "FileSPW.h"
#include "..\IGameEngine.h"
#include <EntitySystem\IEntity.h>
#include <EntitySystem\IScene.h>
#include <EntitySystem\IEntitySystem.h>
#include <3DEngine\IGeometry.h>
#include <3DEngine\IMaterial.h>
#include <3DEngine\I3DEngine.h>
#include <3DEngine\IParticleSystem.h>
#include <Physics\IPhysics.h>
#include <Renderer\IResourcePool.h>
#include <Renderer\ITexture.h>
#include <Common\SerializationTools.h>
#include <Common\FileUtils.h>

SP_NMSPACE_BEG


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	if (!pComponent)
		return;

	// Convert params to a serialization container
	// TODO: Fix that.
	CBasicSerContainer ser(pComponent->GetName());
	for (auto itParam : params)
		ser.SetRaw(itParam.first, itParam.second);

	// Deserialize component
	pComponent->Serialize(&ser, false);
}

S_API void CSPWLoader::ReadAndParseEntityBlock(unsigned int blockIndent, const string& paramsExpr, unsigned int& trailingIndent)
{
	IEntitySystem* pEntitySystem = SpeedPointEnv::GetEngine()->GetEntitySystem();
	IScene* pScene = SpeedPointEnv::GetScene();

	map<string, string> params;
	params["name"] = SerializeString("entity");
	params["pos"] = SerializeVector(Vec3f(0));
	params["rot"] = SerializeQuaternion(Quat(1.0f, 0, 0, 0));
	params["scale"] = SerializeVector(Vec3f(1.0f));
	ParseParams(paramsExpr, params);

	string name = DeserializeString(params["name"]);
	IEntity* pEntity = pScene->SpawnEntity(name);

	pEntity->SetPos(DeserializeVector(params["pos"]));
	pEntity->SetRotation(DeserializeQuaternion(params["rot"]));
	pEntity->SetScale(DeserializeVector(params["scale"]));

	CLog::Log(S_DEBUG, "Loading entity '%s'...", name.c_str());


	unsigned int nextBlockIndent = ReadIndent();
	while (nextBlockIndent > blockIndent)
	{
		string nextBlockType, nextBlockParamsExpr;
		ReadAndParseBlockHeadline(nextBlockType, nextBlockParamsExpr);

		map<string, string> nextBlockParams;
		ParseParams(nextBlockParamsExpr, nextBlockParams);

		unsigned int nextBlockTrailingIndent = 0;

		if (nextBlockType == "Entity")
		{


			// TODO: Child entities..


		}
		else
		{

			// TODO: Find some way to register components in a registry instead of hardcoding them here...
			//		The problem is, that the components have to be created by the system.

			//IComponent* pComponent = pEntitySystem->CreateComponent(nextBlockType);
			IComponent *pComponent = 0;
			if (nextBlockType == "RenderMesh")
				pComponent = dynamic_cast<IComponent*>(SpeedPointEnv::Get3DEngine()->CreateMesh());
			else if (nextBlockType == "ParticleEmitter")
				pComponent = dynamic_cast<IComponent*>(SpeedPointEnv::Get3DEngine()->GetParticleSystem()->CreateEmitter());
			else if (nextBlockType == "Physical")
				pComponent = dynamic_cast<IComponent*>(SpeedPointEnv::GetPhysics()->CreatePhysObject());

			if (pComponent)
			{
				DeserializeComponent(nextBlockParams, pComponent);
				pEntity->AddComponent(pComponent);
			}
		}

		// Components can't have sub-blocks currently..
		SkipSubBlocks(nextBlockIndent, nextBlockTrailingIndent);

		nextBlockIndent = ReadIndent() + nextBlockTrailingIndent;
	}

	trailingIndent = nextBlockIndent;
}





// file - path to .raw texture file. If relative is assumed to be relative to worldDir
// worldDir - absolute resource path to a directory
ITexture* LoadRawTexture(const string& file, const string& worldDir, ETextureType type, IResourcePool* pResources)
{
	if (!pResources)
	{
		CLog::Log(S_ERROR, "Failed LoadRawTexture('%s'): ResourcePool invalid", file.c_str());
		return 0;
	}

	string sysFile;
	sysFile = pResources->GetAbsoluteResourcePath(file, worldDir);
	sysFile = pResources->GetResourceSystemPath(sysFile);

	std::ifstream stream;
	stream.open(sysFile, stream.in | stream.binary);
	if (!stream.good())
	{
		CLog::Log(S_ERROR, "Failed LoadRawTexture('%s'): Cannot open file", sysFile.c_str());
		return 0;
	}

	unsigned int sz[2];
	unsigned int bitsPerPixel;
	stream.read((char*)&sz[0], sizeof(unsigned int));
	stream.read((char*)&sz[1], sizeof(unsigned int));
	stream.read((char*)&bitsPerPixel, sizeof(unsigned int));

	ITexture* pTex = pResources->GetTexture(file);
	if (Failure(pTex->CreateEmpty(file, sz[0], sz[1], 1, type, SColor::Black())))
	{
		CLog::Log(S_ERROR, "Failed LoadRawTexture('%s'): Failed CreateEmpty()", sysFile.c_str());
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
		CLog::Log(S_ERROR, "Failed LoadRawTexture('%s'): Cannot lock texture", sysFile.c_str());
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

	IResourcePool* pResources = m_p3DEngine->GetRenderer()->GetResourcePool();

	STerrainLayer layer;
	layer.pDetailMap = pResources->GetTexture(DeserializeString(params["detailmap"]));
	layer.pAlphaMask = LoadRawTexture(DeserializeString(params["alphamap"]), m_WorldFileDir, eTEXTURE_R8G8B8A8_UNORM, pResources);

	pTerrain->AddLayer(layer);


	SkipSubBlocks(blockIndent, trailingIndent);
}

S_API void CSPWLoader::ReadAndParseTerrainBlock(unsigned int blockIndent, const string& paramsExpr, unsigned int& trailingIndent)
{
	map<string, string> params;
	params["heightmap"]		= "\"\"";
	params["colormap"]		= "\"\"";
	params["segments"]		= "512";
	params["chunkSegments"]	= "16";
	params["size"]			= "512.0f";
	params["baseHeight"]	= "0.0f";
	params["chunkStepDist"]	= "20.0f";
	params["lodLevels"]		= "4";
	params["center"]		= "true";
	ParseParams(paramsExpr, params);

	STerrainParams terrain;
	terrain.segments		= DeserializeUInt(params["segments"]);
	terrain.chunkSegments	= DeserializeUInt(params["chunkSegments"]);
	terrain.size			= DeserializeFloat(params["size"]);
	terrain.baseHeight		= DeserializeFloat(params["baseHeight"]);
	terrain.fChunkStepDist	= DeserializeFloat(params["chunkStepDist"]);
	terrain.nLodLevels		= DeserializeUInt(params["lodLevels"]);
	terrain.center			= DeserializeBool(params["center"]);
	terrain.detailmapRange	= 20.0f;

	IResourcePool* pResources = m_p3DEngine->GetRenderer()->GetResourcePool();

	m_p3DEngine->ClearTerrain();
	ITerrain* pTerrain = m_p3DEngine->CreateTerrain(terrain);
	
	pTerrain->SetHeightScale(10.0f);
	pTerrain->SetHeightmap(LoadRawTexture(DeserializeString(params["heightmap"]), m_WorldFileDir, eTEXTURE_R32_FLOAT, pResources));
	
	// Initialize terrain proxy
	Vec2f minXZ = pTerrain->GetMinXZ();
	SPhysTerrainParams physTerrain;
	physTerrain.heightScale	= pTerrain->GetHeightScale();
	physTerrain.offset		= Vec3f(minXZ.x, 0, minXZ.y);
	physTerrain.segments[0]	= terrain.segments; // Currently using the same resolution for rendering and proxy mesh
	physTerrain.segments[1]	= terrain.segments;
	physTerrain.size[0]		= terrain.size;
	physTerrain.size[1]		= terrain.size;

	unsigned int heightmapSz[2];
	pTerrain->GetHeightmap()->GetSize(&heightmapSz[0], &heightmapSz[1]);
	SpeedPointEnv::GetPhysics()->CreateTerrainProxy(pTerrain->GetHeightData(), heightmapSz, physTerrain);
	
	
	pTerrain->SetColorMap(LoadRawTexture(DeserializeString(params["colormap"]), m_WorldFileDir, eTEXTURE_R8G8B8A8_UNORM, pResources));

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
			ReadAndParseEntityBlock(nextBlockIndent, nextBlockParamsExpr, nextBlockTrailingIndent);
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
		ReadAndParseWorldBlock(baseIndent, m_pScene, blockParamsExpr, blockTrailingIndent);
	}

	// We only support 1 World as of now
	return;
}





S_API void CSPWLoader::Load(I3DEngine* p3DEngine, IScene* pScene, const string& worldFile)
{
	m_p3DEngine = p3DEngine;
	m_pScene = pScene;

	m_WorldFile = worldFile;
	m_WorldFileDir = GetDirectoryPath(worldFile);

	string file = SpeedPointEnv::GetEngine()->GetResourceSystemPath(m_WorldFile);
	m_Stream.open(file.c_str(), ifstream::in);
	if (m_Stream.good())
	{
		ReadAndParseFile();
		m_Stream.close();
	}
	else
	{
		CLog::Log(S_ERROR, "CSPWLoader: Cannot open file '%s'", file.c_str());
	}
}

SP_NMSPACE_END
