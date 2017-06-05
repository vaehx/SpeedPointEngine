#include "SerializationTools.h"

namespace SpeedPoint
{
	S_API CBasicSerContainer::CBasicSerContainer(const string& name)
		: ISerContainer(name)
	{
	}

	S_API ISerContainer* CBasicSerContainer::CreateChildContainer(const string& name)
	{
		m_Children.emplace_back(name);
		ISerContainer* pChild = &m_Children.back();
		m_ChildPointers.push_back(pChild);
		return pChild;
	}

	S_API const vector<ISerContainer*>& CBasicSerContainer::GetChildContainers() const
	{
		return m_ChildPointers;
	}

	S_API unsigned short CBasicSerContainer::GetNumChildren() const
	{
		return (unsigned short)m_Children.size();
	}

	S_API void CBasicSerContainer::DestroyChildContainer(const string& name)
	{
		auto itDestroyChild = m_Children.end();
		for (auto itChild = m_Children.begin(); itChild != m_Children.end(); ++itChild)
		{
			if (itChild->GetName() == name)
			{
				itDestroyChild = itChild;
				break;
			}
		}

		if (itDestroyChild == m_Children.end())
			return;

		ISerContainer* pChild = &*itDestroyChild;
		for (auto itChildPtr = m_ChildPointers.begin(); itChildPtr != m_ChildPointers.end(); ++itChildPtr)
		{
			if (*itChildPtr == pChild)
			{
				m_ChildPointers.erase(itChildPtr);
				break;
			}
		}

		m_Children.erase(itDestroyChild);
	}

	S_API unsigned short CBasicSerContainer::GetNumAttributes() const
	{
		return (unsigned short)m_Attributes.size();
	}


#define GET_AND_ASSERT_ATTR(name) \
	auto attr = m_Attributes.find(name); \
	if (attr == m_Attributes.end()) return def;

	S_API int CBasicSerContainer::GetInt(const string& attrname, int def)
	{
		GET_AND_ASSERT_ATTR(attrname);
		return DeserializeInt(attr->second);
	}

	S_API unsigned int CBasicSerContainer::GetUInt(const string& attrname, unsigned int def)
	{
		GET_AND_ASSERT_ATTR(attrname);
		return DeserializeUInt(attr->second);
	}

	S_API float CBasicSerContainer::GetFloat(const string& attrname, float def)
	{
		GET_AND_ASSERT_ATTR(attrname);
		return DeserializeFloat(attr->second);
	}

	S_API string CBasicSerContainer::GetString(const string& attrname, const string& def)
	{
		GET_AND_ASSERT_ATTR(attrname);
		return DeserializeString(attr->second);
	}

	S_API Vec3f CBasicSerContainer::GetVec3f(const string& attrname, const Vec3f& def)
	{
		GET_AND_ASSERT_ATTR(attrname);
		return DeserializeVec3f(attr->second);
	}


	S_API void CBasicSerContainer::SetRaw(const string& attrname, const string& val)
	{
		if (!attrname.empty())
			m_Attributes[attrname] = val;
	}

	S_API void CBasicSerContainer::SetInt(const string& attrname, int val)
	{
		if (!attrname.empty())
			m_Attributes[attrname] = SerializeInt(val);
	}

	S_API void CBasicSerContainer::SetUInt(const string& attrname, unsigned int val)
	{
		if (!attrname.empty())
			m_Attributes[attrname] = SerializeUInt(val);
	}

	S_API void CBasicSerContainer::SetFloat(const string& attrname, float val)
	{
		if (!attrname.empty())
			m_Attributes[attrname] = SerializeFloat(val);
	}

	S_API void CBasicSerContainer::SetString(const string& attrname, const string& val)
	{
		if (!attrname.empty())
			m_Attributes[attrname] = SerializeString(val);
	}

	S_API void CBasicSerContainer::SetVec3f(const string& attrname, const Vec3f& val)
	{
		if (!attrname.empty())
			m_Attributes[attrname] = SerializeVec3f(val);
	}
}
