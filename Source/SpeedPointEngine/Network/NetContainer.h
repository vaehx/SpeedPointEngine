//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Common\SerializationTools.h>
#include <Common\SPrerequisites.h>
#include <string>
#include <map>

using std::string;
using std::map;

SP_NMSPACE_BEG


/*

	+------------------+	Container Header:
	| Container Header |		16B Name
	+------------------+		 4B Size (Excluding Header)
	|                  |		 2B Number of attributes
	| Attributes       |		 2B Number of child containers
	|                  |
	+------------------+	Attribute Header:
	|                  |		16B Name
	| Child Containers |		 2B Buffer length
	|                  |		 2B Type
	+------------------+

*/

struct NetRawContainer
{
	struct {
		char name[16];
		unsigned long bufsz;
		unsigned short numAttributes;
		unsigned short numChildContainers;
	} header;
	char* buffer;
};

struct NetRawAttribute
{
	struct {
		char name[16];
		unsigned short bufsz;
		unsigned short type;
	} header;
	char* buffer;
};


//////////////////////////////////////////////////////////////////////////////////////////////////

class NetContainer : public ISerContainer
{
private:
	enum AttributeType
	{
		ATTR_TYPE_INT = 0,
		ATTR_TYPE_UINT,
		ATTR_TYPE_FLOAT,
		ATTR_TYPE_STRING,
		ATTR_TYPE_VEC3f
	};

	struct AttributeValue
	{
		AttributeType type;
		unsigned short bufsz;
		char buffer[256];
	};
	map<string, AttributeValue> m_Attributes;

	vector<NetContainer> m_Children;
	vector<ISerContainer*> m_ChildrenPointers;

	void RebuildChildPointerCache();

public:
	NetContainer(const string& name);
	~NetContainer();

	virtual ISerContainer* CreateChildContainer(const string& name);
	virtual const vector<ISerContainer*>& GetChildContainers() const;
	virtual unsigned short GetNumChildren() const;
	virtual void DestroyChildContainer(const string& name);

	virtual unsigned short GetNumAttributes() const;

	virtual int GetInt(const string& attrname, int def = INT_MAX);
	virtual unsigned int GetUInt(const string& attrname, unsigned int def = UINT_MAX);
	virtual float GetFloat(const string& attrname, float def = FLT_MAX);
	virtual string GetString(const string& attrname, const string& def = "???");
	virtual Vec3f GetVec3f(const string& attrname, const Vec3f& def = Vec3f());

	virtual void SetInt(const string& attrname, int val);
	virtual void SetUInt(const string& attrname, unsigned int val);
	virtual void SetFloat(const string& attrname, float val);
	virtual void SetString(const string& attrname, const string& val);
	virtual void SetVec3f(const string& attrname, const Vec3f& val);

	// Outputs the whole container as a single bytestream
	void Output(char** pBuffer, unsigned long* pBufSz);

	// Reads the container and child containers from the given binary buffer
	// Returns false on failure and logs the error
	bool Read(const char* buffer, unsigned long bufsz);
};

SP_NMSPACE_END
