#include "LSM303AGR.h"
#include <Wire.h>
#include "math.h"
#include "Arduino.h"

void LSM303AGR::writeRegMag(byte addr, byte data)
{
	
	Wire.beginTransmission(LSM303AGR_MAG_ADDRES);
	Wire.write(addr);
	Wire.write(data);
	Wire.endTransmission();
}
void LSM303AGR::writeRegAcc(byte addr, byte data)
{
	
	Wire.beginTransmission(LSM303AGR_ACC_ADDRESS);
	Wire.write(addr);
	Wire.write(data);
	Wire.endTransmission();
}

void LSM303AGR::init()
{
	writeRegMag(0x60,0b10000000); //Setup magnetometer
	writeRegMag(0x62,0b00010000); //Enable BDU

	writeRegAcc(0x20, 0b00100111); //Set Acc to 10Hz mode and enable all axes
	writeRegAcc(0x23, 0b10001000); //Set Acc to HiRes mode, enable Block Data Update (BDU)
}

uint16_t LSM303AGR::getBearing()
{
	Wire.beginTransmission(LSM303AGR_MAG_ADDRES);
	Wire.write(LSM303AGR_OUTX_L_REG_M | AINC);
	Wire.endTransmission();
	Wire.requestFrom(LSM303AGR_MAG_ADDRES, 6);
	int x = Wire.read() | (Wire.read() << 8);
	int y = Wire.read() | (Wire.read() << 8);
	int z = Wire.read() | (Wire.read() << 8);
	
	m_max.x = max(x, m_max.x);
	m_max.y = max(y, m_max.y);
	m_max.z = max(z, m_max.z);
	
	m_min.x = min(x, m_min.x);
	m_min.y = min(y, m_min.y);
	m_min.z = min(z, m_min.z);
	
	offset = Vector3f((m_max.x + m_min.x )/ 2,(m_max.y + m_min.y )/ 2,(m_max.z + m_min.z )/ 2);
	
	Vector3f mag = Vector3f(x - offset.x, y - offset.y, z - offset.z);
	mag.Normalize();
	
// 	m_max.x = max(x, m_max.x);
// 	m_max.y = max(y, m_max.y);
// 	m_max.z = max(z, m_max.z);
// 	
// 	m_min.x = min(x, m_min.x);
// 	m_min.y = min(y, m_min.y);
// 	m_min.z = min(z, m_min.z);
// 	
// 	offset = Vector3f((m_max.x + m_min.x )/ 2,(m_max.y + m_min.y )/ 2,(m_max.z + m_min.z )/ 2);
	
	Wire.beginTransmission(LSM303AGR_ACC_ADDRESS);
	Wire.write(LSM303AGR_OUT_X_L_A | AINC);
	Wire.endTransmission();
	Wire.requestFrom(LSM303AGR_ACC_ADDRESS, 6);
	
	x = (Wire.read() | (Wire.read() << 8)) / 16;
	y = (Wire.read() | (Wire.read() << 8)) / 16;
	z = (Wire.read() | (Wire.read() << 8)) / 16;
	Vector3f acc = Vector3f(x, y, z);
	acc.Normalize();

	Vector3f m_a_plane_vect  = Vector3f::CrossProduct(mag, acc);
	m_a_plane_vect.Normalize();
	Vector3f perp_m_vect = Vector3f::CrossProduct(acc, m_a_plane_vect);
	
	Vector3f front_vect = Vector3f(1,0,0);
	
	perp_m_vect.Normalize();
	
	return 360-(atan2(Vector3f::DotProduct(m_a_plane_vect, front_vect), Vector3f::DotProduct(perp_m_vect,front_vect)) + PI) * RAD_TO_DEG;
}


/************************************************************************/
/*                             Vector3                                  */
/************************************************************************/
/*
Vector3::Vector3(long _x, long _y, long _z)
{
	x = _x;
	y = _y;
	z = _z;
}
static Vector3 Vector3::CrossProduct(Vector3 a, Vector3 b)
{
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

long Vector3::Magnitude()
{
	return sqrt(pow(x,2) + pow(y,2) +pow(z,2));
}*/


Vector3f::Vector3f(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

static Vector3f Vector3f::CrossProduct(Vector3f a, Vector3f b)
{
	return Vector3f(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float Vector3f::DotProduct(Vector3f a, Vector3f b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Vector3f::Magnitude()
{
	return sqrt(pow(x,2) + pow(y,2) +pow(z,2));
}

void Vector3f::Normalize()
{
	float m = Magnitude();
	x /= m;
	y /= m;
	z /= m;
}