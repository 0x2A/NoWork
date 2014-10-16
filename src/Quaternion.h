#include "Common.h"

class quaternion
{
public:
	NOWORK_API quaternion();
	NOWORK_API quaternion(quaternion const& q);
	NOWORK_API virtual ~quaternion();
	NOWORK_API void initUnitQuat();
	NOWORK_API void initRotatedQuat(double alpha, double x, double y, double z);
	NOWORK_API double magnitude();
	NOWORK_API quaternion operator*=(quaternion const& q);
	NOWORK_API quaternion operator*(quaternion const& q);
	NOWORK_API void normalize();
	NOWORK_API quaternion conjugate();
	NOWORK_API double a() const { return ma; };
	NOWORK_API double b() const { return mb; };
	NOWORK_API double c() const { return mc; };
	NOWORK_API double d() const { return md; };

	NOWORK_API glm::mat4 getRM();
private: 
	double ma, mb, mc, md;
	
};