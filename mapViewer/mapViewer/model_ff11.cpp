 Displaying: model_ff11.cpp

//========================================================================================
// Noesis FF11 support
// Life regrettably wasted by Rich Whitehouse
// (c) Never, all rights to be taken to the grave.
//========================================================================================

#include "stdafx.h"
#include "model_ff11.h"
#include "model_ff11_decrypt.h"

//========================================================================================

template<typename T>
static const T *get_and_incr_offset(const unsigned char *pBuffer, int &bufferOfs, const int count = 1)
{
	const T *pCmd = (const T *)(pBuffer + bufferOfs);
	bufferOfs += sizeof(T) * count;
	return pCmd;
}

static void align_offset(int &bufferOfs, const int alignment)
{
	//assumes power of 2
	const int alignmentMinusOne = alignment - 1;
	bufferOfs = ((bufferOfs + alignmentMinusOne) & ~alignmentMinusOne);
}

//========================================================================================

class CFFXITextureHandler : public CFFXIChunkHandler
{
public:
	static const int skTexNameLength = 16;
	static const int skMaterialNamePad = 32;
	static const char *skpShinySuffix;
	static const char *skpSoftBlendSuffix;
	static const char *skpNoBlendSuffix;

	struct STexHeader
	{
		unsigned char mType;
		char mName[skTexNameLength];
		unsigned int mVer; //maybe?
		int mWidth;
		int mHeight;
		unsigned int mUnknown[6];
		unsigned int mBitsPerPalClr; //unverified - kind of seems more like bits per pixel * 4, since i've only seen 16 for dxt1.
	};

	CFFXITextureHandler()
		: CFFXIChunkHandler(CFFXIDat::skChunkType_Texture)
		, mFlatNormalIndex(-1)
		, mFlatSpecIndex(-1)
	{
	}

	static const int skDefaultColorFixShift = 0;
	static const int skDefaultAlphaFixShift = 2;

	static const int skTextureVersion = 40;
	//texture type is probably only the high 4 bits, but 1 always seems to be set
	static const int skTextureType_DXT = 0xA1;
	static const int skTextureType_Pal = 0x91;
	static const int skTextureType_Pal2 = 0x01; //unsure how this is different from skTextureType_Pal
	static const int skTextureType_PalCombo = 0x81; //paletted followed by dxt
	static const int skTextureType_PalLeadingInt = 0xB1; //preceded by 32 bits, unknown

	virtual CFFXIDat::EValidateChunkResult ValidateChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
															const unsigned char *pChunkData, const int dataSize) const
	{
		if (dataSize <= sizeof(STexHeader))
		{
			return CFFXIDat::kVCR_Invalid;
		}
		const STexHeader *pTexHdr = (const STexHeader *)pChunkData;
		if (pTexHdr->mWidth <= 0 || pTexHdr->mWidth > 4096 ||
			pTexHdr->mHeight <= 0 || pTexHdr->mHeight > 4096 ||
			pTexHdr->mVer != skTextureVersion ||
			//unknown, but seems to be consistent
			pTexHdr->mUnknown[0] == 0 ||
			pTexHdr->mUnknown[1] != 0 || pTexHdr->mUnknown[2] != 0 || pTexHdr->mUnknown[3] != 0 ||
			pTexHdr->mUnknown[4] != 0 || pTexHdr->mUnknown[5] != 0)
		{
			return CFFXIDat::kVCR_Invalid;
		}
		else if (pTexHdr->mType != skTextureType_DXT && pTexHdr->mType != skTextureType_Pal && pTexHdr->mType != skTextureType_Pal2 &&
				pTexHdr->mType != skTextureType_PalCombo && pTexHdr->mType != skTextureType_PalLeadingInt)
		{
			return CFFXIDat::kVCR_Invalid;
		}

		return CFFXIDat::kVCR_Supported;
	}

	static unsigned char *CreateRgbaFromPaletted(noeRAPI_t *pRapi, const unsigned char *pRawPalData, const unsigned char *pPixelData,
													const int width, const int height, const int bitsPerColor)
	{
		//unverified - can bpc be 16 in this context, and if so, does it mean 16-color 8888 or 256-color 5551?
		const int palSize = 256 * (bitsPerColor / 8);
		unsigned int *pPalData = (unsigned int *)pRapi->Noesis_ImageDecodeRaw(const_cast<unsigned char *>(pRawPalData), palSize, 256, 1,
			(bitsPerColor == 32) ? "b8g8r8a8" : "b5g5r5a1");
		unsigned int *pDst = (unsigned int *)pRapi->Noesis_UnpooledAlloc(width * height * 4);
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				const int palIndex = pPixelData[(height - y - 1) * width + x];
				pDst[y * width + x] = pPalData[palIndex];
			}
		}
		pRapi->Noesis_UnpooledFree(pPalData);

		return (unsigned char *)pDst;
	}

	static void ShiftRgbaData(unsigned char *pTexData, const int width, const int height, const int texColorShift, const int texAlphaShift)
	{
		for (int pixelIndex = 0; pixelIndex < width * height; ++pixelIndex)
		{
			unsigned char *pPix = pTexData + pixelIndex * 4;
			if (texColorShift)
			{
				pPix[0] = std::min<int>((int)pPix[0] << texColorShift, 255);
				pPix[1] = std::min<int>((int)pPix[1] << texColorShift, 255);
				pPix[2] = std::min<int>((int)pPix[2] << texColorShift, 255);
			}
			if (texAlphaShift)
			{
				pPix[3] = std::min<int>((int)pPix[3] << texAlphaShift, 255);
			}
		}
	}

	virtual bool HandleChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
								const unsigned char *pChunkData, const int dataSize)
	{
		//dxt's generally don't appear to have been encoded from a higher-bit-depth source, so paletted always provides better quality.
		const bool preferDxtToPalette = false;
		const int texColorShift = (gpFF11Opts && gpFF11Opts->explicitColorShift) ? gpFF11Opts->fixColorShift : skDefaultColorFixShift;
		const int texAlphaShift = (gpFF11Opts && gpFF11Opts->explicitAlphaShift) ? gpFF11Opts->fixAlphaShift : skDefaultAlphaFixShift;
		const bool fixAlphaOrColor = (texColorShift || texAlphaShift);

		const STexHeader *pTexHdr = (const STexHeader *)pChunkData;

		bool copyFromSource = false;
		unsigned char *pSrcData = (unsigned char *)(pTexHdr + 1);
		int srcDataSize = dataSize - sizeof(STexHeader);
		unsigned char *pTexData = NULL;
		int texDataSize = 0;
		noesisTexType_e texType = NOESISTEX_UNKNOWN;
		noeRAPI_t *pRapi = dat.GetRAPI();

		const int palSize = 256 * (pTexHdr->mBitsPerPalClr / 8);

		switch (pTexHdr->mType)
		{
		case skTextureType_PalCombo:
			if (!preferDxtToPalette)
			{
				goto PickPalOverDXT; //))<>((
			}
			else
			{
				//push is up and prefer to take the dxt version
				const int palTextureSize = palSize + pTexHdr->mWidth * pTexHdr->mHeight;
				pSrcData += palTextureSize;
				srcDataSize -= palTextureSize;
			}
			//fall through intentionally in the else case
		case skTextureType_DXT:
			{
				const int dxtType = *(const int *)pSrcData;
				switch (dxtType)
				{
				case 'DXT1':
					texType = NOESISTEX_DXT1;
					break;
				case 'DXT3':
					texType = NOESISTEX_DXT3;
					break;
				case 'DXT5':
					texType = NOESISTEX_DXT5;
					break;
				default:
					break;
				}
				if (texType != NOESISTEX_UNKNOWN)
				{
					if (fixAlphaOrColor)
					{
						//it wouldn't be too much work to just shift the 4-bit alphas in the dxt3 blocks, but, fuck it.
						pTexData = pRapi->Noesis_ConvertDXT(pTexHdr->mWidth, pTexHdr->mHeight, const_cast<unsigned char *>(pSrcData) + 12, texType);
						ShiftRgbaData(pTexData, pTexHdr->mWidth, pTexHdr->mHeight, texColorShift, texAlphaShift);
						texType = NOESISTEX_RGBA32;
					}
					else
					{
						copyFromSource = true;
						pTexData = const_cast<unsigned char *>(pSrcData) + 12;
						texDataSize = srcDataSize - 12;
					}
				}
				else
				{
					pRapi->LogOutput("WARNING: Unknown DXT texture type.\n");
				}
			}
			break;

		case skTextureType_PalLeadingInt:
			//not sure what this is used for
			pSrcData += sizeof(int);
			srcDataSize -= sizeof(int);
			//intentionally fall through
		case skTextureType_Pal:
		case skTextureType_Pal2:
		PickPalOverDXT: //~~~~C====((__)
			pTexData = CreateRgbaFromPaletted(pRapi, pSrcData, pSrcData + palSize, pTexHdr->mWidth, pTexHdr->mHeight, pTexHdr->mBitsPerPalClr);
			if (fixAlphaOrColor)
			{
				ShiftRgbaData(pTexData, pTexHdr->mWidth, pTexHdr->mHeight, texColorShift, texAlphaShift);
			}
			texDataSize = pTexHdr->mWidth * pTexHdr->mHeight * 4;
			texType = NOESISTEX_RGBA32;
			break;
		}

		if (!pTexData)
		{
			//just use a stub if something went wrong
			texDataSize = pTexHdr->mWidth * pTexHdr->mHeight * 4;
			pTexData = (unsigned char *)pRapi->Noesis_UnpooledAlloc(texDataSize);
			memset(pTexData, 0, texDataSize);
			texType = NOESISTEX_RGBA32;
		}
		else if (copyFromSource)
		{
			NoeAssert(texDataSize > 0);
			const unsigned char *pSourceData = pTexData;
			pTexData = (unsigned char *)pRapi->Noesis_UnpooledAlloc(texDataSize);
			memcpy(pTexData, pSourceData, texDataSize);
		}

		char texName[skTexNameLength + skMaterialNamePad];
		memcpy(texName, pTexHdr->mName, skTexNameLength);
		texName[skTexNameLength] = 0;

		//allocate flat normal and spec texture in case we haven't yet, used for "shiny" material
		if (mFlatNormalIndex == -1 && (!gpFF11Opts || !gpFF11Opts->noShinyMaterials))
		{
			NoeAssert(mFlatSpecIndex == -1);
			unsigned char *pFlatNormalData = (unsigned char *)pRapi->Noesis_UnpooledAlloc(4 * 4 * 4);
			for (int pixelIndex = 0; pixelIndex < 4 * 4; ++pixelIndex)
			{
				unsigned char *pPixel = pFlatNormalData + pixelIndex * 4;
				pPixel[0] = 127;
				pPixel[1] = 127;
				pPixel[2] = 255;
				pPixel[3] = 255;
			}
			noesisTex_t *pFlatNormalTex = pRapi->Noesis_TextureAlloc("__flat_normal", 4, 4, pFlatNormalData, NOESISTEX_RGBA32);
			mFlatNormalIndex = mTextures.Num();
			pFlatNormalTex->shouldFreeData = true;
			mTextures.Append(pFlatNormalTex);

			unsigned char *pFlatSpecData = (unsigned char *)pRapi->Noesis_UnpooledAlloc(4 * 4 * 4);
			memset(pFlatSpecData, 0xFF, 4 * 4 * 4);
			noesisTex_t *pFlatSpecTex = pRapi->Noesis_TextureAlloc("__flat_spec", 4, 4, pFlatSpecData, NOESISTEX_RGBA32);
			mFlatSpecIndex = mTextures.Num();
			pFlatSpecTex->shouldFreeData = true;
			mTextures.Append(pFlatSpecTex);
		}

		noesisTex_t *pTex = pRapi->Noesis_TextureAllocEx(texName, pTexHdr->mWidth, pTexHdr->mHeight, pTexData, texDataSize, texType, 0, 0);
		pTex->shouldFreeData = true;

		const int defaultMtlFlags = (gpFF11Opts && gpFF11Opts->forceCull) ? 0 : NMATFLAG_TWOSIDED;

		noesisMaterial_t *pMat = pRapi->Noesis_GetMaterialList(1, true);
		pMat->name = pRapi->Noesis_PooledString(texName);
		pMat->texIdx = mTextures.Num();
		pMat->flags |= defaultMtlFlags;
		//don't alpha blend by default, but do alpha test
		pMat->noDefaultBlend = true;
		pMat->alphaTest = 0.5f;

		char mtlVariantName[skTexNameLength + skMaterialNamePad];

		//add a material variant for shiny stuff
		if (mFlatNormalIndex >= 0 && mFlatSpecIndex >= 0)
		{
			noesisMaterial_t *pMatShiny = pRapi->Noesis_GetMaterialList(1, true);
			sprintf_s(mtlVariantName, "%s%s", texName, skpShinySuffix);
			pMatShiny->name = pRapi->Noesis_PooledString(mtlVariantName);
			pMatShiny->texIdx = mTextures.Num();
			pMatShiny->flags |= defaultMtlFlags;
			pMatShiny->noDefaultBlend = true;
			pMatShiny->alphaTest = 0.5f;
			pMatShiny->normalTexIdx = mFlatNormalIndex;
			pMatShiny->specularTexIdx = mFlatSpecIndex;
			pMatShiny->specular[0] = 0.5f;
			pMatShiny->specular[1] = 0.5f;
			pMatShiny->specular[2] = 0.5f;
			pMatShiny->specular[3] = 64.0f;
			mMaterials.Append(pMatShiny);
		}

		//add soft blend and no blend variants
		noesisMaterial_t *pMatSoftBlend = pRapi->Noesis_GetMaterialList(1, true);
		sprintf_s(mtlVariantName, "%s%s", texName, skpSoftBlendSuffix);
		pMatSoftBlend->name = pRapi->Noesis_PooledString(mtlVariantName);
		pMatSoftBlend->texIdx = mTextures.Num();
		pMatSoftBlend->flags |= defaultMtlFlags;
		pMatSoftBlend->noDefaultBlend = false;
		mMaterials.Append(pMatSoftBlend);
		noesisMaterial_t *pMatNoBlend = pRapi->Noesis_GetMaterialList(1, true);
		sprintf_s(mtlVariantName, "%s%s", texName, skpNoBlendSuffix);
		pMatNoBlend->name = pRapi->Noesis_PooledString(mtlVariantName);
		pMatNoBlend->texIdx = mTextures.Num();
		pMatNoBlend->flags |= defaultMtlFlags;
		pMatNoBlend->noDefaultBlend = true;
		mMaterials.Append(pMatNoBlend);

		mTextures.Append(pTex);
		mMaterials.Append(pMat);

		return true;
	}

	CArrayList<noesisTex_t *> &Textures() { return mTextures; }
	CArrayList<noesisMaterial_t *> &Materials() { return mMaterials; }

protected:
	CArrayList<noesisTex_t *> mTextures;
	CArrayList<noesisMaterial_t *> mMaterials;
	int mFlatNormalIndex;
	int mFlatSpecIndex;
};

const char *CFFXITextureHandler::skpShinySuffix = "_explicitshiny";
const char *CFFXITextureHandler::skpSoftBlendSuffix = "_explicitsoftblend";
const char *CFFXITextureHandler::skpNoBlendSuffix = "_explicitnoblend";

//========================================================================================

class CFFXISkelHandler : public CFFXIChunkHandler
{
public:
	struct SSkelHeader
	{
		short mUnknown;
		short mBoneCount;
	};

	struct SSkelBone
	{
		unsigned char mParentIndex;
		unsigned char mUnknown;
		RichQuat mQuat;
		RichVec3 mTran;
	};

	struct SInterpretedSkel
	{
		explicit SInterpretedSkel(modelBone_t *pBones, const int boneCount)
			: mpBones(pBones)
			, mBoneCount(boneCount)
		{
		}

		modelBone_t *mpBones;
		int mBoneCount;
	};
	typedef std::vector<SInterpretedSkel> TSInterpretedSkelList;

	CFFXISkelHandler()
		: CFFXIChunkHandler(CFFXIDat::skChunkType_Skeleton)
	{
	}

	virtual CFFXIDat::EValidateChunkResult ValidateChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
															const unsigned char *pChunkData, const int dataSize) const
	{
		if (dataSize <= sizeof(SSkelHeader))
		{
			return CFFXIDat::kVCR_Invalid;
		}
		const SSkelHeader *pSkelHdr = (const SSkelHeader *)pChunkData;
		if (pSkelHdr->mBoneCount <= 0)
		{
			return CFFXIDat::kVCR_Invalid;
		}
		const int expectedBoneEndOfs = sizeof(SSkelHeader) + sizeof(SSkelBone) * pSkelHdr->mBoneCount;
		if (expectedBoneEndOfs <= 0 || expectedBoneEndOfs > dataSize)
		{
			return CFFXIDat::kVCR_Invalid;
		}
		return CFFXIDat::kVCR_Supported;
	}

	virtual bool HandleChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
								const unsigned char *pChunkData, const int dataSize)
	{
		const SSkelHeader *pSkelHdr = (const SSkelHeader *)pChunkData;
		const SSkelBone *pSkelBones = (const SSkelBone *)(pSkelHdr + 1);
		
		noeRAPI_t *pRapi = dat.GetRAPI();
		modelBone_t *pBones = pRapi->Noesis_AllocBones(pSkelHdr->mBoneCount);

		for (int boneIndex = 0; boneIndex < pSkelHdr->mBoneCount; ++boneIndex)
		{
			const SSkelBone *pSkelBone = pSkelBones + boneIndex;
			modelBone_t *pBone = pBones + boneIndex;
			RichMat43 &boneMat = (RichMat43 &)pBone->mat;

			pBone->index = boneIndex;
			sprintf_s(pBone->name, "bone%04i", boneIndex);
			boneMat = pSkelBone->mQuat.ToMat43(true);
			boneMat[3] = pSkelBone->mTran;
			pBone->eData.parent = (pSkelBone->mParentIndex != boneIndex) ? pBones + pSkelBone->mParentIndex : NULL;
		}

		pRapi->rpgMultiplyBones(pBones, pSkelHdr->mBoneCount);

		mSkeletons.push_back(SInterpretedSkel(pBones, pSkelHdr->mBoneCount));

		return true;
	}

	TSInterpretedSkelList &Skeletons() { return mSkeletons; }

protected:
	TSInterpretedSkelList mSkeletons;
};

//========================================================================================

class CFFXIAnimHandler : public CFFXIChunkHandler
{
public:
	struct SAnimHeader
	{
		unsigned short mUnknown;
		unsigned short mElemCount;
		unsigned short mFrameCount;
		float mSpeedScale; //factor with desired frame interval
	};

	//each animation element may reference up to 10 channels, 1 for each component of each transform element
	struct SAnimElemHeader
	{
		int mBoneIndex;
		int mQuatIndex[4];
		RichQuat mQuatBase;
		int mTranIndex[3];
		RichVec3 mTranBase;
		int mScaleIndex[3];
		RichVec3 mScaleBase;
	};

	struct SAnimHeaderData
	{
		explicit SAnimHeaderData(const SAnimHeader *pAnimHdr, const int animDataSize, const char *pName)
			: mpAnimHdr(pAnimHdr)
			, mAnimDataSize(animDataSize)
		{
			memcpy(mName, pName, 4);
			mName[4] = 0;
		}

		const SAnimHeader *mpAnimHdr;
		int mAnimDataSize;
		char mName[8];
	};
	typedef std::vector<SAnimHeaderData> TAnimHeaderList;

	CFFXIAnimHandler()
		: CFFXIChunkHandler(CFFXIDat::skChunkType_Animation)
	{
	}

	virtual CFFXIDat::EValidateChunkResult ValidateChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
															const unsigned char *pChunkData, const int dataSize) const
	{
		if (dataSize <= sizeof(SAnimHeader))
		{
			return CFFXIDat::kVCR_Invalid;
		}
		const SAnimHeader *pAnimHdr = (const SAnimHeader *)pChunkData;
		const int expectedElemEndOfs = sizeof(SAnimHeader) + sizeof(SAnimElemHeader) * pAnimHdr->mElemCount;
		if (expectedElemEndOfs <= 0 || expectedElemEndOfs > dataSize)
		{
			return CFFXIDat::kVCR_Invalid;
		}
		//could potentially validate elem offsets if we really care
		//const float *pData = (const float *)(pAnimHdr + 1);
		//const SAnimElemHeader *pElemHdr = (const SAnimElemHeader *)pData;
		return CFFXIDat::kVCR_Supported;
	}

	virtual bool HandleChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
								const unsigned char *pChunkData, const int dataSize)
	{
		const SAnimHeader *pAnimHdr = (const SAnimHeader *)pChunkData;
		mAnimHeaderList.push_back(SAnimHeaderData(pAnimHdr, dataSize, chunk.mName));
		return true;
	}

	noesisAnim_t *ConstructAnimations(noeRAPI_t *pRapi, const CFFXISkelHandler::SInterpretedSkel *pSkel) const
	{
		CArrayList<noesisAnim_t *> anims;

		for (TAnimHeaderList::const_iterator it = mAnimHeaderList.begin(); it != mAnimHeaderList.end(); ++it)
		{
			const SAnimHeaderData &animHeaderData = *it;
			const SAnimHeader *pAnimHdr = animHeaderData.mpAnimHdr;
			if (pAnimHdr->mFrameCount > 0 && pAnimHdr->mElemCount > 0)
			{
				const int transformCount = pSkel->mBoneCount * pAnimHdr->mFrameCount;
				//create a parent-relative base frame from the skeleton
				RichMat43 *pBaseMats = (RichMat43 *)pRapi->Noesis_UnpooledAlloc(sizeof(RichMat43) * pSkel->mBoneCount);
				for (int boneIndex = 0; boneIndex < pSkel->mBoneCount; ++boneIndex)
				{
					const modelBone_t *pBone = pSkel->mpBones + boneIndex;
					const RichMat43 &boneMat = (const RichMat43 &)pBone->mat;
					if (!pBone->eData.parent)
					{
						pBaseMats[boneIndex] = boneMat;
					}
					else
					{
						const RichMat43 &parentBoneMat = (const RichMat43 &)pBone->eData.parent->mat;
						pBaseMats[boneIndex] = boneMat * parentBoneMat.GetInverse();
					}
				}

				//initialize the anim frames with the default pose
				RichMat43 *pMats = (RichMat43 *)pRapi->Noesis_UnpooledAlloc(sizeof(RichMat43) * transformCount);
				for (int frameIndex = 0; frameIndex < pAnimHdr->mFrameCount; ++frameIndex)
				{
					memcpy(pMats + frameIndex * pSkel->mBoneCount, pBaseMats, sizeof(RichMat43) * pSkel->mBoneCount);
				}

				//now modify the transforms
				const SAnimElemHeader *pElems = (const SAnimElemHeader *)(pAnimHdr + 1);
				const float *pAnimData = (const float *)pElems;
				RichQuat frameQ;
				RichVec3 frameT;
				RichVec3 frameS;
				for (int elemIndex = 0; elemIndex < pAnimHdr->mElemCount; ++elemIndex)
				{
					const SAnimElemHeader *pElem = pElems + elemIndex;
					if (pElem->mBoneIndex < 0 || pElem->mBoneIndex >= pSkel->mBoneCount)
					{
						pRapi->LogOutput("WARNING: Out of range animation element! (index %i, but only %i bones)\n",
							pElem->mBoneIndex, pSkel->mBoneCount);
						continue;
					}

					for (int frameIndex = 0; frameIndex < pAnimHdr->mFrameCount; ++frameIndex)
					{
						RichMat43 *pFrameMats = pMats + pSkel->mBoneCount * frameIndex;
						RichMat43 &mat = pFrameMats[pElem->mBoneIndex];
						if (pElem->mQuatIndex[0] < 0 || pElem->mQuatIndex[1] < 0 || pElem->mQuatIndex[2] < 0 || pElem->mQuatIndex[3] < 0)
						{
							//no change
							mat = pBaseMats[pElem->mBoneIndex];
						}
						else
						{
							frameQ[0] = (pElem->mQuatIndex[0] > 0) ? pAnimData[pElem->mQuatIndex[0] + frameIndex] : pElem->mQuatBase[0];
							frameQ[1] = (pElem->mQuatIndex[1] > 0) ? pAnimData[pElem->mQuatIndex[1] + frameIndex] : pElem->mQuatBase[1];
							frameQ[2] = (pElem->mQuatIndex[2] > 0) ? pAnimData[pElem->mQuatIndex[2] + frameIndex] : pElem->mQuatBase[2];
							frameQ[3] = (pElem->mQuatIndex[3] > 0) ? pAnimData[pElem->mQuatIndex[3] + frameIndex] : pElem->mQuatBase[3];
							frameT[0] = (pElem->mTranIndex[0] > 0) ? pAnimData[pElem->mTranIndex[0] + frameIndex] : pElem->mTranBase[0];
							frameT[1] = (pElem->mTranIndex[1] > 0) ? pAnimData[pElem->mTranIndex[1] + frameIndex] : pElem->mTranBase[1];
							frameT[2] = (pElem->mTranIndex[2] > 0) ? pAnimData[pElem->mTranIndex[2] + frameIndex] : pElem->mTranBase[2];
							frameS[0] = (pElem->mScaleIndex[0] > 0) ? pAnimData[pElem->mScaleIndex[0] + frameIndex] : pElem->mScaleBase[0];
							frameS[1] = (pElem->mScaleIndex[1] > 0) ? pAnimData[pElem->mScaleIndex[1] + frameIndex] : pElem->mScaleBase[1];
							frameS[2] = (pElem->mScaleIndex[2] > 0) ? pAnimData[pElem->mScaleIndex[2] + frameIndex] : pElem->mScaleBase[2];

							const RichVec3 &centerPoint = pBaseMats[pElem->mBoneIndex][3];
							//re-assign, just in case something went wrong with multiple elements referencing a single bone
							mat = pBaseMats[pElem->mBoneIndex];
							mat.TransformQST(&centerPoint, &frameQ, &frameS, &centerPoint, &frameQ, &frameT);
						}
					}
				}

				noesisAnim_t *pAnim = pRapi->rpgAnimFromBonesAndMatsFinish(pSkel->mpBones, pSkel->mBoneCount, (modelMatrix_t *)pMats,
																			pAnimHdr->mFrameCount, pAnimHdr->mSpeedScale * 30.0f);
				pRapi->Noesis_UnpooledFree(pBaseMats);
				pRapi->Noesis_UnpooledFree(pMats);
				if (pAnim)
				{
					pAnim->filename = pRapi->Noesis_PooledString(const_cast<char *>(animHeaderData.mName));
					pAnim->flags |= NANIMFLAG_FILENAMETOSEQ;
					anims.Append(pAnim);
				}
			}
		}

		return (anims.Num() > 0) ? pRapi->Noesis_AnimFromAnimsList(anims, anims.Num()) : NULL;
	}

	bool AnimDataIsPresent() const { return mAnimHeaderList.size() > 0; }

protected:
	TAnimHeaderList mAnimHeaderList;
};

//========================================================================================

class CFFXIGeoHandler : public CFFXIChunkHandler
{
public:
	struct SGeoHeader
	{
		//most offsets/sizes are in quantity of shorts
		short mUnknown1;
		unsigned short mVertAndBoneRefFlag;
		unsigned short mMirror;

		int mDrawDataOfs;
		unsigned short mDrawDataSize;

		int mBoneRefOfs;
		unsigned short mBoneRefCount;

		//weird setup - offset to short-sized counts
		int mWeightedVertCountOfs;
		unsigned short mMaxWeightsPerVertex;

		//each data entry contains 2 bone indices and mirror type (see SGeoWeightData)
		int mWeightDataOfs;
		unsigned short mWeightDataCount;

		int mVertOfs;
		unsigned short mVertDataSize;

		int mUnknown2Ofs; //usually offset to end
		unsigned short mUnknown3;
		unsigned short mUnknown4;
		unsigned short mUnknown5;

		int mUnknown6Ofs; //usually offset to end
		unsigned short mUnknown7;

		int mUnknown8;
		int mUnknown9;
		unsigned short mUnknown10;
		unsigned short mUnknown11;
	};

	struct SGeoDrawState
	{
		SGeoDrawState()
		{
			mEnableShinyMat = false;
			mMaterialName[0] = 0;
		}

		bool mEnableShinyMat;
		char mMaterialName[CFFXITextureHandler::skTexNameLength + 1];
	};

	struct SGeoTriPrim
	{
		unsigned short mIndices[3];
		RichVec2 mUVs[3];
	};

	struct SGeoStripPrim
	{
		unsigned short mIndex;
		RichVec2 mUV;
	};

	struct SGeoWeightData
	{
		unsigned short mBoneIndexPass0 : 7; //unmirrored index
		unsigned short mBoneIndexPass1 : 7; //mirrored index
		unsigned short mMirrorAxis : 2; //only relevant for mirror pass
	};

	struct SGeoNativeDrawState
	{
		unsigned char mColor[4]; //just a guess, "alpha" seems to frequently switch between 0 and 128
		float mUnknown1[2];
		unsigned int mUnknown3;
		float mUnknown4[4];
		unsigned int mUnknown5;
		float mSpecular[2];
	};

	struct SGeoHeaderData
	{
		explicit SGeoHeaderData(const SGeoHeader *pGeoHdr, const int geoDataSize, const char *pName)
			: mpGeoHdr(pGeoHdr)
			, mGeoDataSize(geoDataSize)
		{
			memcpy(mName, pName, 4);
			mName[4] = 0;
		}

		const SGeoHeader *mpGeoHdr;
		int mGeoDataSize;
		char mName[8];
	};
	typedef std::vector<SGeoHeaderData> TGeoHeaderList;

	static const int skVertFlag_NoNormals = 0x7F; //if any of these bits are set, seems to indicate lack of normals. not sure what else this means.
	static const int skVertFlag_UseBoneRefs = 0x80;

	static const int skDrawCmd_SetMaterial = 0x8000;
	static const int skDrawCmd_TriList = 0x0054;
	static const int skDrawCmd_TriStrip = 0x5453;
	static const int skDrawCmd_DrawState = 0x8010;
	static const int skDrawCmd_Unknown2 = 0x4353;
	static const int skDrawCmd_Unknown3 = 0x0043;
	static const int skDrawCmd_End = 0xFFFF;

	static const int skTriCWIdx[3];
	static const int skTriCCWIdx[3];
	static const RichMat44 skMirrorTransforms[3];

	CFFXIGeoHandler()
		: CFFXIChunkHandler(CFFXIDat::skChunkType_Geo)
	{
	}

	virtual CFFXIDat::EValidateChunkResult ValidateChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
															const unsigned char *pChunkData, const int dataSize) const
	{
		if (dataSize <= sizeof(SGeoHeader))
		{
			return CFFXIDat::kVCR_Invalid;
		}
		const SGeoHeader *pGeoHdr = (const SGeoHeader *)pChunkData;
		if (pGeoHdr->mDrawDataOfs <= 0 || (pGeoHdr->mDrawDataOfs * 2) >= dataSize ||
			pGeoHdr->mVertOfs <= 0 || (pGeoHdr->mVertOfs * 2) >= dataSize ||
			(pGeoHdr->mBoneRefOfs * 2) >= dataSize ||
			(pGeoHdr->mWeightedVertCountOfs * 2) >= dataSize ||
			(pGeoHdr->mWeightDataOfs * 2) >= dataSize ||
			pGeoHdr->mDrawDataSize == 0 || pGeoHdr->mVertDataSize == 0)
		{
			return CFFXIDat::kVCR_Invalid;
		}
		else if (pGeoHdr->mWeightedVertCountOfs > 0 && pGeoHdr->mMaxWeightsPerVertex != 2)
		{
			//seems to always be in this form, even though the format would allow for more varying amounts of weights per vert
			//if interpreted that way.
			return CFFXIDat::kVCR_Invalid;
		}
		return CFFXIDat::kVCR_Supported;
	}

	virtual bool HandleChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
								const unsigned char *pChunkData, const int dataSize)
	{
		const SGeoHeader *pGeoHdr = (const SGeoHeader *)pChunkData;
		mGeoHeaderList.push_back(SGeoHeaderData(pGeoHdr, dataSize, chunk.mName));
		return true;
	}

	void RenderGeoData(noeRAPI_t *pRapi, const CFFXISkelHandler::SInterpretedSkel *pSkel) const
	{
		for (TGeoHeaderList::const_iterator it = mGeoHeaderList.begin(); it != mGeoHeaderList.end(); ++it)
		{
			const SGeoHeaderData &geoHeaderData = *it;
			const SGeoHeader *pGeoHdr = geoHeaderData.mpGeoHdr;
			const unsigned short *pSData = (const unsigned short *)pGeoHdr;

			pRapi->rpgSetName(const_cast<char *>(geoHeaderData.mName));

			SGeoDrawState drawState;

			const unsigned char *pDrawCommands = (const unsigned char *)(pSData + pGeoHdr->mDrawDataOfs);
			const int drawCommandEndOfs = (pGeoHdr->mDrawDataSize << 1);
			const int drawCommandLastCommandOfs = drawCommandEndOfs - sizeof(unsigned short);
			const int passCount = (pGeoHdr->mMirror) ? 2 : 1;
			for (int passIndex = 0; passIndex < passCount; ++passIndex)
			{
				const bool isMirroring = (passIndex > 0);
				int drawCommandOfs = 0;
				while (drawCommandOfs <= drawCommandLastCommandOfs)
				{
					const unsigned short cmdType = *get_and_incr_offset<unsigned short>(pDrawCommands, drawCommandOfs);
					switch (cmdType)
					{
					case skDrawCmd_DrawState:
						{
							//draw state isn't mapped out. might have 2-sided and blending bits in here.
							const SGeoNativeDrawState *pNativeState = get_and_incr_offset<SGeoNativeDrawState>(pDrawCommands, drawCommandOfs);
							//might be something like exponent and scale?
							drawState.mEnableShinyMat = (pNativeState->mSpecular[0] < 128.0f || pNativeState->mSpecular[1] != 0.0f);
							UpdateDrawState(pRapi, drawState);
						}
						break;

					case skDrawCmd_SetMaterial:
						{
							const char *pMatNameData = get_and_incr_offset<char>(pDrawCommands, drawCommandOfs, CFFXITextureHandler::skTexNameLength);
							memcpy(drawState.mMaterialName, pMatNameData, CFFXITextureHandler::skTexNameLength);
							drawState.mMaterialName[CFFXITextureHandler::skTexNameLength] = 0;

							UpdateDrawState(pRapi, drawState);
						}
						break;

					case skDrawCmd_TriList:
						{
							const unsigned short primCount = *get_and_incr_offset<unsigned short>(pDrawCommands, drawCommandOfs);
							const int primDataSize = sizeof(SGeoTriPrim) * primCount;
							const SGeoTriPrim *pTris = get_and_incr_offset<SGeoTriPrim>(pDrawCommands, drawCommandOfs, primCount);

							pRapi->rpgBegin(RPGEO_TRIANGLE);

							const int *pTriWindIdx = (isMirroring) ? skTriCCWIdx : skTriCWIdx;
							for (int triIdx = 0; triIdx < primCount; ++triIdx)
							{
								const SGeoTriPrim *pTri = pTris + triIdx;
								for (int triVertIdx = 0; triVertIdx < 3; ++triVertIdx)
								{
									const int windIdx = pTriWindIdx[triVertIdx];
									//really need to fix the fucking constness on these functions one of these days.
									pRapi->rpgVertUV2f(const_cast<float *>(pTri->mUVs[windIdx].v), 0);
									PlotVertex(pRapi, pTri->mIndices[windIdx], isMirroring, pGeoHdr, pSkel);
								}
							}

							pRapi->rpgEnd();
						}
						break;

					case skDrawCmd_TriStrip:
						{
							const unsigned short primCount = *get_and_incr_offset<unsigned short>(pDrawCommands, drawCommandOfs);
							const int stripVertCount = primCount - 1;
							const SGeoTriPrim *pFirstTri = get_and_incr_offset<SGeoTriPrim>(pDrawCommands, drawCommandOfs);
							const SGeoStripPrim *pStripVerts = get_and_incr_offset<SGeoStripPrim>(pDrawCommands, drawCommandOfs, stripVertCount);

							const rpgeoPrimType_e stripType = (isMirroring) ? RPGEO_TRIANGLE_STRIP_FLIPPED : RPGEO_TRIANGLE_STRIP;
							pRapi->rpgBegin(stripType);

							for (int triVertIdx = 0; triVertIdx < 3; ++triVertIdx)
							{
								pRapi->rpgVertUV2f(const_cast<float *>(pFirstTri->mUVs[triVertIdx].v), 0);
								PlotVertex(pRapi, pFirstTri->mIndices[triVertIdx], isMirroring, pGeoHdr, pSkel);
							}
							for (int stripIdx = 0; stripIdx < stripVertCount; ++stripIdx)
							{
								const SGeoStripPrim *pStripVert = pStripVerts + stripIdx;
								pRapi->rpgVertUV2f(const_cast<float *>(pStripVert->mUV.v), 0);
								PlotVertex(pRapi, pStripVert->mIndex, isMirroring, pGeoHdr, pSkel);
							}

							pRapi->rpgEnd();
						}
						break;

					case skDrawCmd_Unknown2:
						{
							const unsigned short primCount = *get_and_incr_offset<unsigned short>(pDrawCommands, drawCommandOfs);
							get_and_incr_offset<unsigned char>(pDrawCommands, drawCommandOfs, 8);
							get_and_incr_offset<unsigned short>(pDrawCommands, drawCommandOfs, primCount);
						}
						break;

					case skDrawCmd_Unknown3:
						{
							const unsigned short primCount = *get_and_incr_offset<unsigned short>(pDrawCommands, drawCommandOfs);
							get_and_incr_offset<unsigned char>(pDrawCommands, drawCommandOfs, primCount * 10);
						}
						break;

					default:
						{
							NoeAssert(cmdType == skDrawCmd_End);
							drawCommandOfs = drawCommandEndOfs;
						}
						break;
					}
				}
			}
		}

		if (pSkel)
		{
			pRapi->rpgSetExData_Bones(pSkel->mpBones, pSkel->mBoneCount);
		}
	}

	bool GeoDataIsPresent() const { return mGeoHeaderList.size() > 0; }

protected:
	static void PlotVertex(noeRAPI_t *pRapi, const int index, const bool isMirroring,
							const SGeoHeader *pGeoHdr, const CFFXISkelHandler::SInterpretedSkel *pSkel)
	{
		const unsigned short *pSData = (const unsigned short *)pGeoHdr;
		const bool isSkinned = (pSkel && pGeoHdr->mWeightedVertCountOfs > 0 && pGeoHdr->mWeightDataOfs > 0);
		const bool noNormals = (pGeoHdr->mVertAndBoneRefFlag & skVertFlag_NoNormals) != 0;
		int oneWeightVertCount;
		if (pGeoHdr->mWeightedVertCountOfs > 0)
		{
			const unsigned short *pWeightedCounts = pSData + pGeoHdr->mWeightedVertCountOfs;
			oneWeightVertCount = pWeightedCounts[0];
		}
		else
		{
			//consider every vert single-weight
			oneWeightVertCount = 0x10000;
		}

		const int weightCount = (index < oneWeightVertCount) ? 1 : 2;
		const int oneWeightVertSize = (noNormals) ? sizeof(float) * 3 : sizeof(float) * 6;
		const int twoWeightVertSize = oneWeightVertSize * 2 + sizeof(float) * 2;
		const int oneWeightVertElemCount = (oneWeightVertSize >> 2);
		const int twoWeightVertElemCount = (twoWeightVertSize >> 2);

		const int firstTwoWeightElemIndex = oneWeightVertCount * oneWeightVertElemCount;

		const float *pVertElems = (const float *)(pSData + pGeoHdr->mVertOfs);
		const float *pVertData = (weightCount == 1) ? pVertElems + index * oneWeightVertElemCount :
									pVertElems + firstTwoWeightElemIndex + (index - oneWeightVertCount) * twoWeightVertElemCount;
		const float *pNrmVertData = (noNormals) ? NULL : pVertData + 3 * weightCount + ((weightCount > 1) ? weightCount : 0);
		
		//interleaved elements for each weight, pretty nasty.
		if (!isSkinned)
		{
			pRapi->rpgVertBoneIndexI(NULL, 0);
			pRapi->rpgVertBoneWeightF(NULL, 0);
			if (pNrmVertData)
			{
				const RichVec3 nrm(pNrmVertData[0 * weightCount], pNrmVertData[1 * weightCount], pNrmVertData[2 * weightCount]);
				pRapi->rpgVertNormal3f(const_cast<float *>(nrm.v));
			}
			else
			{
				pRapi->rpgVertNormal3f(NULL);
			}
			const RichVec3 pos(pVertData[0 * weightCount], pVertData[1 * weightCount], pVertData[2 * weightCount]);
			pRapi->rpgVertex3f(const_cast<float *>(pos.v));
		}
		else
		{
			const unsigned short *pBoneRefs = ((pGeoHdr->mVertAndBoneRefFlag & skVertFlag_UseBoneRefs) && pGeoHdr->mBoneRefOfs > 0) ?
												pSData + pGeoHdr->mBoneRefOfs : NULL;

			static const float skDefaultWeights[2] = { 1.0f, 0.0f };
			const SGeoWeightData *pWeightDatas = (const SGeoWeightData *)(pSData + pGeoHdr->mWeightDataOfs) + index * 2;
			int boneIndices[2] = { 0, 0 };
			RichMat44 skinMats[2]; //we pull the matrices out, as we may end up needing to modify them for mirroring anyway. could be optimized.
			for (int weightIndex = 0; weightIndex < weightCount; ++weightIndex)
			{
				const SGeoWeightData *pWeightData = pWeightDatas + weightIndex;
				int &boneIndex = boneIndices[weightIndex];
				boneIndex = (isMirroring) ? pWeightData->mBoneIndexPass1 : pWeightData->mBoneIndexPass0;
				NoeAssert(!pBoneRefs || boneIndex < pGeoHdr->mBoneRefCount);
				if (pBoneRefs && boneIndex < pGeoHdr->mBoneRefCount)
				{
					boneIndex = pBoneRefs[boneIndex];
				}
				NoeAssert(boneIndex >= 0 && boneIndex < pSkel->mBoneCount);
				skinMats[weightIndex] = ((RichMat43 *)&pSkel->mpBones[boneIndex].mat)->ToMat44();
				if (isMirroring && pWeightData->mMirrorAxis)
				{
					skinMats[weightIndex] = skMirrorTransforms[pWeightData->mMirrorAxis - 1] * skinMats[weightIndex];
				}
			}
			const float *pWeightValues = (weightCount > 1) ? pVertData + 3 * weightCount : skDefaultWeights;
			//feed 2 weights to Noesis in order to keep consistent per-triangle. Noesis doesn't require this, but this allows the draws between
			//1 and 2 weight segments to not be partitioned.
			pRapi->rpgVertBoneIndexI(const_cast<int *>(boneIndices), 2);
			pRapi->rpgVertBoneWeightF(const_cast<float *>(pWeightValues), 2);

			//now that we've pulled the weights out and fed them in, we need to put our verts in model space.
			RichVec4 transformedPos;
			RichVec3 transformedNrm;
			for (int weightIndex = 0; weightIndex < weightCount; ++weightIndex)
			{
				const RichMat44 &skinMat = skinMats[weightIndex];
				const RichVec4 pos(
					pVertData[weightIndex + 0 * weightCount],
					pVertData[weightIndex + 1 * weightCount],
					pVertData[weightIndex + 2 * weightCount],
					//this is certainly one of the most terrible ways you can possibly do weighted transforms. we're effectively weighting the
					//matrix translation with each transform. this is why the per-weight positions are needed. so, rather than having to actually
					//weight each translation, we can just stuff the weight into the w for our 4x4 transform.
					pWeightValues[weightIndex]
				);

				transformedPos += skinMat.TransformVec4(pos);

				if (pNrmVertData)
				{
					const RichVec3 nrm(
						pNrmVertData[weightIndex + 0 * weightCount],
						pNrmVertData[weightIndex + 1 * weightCount],
						pNrmVertData[weightIndex + 2 * weightCount]
					);
					//this is inconsistent with the position transform, but appears to be in line with the game itself.
					transformedNrm += skinMat.TransformNormal(nrm) * pWeightValues[weightIndex];
				}
			}

			if (pNrmVertData)
			{
				transformedNrm.Normalize();
				pRapi->rpgVertNormal3f(transformedNrm.v);
			}
			else
			{
				pRapi->rpgVertNormal3f(NULL);
			}
			pRapi->rpgVertex3f(transformedPos.v);
		}
	}

	static void UpdateDrawState(noeRAPI_t *pRapi, SGeoDrawState &drawState)
	{
		if (drawState.mEnableShinyMat && (!gpFF11Opts || !gpFF11Opts->noShinyMaterials))
		{
			char materialName[CFFXITextureHandler::skTexNameLength + CFFXITextureHandler::skMaterialNamePad];
			sprintf_s(materialName, "%s%s", drawState.mMaterialName, CFFXITextureHandler::skpShinySuffix);
			pRapi->rpgSetMaterial(materialName);
		}
		else
		{
			pRapi->rpgSetMaterial(drawState.mMaterialName);
		}
	}

	TGeoHeaderList mGeoHeaderList;
};

const RichMat44 CFFXIGeoHandler::skMirrorTransforms[3] =
{
	RichMat44(
		-RichVec4(g_identityMatrix4x4.c1),
		 RichVec4(g_identityMatrix4x4.c2),
		 RichVec4(g_identityMatrix4x4.c3),
		 RichVec4(g_identityMatrix4x4.c4)
	),
	RichMat44(
		 RichVec4(g_identityMatrix4x4.c1),
		-RichVec4(g_identityMatrix4x4.c2),
		 RichVec4(g_identityMatrix4x4.c3),
		 RichVec4(g_identityMatrix4x4.c4)
	),
	RichMat44(
		 RichVec4(g_identityMatrix4x4.c1),
		 RichVec4(g_identityMatrix4x4.c2),
		-RichVec4(g_identityMatrix4x4.c3),
		 RichVec4(g_identityMatrix4x4.c4)
	)
};

const int CFFXIGeoHandler::skTriCWIdx[3] = { 0, 1, 2 };

const int CFFXIGeoHandler::skTriCCWIdx[3] = { 2, 1, 0 };

//========================================================================================

class CFFXIMapHandler : public CFFXIChunkHandler
{
public:
	struct SMapHeader
	{
		unsigned char mHeaderData[4];
		unsigned int mObjectCount : 24;
		unsigned int mUnknown1 : 8;
		unsigned int mUnknown2[6];
	};

	static const int skObjectNameLength = 16;
	struct SMapObject
	{
		char mObjectName[skObjectNameLength];
		
		RichVec3 mTrans;
		float mRawAngles[3];
		RichVec3 mScale;

		RichVec4 mVec;
		int mData2[8];
	};

	struct SInterpretedMapObject
	{
		explicit SInterpretedMapObject(const SMapObject *pMapObject)
		{
			mTransform = RichAngles(pMapObject->mRawAngles, true).ToMat43_XYZ();
			mTransform[3] = pMapObject->mTrans;
			mTransform[0][0] *= pMapObject->mScale[0];
			mTransform[1][0] *= pMapObject->mScale[0];
			mTransform[2][0] *= pMapObject->mScale[0];
			mTransform[0][1] *= pMapObject->mScale[1];
			mTransform[1][1] *= pMapObject->mScale[1];
			mTransform[2][1] *= pMapObject->mScale[1];
			mTransform[0][2] *= pMapObject->mScale[2];
			mTransform[1][2] *= pMapObject->mScale[2];
			mTransform[2][2] *= pMapObject->mScale[2];
			mBackwardWinding = (pMapObject->mScale[0] * pMapObject->mScale[1] * pMapObject->mScale[2]) < 0.0f;
			memcpy(mObjectName, pMapObject->mObjectName, skObjectNameLength);
			mObjectFlags[0] = pMapObject->mData2[0];
			mObjectFlags[1] = pMapObject->mData2[4];
			mVec = pMapObject->mVec;
		};

		char mObjectName[skObjectNameLength];

		RichMat43 mTransform;
		bool mBackwardWinding;

		unsigned int mObjectFlags[2];
		RichVec4 mVec;
	};
	typedef std::vector<SInterpretedMapObject> TMapObjectList;

	CFFXIMapHandler()
		: CFFXIChunkHandler(CFFXIDat::skChunkType_Map)
	{
	}

	virtual CFFXIDat::EValidateChunkResult ValidateChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
															const unsigned char *pChunkData, const int dataSize) const
	{
		if (dataSize < 16)
		{
			return CFFXIDat::kVCR_Invalid;
		}
		else if (pChunkData[3] < 0x1B)
		{
			if ((*(unsigned int *)pChunkData & 0xFFFFFF) != 0x00425a4d) //MZB
			{
				return CFFXIDat::kVCR_Invalid;
			}
		}
		else
		{
			const int dataLen = ((*(const unsigned int *)pChunkData) & 0xFFFFFF);
			if (dataLen > dataSize ||
				(dataLen + 16) < dataSize)
			{
				return CFFXIDat::kVCR_Invalid;
			}
		}
		return CFFXIDat::kVCR_Supported;
	}

	virtual bool HandleChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
								const unsigned char *pChunkData, const int dataSize)
	{
		noeRAPI_t *pRapi = dat.GetRAPI();
		unsigned char *pDecrypted = (unsigned char *)pRapi->Noesis_UnpooledAlloc(dataSize);
		memcpy(pDecrypted, pChunkData, dataSize);
		Model_FF11_DecryptMZB(pDecrypted, dataSize);

		const SMapHeader *pMapHdr = (const SMapHeader *)pDecrypted;
		const SMapObject *pMapObjects = (const SMapObject *)(pMapHdr + 1);

		mMapObjects.reserve(mMapObjects.size() + pMapHdr->mObjectCount);
		for (unsigned int objectIndex = 0; objectIndex < pMapHdr->mObjectCount; ++objectIndex)
		{
			const SMapObject *pMapObject = pMapObjects + objectIndex;
			mMapObjects.push_back(SInterpretedMapObject(pMapObject));
		}

		//data following map objects isn't mapped out, but likely pertains to visibiltiy and partitioning

		pRapi->Noesis_UnpooledFree(pDecrypted);
		return true;
	}

	TMapObjectList &MapObjects() { return mMapObjects; }

protected:
	TMapObjectList mMapObjects;
};

//========================================================================================

class CFFXIMapGeoHandler : public CFFXIChunkHandler
{
public:
	struct SMapGeoHeader
	{
		unsigned char mHeaderData[4];
		unsigned int mUnknown1;
		char mUnknownName[8];
		char mObjectName[CFFXIMapHandler::skObjectNameLength];
	};

	struct SMapGeoData
	{
		explicit SMapGeoData(const SMapGeoHeader *pMapGeoHdr, noeRAPI_t *pAllocatingInstance, const int dataSize)
			: mpMapGeoHdr(pMapGeoHdr)
			, mpAllocatingInstance(pAllocatingInstance)
			, mDataSize(dataSize)
		{
		}

		const SMapGeoHeader *mpMapGeoHdr;
		noeRAPI_t *mpAllocatingInstance;
		int mDataSize;
	};
	typedef std::vector<SMapGeoData> TMapGeoList;

	struct SDrawHeader
	{
		int mSegCount;
		float mBounds[6];
		int mFlag; //typically 64 for super header, varying for sub
	};

	static const int skMapGeoFlag_BlendHardAlpha = 0x2000;
	static const int skMapGeoFlag_BlendTerrain = 0x8000;

	static const int skDefaultVertColorFixShift = 1;
	static const int skDefaultVertAlphaFixShift = 1;

	CFFXIMapGeoHandler()
		: CFFXIChunkHandler(CFFXIDat::skChunkType_MapGeo)
		, mMapGeoHash(CFFXIMapHandler::skObjectNameLength)
	{
	}

	virtual ~CFFXIMapGeoHandler()
	{
		for (TMapGeoList::iterator it = mMapGeoList.begin(); it != mMapGeoList.end(); ++it)
		{
			SMapGeoData &geoData = *it;
			geoData.mpAllocatingInstance->Noesis_UnpooledFree((void *)geoData.mpMapGeoHdr);
		}
	}

	virtual CFFXIDat::EValidateChunkResult ValidateChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
															const unsigned char *pChunkData, const int dataSize) const
	{
		if (dataSize < 16)
		{
			return CFFXIDat::kVCR_Invalid;
		}
		else if (pChunkData[3] < 5)
		{
			if ((*(unsigned int *)pChunkData & 0xFFFFFF) != 0x00424d4d) //MMB
			{
				return CFFXIDat::kVCR_Invalid;
			}
		}
		else
		{
			const int dataLen = ((*(const unsigned int *)pChunkData) & 0xFFFFFF);
			if (dataLen > dataSize ||
				(dataLen + 16) < dataSize)
			{
				return CFFXIDat::kVCR_Invalid;
			}
		}
		return CFFXIDat::kVCR_Supported;
	}

	virtual bool HandleChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
								const unsigned char *pChunkData, const int dataSize)
	{
		noeRAPI_t *pRapi = dat.GetRAPI();
		unsigned char *pDecrypted = (unsigned char *)pRapi->Noesis_UnpooledAlloc(dataSize);
		memcpy(pDecrypted, pChunkData, dataSize);
		Model_FF11_DecryptMMB(pDecrypted, dataSize);

		const SMapGeoHeader *pMapGeoHdr = (const SMapGeoHeader *)pDecrypted;
		const int index = mMapGeoList.size();
		mMapGeoList.push_back(SMapGeoData(pMapGeoHdr, pRapi, dataSize));
		mMapGeoHash.FindOrAddResource(pMapGeoHdr->mObjectName, index, true);
		return true;
	}

	void RenderMapObjectGeo(noeRAPI_t *pRapi, const int index, const RichMat43 &transform, const bool backwardWinding,
							const CFFXIMapHandler::SInterpretedMapObject *pMapObject)
	{
		SMapGeoData &geoData = mMapGeoList[index];
		const SMapGeoHeader *pMapGeoHdr = geoData.mpMapGeoHdr;
		const unsigned char *pDrawData = (const unsigned char *)pMapGeoHdr;
		const int endDrawOfs = geoData.mDataSize - sizeof(SDrawHeader);
		const unsigned int objectFlags0 = (pMapObject) ? pMapObject->mObjectFlags[0] : 0;
		const unsigned int objectFlags1 = (pMapObject) ? pMapObject->mObjectFlags[1] : 0;

#if !defined(_DEBUG_MAP_MESHES)
		if (gpFF11Opts && gpFF11Opts->keepNames)
		{
			char nameString[CFFXIMapHandler::skObjectNameLength + 1];
			memcpy(nameString, pMapGeoHdr->mObjectName, CFFXIMapHandler::skObjectNameLength);
			nameString[CFFXIMapHandler::skObjectNameLength] = 0;
			pRapi->rpgSetName(nameString);
		}
#endif

		pRapi->rpgSetTransform(const_cast<modelMatrix_t *>(&transform.m));

		char matName[CFFXITextureHandler::skTexNameLength + CFFXITextureHandler::skMaterialNamePad];

		int drawOfs = sizeof(SMapGeoHeader);
		while (drawOfs <= endDrawOfs)
		{
			const SDrawHeader *pSuperHeader = get_and_incr_offset<SDrawHeader>(pDrawData, drawOfs);
			for (int superIndex = 0; superIndex < pSuperHeader->mSegCount && drawOfs <= endDrawOfs; ++superIndex)
			{
				const SDrawHeader *pSubHeader = get_and_incr_offset<SDrawHeader>(pDrawData, drawOfs);
				for (int subIndex = 0; subIndex < pSubHeader->mSegCount && drawOfs <= endDrawOfs; ++subIndex)
				{
					NoeAssert((drawOfs & 3) == 0);
					const char *pMatName = get_and_incr_offset<char>(pDrawData, drawOfs, CFFXITextureHandler::skTexNameLength);

					const unsigned short vertCount = *get_and_incr_offset<unsigned short>(pDrawData, drawOfs);
					const unsigned short blendFlags = *get_and_incr_offset<unsigned short>(pDrawData, drawOfs);
					const int vertStride = (pSubHeader->mFlag == 0) ? 48 : 36;
					const unsigned char *pVertData = get_and_incr_offset<unsigned char>(pDrawData, drawOfs, vertStride * vertCount);
					const unsigned short indexCount = *get_and_incr_offset<unsigned short>(pDrawData, drawOfs);
					const unsigned short flags2 = *get_and_incr_offset<unsigned short>(pDrawData, drawOfs);
					const unsigned short *pIndexData = get_and_incr_offset<unsigned short>(pDrawData, drawOfs, indexCount);
					align_offset(drawOfs, 4);

#if defined(_DEBUG_MAP_MESHES)
					//segment things up to give us some more info about each mesh in debug
					char nameString[CFFXIMapHandler::skObjectNameLength + 128];
					memcpy(nameString, pMapGeoHdr->mObjectName, CFFXIMapHandler::skObjectNameLength);
					NoeAssert(pMapObject);
					//sprintf_s(&nameString[CFFXIMapHandler::skObjectNameLength], 128, "_fl%08x_fb%08x_ps%08x_bl%08x",
					//			objectFlags0, objectFlags1, pSubHeader->mFlag, blendFlags);
					sprintf_s(&nameString[CFFXIMapHandler::skObjectNameLength], 128, "_fl%08x_x%.02fy%.02fz%.02fw%.02f",
								objectFlags0, pMapObject->mVec[0], pMapObject->mVec[1], pMapObject->mVec[2], pMapObject->mVec[3]);
					pRapi->rpgSetName(nameString);
#endif

					memcpy(matName, pMatName, CFFXITextureHandler::skTexNameLength);
					matName[CFFXITextureHandler::skTexNameLength] = 0;
					int candidateBlendFlags = skMapGeoFlag_BlendTerrain;
					//this is pretty certainly not correct
					const bool explicitObjectTransparency = (objectFlags0 & 0x01000000) != 0;
					if (explicitObjectTransparency)
					{
						candidateBlendFlags |= skMapGeoFlag_BlendHardAlpha;
					}
					//this is a bunch of bullshit, still not sure what reliably controls blended objects.
					//terrain blend flags are definitely in blendFlags, but lots of non-terrain objects still look to need blending.
					const bool shouldBlend = ((explicitObjectTransparency && vertStride == 48) || (blendFlags & candidateBlendFlags) ||
												(pMapObject && objectFlags0 == 0x01000000 && pMapObject->mVec[3] < 1000.0f));
					const char *pBlendSuffix = (shouldBlend) ?
												CFFXITextureHandler::skpSoftBlendSuffix : CFFXITextureHandler::skpNoBlendSuffix;
					strcpy_s(&matName[CFFXITextureHandler::skTexNameLength], CFFXITextureHandler::skMaterialNamePad, pBlendSuffix);
					pRapi->rpgSetMaterial(matName);

					if (vertCount == 0 || indexCount < 3)
					{
						pRapi->LogOutput("WARNING: Unexpected vert/index count.\n");
						break;
					}
					else if (drawOfs > geoData.mDataSize)
					{
						pRapi->LogOutput("WARNING: Ran off end of MapGeo.\n");
						break;
					}

					int posOfs, nrmOfs, clrOfs, uvOfs;
					switch (vertStride)
					{
					case 48:
						posOfs = 0;
						nrmOfs = 24;
						clrOfs = 36;
						uvOfs = 40;
						break;
					default:
						NoeAssert(vertStride == 36);
						posOfs = 0;
						nrmOfs = 12;
						clrOfs = 24;
						uvOfs = 28;
						break;
					}

					const int colorShift = (gpFF11Opts && gpFF11Opts->explicitVertColorShift) ?
											gpFF11Opts->fixVertColorShift : skDefaultVertColorFixShift;
					const int alphaShift = (gpFF11Opts && gpFF11Opts->explicitVertAlphaShift) ?
											gpFF11Opts->fixVertAlphaShift : skDefaultVertAlphaFixShift;

					const unsigned int debugSeedBase = 0;

					if (pSubHeader->mFlag == 0)
					{
						//tri list
						const int *pTriWindIdx = (backwardWinding) ? CFFXIGeoHandler::skTriCCWIdx : CFFXIGeoHandler::skTriCWIdx;
						pRapi->rpgBegin(RPGEO_TRIANGLE);
						for (int index = 0; index < indexCount; index += 3)
						{
							for (int triIndex = 0; triIndex < 3; ++triIndex)
							{
								const int vertIndex = pIndexData[index + pTriWindIdx[triIndex]];
								const unsigned char *pVert = pVertData + vertIndex * vertStride;
								PlotMapVertex(pRapi, pVert, posOfs, nrmOfs, clrOfs, uvOfs, colorShift, alphaShift, debugSeedBase);
							}
						}
						pRapi->rpgEnd();
					}
					else
					{
						//tri strip
						const rpgeoPrimType_e primType = (backwardWinding) ? RPGEO_TRIANGLE_STRIP_FLIPPED : RPGEO_TRIANGLE_STRIP;
						pRapi->rpgBegin(primType);
						for (int index = 0; index < indexCount; ++index)
						{
							const int vertIndex = pIndexData[index];
							const unsigned char *pVert = pVertData + vertIndex * vertStride;
							PlotMapVertex(pRapi, pVert, posOfs, nrmOfs, clrOfs, uvOfs, colorShift, alphaShift, debugSeedBase);
						}
						pRapi->rpgEnd();
					}
				}
			}
		}

		pRapi->rpgSetTransform(NULL);
	}

	void RenderMapObjectGeoForMapObject(noeRAPI_t *pRapi, const CFFXIMapHandler::SInterpretedMapObject &mapObject)
	{
		const int index = mMapGeoHash.FindOrAddResource(mapObject.mObjectName, -1);
		if (index < 0)
		{
			pRapi->LogOutput("WARNING: Could not find object in resource hash, skipping.\n");
		}
		else
		{
			RenderMapObjectGeo(pRapi, index, mapObject.mTransform, mapObject.mBackwardWinding, &mapObject);
		}
	}

	const TMapGeoList &GetMapGeoList() const { return mMapGeoList; }

protected:
	void PlotMapVertex(noeRAPI_t *pRapi, const unsigned char *pVert, const int posOfs, const int nrmOfs, const int clrOfs, const int uvOfs,
						const int colorShift, const int alphaShift, const unsigned int debugSeedBase)
	{
		pRapi->rpgVertNormal3f((float *)(pVert + nrmOfs));
						
		if (!debugSeedBase)
		{
			pRapi->rpgVertUV2f((float *)(pVert + uvOfs), 0);
			if (!gpFF11Opts || !gpFF11Opts->noVertColors)
			{
				if (colorShift || alphaShift)
				{
					//fix the color/alpha range
					unsigned char clr[4];
					memcpy(clr, pVert + clrOfs, 4);
					clr[0] = (unsigned char)std::min<int>((int)clr[0] << colorShift, 255);
					clr[1] = (unsigned char)std::min<int>((int)clr[1] << colorShift, 255);
					clr[2] = (unsigned char)std::min<int>((int)clr[2] << colorShift, 255);
					clr[3] = (unsigned char)std::min<int>((int)clr[3] << alphaShift, 255);
					pRapi->rpgVertColor4ub(clr);
				}
				else
				{
					pRapi->rpgVertColor4ub(const_cast<unsigned char *>(pVert + clrOfs));
				}
			}
		}
		else
		{
			unsigned int debugSeed = debugSeedBase;
			pRapi->rpgVertUV2f(NULL, 0);
			const float debugColor[4] =
			{
				g_mfn->Math_RandFloatOnSeed(0.5f, 1.0f, debugSeed),
				g_mfn->Math_RandFloatOnSeed(0.5f, 1.0f, debugSeed),
				g_mfn->Math_RandFloatOnSeed(0.5f, 1.0f, debugSeed),
				1.0f
			};
			pRapi->rpgVertColor4f(const_cast<float *>(debugColor));
		}

		pRapi->rpgVertex3f((float *)(pVert + posOfs));
	}

	TMapGeoList mMapGeoList;
	CLocalResHash mMapGeoHash;
};

//========================================================================================

bool Model_FF11_CheckDAT(BYTE *fileBuffer, int bufferLen, noeRAPI_t *rapi)
{
	CFFXIDat dat(fileBuffer, bufferLen, rapi);
	CFFXIDefaultHandlerSet datHandlers(&dat);

	return dat.ParseChunksOfInterest();
}

static noesisModel_t *Model_FF11_ConstructModelFromHandlerSet(noeRAPI_t *pRapi, CFFXIDefaultHandlerSet &datHandlers, bool promptForExternalSkel)
{
	noesisMatData_t *pMd = NULL;

	CFFXITextureHandler *pTextureHandler = datHandlers.TextureHandler();
	if (pTextureHandler->Textures().Num() > 0)
	{
		pMd = pRapi->Noesis_GetMatDataFromLists(pTextureHandler->Materials(), pTextureHandler->Textures());
	}

	noesisModel_t *pMdl = NULL;

	//possible todo - can we have more than 1 skeleton in a dat with skinned geo and/or anims?
	const CFFXISkelHandler::SInterpretedSkel *pSkel = NULL;
	CFFXISkelHandler *pSkelHandler = datHandlers.SkelHandler();
	if (pSkelHandler->Skeletons().size() > 0)
	{
		//just pick the first skeleton for now
		pSkel = &pSkelHandler->Skeletons()[0];
	}

	noesisAnim_t *pAnim = NULL;
	const CFFXIAnimHandler *pAnimHandler = datHandlers.AnimHandler();
	const CFFXIGeoHandler *pGeoHandler = datHandlers.GeoHandler();

	CFFXIDat *pSkelDat = NULL;
	unsigned char *pSkelDatBuffer = NULL;
	if (promptForExternalSkel &&
		!pSkel &&
		(pAnimHandler->AnimDataIsPresent() || pGeoHandler->GeoDataIsPresent()))
	{
		//prompt to load skeleton from another dat
		int skelDatSize = 0;
		pSkelDatBuffer = pRapi->Noesis_LoadPairedFile("FFXI Skeleton DAT", ".dat", skelDatSize, NULL);
		if (pSkelDatBuffer)
		{
			pSkelDat = new CFFXIDat(pSkelDatBuffer, skelDatSize, pRapi);
			//register the existing handlers with the new dat and just load the skeleton (and possibly animations) into the exisating handlers.
			datHandlers.RegisterHandlersWithDat(*pSkelDat);
			if (pSkelDat->ParseChunksOfInterest() &&
				pSkelDat->RunChunkHandlersForChunksOfInterest(CFFXIDat::skChunkType_Skeleton))
			{
				if (pSkelHandler->Skeletons().size() > 0)
				{
					pSkel = &pSkelHandler->Skeletons()[0];
					if (!pAnimHandler->AnimDataIsPresent())
					{ //if there are no animations in the dat being loaded, try loading them from the skeleton dat.
						pSkelDat->RunChunkHandlersForChunksOfInterest(CFFXIDat::skChunkType_Animation);
					}
				}
			}
		}
	}

	if (pAnimHandler->AnimDataIsPresent())
	{
		if (!pSkel)
		{
			pRapi->LogOutput("WARNING: Discarding animation data, because no skeleton is present.\n");
		}
		else
		{
			pAnim = pAnimHandler->ConstructAnimations(pRapi, pSkel);
		}
	}

	CFFXIMapHandler *pMapHandler = datHandlers.MapHandler();
	CFFXIMapGeoHandler *pMapGeoHandler = datHandlers.MapGeoHandler();
	const CFFXIMapGeoHandler::TMapGeoList &mapGeoList = pMapGeoHandler->GetMapGeoList();
	const int mapGeoCount = mapGeoList.size();

	const bool anyGeoDataIsPresent = (mapGeoCount > 0 || pGeoHandler->GeoDataIsPresent());

	void *pCtx = NULL;
	if (anyGeoDataIsPresent)
	{
		pCtx = pRapi->rpgCreateContext();
		pRapi->rpgSetOption(RPGOPT_TRIWINDBACKWARD, true);
	}

	//render any character data
	if (pGeoHandler->GeoDataIsPresent())
	{
		pGeoHandler->RenderGeoData(pRapi, pSkel);
	}

	//check for map geo, and toss it into the same rpg context (for now - if these things are often present at once, throwing into separate
	//models probably makes sense)
	if (mapGeoCount > 0)
	{
		CFFXIMapHandler::TMapObjectList &mapObjects = pMapHandler->MapObjects();
		if (mapObjects.size() > 0)
		{
			for (CFFXIMapHandler::TMapObjectList::const_iterator it = mapObjects.begin(); it != mapObjects.end(); ++it)
			{
				const CFFXIMapHandler::SInterpretedMapObject &mapObject = *it;
				pMapGeoHandler->RenderMapObjectGeoForMapObject(pRapi, mapObject);
			}

			if (gpFF11Opts && gpFF11Opts->renderUnreferenced)
			{
				RichMat43 unreferencedTransform;
				//run through and manually render any geometry that wasn't referenced by a map object
				for (int mapGeoIndex = 0; mapGeoIndex < mapGeoCount; ++mapGeoIndex)
				{
					//not particularly concerned about speed here, it's not a default option
					const CFFXIMapGeoHandler::SMapGeoData &mapGeoData = mapGeoList[mapGeoIndex];
					bool isReferenced = false;
					for (CFFXIMapHandler::TMapObjectList::const_iterator it = mapObjects.begin(); it != mapObjects.end(); ++it)
					{
						const CFFXIMapHandler::SInterpretedMapObject &mapObject = *it;
						if (memcmp(mapGeoData.mpMapGeoHdr->mObjectName, mapObject.mObjectName, CFFXIMapHandler::skObjectNameLength) == 0)
						{
							isReferenced = true;
							break;
						}
					}

					if (!isReferenced)
					{
						pMapGeoHandler->RenderMapObjectGeo(pRapi, mapGeoIndex, unreferencedTransform, false, NULL);
					}
				}
			}
		}
		else
		{
			//if no map data exists alongside the map geo, just render the raw data at identity
			RichMat43 defaultTransform;
			for (int mapGeoIndex = 0; mapGeoIndex < mapGeoCount; ++mapGeoIndex)
			{
				pMapGeoHandler->RenderMapObjectGeo(pRapi, mapGeoIndex, defaultTransform, false, NULL);
			}
		}
	}

	//construct the model from the combined rendering
	if (pCtx)
	{
		if (pAnim)
		{
			pRapi->rpgSetExData_Anims(pAnim);
		}
		if (pMd)
		{
			pRapi->rpgSetExData_Materials(pMd);
		}

		NoeAssert(anyGeoDataIsPresent);
		if (gpFF11Opts && gpFF11Opts->optimizeGeo)
		{
			pRapi->rpgOptimize();
			pMdl = pRapi->rpgConstructModel();
		}
		else
		{
			pMdl = pRapi->rpgConstructModelAndSort();
		}
		pRapi->rpgDestroyContext(pCtx);
	}

	//if a model wasn't constructed, create a container for any anims and/or textures we loaded
	if ((pAnim || pMd) && !pMdl)
	{
		pMdl = pRapi->Noesis_AllocModelContainer(pMd, pAnim, (pAnim) ? 1 : 0);
	}

	//free second skeleton dat if it was created
	if (pSkelDatBuffer)
	{
		pRapi->Noesis_UnpooledFree(pSkelDatBuffer);
		if (pSkelDat)
		{
			delete pSkelDat;
		}
	}

	return pMdl;
}

static void Model_FF11_SetPreviewOffset(noeRAPI_t *pRapi)
{
	float mdlAngOfs[3] = { 0.0f, 180.0f, 270.0f };
	pRapi->SetPreviewAngOfs(mdlAngOfs);
}

noesisModel_t *Model_FF11_LoadDAT(BYTE *fileBuffer, int bufferLen, int &numMdl, noeRAPI_t *rapi)
{
	CFFXIDat dat(fileBuffer, bufferLen, rapi);
	CFFXIDefaultHandlerSet datHandlers(&dat);
	dat.ParseChunksOfInterest();

	if (!dat.RunChunkHandlersForChunksOfInterest())
	{
		rapi->LogOutput("Error: Unrecoverable error during chunk handling.\n");
		return NULL;
	}

	noesisModel_t *pMdl = Model_FF11_ConstructModelFromHandlerSet(rapi, datHandlers, true);

	Model_FF11_SetPreviewOffset(rapi);

	numMdl = (pMdl) ? 1 : 0;
	return pMdl;
}

ff11Opts_t *gpFF11Opts = NULL;

#define FF11_LOCAL_DECL_OPTS(argRequired) \
	ff11Opts_t *pOpts = (ff11Opts_t *)store; \
	NoeAssert(storeSize == sizeof(ff11Opts_t)); \
	if (argRequired && !arg) \
	{ \
		return false; \
	}

bool Model_FF11_ShiftColorHandler(const char *arg, unsigned char *store, int storeSize)
{
	FF11_LOCAL_DECL_OPTS(true);
	pOpts->fixColorShift = atoi(arg);
	pOpts->explicitColorShift = true;
	return true;
}

bool Model_FF11_ShiftAlphaHandler(const char *arg, unsigned char *store, int storeSize)
{
	FF11_LOCAL_DECL_OPTS(true);
	pOpts->fixAlphaShift = atoi(arg);
	pOpts->explicitAlphaShift = true;
	return true;
}

bool Model_FF11_ShiftVertColorHandler(const char *arg, unsigned char *store, int storeSize)
{
	FF11_LOCAL_DECL_OPTS(true);
	pOpts->fixVertColorShift = atoi(arg);
	pOpts->explicitVertColorShift = true;
	return true;
}

bool Model_FF11_ShiftVertAlphaHandler(const char *arg, unsigned char *store, int storeSize)
{
	FF11_LOCAL_DECL_OPTS(true);
	pOpts->fixVertAlphaShift = atoi(arg);
	pOpts->explicitVertAlphaShift = true;
	return true;
}

bool Model_FF11_NoShinyHandler(const char *arg, unsigned char *store, int storeSize)
{
	FF11_LOCAL_DECL_OPTS(false);
	pOpts->noShinyMaterials = true;
	return true;
}

bool Model_FF11_NoVertColorHandler(const char *arg, unsigned char *store, int storeSize)
{
	FF11_LOCAL_DECL_OPTS(false);
	pOpts->noVertColors = true;
	return true;
}

bool Model_FF11_ForceCullHandler(const char *arg, unsigned char *store, int storeSize)
{
	FF11_LOCAL_DECL_OPTS(false);
	pOpts->forceCull = true;
	return true;
}

bool Model_FF11_RenderUnreferencedHandler(const char *arg, unsigned char *store, int storeSize)
{
	FF11_LOCAL_DECL_OPTS(false);
	pOpts->renderUnreferenced = true;
	return true;
}

bool Model_FF11_KeepNamesHandler(const char *arg, unsigned char *store, int storeSize)
{
	FF11_LOCAL_DECL_OPTS(false);
	pOpts->keepNames = true;
	return true;
}

bool Model_FF11_OptimizeGeoHandler(const char *arg, unsigned char *store, int storeSize)
{
	FF11_LOCAL_DECL_OPTS(false);
	pOpts->optimizeGeo = true;
	return true;
}

static const char *skpDatSetHeader = "NOESIS_FF11_DAT_SET";
static const int skDatSetHeaderSize = strlen(skpDatSetHeader);

bool Model_FF11_CheckDATSet(BYTE *fileBuffer, int bufferLen, noeRAPI_t *rapi)
{
	if (bufferLen <= skDatSetHeaderSize || memcmp(fileBuffer, skpDatSetHeader, skDatSetHeaderSize) != 0)
	{
		return false;
	}
	return true;
}

noesisModel_t *Model_FF11_LoadDATSet(BYTE *fileBuffer, int bufferLen, int &numMdl, noeRAPI_t *rapi)
{
	std::vector<CFFXIDat *> dats;
	CFFXIDefaultHandlerSet datHandlers;

	char basePath[MAX_NOESIS_PATH];
	rapi->Noesis_GetDirForFilePath(basePath, rapi->Noesis_GetLastCheckedName());
	char loadPath[MAX_NOESIS_PATH];
	strcpy_s(loadPath, basePath);

	char currentDatName[MAX_NOESIS_PATH];
	char currentDatFilename[MAX_NOESIS_PATH];

	textParser_t *pParser = rapi->Parse_InitParser((char *)fileBuffer);
	parseToken_t tok;
	while (rapi->Parse_GetNextToken(pParser, &tok))
	{
		if (!stricmp(tok.text, "setPathKey"))
		{
			HKEY baseKey;
			rapi->Parse_GetNextToken(pParser, &tok);
			if (!stricmp(tok.text, "HKEY_LOCAL_MACHINE"))
			{
				baseKey = HKEY_LOCAL_MACHINE;
			}
			else if (!stricmp(tok.text, "HKEY_CURRENT_USER"))
			{
				baseKey = HKEY_CURRENT_USER;
			}
			else if (!stricmp(tok.text, "HKEY_CURRENT_CONFIG"))
			{
				baseKey = HKEY_CURRENT_CONFIG;
			}
			else if (!stricmp(tok.text, "HKEY_USERS"))
			{
				baseKey = HKEY_USERS;
			}
			else if (!stricmp(tok.text, "HKEY_CLASSES_ROOT"))
			{
				baseKey = HKEY_CLASSES_ROOT;
			}
			else
			{ //default to local machine
				baseKey = HKEY_LOCAL_MACHINE;
			}

			//grab the key name
			rapi->Parse_GetNextToken(pParser, &tok);
			HKEY key;
			if (RegOpenKeyExA(baseKey, tok.text, 0, KEY_READ, &key) == ERROR_SUCCESS)
			{
				//grab the value name
				rapi->Parse_GetNextToken(pParser, &tok);
				char keyData[MAX_NOESIS_PATH];
				DWORD keyDataSize = MAX_NOESIS_PATH;
				DWORD keyType;
				if (RegQueryValueExA(key, tok.text, NULL, &keyType, (LPBYTE)keyData, &keyDataSize) == ERROR_SUCCESS)
				{
					strcpy_s(loadPath, keyData);
				}
				RegCloseKey(key);
			}
		}
		else if (!stricmp(tok.text, "setPathRel"))
		{
			//set the path relative to this file's location
			rapi->Parse_GetNextToken(pParser, &tok);
			sprintf_s(loadPath, "%s%s", basePath, tok.text);
		}
		else if (!stricmp(tok.text, "setPathAbs"))
		{
			//set an absolute path
			rapi->Parse_GetNextToken(pParser, &tok);
			strcpy_s(loadPath, tok.text);
		}
		else if (!stricmp(tok.text, "dat"))
		{
			rapi->Parse_GetNextToken(pParser, &tok);
			strcpy_s(currentDatName, tok.text);
			//grab the filename
			rapi->Parse_GetNextToken(pParser, &tok);
			sprintf_s(currentDatFilename, "%s%s", loadPath, tok.text);

			int datBufferSize;
			unsigned char *pDatBuffer = rapi->Noesis_ReadFile(currentDatFilename, &datBufferSize);
			if (pDatBuffer)
			{
				CFFXIDat *pDat = new CFFXIDat(pDatBuffer, datBufferSize, rapi);
				datHandlers.RegisterHandlersWithDat(*pDat);
				pDat->ParseChunksOfInterest();

				dats.push_back(pDat);
				if (!stricmp(currentDatName, "__skeleton"))
				{
					pDat->RunChunkHandlersForChunksOfInterest(CFFXIDat::skChunkType_Skeleton);
				}
				else if (!stricmp(currentDatName, "__animation"))
				{
					pDat->RunChunkHandlersForChunksOfInterest(CFFXIDat::skChunkType_Animation);
				}
				else
				{
					//else, name is currently unused. might be useful for future functionality.
					pDat->RunChunkHandlersForChunksOfInterest(CFFXIDat::skChunkType_Texture);
					pDat->RunChunkHandlersForChunksOfInterest(CFFXIDat::skChunkType_Geo);
					pDat->RunChunkHandlersForChunksOfInterest(CFFXIDat::skChunkType_Map);
					pDat->RunChunkHandlersForChunksOfInterest(CFFXIDat::skChunkType_MapGeo);
				}
			}
			else
			{
				rapi->LogOutput("Failed to load file: '%s'\n", currentDatFilename);
			}
		}
	}
	rapi->Parse_FreeParser(pParser);

	if (!datHandlers.TextureHandler())
	{
		//if any of the handlers are null, we didn't encouner a single loadable dat.
		rapi->LogOutput("Error: No relevant data was loaded.\n");
		return NULL;
	}

	noesisModel_t *pMdl = Model_FF11_ConstructModelFromHandlerSet(rapi, datHandlers, false);

	Model_FF11_SetPreviewOffset(rapi);

	for (std::vector<CFFXIDat *>::iterator it = dats.begin(); it != dats.end(); ++it)
	{
		CFFXIDat *pDat = *it;
		rapi->Noesis_UnpooledFree((void *)pDat->GetData());
		delete pDat;
	}

	numMdl = (pMdl) ? 1 : 0;
	return pMdl;
}

//========================================================================================

CFFXIDat::EValidateChunkResult CFFXIDat::ValidateChunk(const CFFXIDat::SChunk &chunk) const
{
	TChunkHandlerContainer::const_iterator it = mChunkHandlers.find(chunk.mType);
	if (it == mChunkHandlers.end())
	{
		return kVCR_NotSupported;
	}

	const int dataSize = chunk.mSize - skBinaryChunkSize;
	return it->second->ValidateChunk(*this, chunk, mpData + chunk.mDataOffset, dataSize);
}

bool CFFXIDat::ParseChunksOfInterest()
{
	mChunks.clear();

	int ofs = 0;
	while (ofs <= (mDataSize - skBinaryChunkSize))
	{
		SChunk chunk(mpData + ofs, ofs);
		if ((ofs + chunk.mSize) > mDataSize)
		{
			//would run off the end of the buffer
			return false;
		}
		//consider a 0-sized chunk to be a terminator
		if (chunk.mSize <= 0)
		{
			break;
		}

		const EValidateChunkResult validateChunkResult = ValidateChunk(chunk);
		if (validateChunkResult == kVCR_Supported)
		{
			mChunks.push_back(chunk);
		}
		else if (validateChunkResult == kVCR_Invalid)
		{ //bad chunk data, abort parsing
			return false;
		}

		ofs += chunk.mSize;
	}

	return mChunks.size() > 0;
}

bool CFFXIDat::RunChunkHandlersForChunksOfInterest(const int forChunkType) const
{
	//run through the pre-built chunks of interest list, and run registered handlers for them.
	for (TChunkList::const_iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		const SChunk &chunk = *it;
		if (forChunkType >= 0 && chunk.mType != forChunkType)
		{
			continue;
		}

		TChunkHandlerContainer::const_iterator itHandler = mChunkHandlers.find(chunk.mType);
		if (itHandler != mChunkHandlers.end())
		{
			const int dataSize = chunk.mSize - skBinaryChunkSize;
			if (!itHandler->second->HandleChunk(*this, chunk, mpData + chunk.mDataOffset, dataSize))
			{
				mpRapi->LogOutput("WARNING: Chunk handler failed on chunk type %i at %i.\n", chunk.mType, chunk.mDataOffset);
			}
		}
	}

	return true;
}

void CFFXIDat::RegisterChunkHandler(CFFXIChunkHandler *pChunkHandler)
{
	const int chunkType = pChunkHandler->GetChunkType();
	TChunkHandlerContainer::iterator it = mChunkHandlers.lower_bound(chunkType);
	if (it != mChunkHandlers.end() && it->first == chunkType)
	{
		NoeAssert(!"Handler already registered to chunk type!");
		return;
	}
	mChunkHandlers.insert(it, TChunkHandlerContainer::value_type(chunkType, pChunkHandler));
}

//========================================================================================

#define FFXI_CREATE_AND_REGISTER_HANDLER(dat, handlerPointer, handlerType, ...) \
	if (!handlerPointer) \
	{ \
		handlerPointer = new handlerType(__VA_ARGS__); \
	} \
	dat.RegisterChunkHandler(handlerPointer);

CFFXIDefaultHandlerSet::CFFXIDefaultHandlerSet(CFFXIDat *pDat)
{
	if (pDat)
	{
		RegisterHandlersWithDat(*pDat);
	}
}

void CFFXIDefaultHandlerSet::RegisterHandlersWithDat(CFFXIDat &dat)
{
	FFXI_CREATE_AND_REGISTER_HANDLER(dat, mpTextureHandler, CFFXITextureHandler);
	FFXI_CREATE_AND_REGISTER_HANDLER(dat, mpSkelHandler, CFFXISkelHandler);
	FFXI_CREATE_AND_REGISTER_HANDLER(dat, mpAnimHandler, CFFXIAnimHandler);
	FFXI_CREATE_AND_REGISTER_HANDLER(dat, mpGeoHandler, CFFXIGeoHandler);
	FFXI_CREATE_AND_REGISTER_HANDLER(dat, mpMapHandler, CFFXIMapHandler);
	FFXI_CREATE_AND_REGISTER_HANDLER(dat, mpMapGeoHandler, CFFXIMapGeoHandler);
}

