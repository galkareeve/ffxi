 Displaying: model_mri_dicom.cpp

#include "stdafx.h"
#include "CharLS/interface.h"
#include <float.h>
#include <algorithm>

//section/table/figure/etc. references in this module apply to http://medical.nema.org/Dicom/2011/11_05pu.pdf
//"Digital Imaging and Communications in Medicine (DICOM)
//		Part 5: Data Structures and Encoding"
//...unless otherwise explicitly noted. (some things also explicitly refer to part 3)

dicomOpts_t *g_dicomOpts = NULL;

/*
Private UID format (see Annex B):
1.2.840.xxxxx.3.152.235.2.12.187636473"
root          suffix

In this example, the root is:
1			Identifies ISO
2			Identifies ANSI Member Body
840			Country code of a specific Member Body (U.S. for ANSI)
xxxxx		Identifies a specific Organization.(provided by ANSI)

In this example the first two components of the suffix relate to the identification of the device:
3			Manufacturer or user defined device type
152			Manufacturer or user defined serial number

The remaining four components of the suffix relate to the identification of the image:

235			Study number
2			Series number
12			Image number
187636473	Encoded date and time stamp of image acquisition 
*/

enum EDicomTransferSyntax
{
	kDTS_Unknown = 0,

	kDTS_ImplicitVRLittleEndian,
	kDTS_ExplicitVRLittleEndian,
	kDTS_ExplicitVRBigEndian,

	kDTS_RLE,

	//is_jpeg_transfer_syntax reflects kDTS_JpegBaseline as first jpeg transfer syntax
	kDTS_JpegBaseline,
	kDTS_JpegExtended,
	kDTS_JpegSpectral,
	kDTS_JpegProgressive,
	kDTS_JpegLossless,
	kDTS_JpegLosslessFirstOrder,
	kDTS_JpegLSLossless,
	kDTS_JpegLSNearLossless,
	kDTS_Jpeg2000LosslessReversible,
	kDTS_Jpeg2000LosslessOrLossy,
	kDTS_Jpeg2000Part2LosslessReversible,
	kDTS_Jpeg2000Part2LosslessOrLossy,
	//is_jpeg_transfer_syntax reflects kDTS_Jpeg2000Part2LosslessOrLossy as last jpeg transfer syntax

	kDTS_MPEG2_ML,
	kDTS_MPEG2_HL,
	kDTS_MPEG4,
	kDTS_MPEG4Temporal,

	kDTS_Deflate,

	kDTS_Count
};

static const char *skTransferSyntaxStrings[kDTS_Count] =
{
	"Unknown", //kDTS_Unknown

	"1.2.840.10008.1.2", //kDTS_ImplicitVRLittleEndian (A.1)
	"1.2.840.10008.1.2.1", //kDTS_ExplicitVRLittleEndian (A.2)
	"1.2.840.10008.1.2.2", //kDTS_ExplicitVRBigEndian (A.3)

	"1.2.840.10008.1.2.5", //kDTS_RLE (A.4.2)

	"1.2.840.10008.1.2.4.50", //kDTS_JpegBaseline (table A.4-3)
	"1.2.840.10008.1.2.4.51", //kDTS_JpegExtended (table A.4-3)
	"1.2.840.10008.1.2.4.53", //kDTS_JpegSpectral (not listed)
	"1.2.840.10008.1.2.4.55", //kDTS_JpegProgressive (not listed)
	"1.2.840.10008.1.2.4.57", //kDTS_JpegLossless (table A.4-3)
	"1.2.840.10008.1.2.4.70", //kDTS_JpegLosslessFirstOrder (table A.4-3)
	"1.2.840.10008.1.2.4.80", //kDTS_JpegLSLossless (A.4.3)
	"1.2.840.10008.1.2.4.81", //kDTS_JpegLSNearLossless (A.4.3)
	"1.2.840.10008.1.2.4.90", //kDTS_Jpeg2000LosslessReversible (A.4.4)
	"1.2.840.10008.1.2.4.91", //kDTS_Jpeg2000LosslessOrLossy (A.4.4)
	"1.2.840.10008.1.2.4.92", //kDTS_Jpeg2000Part2LosslessReversible (A.4.4)
	"1.2.840.10008.1.2.4.93", //kDTS_Jpeg2000Part2LosslessOrLossy (A.4.4)

	"1.2.840.10008.1.2.4.100", //kDTS_MPEG2_ML (table A.4.5)
	"1.2.840.10008.1.2.4.101", //kDTS_MPEG2_HL (table A.4.5)
	"1.2.840.10008.1.2.4.102", //kDTS_MPEG4 (table A.4.6)
	"1.2.840.10008.1.2.4.103", //kDTS_MPEG4Temporal (table A.4.6)

	"1.2.840.10008.1.2.1.99" //kDTS_Deflate (A.5)

	//A.6 - DICOM JPIP REFERENCED - 1.2.840.10008.1.2.4.94
	//A.7 - DICOM JPIP REFERENCED DEFLATE - 1.2.840.10008.1.2.4.95
};

const EDicomTransferSyntax find_transfer_syntax_for_string(const char *pSyntaxString)
{
	for (int tsIndex = 0; tsIndex < kDTS_Count; ++tsIndex)
	{
		if (!strcmp(skTransferSyntaxStrings[tsIndex], pSyntaxString))
		{
			return EDicomTransferSyntax(tsIndex);
		}
	}

	return kDTS_Unknown;
}

const bool is_jpeg_transfer_syntax(const EDicomTransferSyntax transferSyntax)
{
	return (transferSyntax >= kDTS_JpegBaseline && transferSyntax <= kDTS_Jpeg2000Part2LosslessOrLossy);
}

//see part 3, C.7.6.3.1.2
enum EDicomPhotometricInterpretation
{
	kDPI_Unknown = 0,

	kDPI_Monochrome1,
	kDPI_Monochrome2,
	kDPI_PaletteColor,
	kDPI_RGB,
	kDPI_HSV, //"retired"
	kDPI_ARGB, //"retired"
	kDPI_CMYK, //"retired"
	kDPI_YBR_Full,
	kDPI_YBR_Full_422,
	kDPI_YBR_Partial_422,
	kDPI_YBR_Partial_420,
	kDPI_YBR_ICT,
	kDPI_YBR_RCT,

	kDPI_Count
};

static const char *skPhotometricInterpretationStrings[kDPI_Count] =
{
	"Unknown", //kDPI_Unknown

	"MONOCHROME1", //kDPI_Monochrome1
	"MONOCHROME2", //kDPI_Monochrome2
	"PALETTE COLOR", //kDPI_PaletteColor
	"RGB", //kDPI_RGB
	"HSV", //kDPI_HSV
	"ARGB", //kDPI_ARGB
	"CMYK", //kDPI_CMYK
	"YBR_FULL", //kDPI_YBR_Full
	"YBR_FULL_422", //kDPI_YBR_Full_422
	"YBR_PARTIAL_422", //kDPI_YBR_Partial_422
	"YBR_PARTIAL_420", //kDPI_YBR_Partial_420
	"YBR_ICT", //kDPI_YBR_ICT
	"YBR_RCT" //kDPI_YBR_RCT
};

const EDicomPhotometricInterpretation photometric_interpretation_for_string(const char *pPhotoString)
{
	for (int piIndex = 0; piIndex < kDPI_Count; ++piIndex)
	{
		if (!stricmp(skPhotometricInterpretationStrings[piIndex], pPhotoString))
		{
			return EDicomPhotometricInterpretation(piIndex);
		}
	}

	return kDPI_Unknown;
}

//see table 6.2-1 for a full list of value representations
static const unsigned short skVR_ApplicationEntity = 'EA'; //AE - application entity
static const unsigned short skVR_AgeString = 'SA'; //AS - age string
static const unsigned short skVR_AttributeTag = 'TA'; //AT - attribute tag
static const unsigned short skVR_CodeString = 'SC'; //CS - code string
static const unsigned short skVR_Date = 'AD'; //DA - date
static const unsigned short skVR_DecimalString = 'SD'; //DS - decimal string
static const unsigned short skVR_DateTime = 'TD'; //DT - date time
static const unsigned short skVR_Float = 'LF'; //FL - float 32
static const unsigned short skVR_Double = 'DF'; //FD - float 64
static const unsigned short skVR_IntString = 'SI'; //IS - integer string (string character representation of int)
static const unsigned short skVR_LongString = 'OL'; //LO - long string (string character representation of long)
static const unsigned short skVR_LongText = 'TL'; //LT - long text
static const unsigned short skVR_OtherByte = 'BO'; //OB - other byte string
static const unsigned short skVR_OtherFloat = 'FO'; //OF - other float string
static const unsigned short skVR_OtherWord = 'WO'; //OW - other word (16-bit) string
static const unsigned short skVR_PersonName = 'NP'; //PN - person name
static const unsigned short skVR_ShortString = 'HS'; //SH - short string (string character representation of short)
static const unsigned short skVR_SignedLong = 'LS'; //SL - signed long (32-bit)
static const unsigned short skVR_Sequence = 'QS'; //SQ - sequence, see table 7.5-3 for data layout
static const unsigned short skVR_SignedShort = 'SS'; //SS - signed short (16-bit)
static const unsigned short skVR_ShortText = 'TS'; //ST - short text
static const unsigned short skVR_Time = 'MT'; //TM - time
static const unsigned short skVR_UID = 'IU'; //UI - unique identifier
static const unsigned short skVR_UnsignedLong = 'LU'; //UL - unsigned long (32-bit)
static const unsigned short skVR_Unknown = 'NU'; //UN - unknown
static const unsigned short skVR_UnsignedShort = 'SU'; //US - unsigned short (16-bit)
static const unsigned short skVR_UnlimitedText = 'TU'; //UT - unlimited text
static const unsigned short skVR_Invalid = 0;

//see section 7.1.2 (table 7.1-1)
static const unsigned short skExplicitVRsFor32BitLength[] =
{
	skVR_OtherByte,
	skVR_OtherWord,
	skVR_OtherFloat,
	skVR_Sequence,
	skVR_UnlimitedText,
	skVR_Unknown
};
static const int skExplicitVRsFor32BitLengthCount = sizeof(skExplicitVRsFor32BitLength) / sizeof(unsigned short);

static const int skHeaderOffset = 128;
static const int skDataElemsOffset = skHeaderOffset + 4;

static const unsigned int skTemporaryStringElementValueSize = 512;

static const unsigned short skGroup2 = 0x0002;
static const unsigned short skG2TransferSyntaxUID = 0x0010;

static const unsigned short skGroupDelimter = 0xFFFE;

#define MAKE_ELEMENT_ID(groupNum, elemNum) (groupNum | (elemNum << 16))

static const unsigned int skFrameTimeElement = MAKE_ELEMENT_ID(0x0018, 0x1063);
static const unsigned int skWaveformDataElement = MAKE_ELEMENT_ID(0x5400, 0x1010);
static const unsigned int skRedPaletteLookupTableDataElement = MAKE_ELEMENT_ID(0x0028, 0x1201);
static const unsigned int skGreenPaletteLookupTableDataElement = MAKE_ELEMENT_ID(0x0028, 0x1202);
static const unsigned int skBluePaletteLookupTableDataElement = MAKE_ELEMENT_ID(0x0028, 0x1203);
static const unsigned int skAlphaPaletteLookupTableDataElement = MAKE_ELEMENT_ID(0x0028, 0x1204);
static const unsigned int skRedPaletteLookupTableDescElement = MAKE_ELEMENT_ID(0x0028, 0x1101);
static const unsigned int skGreenPaletteLookupTableDescElement = MAKE_ELEMENT_ID(0x0028, 0x1102);
static const unsigned int skBluePaletteLookupTableDescElement = MAKE_ELEMENT_ID(0x0028, 0x1103);
static const unsigned int skAlphaPaletteLookupTableDescElement = MAKE_ELEMENT_ID(0x0028, 0x1104);
static const unsigned int skRedSegPaletteLookupTableDataElement = MAKE_ELEMENT_ID(0x0028, 0x1221);
static const unsigned int skGreenSegPaletteLookupTableDataElement = MAKE_ELEMENT_ID(0x0028, 0x1222);
static const unsigned int skBlueSegPaletteLookupTableDataElement = MAKE_ELEMENT_ID(0x0028, 0x1223);
static const unsigned int skLookupTableDataElement = MAKE_ELEMENT_ID(0x0028, 0x3006);
static const unsigned int skLookupTableDescElement = MAKE_ELEMENT_ID(0x0028, 0x3002);
static const unsigned int skBlendingLookupTableDataElement = MAKE_ELEMENT_ID(0x0028, 0x1408);
static const unsigned int skOverlayDataBaseElement = MAKE_ELEMENT_ID(0x6000, 0x3000); //60xx represents xx overlays

//see 8.1.1
//however, pixel macro attributes are only vaguely referenced in part 5, see part 3 table C.7-11b for a complete list.
//see also part 3 table C.7-14 for multi-frame attributes.
static const unsigned int skPixelDataSamplesPerPixelElement = MAKE_ELEMENT_ID(0x0028, 0x0002);
static const unsigned int skPixelDataPhotometricInterpretationElement = MAKE_ELEMENT_ID(0x0028, 0x0004);
static const unsigned int skPixelDataRowsElement = MAKE_ELEMENT_ID(0x0028, 0x0010);
static const unsigned int skPixelDataColumnsElement = MAKE_ELEMENT_ID(0x0028, 0x0011);
static const unsigned int skPixelDataBitsAllocatedElement = MAKE_ELEMENT_ID(0x0028, 0x0100);
static const unsigned int skPixelDataBitsStoredElement = MAKE_ELEMENT_ID(0x0028, 0x0101);
static const unsigned int skPixelDataHighBitElement = MAKE_ELEMENT_ID(0x0028, 0x0102);
static const unsigned int skPixelDataRepresentationElement = MAKE_ELEMENT_ID(0x0028, 0x0103);
static const unsigned int skPixelDataElement = MAKE_ELEMENT_ID(0x7FE0, 0x0010);
static const unsigned int skPixelDataPlanarConfigurationElement = MAKE_ELEMENT_ID(0x0028, 0x0006);
static const unsigned int skPixelDataFrameCountElement = MAKE_ELEMENT_ID(0x0028, 0x0008);
static const unsigned int skPixelDataFrameIncrementPointerElement = MAKE_ELEMENT_ID(0x0028, 0x0009);
static const unsigned int skPixelDataAspectRatioElement = MAKE_ELEMENT_ID(0x0028, 0x0034);
static const unsigned int skPixelDataSmallestValueElement = MAKE_ELEMENT_ID(0x0028, 0x0106);
static const unsigned int skPixelDataLargestValueElement = MAKE_ELEMENT_ID(0x0028, 0x0107);
static const unsigned int skPixelDataICCProfileElement = MAKE_ELEMENT_ID(0x0028, 0x2000);
static const unsigned int skPixelDataRescaleIntercept = MAKE_ELEMENT_ID(0x0028, 0x1052);
static const unsigned int skPixelDataRescaleSlope = MAKE_ELEMENT_ID(0x0028, 0x1053);
//see part 3 table C.7.6.24-1 (floating point image pixel module attributes)
static const unsigned int skFloatPixelDataElement = MAKE_ELEMENT_ID(0x7FE0, 0x0008);
static const unsigned int skDoublePixelDataElement = MAKE_ELEMENT_ID(0x7FE0, 0x0009);
static const unsigned int skSequenceTag = MAKE_ELEMENT_ID(0xFFFE, 0xE000);
static const unsigned int skSequenceEndTag = MAKE_ELEMENT_ID(0xFFFE, 0xE0DD);

//possible todo - support volume data, see part 3 table C.8.12.4-1

//not thread-safe
const char *temporary_string_for_element_value(RichBitStreamEx &bs, const unsigned int valueLength)
{
	static char sReadBuffer[skTemporaryStringElementValueSize];
	const int readLength = std::min<unsigned int>(valueLength, skTemporaryStringElementValueSize-1);
	bs.ReadBytes(sReadBuffer, readLength);
	sReadBuffer[readLength] = 0;
	//eliminate trailing whitespace
	for (int trailingWhiteSpace = readLength - 1; trailingWhiteSpace > 0; --trailingWhiteSpace)
	{
		if (sReadBuffer[trailingWhiteSpace] == ' ' ||
			sReadBuffer[trailingWhiteSpace] == '\r' ||
			sReadBuffer[trailingWhiteSpace] == '\n' ||
			sReadBuffer[trailingWhiteSpace] == 9 /*tab*/)
		{
			sReadBuffer[trailingWhiteSpace] = 0;
		}
		else if (sReadBuffer[trailingWhiteSpace] != 0)
		{
			break;
		}
	}

	return sReadBuffer;
}

template<class DestType>
static const DestType read_single_element_value_for_vr(RichBitStreamEx &bs,
														const unsigned short vr, const unsigned int valueLength)
{
	switch (vr)
	{
	case skVR_Double:
		return (DestType)bs.ReadDouble();
	case skVR_Float:
	case skVR_OtherFloat:
		return (DestType)bs.ReadFloat();
	case skVR_SignedLong:
		return (DestType)bs.ReadInt();
	case skVR_UnsignedLong:
		return (DestType)bs.ReadUInt();
	case skVR_SignedShort:
		return (DestType)bs.ReadShort();
	case skVR_UnsignedShort:
	case skVR_OtherWord:
	case skVR_AttributeTag:
		return (DestType)bs.ReadUShort();
	case skVR_OtherByte:
		return (DestType)bs.ReadByte();
	case skVR_IntString:
	case skVR_LongString:
	case skVR_ShortString:
		{
			const char *pTempBuffer = temporary_string_for_element_value(bs, valueLength);
			return (DestType)atoi(pTempBuffer);
		}
	case skVR_DecimalString:
		{
			const char *pTempBuffer = temporary_string_for_element_value(bs, valueLength);
			return (DestType)atof(pTempBuffer);
		}
	case skVR_Invalid:
		{
			//we don't have a VR for this element, try to intuit it based on size
			switch (valueLength)
			{
			case 1:
				return (DestType)bs.ReadByte();
			case 2:
				return (DestType)bs.ReadUShort();
			case 4:
				return (DestType)bs.ReadUInt();
			default:
				return 0;
			}
		}
	default:
		return 0;
	}
}

//part 3, C.7.6.3.1.5
struct SDicomPalette
{
	SDicomPalette()
		: mEntryCount(0)
		, mFirstMappedEntry(0)
		, mBitsPerEntry(0)
		, mpData(NULL)
		, mDataLength(0)
		, mDataVR(skVR_Invalid)
	{
	}

	union
	{
		unsigned int mDescriptor[3];
		struct
		{
			unsigned int mEntryCount;
			unsigned int mFirstMappedEntry;
			unsigned int mBitsPerEntry;
		};
	};

	unsigned char *mpData;
	int mDataLength;
	unsigned short mDataVR;
};

struct SDicomImage
{
	SDicomImage()
		: mpData(NULL)
		, mDataLength(0)
		, mDataVR(skVR_Invalid)
		, mSamplesPerPixel(0)
		, mBitsPerChannel(0)
		, mBitsStoredPerChannel(0)
		, mHighBit(0)
		, mPhotometricInterpretation(kDPI_Unknown)
		, mPlanarConfiguration(0)
		, mRowCount(0)
		, mColumnCount(0)
		, mSliceCount(0)
		, mRescaleIntercept(0.0f)
		, mRescaleSlope(1.0f)
	{
	}

	unsigned char *mpData;
	int mDataLength;
	//we should always end up with an explicit VR for image data, even with an implicit VR syntax,
	//because implicit VR requirements dictate OW.
	unsigned short mDataVR;

	int mSamplesPerPixel;
	int mBitsPerChannel;
	int mBitsStoredPerChannel;
	int mHighBit;
	EDicomPhotometricInterpretation mPhotometricInterpretation;
	int mPlanarConfiguration;
	int mRowCount;
	int mColumnCount;
	int mSliceCount;

	//scale and bias on data (intercept is bias, slope is scale)
	float mRescaleIntercept;
	float mRescaleSlope;

	SDicomPalette mPaletteRgba[4];
};

class CDicomDataElement
{
public:
	//transfer syntax may be provided as kDTS_Unknown before a skG2TransferSyntaxUID element is found.
	explicit CDicomDataElement(RichBitStreamEx &bs, const EDicomTransferSyntax transferSyntax)
	{
		bs.SetFlags(bs.GetFlags() & ~BITSTREAMFL_BIGENDIAN);
		mGroupNum = bs.ReadUShort();

		//set the stream endian for the rest of the element header and data
		if (SetStreamEndian(bs, transferSyntax) && mGroupNum != skGroup2)
		{
			//group numbers other than 2 are endian-swapped
			LITTLE_BIG_SWAP(mGroupNum);
		}

		mElemNum = bs.ReadUShort();

		bool isStandardElem = true;
		const bool isImplicitVR = (transferSyntax == kDTS_ImplicitVRLittleEndian && mGroupNum != skGroup2);
		if (isImplicitVR)
		{
			//see Annex A (A.1) which specifies implicit VR requirements for these groups/elements.
			//many of these same requirements are dictated for default explicit VR, but we don't really
			//give a shit if the binary wants to specify something non-standards-conformant.
			if ((mElementId & 0xFFFFFF00) == skOverlayDataBaseElement)
			{
				//special-case for overlay data
				mVR = skVR_OtherWord;
			}
			else
			{
				switch (mElementId)
				{
				case skPixelDataElement:
				case skWaveformDataElement:
				case skRedPaletteLookupTableDataElement:
				case skGreenPaletteLookupTableDataElement:
				case skBluePaletteLookupTableDataElement:
				case skAlphaPaletteLookupTableDataElement:
				case skRedSegPaletteLookupTableDataElement:
				case skGreenSegPaletteLookupTableDataElement:
				case skBlueSegPaletteLookupTableDataElement:
				case skLookupTableDataElement:
				case skBlendingLookupTableDataElement:
					mVR = skVR_OtherWord;
					break;
				case skRedPaletteLookupTableDescElement:
				case skGreenPaletteLookupTableDescElement:
				case skBluePaletteLookupTableDescElement:
				case skAlphaPaletteLookupTableDescElement:
				case skLookupTableDescElement:
					mVR = skVR_UnsignedShort;
					break;
				case skPixelDataRescaleIntercept:
				case skPixelDataRescaleSlope:
					mVR = skVR_DecimalString;
					break;
				default:
					mVR = skVR_Invalid;
					break;
				}
			}
		}
		else if (mGroupNum == skGroupDelimter)
		{
			mVR = skVR_Invalid;
		}
		else
		{
			//read VR byte by byte as it's actually a string representation
			mVR = bs.ReadByte() | ((unsigned short)bs.ReadByte() << 8);
			isStandardElem = !ExplicitVRFor32BitLength();
		}

		if (isStandardElem)
		{
			//see 7.1.3, length is always 32 bits in implicit VR mode and for delimiters
			mValueLength = (isImplicitVR || mGroupNum == skGroupDelimter) ? bs.ReadUInt() : bs.ReadUShort();
		}
		else
		{
			bs.ReadUShort(); //reserved bytes, discard
			mValueLength = bs.ReadUInt();
		}

		mValueOfs = bs.GetOffset();

		if (mGroupNum == skGroupDelimter || mVR == skVR_Sequence)
		{
			//we don't care about respecting sequences encountered mid-stream, so just skip over the
			//element header.
			mValueLength = 0;
		}
		else if (mValueLength == 0xFFFFFFFF)
		{
			//section 7.1.1 notes that SQ and UN can be total shitfaces about providing a value length.
			//7.5 has further details on item encoding and delimitation.
			//it seems there are also some proprietary data element types which can do this. OW and OB
			//may also be "undefined" depending on transfer syntax. if this happens, we try to iterate
			//over the sequence tags to find the total length. if we don't start on a sequence tag, we
			//just say fuck it and act like the start of the data is the start of the next data element,
			//which may or may not explode gloriously under unexpected circumstances.
			unsigned int delimiter = bs.ReadUInt();
			if (delimiter != skSequenceTag)
			{
				//alright then, fuck it.
				mValueLength = 0;
			}
			else
			{
				while (delimiter == skSequenceTag || delimiter == skSequenceEndTag)
				{
					const int seqSegSize = bs.ReadInt();
					if (seqSegSize < 0)
					{
						break;
					}

					bs.SetOffset(bs.GetOffset() + seqSegSize);
					if (delimiter == skSequenceEndTag)
					{
						//all done
						break;
					}
					delimiter = bs.ReadUInt();
				}
				mValueLength = bs.GetOffset() - mValueOfs;
			}
			//default us back to the start of the value data
			bs.SetOffset(mValueOfs);
		}
	}

	explicit CDicomDataElement(const unsigned int elementId, const unsigned short vr)
		: mElementId(elementId)
		, mVR(vr)
		, mValueLength(0)
		, mValueOfs(0)
	{
	}

	explicit CDicomDataElement(const unsigned short groupNum, const unsigned short elemNum,
								const unsigned short vr)
		: mGroupNum(groupNum)
		, mElemNum(elemNum)
		, mVR(vr)
		, mValueLength(0)
		, mValueOfs(0)
	{
	}

	void PrepDataStreamForValueRead(RichBitStreamEx &bs, const EDicomTransferSyntax transferSyntax) const
	{
		//set the correct endian mode given the group number and uid-based preference
		SetStreamEndian(bs, transferSyntax);
		bs.SetOffset(mValueOfs);
	}

	void SetStreamToNextDataElement(RichBitStreamEx &bs) const
	{
		bs.SetOffset(mValueOfs + mValueLength);
	}

	template<class DestType>
	const DestType ReadSingleElementValue(RichBitStreamEx &bs, const EDicomTransferSyntax transferSyntax) const
	{
		PrepDataStreamForValueRead(bs, transferSyntax);
		return read_single_element_value_for_vr<DestType>(bs, mVR, mValueLength);
	}

	template<class DestType>
	void ReadArrayElementValues(DestType *pOutValues, const int valueCount, RichBitStreamEx &bs,
								const EDicomTransferSyntax transferSyntax) const
	{
		PrepDataStreamForValueRead(bs, transferSyntax);
		for (int valueIndex = 0; valueIndex < valueCount; ++valueIndex)
		{
			pOutValues[valueIndex] = read_single_element_value_for_vr<DestType>(bs, mVR, mValueLength);
		}
	}

	//not thread-safe
	const char *TemporaryStringForElementValue(RichBitStreamEx &bs) const
	{
		return temporary_string_for_element_value(bs, mValueLength);
	}

	const unsigned short GetGroupNum() const { return mGroupNum; }
	const unsigned short GetElemNum() const { return mElemNum; }
	const unsigned int GetElementId() const { return mElementId; }
	const unsigned short GetVR() const { return mVR; }
	const unsigned int GetValueLength() const { return mValueLength; }
	const unsigned int GetValueOfs() const { return mValueOfs; }

	void WriteToStream(RichBitStreamEx &bs, const EDicomTransferSyntax transferSyntax,
						const void *pData, const int dataSize) const
	{
		//possible todo - do the right thing here based on transfer syntax
		bs.WriteUShort(mGroupNum);
		bs.WriteUShort(mElemNum);
		const unsigned char *pVR = (const unsigned char *)&mVR;
		bs.WriteByte(pVR[0]);
		bs.WriteByte(pVR[1]);

		const int alignedSize = ((dataSize + 1) & ~1);
		if (!ExplicitVRFor32BitLength())
		{
			bs.WriteUShort((unsigned short)alignedSize);
		}
		else
		{
			bs.WriteUShort(0); //reserved bytes
			bs.WriteUInt(alignedSize);
		}

		if (pData)
		{
			//write the data
			bs.WriteBytes(pData, dataSize);
			if (alignedSize > dataSize)
			{
				//write even byte pad
				bs.WriteByte(0);
			}
		}
	}

	template<class DataType>
	void WriteTypeToStream(RichBitStreamEx &bs, const EDicomTransferSyntax transferSyntax,
							const DataType &data) const
	{
		WriteToStream(bs, transferSyntax, &data, sizeof(DataType));
	}

	void WriteStringToStream(RichBitStreamEx &bs, const EDicomTransferSyntax transferSyntax,
								const char *pDataString) const
	{
		//terminator not included
		const int stringLength = (int)strlen(pDataString);
		WriteToStream(bs, transferSyntax, pDataString, stringLength);
	}

private:
	bool SetStreamEndian(RichBitStreamEx &bs, const EDicomTransferSyntax transferSyntax) const
	{
		bool isBigEndian = (transferSyntax == kDTS_ExplicitVRBigEndian);
		const int flags = bs.GetFlags();
		const int streamEndianFlags = (mGroupNum == skGroup2 || !isBigEndian) ?
										(flags & ~BITSTREAMFL_BIGENDIAN) :
										(flags | BITSTREAMFL_BIGENDIAN);
		bs.SetFlags(streamEndianFlags);
		return isBigEndian;
	}

	bool ExplicitVRFor32BitLength() const
	{
		for (int vrCheckIndex = 0; vrCheckIndex < skExplicitVRsFor32BitLengthCount; ++vrCheckIndex)
		{
			if (skExplicitVRsFor32BitLength[vrCheckIndex] == mVR)
			{
				//value representation is in the list of types with reserved bytes and 32-bit length
				return true;
			}
		}
		return false;
	}

	//see figure 7.1-1
	union
	{
		unsigned int mElementId;
		struct
		{
			unsigned short mGroupNum;
			unsigned short mElemNum;
		};
	};

	unsigned short mVR;

	unsigned int mValueLength;

	unsigned int mValueOfs; //offset from the start of stream data, in bytes
};

static const EDicomTransferSyntax find_transfer_syntax(RichBitStreamEx &bs)
{
	while (bs.GetOffset() < bs.GetSize())
	{
		//iterate the group2 elements until we find the transfer syntax
		const CDicomDataElement elem(bs, kDTS_Unknown);
		if (elem.GetGroupNum() != skGroup2)
		{
			//always expect group2 elements up to the uid
			return kDTS_Unknown;
		}
		const int endOfElemData = int(elem.GetValueOfs() + elem.GetValueLength());
		if (endOfElemData <= 0 || endOfElemData >= bs.GetSize())
		{
			//bad data size
			return kDTS_Unknown;
		}

		if (elem.GetElemNum() == skG2TransferSyntaxUID)
		{
			//alright, found it. let's see what we're dealing with.
			bs.SetOffset(elem.GetValueOfs());
			const char *pTransferSyntaxString = elem.TemporaryStringForElementValue(bs);

			//leave the stream at the start of the next element
			elem.SetStreamToNextDataElement(bs);

			return find_transfer_syntax_for_string(pTransferSyntaxString);
		}
		else
		{
			//skip it
			elem.SetStreamToNextDataElement(bs);
		}
	}

	return kDTS_Unknown;
}

bool Image_MRI_CheckDicom(BYTE *fileBuffer, int bufferLen, noeRAPI_t *rapi)
{
	if (bufferLen <= skDataElemsOffset)
	{
		return false;
	}
	if (memcmp(fileBuffer + skHeaderOffset, "DICM", 4) != 0)
	{
		return false;
	}

	RichBitStreamEx bs(fileBuffer, bufferLen);
	bs.SetOffset(skDataElemsOffset);

	const EDicomTransferSyntax transferSyntax = find_transfer_syntax(bs);
	return transferSyntax != kDTS_Unknown;
}

static void converted_float_to_rgb(unsigned char *pRgbOut, const int channelsOut,
									const int width, const int height, const float *pConvertedFloat,
									const int channelsPerPixel, const unsigned short sourceVR,
									const float minSampleVal, const float maxSampleVal)
{
	const float sampleRange = (maxSampleVal - minSampleVal);
	for (int pixelIndex = 0; pixelIndex < width * height; ++pixelIndex)
	{
		unsigned char *pRgbDst = pRgbOut + pixelIndex * channelsOut;
		const float *pFloatSrc = pConvertedFloat + pixelIndex * channelsPerPixel;
		for (int channelIndex = 0; channelIndex < channelsPerPixel; ++channelIndex)
		{
			if (sourceVR == skVR_OtherByte)
			{
				//if the source was u8 data, convert 1:1
				pRgbDst[channelIndex] = (unsigned char)pFloatSrc[channelIndex];
			}
			else
			{
				//otherwise, quantize into the utilized range
				const float v = (pFloatSrc[channelIndex] - minSampleVal) / sampleRange;
				pRgbDst[channelIndex] = (unsigned char)(v * 255.0f);
			}
		}
		//copy up from 0 if the float source doesn't have enough channels
		for (int remainingChannelIndex = channelsPerPixel; remainingChannelIndex < channelsOut; ++remainingChannelIndex)
		{
			pRgbDst[remainingChannelIndex] = (remainingChannelIndex == 3) ? 255 : pRgbDst[0];
		}
	}
}

static float normalize_value_for_image_data(const float value, const SDicomImage &dicomImage)
{
	float valueOut = value;
	if (dicomImage.mDataVR == skVR_OtherWord &&
		dicomImage.mBitsPerChannel > 0)
	{
		valueOut /= (float)((1 << dicomImage.mBitsPerChannel) - 1);
	}
	else
	{
		float scale;
		switch (dicomImage.mDataVR)
		{
		case skVR_SignedLong:
			scale = 1.0f / 2147483647.0f;
			break;
		case skVR_UnsignedLong:
			scale = 1.0f / 4294967295.0f;
			break;
		case skVR_SignedShort:
			scale = 1.0f / 32767.0f;
			break;
		case skVR_UnsignedShort:
		case skVR_OtherWord:
			scale = 1.0f / 65535.0f;
			break;
		case skVR_OtherByte:
			scale = 1.0f / 255.0f;
			break;
		default:
			scale = 1.0f;
			break;
		}
		valueOut *= scale;
	}

	return valueOut;
}

static float apply_value_slope(const float value, const SDicomImage &dicomImage)
{
	return (value * dicomImage.mRescaleSlope) + dicomImage.mRescaleIntercept;
}

static float apply_value_sbp(const float value)
{
	//apply bias first, as we typically want to use it to range-correct
	return powf((g_dicomOpts->mSBPBias + value) * g_dicomOpts->mSBPScale,
				g_dicomOpts->mSBPExponent);
}

static void convert_dicom_image(CArrayList<noesisTex_t *> &texturesOut,
								const SDicomImage &dicomImage, const EDicomTransferSyntax transferSyntax, noeRAPI_t *rapi)
{
	if (!dicomImage.mpData || dicomImage.mDataLength <= 0 ||
		dicomImage.mDataVR == skVR_Invalid || dicomImage.mBitsPerChannel <= 0)
	{
		rapi->LogOutput("WARNING: Invalid image data/size, discarding.\n");
		return;
	}
	if (dicomImage.mRowCount <= 0 || dicomImage.mColumnCount <= 0)
	{
		rapi->LogOutput("WARNING: Invalid image dimensions, discarding.\n");
		return;
	}

	//don't do anything intelligent about orientation at the moment
	const int width = dicomImage.mColumnCount;
	const int height = dicomImage.mRowCount;
	const int sliceCount = std::max<int>(dicomImage.mSliceCount, 1);

	const bool applySlope = (g_dicomOpts && g_dicomOpts->mApplySlope);
	const bool applySBP = (g_dicomOpts && g_dicomOpts->mApplySBP);
	const bool dataNormalize = (g_dicomOpts && g_dicomOpts->mDataNormalize);

	RichBitStreamEx bs(dicomImage.mpData, dicomImage.mDataLength);
	//possible todo - do we need to switch the endianness of the image data stream with a big-endian transfer syntax?
	//writing this to parse each pixel element through the bitstream in case this does end up being necessary.

	for (int sliceIndex = 0; sliceIndex < sliceCount; ++sliceIndex)
	{
		unsigned char *pConvertedRgb = NULL;
		float *pConvertedFloat = NULL;
		ENoeHdrTexFormat convertedFloatFormat;
		bool convertedHasAlpha = false;
		bool isColor = false;
		bool expectJpeg = false;

		switch (dicomImage.mPhotometricInterpretation)
		{
		//we rely on libjpeg to do the color conversion for these interpretations, and just accept them here as rgb.
		//if we came across one in non-jpeg form, this would certainly not work!
		case kDPI_YBR_Full:
		case kDPI_YBR_Full_422:
		case kDPI_YBR_Partial_422:
		case kDPI_YBR_Partial_420:
		case kDPI_YBR_ICT:
		case kDPI_YBR_RCT:
			expectJpeg = true;
			//fall through intentionally
		case kDPI_RGB:
			isColor = true;
			//fall through intentionally
		case kDPI_Monochrome1:
		case kDPI_Monochrome2:
			{
				if (expectJpeg && !is_jpeg_transfer_syntax(transferSyntax))
				{
					rapi->LogOutput("WARNING: Photometric interpretation not supported for transfer syntax.\n");
					break;
				}
				const int channelsPerPixel = (isColor) ? 3 : 1;
				convertedFloatFormat = (isColor) ? kNHDRTF_RGB_F96 : kNHDRTF_Lum_F32;
				pConvertedFloat = (float *)rapi->Noesis_UnpooledAlloc(width * height * sizeof(float) * channelsPerPixel);
				pConvertedRgb = (unsigned char *)rapi->Noesis_UnpooledAlloc(width * height * 3);
				float minSampleVal = FLT_MAX;
				float maxSampleVal = -FLT_MAX;
				//possible todo - could optimize this, written this way for easy tinkering
				const bool planarMode = (dicomImage.mPlanarConfiguration != 0);
				const int channelsOuterLoop = (planarMode) ? channelsPerPixel : 1;
				const int channelsInnerLoop = (!planarMode) ? channelsPerPixel : 1;
				for (int channelIndexOuter = 0; channelIndexOuter < channelsOuterLoop; ++channelIndexOuter)
				{
					for (int y = 0; y < height; ++y)
					{
						for (int x = 0; x < width; ++x)
						{
							const int pixelIndex = y * width + x;
							for (int channelIndexInner = 0; channelIndexInner < channelsInnerLoop; ++channelIndexInner)
							{
								float &destFloat = pConvertedFloat[pixelIndex * channelsPerPixel +
																	channelIndexInner + channelIndexOuter];
								if (dicomImage.mDataVR == skVR_OtherWord &&
									dicomImage.mBitsPerChannel > 0)
								{
									//do an explicit bit read
									destFloat = (float)bs.ReadBits(dicomImage.mBitsPerChannel);
								}
								else
								{
									destFloat = read_single_element_value_for_vr<float>(bs, dicomImage.mDataVR, 0);
								}

								if (dataNormalize)
								{
									destFloat = normalize_value_for_image_data(destFloat, dicomImage);
								}
								if (applySlope)
								{
									destFloat = apply_value_slope(destFloat, dicomImage);
								}
								if (applySBP)
								{
									destFloat = apply_value_sbp(destFloat);
								}

								minSampleVal = (destFloat < minSampleVal) ? destFloat : minSampleVal;
								maxSampleVal = (destFloat > maxSampleVal) ? destFloat : maxSampleVal;
							}
						}
					}
				}

				//sanity-check range
				if (minSampleVal >= maxSampleVal)
				{
					minSampleVal = 0.0f;
					maxSampleVal = 1.0f;
				}
				converted_float_to_rgb(pConvertedRgb, 3, width, height, pConvertedFloat, channelsPerPixel,
										dicomImage.mDataVR, minSampleVal, maxSampleVal);
			}
			break;
		case kDPI_PaletteColor:
			{
				const SDicomPalette *pPalettes = dicomImage.mPaletteRgba;
				if (pPalettes[0].mpData && pPalettes[0].mBitsPerEntry > 0 && pPalettes[0].mEntryCount > 0 &&
					pPalettes[1].mpData && pPalettes[1].mBitsPerEntry > 0 && pPalettes[1].mEntryCount > 0 &&
					pPalettes[2].mpData && pPalettes[2].mBitsPerEntry > 0 && pPalettes[2].mEntryCount > 0)
				{
					const bool alphaIsValid = (pPalettes[3].mpData && pPalettes[3].mBitsPerEntry > 0 &&
												pPalettes[3].mEntryCount > 0);
					float *pPaletteColors[4] =
					{
						(float *)rapi->Noesis_UnpooledAlloc(sizeof(float) * pPalettes[0].mEntryCount),
						(float *)rapi->Noesis_UnpooledAlloc(sizeof(float) * pPalettes[1].mEntryCount),
						(float *)rapi->Noesis_UnpooledAlloc(sizeof(float) * pPalettes[2].mEntryCount),
						alphaIsValid ? (float *)rapi->Noesis_UnpooledAlloc(sizeof(float) * pPalettes[3].mEntryCount) : NULL
					};
					//convert palette colors
					for (int paletteIndex = 0; paletteIndex < 4; ++paletteIndex)
					{
						float *pPaletteDst = pPaletteColors[paletteIndex];
						if (pPaletteDst)
						{
							const SDicomPalette *pPalette = pPalettes + paletteIndex;
							//palette colors are always normalized to the available range
							const float colorScale = (pPalette->mBitsPerEntry >= 16) ? 1.0f / 65535.0f : 1.0f / 255.0f;
							RichBitStreamEx palBs(pPalette->mpData, pPalette->mDataLength);
							for (unsigned int entryIndex = 0; entryIndex < pPalette->mEntryCount; ++entryIndex)
							{
								float &destFloat = pPaletteDst[entryIndex];
								destFloat = read_single_element_value_for_vr<float>(palBs, pPalette->mDataVR, 0) *
												colorScale;
								//apply value processing directly to palette values
								if (applySlope)
								{
									destFloat = apply_value_slope(destFloat, dicomImage);
								}
								if (applySBP)
								{
									destFloat = apply_value_sbp(destFloat);
								}
							}
						}
					}

					convertedFloatFormat = kNHDRTF_RGBA_F128;
					convertedHasAlpha = true;
					pConvertedFloat = (float *)rapi->Noesis_UnpooledAlloc(width * height * sizeof(float) * 4);
					pConvertedRgb = (unsigned char *)rapi->Noesis_UnpooledAlloc(width * height * 4);

					//seems like we ignore the actual VR for the pixel data for this mode, but doing this
					//check just in case.
					const bool ushortPixels = (dicomImage.mDataLength == (width * height * 2));
					for (int y = 0; y < height; ++y)
					{
						for (int x = 0; x < width; ++x)
						{
							float *pDestPixel = pConvertedFloat + (y * width + x) * 4;
							const int palEntryIndex = (dicomImage.mBitsPerChannel > 0) ?
															bs.ReadBits(dicomImage.mBitsPerChannel) :
															(ushortPixels) ? bs.ReadUShort() : bs.ReadByte();
							for (int paletteIndex = 0; paletteIndex < 4; ++paletteIndex)
							{
								const float *pPaletteFloat = pPaletteColors[paletteIndex];
								if (pPaletteFloat)
								{
									const SDicomPalette *pPalette = pPalettes + paletteIndex;
									int clampedPalEntryIndex = std::max<int>(palEntryIndex, pPalette->mFirstMappedEntry);
									clampedPalEntryIndex = std::min<int>(clampedPalEntryIndex, pPalette->mEntryCount - 1);
									pDestPixel[paletteIndex] = pPaletteFloat[clampedPalEntryIndex];
								}
								else
								{
									pDestPixel[paletteIndex] = 1.0f;
								}
							}
						}
					}

					rapi->Noesis_UnpooledFree(pPaletteColors[0]);
					rapi->Noesis_UnpooledFree(pPaletteColors[1]);
					rapi->Noesis_UnpooledFree(pPaletteColors[2]);
					if (pPaletteColors[3])
					{
						rapi->Noesis_UnpooledFree(pPaletteColors[3]);
					}

					//colors are already in 0..1
					const float minSampleVal = 0.0f;
					const float maxSampleVal = 1.0f;
					converted_float_to_rgb(pConvertedRgb, 4, width, height, pConvertedFloat, 4, pPalettes->mDataVR,
											minSampleVal, maxSampleVal);
				}
				else
				{
					rapi->LogOutput("WARNING: Photometric interpretation is %s, but we're missing palette data.\n",
						skPhotometricInterpretationStrings[dicomImage.mPhotometricInterpretation]);
				}
			}
			break;
		default:
			rapi->LogOutput("WARNING: Photometric interpretation not currently supported: %s\n",
				skPhotometricInterpretationStrings[dicomImage.mPhotometricInterpretation]);
			break;
		}

		if (!pConvertedRgb)
		{
			break;
		}

		char sliceName[512];
		sprintf_s(sliceName, "image%04i", texturesOut.Num());
		const int colorChannelCount = (convertedHasAlpha) ? 4 : 3;
		noesisTex_t *pTex = rapi->Noesis_TextureAllocEx(sliceName, width, height, pConvertedRgb,
														width * height * colorChannelCount,
														(convertedHasAlpha) ? NOESISTEX_RGBA32 : NOESISTEX_RGB24, 0, 1);
		if (pConvertedFloat)
		{
			int floatChannelCount;
			switch (convertedFloatFormat)
			{
			case kNHDRTF_RGB_F96:
				floatChannelCount = 3;
				break;
			case kNHDRTF_RGBA_F128:
				floatChannelCount = 4;
				break;
			case kNHDRTF_Lum_F32:
			default:
				floatChannelCount = 1;
				break;
			}

			if (g_dicomOpts && g_dicomOpts->mFloatNormalize)
			{
				//normalize floating point data if desired
				float maxValue = 0.0f;
				for (int valueIndex = 0; valueIndex < width * height * floatChannelCount; ++valueIndex)
				{
					const float v = pConvertedFloat[valueIndex];
					maxValue = (v > maxValue) ? v : maxValue;
				}
				if (maxValue > 0.0f)
				{
					const float invMaxValue = 1.0f / maxValue;
					for (int valueIndex = 0; valueIndex < width * height * floatChannelCount; ++valueIndex)
					{
						pConvertedFloat[valueIndex] *= invMaxValue;
					}
				}
			}

			pTex->pHdr = rapi->Noesis_AllocHDRTexStructure(pConvertedFloat,
															width * height * sizeof(float) * floatChannelCount,
															convertedFloatFormat, NULL, NULL);
		}
		pTex->shouldFreeData = true;
		texturesOut.Append(pTex);
	}
}

static unsigned short *jpeg_decode_ls(const void *pSource, const int sourceSize,
										const int dataPrecision, const SDicomImage &dicomImage, noeRAPI_t *rapi,
										int *pWidthOut, int *pHeightOut, int *pComponentsOut, int *pPrecisionOut)
{
	JlsParameters params;
	if (JpegLsReadHeader(pSource, sourceSize, &params) != OK)
	{
		return NULL;
	}

	const int destSize = params.bytesperline * params.height;
	unsigned short *pDest = (unsigned short *)rapi->Noesis_UnpooledAlloc(destSize);
	//modify parameters for the decode
	params.bytesperline = sizeof(unsigned short) * params.width;
	params.bitspersample = 16;
	if (JpegLsDecode(pDest, destSize, pSource, sourceSize, &params) != OK)
	{
		rapi->Noesis_UnpooledFree(pDest);
		return NULL;
	}

	*pWidthOut = params.width;
	*pHeightOut = params.height;
	*pComponentsOut = params.components;
	*pPrecisionOut = params.bitspersample;
	return pDest;
}

static void decode_jpeg_to_stream(RichBitStreamEx &bsJpegConcat, RichBitStreamEx &bsTotalOut,
									const SDicomImage &dicomImage, const EDicomTransferSyntax transferSyntax,
									noeRAPI_t *rapi)
{
	const int jpegSize = bsJpegConcat.GetOffset();
	if (jpegSize > 0)
	{
		int width, height, components, decodedPrecision;
		const int dataPrecision = (dicomImage.mBitsStoredPerChannel > 0) ? dicomImage.mBitsStoredPerChannel : 8;

		unsigned short *pDecoded;
		switch (transferSyntax)
		{
		case kDTS_JpegLSLossless:
		case kDTS_JpegLSNearLossless:
			pDecoded = jpeg_decode_ls((const unsigned char *)bsJpegConcat.GetBuffer(),
										jpegSize, dataPrecision, dicomImage, rapi,
										&width, &height, &components, &decodedPrecision);
			break;
		case kDTS_Jpeg2000LosslessReversible:
		case kDTS_Jpeg2000LosslessOrLossy:
		case kDTS_Jpeg2000Part2LosslessReversible:
		case kDTS_Jpeg2000Part2LosslessOrLossy:
			pDecoded = Image_JPEG2000_ReadDirectU16((const unsigned char *)bsJpegConcat.GetBuffer(),
										jpegSize,  &width, &height, &components, &decodedPrecision, rapi);
			break;
		default:
			pDecoded = rapi->Image_JPEG_ReadDirect((const unsigned char *)bsJpegConcat.GetBuffer(),
													jpegSize, dataPrecision, &width, &height, &components,
													&decodedPrecision, NULL);
			break;
		}

		if (pDecoded)
		{
			if (dataPrecision != decodedPrecision)
			{
				rapi->LogOutput("WARNING: DICOM stored bits per channel != JPEG data precision: %i vs %i\n",
					dataPrecision, decodedPrecision);
			}

			const int pixelCount = width * height * components;
			//shift to the dicom-desired range.
			const int shiftValue = (dicomImage.mBitsPerChannel - decodedPrecision);
			for (int pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
			{
				int pixelValue = pDecoded[pixelIndex];
				if (shiftValue > 0)
				{
					pixelValue <<= shiftValue;
				}
				else
				{
					pixelValue >>= (-shiftValue);
				}

				bsTotalOut.WriteBits(pixelValue, dicomImage.mBitsPerChannel);
			}
			rapi->Noesis_UnpooledFree(pDecoded);
		}

		bsJpegConcat.SetOffset(0);
	}
}

static unsigned char *decompress_jpeg(int *pSizeOut, const unsigned char *pData, const SDicomImage &dicomImage,
										const EDicomTransferSyntax transferSyntax, const int maxDataSize, noeRAPI_t *rapi)
{
	//hacked together bullshit, can't find any decent documentation on this.
	RichBitStreamEx bsTotalOut;
	RichBitStreamEx bsJpegConcat;
	RichBitStreamEx bs((void *)pData, maxDataSize);
	unsigned int delimiter = bs.ReadUInt();
	bool firstSeg = true;
	const int *pFrameOffsets = NULL;
	int frameCount = 0;
	int currentFrame = 0;
	int baseOfs = 0;
	while (delimiter == skSequenceTag && bs.GetOffset() < bs.GetSize())
	{
		const int seqSegSize = bs.ReadInt();
		const int jpegHeaderOfs = bs.GetOffset();
		if (seqSegSize > 0)
		{
			if (firstSeg)
			{
				pFrameOffsets = (const int *)((const char *)bs.GetBuffer() + jpegHeaderOfs);
				frameCount = seqSegSize / sizeof(int);
				baseOfs = jpegHeaderOfs + seqSegSize + 8;
			}
			else
			{
				if (frameCount > 0 && currentFrame < frameCount)
				{
					//see if we're at the start of a new frame. if so, decode and reset the offset.
					int currentOfs = jpegHeaderOfs - baseOfs;
					if (pFrameOffsets[currentFrame] >= currentOfs)
					{
						//flush to the main stream
						decode_jpeg_to_stream(bsJpegConcat, bsTotalOut, dicomImage, transferSyntax, rapi);
						++currentFrame;
					}
				}
				const unsigned char *pJpegData = (const unsigned char *)bs.GetBuffer() + jpegHeaderOfs;
				bsJpegConcat.WriteBytes(pJpegData, seqSegSize);
			}
		}
		firstSeg = false;

		bs.SetOffset(jpegHeaderOfs + seqSegSize);
		delimiter = bs.ReadUInt();
	}

	//flush the rest of the stream
	decode_jpeg_to_stream(bsJpegConcat, bsTotalOut, dicomImage, transferSyntax, rapi);

	const int outSize = bsTotalOut.GetOffset();
	if (outSize <= 0)
	{
		return NULL;
	}

	*pSizeOut = outSize;
	unsigned char *pOutBuffer = (unsigned char *)rapi->Noesis_UnpooledAlloc(outSize);
	memcpy(pOutBuffer, bsTotalOut.GetBuffer(), outSize);
	return pOutBuffer;
}

//see Annex G
static unsigned char *decompress_rle(int *pSizeOut, const unsigned char *pData, const int maxDataSize,
										const int bitsPerChannel, noeRAPI_t *rapi)
{
	RichBitStreamEx bsOut;
	RichBitStreamEx bs((void *)pData, maxDataSize);
	unsigned int delimiter = bs.ReadUInt();
	while (delimiter == skSequenceTag && bs.GetOffset() < bs.GetSize())
	{
		const unsigned int seqSegSize = bs.ReadUInt();
		const int rleHeaderOfs = bs.GetOffset();
		const int dataOutOfs = bsOut.GetOffset();

		int segCount = bs.ReadInt();
		const unsigned int *pSegOffsets = (const unsigned int *)((unsigned char *)bs.GetBuffer() + bs.GetOffset());
		for (int segIndex = 0; segIndex < segCount; ++segIndex)
		{
			const int segEnd = (segIndex >= (segCount - 1) || pSegOffsets[segIndex + 1] < pSegOffsets[segIndex]) ?
								rleHeaderOfs + seqSegSize : rleHeaderOfs + pSegOffsets[segIndex + 1];
			bs.SetOffset(rleHeaderOfs + pSegOffsets[segIndex]);
			while (bs.GetOffset() < segEnd)
			{
				const char n = bs.ReadChar();
				if (n >= 0)
				{
					const int repCount = n + 1;
					for (int valueIndex = 0; valueIndex < repCount && bs.GetOffset() < segEnd; ++valueIndex)
					{
						bsOut.WriteByte(bs.ReadByte());
					}
				}
				else if (n >= -127 && bs.GetOffset() < segEnd)
				{
					const unsigned char rep = bs.ReadByte();
					const int repCount = -n + 1;
					for (int valueIndex = 0; valueIndex < repCount; ++valueIndex)
					{
						bsOut.WriteByte(rep);
					}
				}
			}
		}
		if (segCount > 1)
		{
			//now we've gotta interleave the bytes
			const unsigned int flatDataSize = bsOut.GetOffset() - dataOutOfs;
			unsigned char *pFlatData = (unsigned char *)bsOut.GetBuffer() + dataOutOfs;
			unsigned char *pTempBuffer = (unsigned char *)rapi->Noesis_UnpooledAlloc(flatDataSize + segCount);
			unsigned char *pTempChannels = pTempBuffer + flatDataSize;
			const int interleavedPixelCount = flatDataSize / segCount;
			//possible todo - do we need to do bitsPerChannel + 7 here? can we have 9..15-bit rle images?
			const int bytesPerChannel = bitsPerChannel / 8;
			//possible todo - optimize this, needs to be refactored after incrementally discovering what a
			//shitmess the rle is.
			for (int pixelIndex = 0; pixelIndex < interleavedPixelCount; ++pixelIndex)
			{
				//"channel index" isn't necessarily the case, but it probably is.
				//monochrome 16 bit formats for example will still split each 8 bit
				//part of each pixel across rle streams.
				for (int channelIndex = 0; channelIndex < segCount; ++channelIndex)
				{
					pTempBuffer[pixelIndex * segCount + channelIndex] =
						pFlatData[pixelIndex + channelIndex * interleavedPixelCount];
				}
				//apparently we also need to swap bytes back within channels if those
				//channels are multi-byte. what a beautiful fucking mess!
				if (bytesPerChannel > 1)
				{
					unsigned char *pInterleavedPixel = pTempBuffer + pixelIndex * segCount;
					for (int channelIndex = 0; channelIndex < segCount; channelIndex += bytesPerChannel)
					{
						for (int channelByteIndex = 0; channelByteIndex < bytesPerChannel; ++channelByteIndex)
						{
							pTempChannels[channelIndex + channelByteIndex] =
								pInterleavedPixel[channelIndex + (bytesPerChannel - channelByteIndex - 1)];
						}
					}
					memcpy(pInterleavedPixel, pTempChannels, segCount);
				}
			}
			//stomp back over the stream data
			memcpy(pFlatData, pTempBuffer, flatDataSize);
			rapi->Noesis_UnpooledFree(pTempBuffer);
		}

		bs.SetOffset(rleHeaderOfs + seqSegSize);
		delimiter = bs.ReadUInt();
	}

	const int outSize = bsOut.GetOffset();
	if (outSize <= 0)
	{
		return NULL;
	}

	*pSizeOut = outSize;
	unsigned char *pOutBuffer = (unsigned char *)rapi->Noesis_UnpooledAlloc(outSize);
	memcpy(pOutBuffer, bsOut.GetBuffer(), outSize);
	return pOutBuffer;
}

static void set_palette_data_from_element(SDicomPalette *pPalette, RichBitStreamEx &bs, const CDicomDataElement &elem)
{
	pPalette->mpData = (unsigned char *)bs.GetBuffer() + elem.GetValueOfs();
	pPalette->mDataLength = elem.GetValueLength();
	pPalette->mDataVR = elem.GetVR();
}

static void set_palette_descriptor_from_element(SDicomPalette *pPalette, RichBitStreamEx &bs,
												const EDicomTransferSyntax transferSyntax, const CDicomDataElement &elem)
{
	elem.ReadArrayElementValues<unsigned int>(pPalette->mDescriptor, 3, bs, transferSyntax);
	if (pPalette->mEntryCount == 0)
	{
		//special-case - 0 means 2^16
		pPalette->mEntryCount = 65536;
	}
}

bool Image_MRI_LoadDicom(BYTE *fileBuffer, int bufferLen, CArrayList<noesisTex_t *> &noeTex, noeRAPI_t *rapi)
{
	RichBitStreamEx bs(fileBuffer, bufferLen);
	bs.SetOffset(skDataElemsOffset);

	bool transferSyntaxSupported;
	const EDicomTransferSyntax transferSyntax = find_transfer_syntax(bs);
	switch (transferSyntax)
	{
	case kDTS_ImplicitVRLittleEndian:
	case kDTS_ExplicitVRLittleEndian:
	case kDTS_ExplicitVRBigEndian:
	case kDTS_RLE:
	case kDTS_JpegBaseline:
	case kDTS_JpegExtended:
	case kDTS_JpegSpectral:
	case kDTS_JpegProgressive:
	case kDTS_JpegLossless:
	case kDTS_JpegLosslessFirstOrder:
	case kDTS_JpegLSLossless:
	case kDTS_JpegLSNearLossless:
	case kDTS_Jpeg2000LosslessReversible:
	case kDTS_Jpeg2000LosslessOrLossy:
	case kDTS_Jpeg2000Part2LosslessReversible:
	case kDTS_Jpeg2000Part2LosslessOrLossy:
		transferSyntaxSupported = true;
		break;
	default:
		transferSyntaxSupported = false;
		break;
	}

	if (!transferSyntaxSupported)
	{
		rapi->LogOutput("The following DICOM transfer syntax is not currently supported: %s\n",
			skTransferSyntaxStrings[transferSyntax]);
		return NULL;
	}

	rapi->LogOutput("Parsing data elements with transfer syntax %s.\n", skTransferSyntaxStrings[transferSyntax]);

	const int logDataElements = (g_dicomOpts) ? g_dicomOpts->mLogDataElements : 0;
	if (logDataElements > 0)
	{
		//if we want to log all of the data elements, set us back so we iterate all of the g2 elements too.
		bs.SetOffset(skDataElemsOffset);
	}

	int dataElementCount = 0;
	SDicomImage currentImage;

	//resume iterating through data elements after the transfer syntax element
	while (bs.GetOffset() < bs.GetSize())
	{
		const CDicomDataElement elem(bs, transferSyntax);

		if (logDataElements > 0)
		{
			const unsigned short vr = elem.GetVR();
			const char *pVR = (vr != 0) ? (const char *)&vr : "NA";
			rapi->LogOutput("%04i - Data Element (%04x,%04x) - VR: %c%c Size: %i Offset: %i\n", dataElementCount,
				elem.GetGroupNum(), elem.GetElemNum(), pVR[0], pVR[1], elem.GetValueLength(), elem.GetValueOfs());
			if (logDataElements > 1)
			{
				//give data info based on the value representation
				rapi->LogOutput("	Value: ");
				switch (elem.GetVR())
				{
				case skVR_ApplicationEntity:
				case skVR_AgeString:
				case skVR_CodeString:
				case skVR_Date:
				case skVR_DecimalString:
				case skVR_DateTime:
				case skVR_IntString:
				case skVR_LongString:
				case skVR_LongText:
				case skVR_PersonName:
				case skVR_ShortString:
				case skVR_ShortText:
				case skVR_Time:
				case skVR_UID:
					{
						const char *pTempBuffer = elem.TemporaryStringForElementValue(bs);
						rapi->LogOutput("%s", (pTempBuffer) ? pTempBuffer : "Unknown"); 
					}
					break;
				case skVR_AttributeTag:
					{
						unsigned short tagValues[2];
						elem.ReadArrayElementValues<unsigned short>(tagValues, 2, bs, transferSyntax);
						rapi->LogOutput("(%04x,%04x)", tagValues[0], tagValues[1]); 
					}
					break;
				case skVR_Float:
				case skVR_Double:
					{
						const double value = elem.ReadSingleElementValue<double>(bs, transferSyntax);
						rapi->LogOutput("%f", value);
					}
					break;
				case skVR_SignedLong:
				case skVR_SignedShort:
					{
						const int value = elem.ReadSingleElementValue<int>(bs, transferSyntax);
						rapi->LogOutput("%i", value);
					}
					break;
				case skVR_UnsignedLong:
				case skVR_UnsignedShort:
					{
						const unsigned int value = elem.ReadSingleElementValue<unsigned int>(bs, transferSyntax);
						rapi->LogOutput("%i", value);
					}
					break;
				case skVR_OtherByte:
				case skVR_OtherFloat:
				case skVR_OtherWord:
				case skVR_Sequence:
					rapi->LogOutput("...");
					break;
				default:
					rapi->LogOutput("N/A");
					break;
				}
				rapi->LogOutput("\n");
			}
		}

		switch (elem.GetElementId())
		{
		case skPixelDataElement:
			{
				unsigned char *pTempBuffer = NULL;
				int decompressedSize = 0;
				currentImage.mpData = (unsigned char *)bs.GetBuffer() + elem.GetValueOfs();
				currentImage.mDataLength = elem.GetValueLength();
				currentImage.mDataVR = elem.GetVR();

				bool convertVR = false;
				switch (transferSyntax)
				{
				case kDTS_RLE:
					pTempBuffer = decompress_rle(&decompressedSize, currentImage.mpData, bs.GetSize() - elem.GetValueOfs(),
													currentImage.mBitsPerChannel, rapi);
					convertVR = true;
					break;
				case kDTS_JpegBaseline:
				case kDTS_JpegExtended:
				case kDTS_JpegSpectral:
				case kDTS_JpegProgressive:
				case kDTS_JpegLossless:
				case kDTS_JpegLosslessFirstOrder:
				case kDTS_JpegLSLossless:
				case kDTS_JpegLSNearLossless:
				case kDTS_Jpeg2000LosslessReversible:
				case kDTS_Jpeg2000LosslessOrLossy:
				case kDTS_Jpeg2000Part2LosslessReversible:
				case kDTS_Jpeg2000Part2LosslessOrLossy:
					pTempBuffer = decompress_jpeg(&decompressedSize, currentImage.mpData, currentImage, transferSyntax,
													bs.GetSize() - elem.GetValueOfs(), rapi);
					convertVR = true;
					break;
				default:
					break;
				}

				if (pTempBuffer && decompressedSize > 0)
				{
					//use the decompressed result if available
					currentImage.mpData = pTempBuffer;
					currentImage.mDataLength = decompressedSize;
				}

				if (convertVR && currentImage.mBitsPerChannel > 0)
				{
					//choose an appropriate VR based on the bits per channel, assuming our provided
					//VR is meaningless in the current transfer syntax.
					//OtherWord is picked for anything less than 32 that isn't exactly 8, as that
					//path provides for n-bit reads.
					if (currentImage.mBitsPerChannel >= 32)
					{
						currentImage.mDataVR = skVR_OtherFloat;
					}
					else if (currentImage.mBitsPerChannel != 8)
					{
						currentImage.mDataVR = skVR_OtherWord;
					}
					else
					{
						currentImage.mDataVR = skVR_OtherByte;
					}
				}

				//if we don't have a photometric interpretation (some files totally lack it), try to figure one out.
				if (currentImage.mPhotometricInterpretation == kDPI_Unknown &&
					currentImage.mBitsPerChannel > 0 && currentImage.mDataLength > 0)
				{
					const int sliceCount = std::max<int>(currentImage.mSliceCount, 1);
					const int pixelCount = currentImage.mColumnCount * currentImage.mRowCount * sliceCount;
					const int pixelBitsForOneChannel = pixelCount * currentImage.mBitsPerChannel;
					const int channelCount = (currentImage.mDataLength * 8) / pixelBitsForOneChannel;
					switch (channelCount)
					{
					case 1:
						currentImage.mPhotometricInterpretation = kDPI_Monochrome2;
						break;
					case 3:
						currentImage.mPhotometricInterpretation = kDPI_RGB;
						break;
						//still haven't found any argb in the wild to implement it.
						/*
					case 4:
						currentImage.mPhotometricInterpretation = kDPI_ARGB;
						break;
						*/
					default:
						//sorry, no idea
						break;
					}
				}

				//try converting the data, and reset the local image structure.
				//not sure if we're always guaranteed to have the data preceded by all image properties,
				//but this is the case in all of the data i've seen thus far.
				convert_dicom_image(noeTex, currentImage, transferSyntax, rapi);
				currentImage = SDicomImage();
				if (pTempBuffer)
				{
					rapi->Noesis_UnpooledFree(pTempBuffer);
				}
			}
			break;
		case skPixelDataPlanarConfigurationElement:
			currentImage.mPlanarConfiguration = elem.ReadSingleElementValue<int>(bs, transferSyntax);
			break;
		case skPixelDataSamplesPerPixelElement:
			currentImage.mSamplesPerPixel = elem.ReadSingleElementValue<int>(bs, transferSyntax);
			break;
		case skPixelDataBitsAllocatedElement:
			currentImage.mBitsPerChannel = elem.ReadSingleElementValue<int>(bs, transferSyntax);
			break;
		case skPixelDataBitsStoredElement:
			currentImage.mBitsStoredPerChannel = elem.ReadSingleElementValue<int>(bs, transferSyntax);
			break;
		case skPixelDataHighBitElement:
			currentImage.mHighBit = elem.ReadSingleElementValue<int>(bs, transferSyntax);
			break;
		case skPixelDataPhotometricInterpretationElement:
			{
				elem.PrepDataStreamForValueRead(bs, transferSyntax);
				const char *pTempBuffer = elem.TemporaryStringForElementValue(bs);
				currentImage.mPhotometricInterpretation = photometric_interpretation_for_string(pTempBuffer);
			}
			break;
		case skPixelDataRowsElement:
			currentImage.mRowCount = elem.ReadSingleElementValue<int>(bs, transferSyntax);
			break;
		case skPixelDataColumnsElement:
			currentImage.mColumnCount = elem.ReadSingleElementValue<int>(bs, transferSyntax);
			break;
		case skPixelDataFrameCountElement:
			currentImage.mSliceCount = elem.ReadSingleElementValue<int>(bs, transferSyntax);
			break;
		case skPixelDataRescaleIntercept:
			currentImage.mRescaleIntercept = elem.ReadSingleElementValue<float>(bs, transferSyntax);
			break;
		case skPixelDataRescaleSlope:
			currentImage.mRescaleSlope = elem.ReadSingleElementValue<float>(bs, transferSyntax);
			break;

		case skRedPaletteLookupTableDescElement:
			set_palette_descriptor_from_element(&currentImage.mPaletteRgba[0], bs, transferSyntax, elem);
			break;
		case skGreenPaletteLookupTableDescElement:
			set_palette_descriptor_from_element(&currentImage.mPaletteRgba[1], bs, transferSyntax, elem);
			break;
		case skBluePaletteLookupTableDescElement:
			set_palette_descriptor_from_element(&currentImage.mPaletteRgba[2], bs, transferSyntax, elem);
			break;
		case skAlphaPaletteLookupTableDescElement:
			set_palette_descriptor_from_element(&currentImage.mPaletteRgba[3], bs, transferSyntax, elem);
			break;
		case skRedPaletteLookupTableDataElement:
			set_palette_data_from_element(&currentImage.mPaletteRgba[0], bs, elem);
			break;
		case skGreenPaletteLookupTableDataElement:
			set_palette_data_from_element(&currentImage.mPaletteRgba[1], bs, elem);
			break;
		case skBluePaletteLookupTableDataElement:
			set_palette_data_from_element(&currentImage.mPaletteRgba[2], bs, elem);
			break;
		case skAlphaPaletteLookupTableDataElement:
			set_palette_data_from_element(&currentImage.mPaletteRgba[3], bs, elem);
			break;

		default:
			break;
		}

		++dataElementCount;
		elem.SetStreamToNextDataElement(bs);
	}

	if (logDataElements > 0)
	{
		rapi->LogOutput("Data Element count (may include sequence delimiters): %i\n", dataElementCount);
	}

	return true;
}

static void flush_group_stream(const unsigned short groupNum, const EDicomTransferSyntax outTS,
								RichBitStreamEx &outStream, RichBitStreamEx &groupStream)
{
	//write the group length element
	CDicomDataElement(groupNum, 0x0000, skVR_UnsignedLong).WriteTypeToStream<unsigned int>(
		outStream, outTS, groupStream.GetOffset());
	//then copy over all of the elements
	outStream.WriteBytes(groupStream.GetBuffer(), groupStream.GetOffset());

	groupStream.SetOffset(0);
}

bool Image_MRI_SaveDicom(char *fileName, noesisTex_t *textures, int numTex, noeRAPI_t *rapi)
{
	//potential optimization - lots of memory stream juggling, could be refactored
	RichBitStreamEx outStream;

	RichBitStreamEx groupStream;

	const int width = textures->w;
	const int height = textures->h;

	//only explicit little is supported at the moment
	const EDicomTransferSyntax outTS = kDTS_ExplicitVRLittleEndian;

	rapi->LogOutput("Exporting DICOM with transfer syntax %s...\n",
		skTransferSyntaxStrings[outTS]);

	for (int preambleIndex = 0; preambleIndex < skHeaderOffset; ++preambleIndex)
	{
		outStream.WriteByte(0);
	}
	outStream.WriteString("DICM");

	unsigned char metaVerData[2] = { 0x00, 0x01 };
	//file meta version
	CDicomDataElement(skGroup2, 0x0001, skVR_OtherByte).WriteToStream(
		groupStream, outTS, metaVerData, sizeof(metaVerData));
	//media storage SOP - raw data
	CDicomDataElement(skGroup2, 0x0002, skVR_UID).WriteStringToStream(
		groupStream, outTS, "1.2.840.10008.5.1.4.1.1.6");
	//media storage SOP instance UID
	CDicomDataElement(skGroup2, 0x0003, skVR_UID).WriteStringToStream(
		groupStream, outTS, "999.999.2.20150101.112000.2.666");
	//transfer syntax UID
	CDicomDataElement(skGroup2, 0x0010, skVR_UID).WriteStringToStream(
		groupStream, outTS, skTransferSyntaxStrings[outTS]);
	//implementation class UID
	CDicomDataElement(skGroup2, 0x0012, skVR_UID).WriteStringToStream(
		groupStream, outTS, "999.999");

	//end of group 2
	flush_group_stream(skGroup2, outTS, outStream, groupStream);

	//image type
	CDicomDataElement(0x0008, 0x0008, skVR_CodeString).WriteStringToStream(
		groupStream, outTS, "ORIGINAL\\PRIMARY");
	//SOP class UID
	CDicomDataElement(0x0008, 0x0016, skVR_UID).WriteStringToStream(
		groupStream, outTS, "1.2.840.10008.5.1.4.1.1.6");
	//SOP instance UID
	CDicomDataElement(0x0008, 0x0018, skVR_UID).WriteStringToStream(
		groupStream, outTS, "999.999.2.20150101.112000.2.666");
	//study date
	CDicomDataElement(0x0008, 0x0020, skVR_Date).WriteStringToStream(
		groupStream, outTS, "2015.01.01");
	//content date
	CDicomDataElement(0x0008, 0x0023, skVR_Date).WriteStringToStream(
		groupStream, outTS, "2015.01.01");
	//study time
	CDicomDataElement(0x0008, 0x0030, skVR_Time).WriteStringToStream(
		groupStream, outTS, "10:00:00");
	//modality
	CDicomDataElement(0x0008, 0x0060, skVR_CodeString).WriteStringToStream(
		groupStream, outTS, "US");
	//manufacturer
	CDicomDataElement(0x0008, 0x0070, skVR_LongString).WriteStringToStream(
		groupStream, outTS, "Noesis");
	//referring physician's name
	CDicomDataElement(0x0008, 0x0090, skVR_PersonName).WriteStringToStream(
		groupStream, outTS, "Whitehouse, Dick");
	//study description
	CDicomDataElement(0x0008, 0x1030, skVR_LongString).WriteStringToStream(
		groupStream, outTS, "Noesis export");
	//series description
	CDicomDataElement(0x0008, 0x103E, skVR_LongString).WriteStringToStream(
		groupStream, outTS, "Very important data");
	//stage name
	CDicomDataElement(0x0008, 0x2120, skVR_LongString).WriteStringToStream(
		groupStream, outTS, "Happy lovely time");
	//stage number
	CDicomDataElement(0x0008, 0x2122, skVR_IntString).WriteStringToStream(
		groupStream, outTS, "1");
	//number of stages
	CDicomDataElement(0x0008, 0x2124, skVR_IntString).WriteStringToStream(
		groupStream, outTS, "1");
	//view number
	CDicomDataElement(0x0008, 0x2128, skVR_IntString).WriteStringToStream(
		groupStream, outTS, "1");
	//number of views in stage
	CDicomDataElement(0x0008, 0x212A, skVR_IntString).WriteStringToStream(
		groupStream, outTS, "1");

	//end of group 8
	flush_group_stream(0x0008, outTS, outStream, groupStream);

	//patient full name
	CDicomDataElement(0x0010, 0x0010, skVR_PersonName).WriteStringToStream(
		groupStream, outTS, "Wong, Johnny");

	//end of group 0x0010
	flush_group_stream(0x0010, outTS, outStream, groupStream);

	//protocol name
	CDicomDataElement(0x0018, 0x1030, skVR_LongString).WriteStringToStream(
		groupStream, outTS, "Noesis Export");
	//frame time
	if (numTex > 1)
	{
		CDicomDataElement(skFrameTimeElement, skVR_DecimalString).WriteStringToStream(
			groupStream, outTS, "33.333");
	}

	//end of group 0x0018
	flush_group_stream(0x0018, outTS, outStream, groupStream);

	//study instance UID
	CDicomDataElement(0x0020, 0x000D, skVR_UID).WriteStringToStream(
		groupStream, outTS, "999.999.2.20150101.112000");
	//series instance UID
	CDicomDataElement(0x0020, 0x000E, skVR_UID).WriteStringToStream(
		groupStream, outTS, "999.999.2.20150101.112000.2");
	//series number
	CDicomDataElement(0x0020, 0x0011, skVR_IntString).WriteStringToStream(
		groupStream, outTS, "2");
	//instance number
	CDicomDataElement(0x0020, 0x0013, skVR_IntString).WriteStringToStream(
		groupStream, outTS, "666");

	//end of group 0x0020
	flush_group_stream(0x0020, outTS, outStream, groupStream);

	//samples per pixel
	CDicomDataElement(skPixelDataSamplesPerPixelElement, skVR_UnsignedShort).WriteTypeToStream<unsigned short>(
		groupStream, outTS, 3);
	//photometric interpretation
	CDicomDataElement(skPixelDataPhotometricInterpretationElement, skVR_CodeString).WriteStringToStream(
		groupStream, outTS, skPhotometricInterpretationStrings[kDPI_RGB]);
	//planar configuration
	CDicomDataElement(skPixelDataPlanarConfigurationElement, skVR_UnsignedShort).WriteTypeToStream<unsigned short>(
		groupStream, outTS, 0);
	//frame count and frame time increment pointer
	if (numTex > 1)
	{
		char tempString[256];
		sprintf_s(tempString, "%i", numTex);
		CDicomDataElement(skPixelDataFrameCountElement, skVR_IntString).WriteStringToStream(
			groupStream, outTS, tempString);
		CDicomDataElement(skPixelDataFrameIncrementPointerElement, skVR_AttributeTag).WriteTypeToStream<unsigned int>(
			groupStream, outTS, skFrameTimeElement);
	}
	//row count
	CDicomDataElement(skPixelDataRowsElement, skVR_UnsignedShort).WriteTypeToStream<unsigned short>(
		groupStream, outTS, height);
	//column count
	CDicomDataElement(skPixelDataColumnsElement, skVR_UnsignedShort).WriteTypeToStream<unsigned short>(
		groupStream, outTS, width);
	//aspect ratio
	CDicomDataElement(skPixelDataAspectRatioElement, skVR_IntString).WriteStringToStream(
		groupStream, outTS, "4\\3");
	//bits allocated
	CDicomDataElement(skPixelDataBitsAllocatedElement, skVR_UnsignedShort).WriteTypeToStream<unsigned short>(
		groupStream, outTS, 8);
	//bits stored
	CDicomDataElement(skPixelDataBitsStoredElement, skVR_UnsignedShort).WriteTypeToStream<unsigned short>(
		groupStream, outTS, 8);
	//high bit
	CDicomDataElement(skPixelDataHighBitElement, skVR_UnsignedShort).WriteTypeToStream<unsigned short>(
		groupStream, outTS, 7);
	//representation
	CDicomDataElement(skPixelDataRepresentationElement, skVR_UnsignedShort).WriteTypeToStream<unsigned short>(
		groupStream, outTS, 0);

	//end of group 0x0028
	flush_group_stream(0x0028, outTS, outStream, groupStream);

	//build up an image buffer stream
	{
		RichBitStreamEx imageStream;
		for (int texIndex = 0; texIndex < numTex; ++texIndex)
		{
			noesisTex_t *pTexture = textures + texIndex;
			bool shouldFreeRgba = false;
			unsigned char *pRgba = rapi->Image_GetTexRGBA(pTexture, shouldFreeRgba);
			if (!pRgba)
			{
				rapi->LogOutput("WARNING: Could not retrieve RGBA data for texture %i.\n", texIndex);
				pRgba = (unsigned char *)rapi->Noesis_UnpooledAlloc(width * height * 4);
				memset(pRgba, 0, width * height * 4);
				shouldFreeRgba = true;
			}
			else
			{
				if (pTexture->w != width || pTexture->h != height)
				{
					//all image dimensions must match, resample it
					unsigned char *pResizedRgba = (unsigned char *)rapi->Noesis_UnpooledAlloc(width * height * 4);
					rapi->Noesis_ResampleImageBilinear(pRgba, pTexture->w, pTexture->h, pResizedRgba, width, height);
					if (shouldFreeRgba)
					{
						rapi->Noesis_UnpooledFree(pRgba);
					}
					//adopt the resized buffer after freeing the original buffer
					pRgba = pResizedRgba;
					shouldFreeRgba = true;
				}

				//write just the rgb values
				for (int pixelIndex = 0; pixelIndex < width * height; ++pixelIndex)
				{
					const unsigned char *pColor = pRgba + pixelIndex * 4;
					imageStream.WriteByte(pColor[0]);
					imageStream.WriteByte(pColor[1]);
					imageStream.WriteByte(pColor[2]);
				}
			}

			if (shouldFreeRgba)
			{
				rapi->Noesis_UnpooledFree(pRgba);
			}
		}

		//write the full image buffer to the group stream as pixel data
		CDicomDataElement(skPixelDataElement, skVR_OtherByte).WriteToStream(
			groupStream, outTS, imageStream.GetBuffer(), imageStream.GetOffset());
	}

	//end of group 0x7FE0
	flush_group_stream(0x7FE0, outTS, outStream, groupStream);

	const int finalSize = outStream.GetSize();
	rapi->Noesis_WriteFile(fileName, outStream.GetBuffer(), finalSize);

	return true;
}

#define DICOM_DECL_OPTS(argRequired) \
	dicomOpts_t *lopts = (dicomOpts_t *)store; \
	assert(storeSize == sizeof(dicomOpts_t)); \
	if (argRequired && !arg) \
	{ \
		return false; \
	}

bool Model_MRI_FloatNormalizeHandler(const char *arg, unsigned char *store, int storeSize)
{
	DICOM_DECL_OPTS(false);

	lopts->mFloatNormalize = true;
	return true;
}

bool Model_MRI_SBPHandler(const char *arg, unsigned char *store, int storeSize)
{
	DICOM_DECL_OPTS(true);

	lopts->mApplySBP = true;
	sscanf(arg, "%f;%f;%f", &lopts->mSBPScale, &lopts->mSBPBias, &lopts->mSBPExponent);
	return true;
}

bool Model_MRI_DataNormalizeHandler(const char *arg, unsigned char *store, int storeSize)
{
	DICOM_DECL_OPTS(false);

	lopts->mDataNormalize = true;
	return true;
}

bool Model_MRI_ApplySlopeHandler(const char *arg, unsigned char *store, int storeSize)
{
	DICOM_DECL_OPTS(false);

	lopts->mApplySlope = true;
	return true;
}

bool Model_MRI_ElemLogHandler(const char *arg, unsigned char *store, int storeSize)
{
	DICOM_DECL_OPTS(true);

	lopts->mLogDataElements = atoi(arg);
	return true;
}

