#include "core/math/math.h"

float Raven::RandomFloat(float a, float b)
{
 float random = ((float)rand()) / (float)RAND_MAX;
 float diff = b - a;
 float r = random * diff;
 return a + r;
}

float Raven::Lerp(float a, float b, float f)
{
	return a + f * (b - a);
}