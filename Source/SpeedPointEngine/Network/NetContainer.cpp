//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "NetContainer.h"

SP_NMSPACE_BEG

S_API NetContainer::NetContainer(const string& name)
	: ISerContainer(name)
{
}

S_API NetContainer::~NetContainer()
{
}

S_API ISerContainer* NetContainer::CreateChildContainer(const string& name)
{
	for (auto itChild = m_Children.begin(); itChild != m_Children.end(); ++itChild)
	{
		if (itChild->GetName() == name)
			return static_cast<ISerContainer*>(&*itChild);
	}

	m_Children.push_back(NetContainer(name));
	RebuildChildPointerCache();

	return &m_Children.back();
}

S_API const vector<ISerContainer*>& NetContainer::GetChildContainers() const
{
	return m_ChildrenPointers;
}

S_API void NetContainer::DestroyChildContainer(const string& name)
{
	for (auto itChild = m_Children.begin(); itChild != m_Children.end(); ++itChild)
	{
		if (itChild->GetName() == name)
		{
			m_Children.erase(itChild);
			RebuildChildPointerCache();
			break;
		}
	}
}

S_API unsigned short NetContainer::GetNumChildren() const
{
	return m_Children.size();
}

S_API void NetContainer::RebuildChildPointerCache()
{
	m_ChildrenPointers.clear();
	for (auto itChild = m_Children.begin(); itChild != m_Children.end(); ++itChild)
	{
		m_ChildrenPointers.push_back(static_cast<ISerContainer*>(&*itChild));
	}
}

S_API unsigned short NetContainer::GetNumAttributes() const
{
	return m_Attributes.size();
}

// -------------------------------------------------------------------------------

#define GET_ATTR_OR_RETURN_DEFAULT(typenm, minBufSz) \
	auto itAttr = m_Attributes.find(attrname); \
	if (itAttr == m_Attributes.end()) \
		return def; \
	AttributeValue& attr = itAttr->second; \
	if (attr.type != typenm || !attr.buffer || attr.bufsz < minBufSz) \
		return def;

S_API int NetContainer::GetInt(const string& attrname, int def /*= INT_MAX*/)
{
	GET_ATTR_OR_RETURN_DEFAULT(ATTR_TYPE_INT, sizeof(int));
	return *reinterpret_cast<int*>(attr.buffer);
}

S_API unsigned int NetContainer::GetUInt(const string& attrname, unsigned int def /*= UINT_MAX*/)
{
	GET_ATTR_OR_RETURN_DEFAULT(ATTR_TYPE_UINT, sizeof(unsigned int));
	return *reinterpret_cast<unsigned int*>(attr.buffer);
}

S_API float NetContainer::GetFloat(const string& attrname, float def /*= FLT_MAX*/)
{
	GET_ATTR_OR_RETURN_DEFAULT(ATTR_TYPE_FLOAT, sizeof(float));
	return *reinterpret_cast<float*>(attr.buffer);
}

S_API string NetContainer::GetString(const string& attrname, const string& def /*= "???"*/)
{
	GET_ATTR_OR_RETURN_DEFAULT(ATTR_TYPE_STRING, 0);

	// Make sure the buffer has a terminating \0
	attr.buffer[attr.bufsz - 1] = '\0';

	return string(attr.buffer);
}

S_API Vec3f NetContainer::GetVec3f(const string& attrname, const Vec3f& def /*= Vec3f()*/)
{
	GET_ATTR_OR_RETURN_DEFAULT(ATTR_TYPE_STRING, sizeof(Vec3f));
	return *reinterpret_cast<Vec3f*>(attr.buffer);
}

// -------------------------------------------------------------------------------

#define NC_GET_ATTR() AttributeValue& attr = m_Attributes[attrname]

S_API void NetContainer::SetInt(const string& attrname, int val)
{
	NC_GET_ATTR();
	attr.type = ATTR_TYPE_INT;
	attr.bufsz = sizeof(int);
	*reinterpret_cast<int*>(attr.buffer) = val;
}

S_API void NetContainer::SetUInt(const string& attrname, unsigned int val)
{
	NC_GET_ATTR();
	attr.type = ATTR_TYPE_UINT;
	attr.bufsz = sizeof(unsigned int);
	*reinterpret_cast<unsigned int*>(attr.buffer) = val;
}

S_API void NetContainer::SetFloat(const string& attrname, float val)
{
	NC_GET_ATTR();
	attr.type = ATTR_TYPE_FLOAT;
	attr.bufsz = sizeof(float);
	*reinterpret_cast<float*>(attr.buffer) = val;
}

S_API void NetContainer::SetString(const string& attrname, const string& val)
{
	NC_GET_ATTR();
	attr.type = ATTR_TYPE_STRING;
	attr.bufsz = min(256, val.length() - 1);
	memcpy(attr.buffer, val.c_str(), attr.bufsz);
	attr.buffer[attr.bufsz - 1] = '\0';
}

S_API void NetContainer::SetVec3f(const string& attrname, const Vec3f& val)
{
	NC_GET_ATTR();
	attr.type = ATTR_TYPE_VEC3f;
	attr.bufsz = sizeof(Vec3f);
	*reinterpret_cast<Vec3f*>(attr.buffer) = val;
}

// -------------------------------------------------------------------------------



S_API void NetContainer::Output(char** pBuffer, unsigned long* pBufSz)
{
	struct ChildBuffer
	{
		unsigned long bufsz;
		char* buffer;
	};

	if (!pBuffer || !pBufSz)
		return;

	unsigned long bufferSz = 0;

	// Output children
	vector<ChildBuffer> childBuffers;
	if (!m_Children.empty())
	{
		for (auto itChild = m_Children.begin(); itChild != m_Children.end(); ++itChild)
		{
			ChildBuffer child;
			itChild->Output(&child.buffer, &child.bufsz);

			childBuffers.push_back(child);

			bufferSz += child.bufsz;
		}
	}

	// Determine attribute sizes
	vector<NetRawAttribute> attributes;
	for (auto itAttribute = m_Attributes.begin(); itAttribute != m_Attributes.end(); ++itAttribute)
	{
		NetRawAttribute attrib;

		memcpy(attrib.header.name, itAttribute->first.c_str(), min(16, itAttribute->first.length()));
		attrib.header.name[15] = '\0';
		attrib.header.type = (unsigned short)itAttribute->second.type;
		attrib.header.bufsz = itAttribute->second.bufsz;

		attrib.buffer = itAttribute->second.buffer;

		attributes.push_back(attrib);

		bufferSz += sizeof(attrib.header) + attrib.header.bufsz;
	}

	char* buffer = new char[bufferSz];
	unsigned long filledBufferSz = 0;

	// Fill container header
	NetRawContainer* rawContainer = reinterpret_cast<NetRawContainer*>(buffer);

	memcpy(rawContainer->header.name, m_Name.c_str(), min(16, m_Name.length()));
	rawContainer->header.name[15] = '\0';
	rawContainer->header.bufsz = bufferSz - sizeof(rawContainer->header);
	rawContainer->header.numAttributes = GetNumAttributes();
	rawContainer->header.numChildContainers = GetNumChildren();

	// Fill attributes
	for (auto itAttrib = attributes.begin(); itAttrib != attributes.end(); ++itAttrib)
	{
		NetRawAttribute& attrib = *itAttrib;

		memcpy(buffer + filledBufferSz, &attrib.header, sizeof(attrib.header));
		filledBufferSz += sizeof(attrib.header);

		memcpy(buffer + filledBufferSz, attrib.buffer, attrib.header.bufsz);
		filledBufferSz += attrib.header.bufsz;
	}

	// Fill child buffers
	if (!childBuffers.empty())
	{
		for (auto itChildBuffer = childBuffers.begin(); itChildBuffer != childBuffers.end(); ++itChildBuffer)
		{
			ChildBuffer& child = *itChildBuffer;

			memcpy(buffer + filledBufferSz, child.buffer, child.bufsz);
			filledBufferSz += child.bufsz;
		}
	}

	*pBuffer = buffer;
	*pBufSz = filledBufferSz;
}

// -------------------------------------------------------------------------------

S_API bool NetContainer::Read(const char* buffer, unsigned long bufsz)
{
	if (!buffer || bufsz == 0)
	{
		CLog::Log(S_ERROR, "Failed NetContainer::Read(): Invalid parameters");
		return false;
	}

	const char* ptr = buffer;

	// Read container header
	const NetRawContainer* rawContainer = reinterpret_cast<const NetRawContainer*>(buffer);
	ptr += sizeof(rawContainer->header);

	if ((unsigned long)(ptr - buffer) > bufsz)
	{
		CLog::Log(S_ERROR, "Failed NetContainer::Read(): Buffer overflow while reading container header");
		return false;
	}

	m_Name = rawContainer->header.name;

	// Read attributes
	for (unsigned short iAttrib = 0; iAttrib < rawContainer->header.numAttributes; ++iAttrib)
	{
		const NetRawAttribute& rawAttrib = *reinterpret_cast<const NetRawAttribute*>(ptr);
		ptr += sizeof(rawAttrib.header) + rawAttrib.header.bufsz;

		if ((unsigned long)(ptr - buffer) > bufsz)
		{
			CLog::Log(S_ERROR, "Failed NetContainer::Read(): Buffer overflow while reading attribute header");
			return false;
		}

		AttributeValue attrib;
		attrib.type = (AttributeType)rawAttrib.header.type;
		attrib.bufsz = rawAttrib.header.bufsz;
		memcpy(attrib.buffer, rawAttrib.buffer, attrib.bufsz);

		m_Attributes[rawAttrib.header.name] = attrib;
	}

	// Read child containers
	m_Children.reserve(rawContainer->header.numChildContainers);
	for (unsigned short iChild = 0; iChild < rawContainer->header.numChildContainers; ++iChild)
	{
		const NetRawContainer& rawChild = *reinterpret_cast<const NetRawContainer*>(ptr);

		if ((unsigned long)(ptr + sizeof(rawChild.header) + rawChild.header.bufsz - buffer) > bufsz)
		{
			CLog::Log(S_ERROR, "Failed NetContainer::Read(): Buffer overflow while reading child container header");
			return false;
		}

		m_Children.push_back(NetContainer(rawChild.header.name));

		NetContainer& child = m_Children.back();
		child.Read(ptr, sizeof(rawChild.header) + rawChild.header.bufsz);

		ptr += sizeof(rawChild.header) + rawChild.header.bufsz;
	}

	return true;
}


SP_NMSPACE_END
