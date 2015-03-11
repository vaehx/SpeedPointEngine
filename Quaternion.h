
// q = w + i*x + j*y + k*z
struct Quat
{
	Vec3f v;
	float w;
	
	Quat() : v(0,0,0), w(1) {}
	Quat(float W, float X, float Y, float Z) : w(W), v(X, Y, Z) {}
	Quat(float scalar, const Vec3f& axis) : w(angle), v(axis) {}

	ILINE static Quat FromAxisAngle(float theta, const Vec3f& axis)
	{
		return Quat(cosf(0.5f * theta), axis * sinf(0.5f * theta));
	}

	ILINE static Quat FromVector(const Vec3f& u)
	{
		return Quat(0, u);
	}

	ILINE Quat operator *(const Quat& q) const
	{
		return Quat(w * q.w - v * q.v,
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x);
	}

	ILINE Quat operator / (float d) const { return Quat(w / d, v / d); }
};

ILINE float Length(const Quat& q)
{
	return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

ILINE Quat Complement(const Quat& q)
{
	return Quat(q.w, -q.v);
}

ILINE Quat Invert(const Quat& q)
{
	float ln = Length(q);
	return Quat(Complement(q) / (ln * ln));
}

ILINE Vec3f RotateVector(const Vec3f& v, const Quat& q)
{
	return q * Quat::FromVector(v) * Invert(q);
}