#include "math.h"

int screenW = GetSystemMetrics(SM_CXSCREEN);
int screenH = GetSystemMetrics(SM_CYSCREEN);

float utils::math::GetDistance(Vector3& enemyPos, Vector3& LocalPos)
{
	Vector3 Delta
	{
		enemyPos.x - LocalPos.x, enemyPos.y - LocalPos.y, enemyPos.z - LocalPos.z 
	};

	return sqrtf(Delta.x * Delta.x + Delta.y * Delta.y + Delta.z * Delta.z);
}