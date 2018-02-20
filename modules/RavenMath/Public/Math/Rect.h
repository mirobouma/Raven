#include "Math.h"

namespace Raven {
	namespace Math {
		/*
		\struct FRect
		\brief Rectangle structure, holding position and size.
		*/
		struct FRect {
			FVector2 Position; /*!< Position of this rect (top-left corner). */
			FVector2 Size; /*!< Size of this rect (top-left corner). */
		};
	}
}