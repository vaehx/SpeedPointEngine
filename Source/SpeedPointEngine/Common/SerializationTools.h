#pragma once

#include "Vector3.h"
#include "Quaternion.h"

#include <string>
using std::string;

#include <map>
using std::map;

#include <climits>

namespace SpeedPoint
{
	// --------------------------------------------------------------------------------------------
	//
	// Serialization
	//
	// --------------------------------------------------------------------------------------------

	inline static string SerializeBool(bool b)
	{
		return (b ? "true" : "false");
	}

#define SerializeShort(s) std::to_string(s)
#define SerializeUShort(us) std::to_string(us)
#define SerializeInt(i) std::to_string(i)
#define SerializeUInt(ui) std::to_string(ui)
#define SerializeLong(l) std::to_string(l)
#define SerializeULong(ul) std::to_string(ul)

#define SerializeFloat(f) std::to_string(f)
#define SerializeDouble(d) std::to_string(d)

	inline static string SerializeString(const string& str)
	{
		static string expr;
		expr = "";
		expr.reserve(str.length() + 2);
		expr += "\"";
		expr += str;
		expr += "\"";
		return expr;
	}

	inline static string SerializeVec3f(const Vec3f& v)
	{
		static string expr;
		expr = "(";
		expr += std::to_string(v.x);
		expr += ",";
		expr += std::to_string(v.y);
		expr += ",";
		expr += std::to_string(v.z);
		expr += ")";
		return expr;
	}
#define SerializeVector SerializeVec3f

	inline static string SerializeQuaternion(const Quat& q)
	{
		static string expr;
		expr = "(";
		expr += std::to_string(q.v.x);
		expr += ",";
		expr += std::to_string(q.v.y);
		expr += ",";
		expr += std::to_string(q.v.z);
		expr += ",";
		expr += std::to_string(q.w);
		expr += ")";
		return expr;
	}


	// --------------------------------------------------------------------------------------------
	//
	// Deserialization
	//
	// --------------------------------------------------------------------------------------------

	inline static bool DeserializeBool(const string& expr)
	{
		return (expr == "true");
	}

// expr - a std::string
#define DeserializeNumeric(type, expr) ((type)strtoul(expr.c_str(), 0, 10))

#define DeserializeShort(expr) DeserializeNumeric(short, expr)
#define DeserializeUShort(expr) DeserializeNumeric(unsigned short, expr)
#define DeserializeInt(expr) DeserializeNumeric(int, expr)
#define DeserializeUInt(expr) DeserializeNumeric(unsigned int, expr)
#define DeserializeLong(expr) DeserializeNumeric(long, expr)
#define DeserializeULong(expr) DeserializeNumeric(unsigned long, expr)

	inline static float DeserializeFloat(const string& expr)
	{
		return (float)atof(expr.c_str());
	}

	inline static double DeserializeDouble(const string& expr)
	{
		return atof(expr.c_str());
	}

	inline static string DeserializeString(const string& expr)
	{
		if (expr.length() <= 2 || expr[0] != '"' || expr[expr.length() - 1] != '"')
			return "";
		else
			return expr.substr(1, expr.length() - 2);
	}

	inline static Vec3f DeserializeVec3f(const string& expr)
	{
		// Minimal vector: (0,0,0)
		if (expr.length() < 7 || expr[0] != '(' || expr[expr.length() - 1] != ')')
			return Vec3f();

		static Vec3f v;
		static size_t lastComma;
		static size_t comma;
		static size_t dist;

		lastComma = 0;

		comma = expr.find_first_of(',');
		dist = comma - lastComma;
		if (comma == expr.npos || dist <= 1)
			return Vec3f();
		v.x = (float)atof(expr.substr(lastComma + 1, dist - 1).c_str());
		lastComma = comma;

		comma = expr.find_first_of(',', lastComma + 1);
		dist = comma - lastComma;
		if (comma == expr.npos || dist <= 1)
			return Vec3f();
		v.y = (float)atof(expr.substr(lastComma + 1, dist - 1).c_str());
		lastComma = comma;

		dist = (expr.length() - 1) - lastComma;
		if (dist <= 1)
			return Vec3f();
		v.z = (float)atof(expr.substr(lastComma + 1, dist - 1).c_str());

		return v;
	}
#define DeserializeVector DeserializeVec3f

	inline static Quat DeserializeQuaternion(const string& expr)
	{
		// Minimal quaternion: (0,0,0,0)
		if (expr.length() < 9 || expr[0] != '(' || expr[expr.length() - 1] != ')')
			return Quat();

		static Quat q;
		static size_t lastComma;
		static size_t comma;
		static size_t dist;

		lastComma = 0;

		comma = expr.find_first_of(',');
		dist = comma - lastComma;
		if (comma == expr.npos || dist <= 1)
			return Quat();
		q.v.x = (float)atof(expr.substr(lastComma + 1, dist - 1).c_str());
		lastComma = comma;

		comma = expr.find_first_of(',', lastComma + 1);
		dist = comma - lastComma;
		if (comma == expr.npos || dist <= 1)
			return Quat();
		q.v.y = (float)atof(expr.substr(lastComma + 1, dist - 1).c_str());
		lastComma = comma;

		comma = expr.find_first_of(',', lastComma + 1);
		dist = comma - lastComma;
		if (comma == expr.npos || dist <= 1)
			return Quat();
		q.v.z = (float)atof(expr.substr(lastComma + 1, dist - 1).c_str());
		lastComma = comma;

		dist = (expr.length() - 1) - lastComma;
		if (dist <= 1)
			return Quat();
		q.w = (float)atof(expr.substr(lastComma + 1, dist - 1).c_str());

		return q;
	}



	// --------------------------------------------------------------------------------------------
	//
	// Container
	//
	// --------------------------------------------------------------------------------------------

	class S_API ISerContainer
	{
	protected:
		string m_Name;

	public:
		ISerContainer(const string& name)
			: m_Name(name)
		{
		}

		virtual ~ISerContainer() {}

		virtual const string& GetName() const { return m_Name; }
		virtual void SetName(const string& name) { m_Name = name; }

		virtual ISerContainer* CreateChildContainer(const string& name) = 0;
		virtual const vector<ISerContainer*>& GetChildContainers() const = 0;
		virtual unsigned short GetNumChildren() const = 0;
		virtual void DestroyChildContainer(const string& name) = 0;

		virtual unsigned short GetNumAttributes() const = 0;

		virtual int GetInt(const string& attrname, int def = INT_MAX) = 0;
		virtual unsigned int GetUInt(const string& attrname, unsigned int def = UINT_MAX) = 0;
		virtual float GetFloat(const string& attrname, float def = FLT_MAX) = 0;
		virtual string GetString(const string& attrname, const string& def = "???") = 0;
		virtual Vec3f GetVec3f(const string& attrname, const Vec3f& def = Vec3f()) = 0;

		virtual void SetInt(const string& attrname, int val) = 0;
		virtual void SetUInt(const string& attrname, unsigned int val) = 0;
		virtual void SetFloat(const string& attrname, float val) = 0;
		virtual void SetString(const string& attrname, const string& val) = 0;
		virtual void SetVec3f(const string& attrname, const Vec3f& val) = 0;
	};
}
