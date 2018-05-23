#pragma once
#include "Math/Math.h"
#include "Rendering/Interfaces/IRenderer.h"

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
		ITexture* mTextures[MAX_TEXTURE_SLOTS];
	};
}