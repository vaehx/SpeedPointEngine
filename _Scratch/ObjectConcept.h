
// SpeedPoint Object concept

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// new global requirements:

// Summary:
//  Information about the execution of a single frame
struct SFrameInfo
{
  float fLastFrameDuration;   // in ms    
};

// ### IF NOT ALREADY THERE ###
typedef float f32;
typedef double f64;
// ########

#define SP_FLOAT_ZERO_THRESHOLD 0.0000001f;
#define SP_DOUBLE_ZERO_THRESHOLD 0.0000001; 

inline f32 sqrt_t(f32 v) { sqrtf(v); }
inline f64 sqrt_t(f64 v) { sqrt(v}; }
inline f32 sqrts_t(f32 v) { sqrtf(v + SP_FLOAT_ZERO_THRESHOLD); }
inline f64 sqrts_t(f64 v) { sqrt(v + SP_DOUBLE_ZERO_THRESHOLD); }

// Typename T has to be castable from int!
template<typename T>
class Vec3<T>
{
public:
  T x, y, z;
  
  Vec3<T>()
  : x(0), y(0), z(0) {}
  
  Vec3<T>(const Vec3<T>& o)
  : x(o.x), y(o.y), z(o.z) {}
  
  Vec3<T>(T aa)
  : x(aa), y(aa), z(aa) {}
  
  Vec3<T>(T xx, T yy, T zz)
  : x(xx), y(yy), z(zz) {}
  
  Vec3<T> Cross(const Vec3<T>& o) const
  {
    return Vec3<T>(y * o.z - z * o.y,
      z * o.x - x * o.z,
      x * o.y - y * o.x);  
  }
  
  T Dot(const Vec3<T>& o) const
  {
    return x * o.x + y * o.y + z * o.z;  
  }
  
  
  // Subtraction
  
  inline Vec3<T> operator -(const Vec3<T>& o) const
  {
    return Vec3<T>(x - o.x, y - o.y, z - o.z);
  }
  inline Vec3<T> operator -(T v) const
  {
    return Vec3<T>(x - v, y - v, z - v);  
  }
  inline Vec3<T>& operator -=(const Vec3<T>& o)
  {
    x = x - o.x; y = y - o.y; z = z - o.z;
    return *this;      
  }
  inline Vec3<T>& operator -=(T v)
  {
    x = x - v; y = y - v; z = z - v;
    return *this;   
  }
  
  
  // Addition
  
  inline Vec3<T> operator +(const Vec3<T>& o) const
  {
    return Vec3<T>(x + o.x, y + o.y, z + o.z);  
  }
  inline Vec3<T> operator +(T v) const
  {
    return Vec3<T>(x + v, y + v, z + v);  
  }
  inline Vec3<T>& operator +=(const Vec3<T>& o)
  {
    x = x + o.x; y = y + o.y; z = z + o.z;
    return *this;  
  }
  inline Vec3<T>& operator +=(T v)
  {
    x = x + v; y = y + v; z = z + v;
    return *this;  
  }  
  
  
  // Multiplication
  
  inline Vec3<T> operator *(const Vec3<T>& o) const
  {
    return Vec3<T>(x * o.x, y * o.y, z * o.z);  
  }
  inline Vec3<T> operator *(T v) const
  {
    return Vec3<T>(x * v, y * v, z * v);
  }
  inline Vec3<T>& operator *=(const Vec3<T>& o)
  {
    x = x * o.x; y = y * o.y; z = z * o.z;
    return *this;  
  }
  inline Vec3<T>& operator *=(T v)
  {
    x = x * v; y = y * v; z = z * v;
    return *this;
  }
  
  
  // Division
  
  inline Vec3<T> operator /(const Vec3<T>& o) const
  {
    return Vec3<T>(x / o.x, y / o.y, z / o.z);  
  }
  inline Vec3<T> operator /(T v) const
  {
    v = F(1.0) / v;
    return Vec3<T>(x * v, y * v, z * v);  
  }
  inline Vec3<T>& operator /=(const Vec3<T>& o)
  {
    x = x / o.x; y = y / o.y; z = z / o.z;
    return *this;  
  }
  inline Vec3<T>& operator /=(T v)
  {
    v = F(1.0) / v;
    x = x * v; y = y * v; z = z * v;
    return *this;
  }          
      
      
  T Magnitude() const
  {
    return sqrt_t(x * x + y * y + z * z);  
  }
  
                  
};

typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;
typedef Vec3<int> Vec3i; 

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


struct STransformable
{
  Vec3f pos;
  Vec3f rot;
  Vec3f scl;
};

struct IVisibleObject;
class VisibleObject : public IVisibleObject, public STransformable
{
};


struct ITerrain : public IVisibleObject;
class Terrain : public ITerrain
{
};

struct IWaterVolume : public IVisibleObject;
class WaterVolume : public IWaterVolume
{
};

struct IOcean : public IVisibleObject;
class Ocean : public IOcean
{
};


struct IDynamicalObject
{
  virtual void OnUpdate(const SFrameInfo& lastFrameInfo) = 0;
};

struct IPhysicalObject : public IDynamicalObject;


// Requirements from Physics Engine:
struct IPEObject;
struct IPECollisionMesh;
struct SPEPhysicalProperties;
// ---

class PhysicalObject : public IPhysicalObject
{
private:
  IPEObject* m_pPEObject;
public:
  virtual void OnUpdate(const SFrameInfo& lastFrameInfo);
};

struct IDestructableObject : public IDynamicalObject
{
  virtual IVisibleObject* GetVisible() = 0;
  virtual IPhysicalObject* GetPhysical() = 0;
};
class DestructableObject : public IDestructableObject
{
private:
  VisibleObject m_VisibleObject;
  PhysicalObject m_PhysicalObject;  

// IDestructableObject:
public:
  virtual IVisibleObject* GetVisible() { return &m_VisibleObject; }
  virtual IPhysicalObject* GetPhysical() { return &m_PhysicalObject; }

// IDynamicalObject:
public:
  virtual void OnUpdate(const SFrameInfo& lastFrameInfo);
};


struct IEntityComponent
{
	virtual const char* GetClassName() = 0;
};
struct IEntity
{
	virtual std::vector<IEntityComponent*>& GetEntityComponents() = 0;
};
class Entity : public IEntity
{
private:
	std::vector<IEntityComponent*> m_Components;
public:
	virtual std::vector<IEntityComponent*>& GetEntityComponents() { return m_Components; }
};

class BaseEntityComponent : public IEntityComponent
{
  virtual const char* GetClassName() { return "BaseEntity"; }
};

class PhysicalEntity : public BaseEntityComponent
{
private:
  PhysicalObject m_Physical;

public:  
  IPhysicalObject* GetPhysical() { return &m_Physical; }

// from BaseEntity
public:
  virtual const char* GetClassName() { return "PhysicalEntity"; }
};

class DestructableEntity : public BaseEntityComponent
{
private:
  DestructableObject m_Destructable;
public:
  virtual const char* GetClassName() { return "DestructableEntity"; }    
};


struct IAnimateable
{
  virtual Vec3f 
};
class AnimateableObject : public IAnimateable, public IDynamicalObject
{
private:
  VisibleObject m_Geometry;
  // Todo: Add Joint and Key Stuff

// from IDynamicalObject:
public:
  virtual void OnUpdate(const SFrameInfo& lastFrameInfo);  
};


struct IBoid;
class Boid : public IBoid
{
private:
  AnimateableObject m_Animateable;    
};

