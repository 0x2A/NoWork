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

void quaternion::initUnitQuat()
{
	ma = 1.0;
	mb = 1.0;
	mc = 1.0;
	md = 1.0;
	this->normalize();
}

void quaternion::initRotatedQuat( double a, double x, double y, double z )
{
	ma = cos(a / 2);
	mb = x * sin(a/2);
	mc = y * sin(a/2);
	md = z * sin(a/2);
}

void quaternion::normalize()
{
	double magni = magnitude();
	ma = ma / magni;
	mb = mb / magni;
	mc = mc / magni;
	md = md / magni;
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

glm::mat4 quaternion::getRM()
{
	glm::mat4 rm;
	rm = glm::mat4(
		1 - 2 * mc * mc - 2 * md * md, 2 * mb * mc - 2 * md * ma, 2 * mb * md + 2 * mc * ma, 0,
		2 * mb*mc + 2 * md*ma, 1 - 2 * mb*mb - 2 * md*md, 2 * mc*md - 2 * mb*ma, 0,
		2 * mb*md - 2 * mc*ma, 2 * mc*md + 2 * mb*ma, 1 - 2 * mb*mb - 2 * mc*mc, 0,
		0, 0, 0, 1);
	return rm;
}

int main(){
	quaternion quat, rotation;
	quat.initUnitQuat();
	double test = quat.magnitude();
	rotation.initRotatedQuat(20,1,0,0);
	quat *= rotation;
	return 0;
}