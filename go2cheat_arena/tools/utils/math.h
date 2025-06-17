#pragma once
#include <Windows.h>
#include "../imgui/imgui.h"
#include <corecrt_math.h>

extern int screenW;
extern int screenH;

namespace utils {

	namespace math {

		struct Vector3
		{
			Vector3(
				const float x = 0.f,
				const float y = 0.f,
				const float z = 0.f
			) noexcept :
				x(x), y(y), z(z) {
			};

			const Vector3 operator-(const Vector3& v) const noexcept
			{
				return Vector3{ x - v.x, y - v.y, z - v.z };
			}

			const Vector3 operator+(const Vector3& v) const noexcept
			{
				return Vector3{ x + v.x, y + v.y, z + v.z };
			}

			const Vector3 operator/(const float v) const noexcept
			{
				return Vector3{ x / v, y / v, z / v };
			}

			const Vector3 operator*(const float v) const noexcept
			{
				return Vector3{ x * v, y * v, z * v };
			}
			float x, y, z;
		};

		struct ViewMatrix
		{
			ViewMatrix() noexcept :
				data() {
			};

			float* operator[](int index) noexcept
			{
				return data[index];
			}
			const float* operator[](int index) const noexcept
			{
				return data[index];
			}

			float data[4][4];
		};
		static bool WTS(const Vector3& world, Vector3& screen, const ViewMatrix& vm)noexcept {
			float w = vm[3][0] * world.x + vm[3][1] * world.y + vm[3][2] * world.z + vm[3][3];

			if (w < 0.001f) {
				return false;
			}

			const float x = world.x * vm[0][0] + world.y * vm[0][1] + world.z * vm[0][2] + vm[0][3];
			const float y = world.x * vm[1][0] + world.y * vm[1][1] + world.z * vm[1][2] + vm[1][3];

			w = 1.f / w;
			float nx = x * w;
			float ny = y * w;

			const ImVec2 size = ImGui::GetIO().DisplaySize;

			screen.x = (screenW * 0.5f * nx) + (nx + screenW * 0.5f);
			screen.y = -(screenH * 0.5f * ny) + (ny + screenH * 0.5f);

			return true;
		}

		float GetDistance(Vector3& enemyPos, Vector3& LocalPos);
	}
}