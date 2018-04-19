#pragma once

namespace Raven
{
	struct ISampler;

	/*! \struct FRootSignatureCreateDesc
	\brief Structure describing the parameters for root signature creation. */
	struct FRootSignatureCreateDesc
	{
		uint32_t numStaticSamplers;
		ISampler* pStaticSamplers;
	};

	/*! \struct IRootSignature
	\brief Interface for a root signature. */
	struct IRootSignature {};
}