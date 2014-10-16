#include "Quaternion.h"
#include <math.h>

quaternion::quaternion() : ma(0), mb(0), mc(0), md(0)
{

}

quaternion::quaternion( quaternion const& q ): ma(q.a()), mb(q.b()), mc(q.c()), md(q.d())
{

}

quaternion::~quaternion()
{

}

void quaternion::initQuat( double a, double x, double y, double z )
{
	ma = cos(a / 2);
	mb = x * sin(a/2);
	mc = y * sin(a/2);
	md = z * sin(a/2);
}

void quaternion::normalize()
{
	double magni = magnitude();
	if (magni)
	{
		ma = ma / magni;
		mb = mb / magni;
		mc = mc / magni;
		md = md / magni;
	}
}

quaternion quaternion::operator*=( quaternion const& q )
{
	quaternion tmp = quaternion( *this * q);
	ma = tmp.a();
	mb = tmp.b();
	mc = tmp.c();
	md = tmp.d();
	return *this;
}

quaternion quaternion::operator*( quaternion const& q )
{
	quaternion tmp;

	double w1 = ma;
	double x1 = mb;
	double y1 = mc;
	double z1 = md;

	double w2 = q.a();
	double x2 = q.b();
	double y2 = q.c();
	double z2 = q.d();

	tmp.ma = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2;
	tmp.mb = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2;
	tmp.mc = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2;
	tmp.md = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2;

	return tmp;
}

double quaternion::magnitude()
{
	return sqrt( ma * ma + mb * mb + mc * mc + md * md);
}

quaternion quaternion::conjugate()
{
	quaternion tmp;
	tmp.ma = ma;
	tmp.mb = -mb;
	tmp.mc = -mc;
	tmp.md = -md;
	return tmp;
}

NOWORK_API rotationQuaternion::rotationQuaternion()
{

}

NOWORK_API rotationQuaternion::~rotationQuaternion()
{

}

NOWORK_API glm::mat4 rotationQuaternion::getRM( double alpha, double x, double y, double z )
{
	quaternion q;
	q.initQuat(alpha, x, y, z);
	q.normalize();
	glm::mat4 rm;
	rm = glm::mat4(
		1 - 2 * q.c() * q.c() - 2 * q.d() * q.d(), 2 * q.b() * q.c() - 2 * q.d() * q.a(), 2 * q.b() * q.d() + 2 * q.c() * q.a(), 0,
		2 * q.b()*q.c() + 2 * q.d()*q.a(), 1 - 2 * q.b()*q.b() - 2 * q.d()*q.d(), 2 * q.c()*q.d() - 2 * q.b()*q.a(), 0,
		2 * q.b()*q.d() - 2 * q.c()*q.a(), 2 * q.c()*q.d() + 2 * q.b()*q.a(), 1 - 2 * q.b()*q.b() - 2 * q.c()*q.c(), 0,
		0, 0, 0, 1);
	return rm;
}
