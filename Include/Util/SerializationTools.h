#pragma once

#include <Abstract\Vector3.h>
#include <Abstract\Quaternion.h>

#include <string>
using std::string;

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

	inline static string SerializeUInt(unsigned int u)
	{
		return std::to_string(u);
	}

	inline static string SerializeFloat(float f)
	{
		return std::to_string(f);
	}

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

	inline static string SerializeVector(const Vec3f& v)
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

	inline static unsigned int DeserializeUInt(const string& expr)
	{
		return (unsigned int)strtoul(expr.c_str(), 0, 10);
	}

	inline static float DeserializeFloat(const string& expr)
	{
		return (float)atof(expr.c_str());
	}

	inline static string DeserializeString(const string& expr)
	{
		if (expr.length() <= 2 || expr[0] != '"' || expr[expr.length() - 1] != '"')
			return "";
		else
			return expr.substr(1, expr.length() - 2);
	}

	inline static Vec3f DeserializeVector(const string& expr)
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
}
