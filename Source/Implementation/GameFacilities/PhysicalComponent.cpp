#include <Implementation\GameFacilities\PhysicalComponent.h>
#include <Util\SerializationTools.h>

SP_NMSPACE_BEG

S_API CPhysicalComponent::CPhysicalComponent()
	: IComponent()
{
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::Serialize(map<string, string>& params) const
{
	//params["weight"] = SerializeFloat(100.0f);
}

// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::Deserialize(const map<string, string>& params)
{
	//m_State.weight = DeserializeFloat(params["weight"]);
}


// ------------------------------------------------------------------------------------------------------------
S_API void CPhysicalComponent::OnEntityTransformed()
{





	//TODO







}

SP_NMSPACE_END
