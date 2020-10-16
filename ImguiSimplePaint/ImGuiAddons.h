#pragma once
#include "imgui.h"
namespace ImGui {
	bool SliderScalar2D(char const* pLabel, float* pValueX, float* pValueY, float const fMinX, float const fMaxX, float const fMinY, float const fMaxY, float const fScale = 1.0f);
	//bool SliderScalar3D(char const* pLabel, float* pValueX, float* pValueY, float* pValueZ, float const fMinX, float const fMaxX, float const fMinY, float const fMaxY, float const fMinZ, float const fMaxZ, float const fScale = 1.0f);

	bool InputVec2(const char* label, ImVec2* value, ImVec2 const vMinValue, ImVec2 const vMaxValue, float const fScale = 1.0f);
	bool InputVec3(const char* label, ImVec4* value, ImVec4 const vMinValue, ImVec4 const vMaxValue, float const fScale = 1.0f);
}