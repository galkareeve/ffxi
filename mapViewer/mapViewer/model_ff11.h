 Displaying: model_ff11.h

//========================================================================================
// Noesis FF11 support
// Life regrettably wasted by Rich Whitehouse
// (c) Never, all rights to be taken to the grave.
//========================================================================================
#pragma once

#ifndef _MODEL_FF11_H
#define _MODEL_FF11_H

#include <vector>
#include <map>
#include <algorithm>

class CFFXIChunkHandler;

//========================================================================================

class CFFXIDat
{
public:
	static const int skChunkType_Texture = 0x20;
	static const int skChunkType_Skeleton = 0x29;
	static const int skChunkType_Geo = 0x2A;
	static const int skChunkType_Animation = 0x2B;
	static const int skChunkType_Map = 0x1C;
	static const int skChunkType_MapGeo = 0x2E;
	
	static const int skBinaryChunkSize = 16;

	enum EValidateChunkResult
	{
		kVCR_Invalid = 0,
		kVCR_Supported,
		kVCR_NotSupported
	};

	struct SChunk
	{
		explicit SChunk(const unsigned char *pData, const int chunkOffset)
		{
			memcpy(mName, pData, 4);
			mName[4] = 0;
			const unsigned int info = *(const unsigned int *)(pData + 4);
			mType = (info & 0x7F);
			mSize = ((info >> 3) & 0x7FFFF0);
			//are low 4 bits ever set?
			mDataOffset = chunkOffset + skBinaryChunkSize;
		}

		char mName[8];
		int mType;
		int mDataOffset;
		int mSize;
	};
	typedef std::vector<SChunk> TChunkList;


	explicit CFFXIDat(unsigned char *pData, const int dataSize, noeRAPI_t *pRapi)
		: mpData(pData)
		, mDataSize(dataSize)
		, mpRapi(pRapi)
	{
	}

	EValidateChunkResult ValidateChunk(const SChunk &chunk) const;

	bool ParseChunksOfInterest();
	bool RunChunkHandlersForChunksOfInterest(const int forChunkType = -1) const;

	void RegisterChunkHandler(CFFXIChunkHandler *pChunkHandler);

	const unsigned char *GetData() const { return mpData; }
	int GetDataSize() const { return mDataSize; }

	noeRAPI_t *GetRAPI() const { return mpRapi; } //what a marine does after getting a girl drunk

protected:
	typedef std::map< int, CRefCountedPtr<CFFXIChunkHandler> > TChunkHandlerContainer;

	unsigned char *mpData;
	int mDataSize;
	noeRAPI_t *mpRapi;
	TChunkList mChunks; //only chunks of interest

	TChunkHandlerContainer mChunkHandlers;
};

//========================================================================================

class CFFXIChunkHandler : public CRefCountedObject
{
public:
	explicit CFFXIChunkHandler(const int chunkType)
		: mChunkType(chunkType)
	{
	}

	virtual CFFXIDat::EValidateChunkResult ValidateChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk, 
															const unsigned char *pChunkData, const int dataSize) const = 0;
	virtual bool HandleChunk(const CFFXIDat &dat, const CFFXIDat::SChunk &chunk,
								const unsigned char *pChunkData, const int dataSize) = 0;

	int GetChunkType() const { return mChunkType; }

private:
	int mChunkType;
};

//========================================================================================

class CFFXITextureHandler;
class CFFXISkelHandler;
class CFFXIAnimHandler;
class CFFXIGeoHandler;
class CFFXIMapHandler;
class CFFXIMapGeoHandler;

class CFFXIDefaultHandlerSet
{
public:
	explicit CFFXIDefaultHandlerSet(CFFXIDat *pDat = NULL);

	void RegisterHandlersWithDat(CFFXIDat &dat);

	CFFXITextureHandler *TextureHandler() { return mpTextureHandler; }
	CFFXISkelHandler *SkelHandler() { return mpSkelHandler; }
	CFFXIAnimHandler *AnimHandler() { return mpAnimHandler; }
	CFFXIGeoHandler *GeoHandler() { return mpGeoHandler; }
	CFFXIMapHandler *MapHandler() { return mpMapHandler; }
	CFFXIMapGeoHandler *MapGeoHandler() { return mpMapGeoHandler; }
protected:
	CRefCountedPtr<CFFXITextureHandler> mpTextureHandler;
	CRefCountedPtr<CFFXISkelHandler> mpSkelHandler;
	CRefCountedPtr<CFFXIAnimHandler> mpAnimHandler;
	CRefCountedPtr<CFFXIGeoHandler> mpGeoHandler;
	CRefCountedPtr<CFFXIMapHandler> mpMapHandler;
	CRefCountedPtr<CFFXIMapGeoHandler> mpMapGeoHandler;
};

#endif //_MODEL_FF11_H

