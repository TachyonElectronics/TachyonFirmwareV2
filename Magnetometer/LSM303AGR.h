


#ifndef LSM303AGR_H_
#define LSM303AGR_H_

#include "stdint.h"

#define byte unsigned short

#define LSM303AGR_MAG_ADDRES 0b0011110
#define LSM303AGR_ACC_ADDRESS 0b0011001

#define LSM303AGR_OUTX_L_REG_M 0x68
#define LSM303AGR_OUT_X_L_A 0x28

#define AINC 0b10000000 //Auto increment flag
/*
class Vector3 {
	public:
	long x;
	long y;
	long z;
	
	Vector3(long _x, long _y, long _z);
	
	static Vector3 CrossProduct(Vector3 a, Vector3 b);
	
	long Magnitude();
};*/

class Vector3f {
	public:
	float x;
	float y;
	float z;
	
	Vector3f(float _x, float _y, float _z);
	
	static Vector3f CrossProduct(Vector3f a, Vector3f b);
	
	static float DotProduct(Vector3f a, Vector3f b);
	
	float Magnitude();
	
	void Normalize();
};

class LSM303AGR
{
	public:
	Vector3f m_max = Vector3f(INT32_MIN,INT32_MIN,INT32_MIN);
	Vector3f m_min = Vector3f(INT32_MAX,INT32_MAX,INT32_MAX);
	Vector3f offset = Vector3f(0,0,0);
	
	void writeRegMag(byte addr, byte data);
	void writeRegAcc(byte addr, byte data);
	
	void init();
	
	uint16_t getBearing();
};



#endif /* LSM303AGR_H_ */