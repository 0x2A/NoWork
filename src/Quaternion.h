class quaternion
{
public:
	quaternion();
	quaternion(quaternion const& q);
	virtual ~quaternion();
	void initUnitQuat();
	void initRotatedQuat(double alpha, double x, double y, double z);
	double magnitude();
	quaternion operator*=( quaternion const& q );
	quaternion operator*( quaternion const& q );
	void normalize();
	quaternion conjugate();
	double a() const { return ma; };
	double b() const { return mb; };
	double c() const { return mc; };
	double d() const { return md; };

	//glmat4 getRM();
private: 
	double ma, mb, mc, md;
	
};