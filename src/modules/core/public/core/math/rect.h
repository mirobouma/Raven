#pragma once
#include "math.h"

namespace Raven 
{
	/*
	\struct FRect
	\brief Rectangle structure, holding position and size.
	*/
	struct FRect {
		FRect() {}
		FRect(FVector2 size, FVector2 position) : Position(position), Size(size) {}
		FVector2 Position; /*!< Position of this rect (top-left corner). */
		FVector2 Size; /*!< Size of this rect. */
	};
}