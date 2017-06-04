#pragma once

#include <FileSPM.h>
#include <Common\Vector3.h>
#include <Common\Quaternion.h>
#include <Common\SPrerequisites.h>
#include <Common\SerializationTools.h>
#include <fstream>
#include <map>
#include <string>

using std::map;
using std::string;

SP_NMSPACE_BEG

struct S_API IScene;
struct S_API I3DEngine;
struct S_API IEntity;
struct S_API IComponent;
struct S_API ITerrain;
struct S_API SInitialGeometryDesc;

#define SPW_MAX_LINE_LENGTH 500

class S_API CSPWSerContainer : public ISerContainer
{
private:
	vector<CSPWSerContainer> m_Children;
	vector<ISerContainer*> m_ChildPointers;
	map<string, string> m_Attributes;

public:
	CSPWSerContainer(const string& name);

	virtual ISerContainer* CreateChildContainer(const string& name);
	virtual const vector<ISerContainer*>& GetChildContainers() const;
	virtual unsigned short GetNumChildren() const;
	virtual void DestroyChildContainer(const string& name);

	virtual unsigned short GetNumAttributes() const;
	virtual const map<string, string>& GetAttributes() const { return m_Attributes; }

	virtual int GetInt(const string& attrname, int def = INT_MAX);
	virtual unsigned int GetUInt(const string& attrname, unsigned int def = UINT_MAX);
	virtual float GetFloat(const string& attrname, float def = FLT_MAX);
	virtual string GetString(const string& attrname, const string& def = "???");
	virtual Vec3f GetVec3f(const string& attrname, const Vec3f& def = Vec3f());

	virtual void SetRaw(const string& attrname, const string& val);
	virtual void SetInt(const string& attrname, int val);
	virtual void SetUInt(const string& attrname, unsigned int val);
	virtual void SetFloat(const string& attrname, float val);
	virtual void SetString(const string& attrname, const string& val);
	virtual void SetVec3f(const string& attrname, const Vec3f& val);
};

class S_API CSPWLoader
{
private:
	std::ifstream m_Stream;
	string m_WorldFile; // abs resource path
	string m_WorldFileDir; // abs resource path
	I3DEngine* m_p3DEngine;
	IScene* m_pScene;

	void LogError(const char* msg) const;

	unsigned int ReadIndent();
	void ReadToEOL(string& content);
	void ParseParams(const string& expr, map<string, string>& params);
	void ReadAndParseBlockHeadline(string& blockType, string& nextBlockParamsExpr);
	void SkipSubBlocks(unsigned int blockIndent, unsigned int& trailingIndent);

	void DeserializeComponent(const map<string, string>& params, IComponent* pComponent) const;
	void ReadAndParseEntityBlock(unsigned int blockIndent, const string& paramsExpr, unsigned int& trailingIndent);

	void ReadAndParseTerrainLayerBlock(unsigned int blockIndent, ITerrain* pTerrain, const string& paramsExpr, unsigned int& trailingIndent);
	void ReadAndParseTerrainBlock(unsigned int blockIndent, const string& paramsExpr, unsigned int& trailingIndent);

	void ReadAndParseWorldBlock(unsigned int blockIndent, IScene* pWorld, const string& paramsExpr, unsigned int& trailingIndent);

	void ReadAndParseFile();

public:
	// file - absolute resource path to SPW file
	void Load(I3DEngine* p3DEngine, IScene* pScene, const string& worldFile);
};

SP_NMSPACE_END
