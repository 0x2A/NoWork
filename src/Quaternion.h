#include "Common.h"

class quaternion
{
public:
	NOWORK_API quaternion();
	NOWORK_API quaternion(quaternion const& q);
	NOWORK_API virtual ~quaternion();
	NOWORK_API void initQuat(double alpha, double x, double y, double z);
	NOWORK_API double magnitude();
	NOWORK_API quaternion operator*=(quaternion const& q);
	NOWORK_API quaternion operator*(quaternion const& q);
	NOWORK_API void normalize();
	NOWORK_API quaternion conjugate();
	NOWORK_API double a() const { return ma; };
	NOWORK_API double b() const { return mb; };
	NOWORK_API double c() const { return mc; };
	NOWORK_API double d() const { return md; };

	
private: 
	double ma, mb, mc, md;
	
};

class rotationQuaternion
{
public:
	NOWORK_API rotationQuaternion();
	NOWORK_API ~rotationQuaternion();
	NOWORK_API static glm::mat4 getRM( double alpha, double x, double y, double z );
};