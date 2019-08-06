#pragma once
#include "core/math/math.h"
#include "rendering_interface/irenderinterface.h"

namespace Raven
{
	typedef enum ETextureSlot
	{
		TEXTURE_SLOT_ALBEDO,
		TEXTURE_SLOT_METALLIC,
		TEXTURE_SLOT_ROUGHNESS,
		TEXTURE_SLOT_NORMAL,
		MAX_TEXTURE_SLOTS
	} ETextureSlot;

	struct FMaterial
	{
		FVector4 mColor;
		IImage* mTextures[MAX_TEXTURE_SLOTS];
	};
}