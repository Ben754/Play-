#include "GsTransferInvalidationTest.h"
#include "gs/GSHandler.h"
#include "gs/GsPixelFormats.h"

static CGSHandler::BITBLTBUF MakeDstBltBuf(uint32 psm, uint32 bufPtr, uint32 bufWidth)
{
	assert((bufPtr & 0xFF) == 0);
	assert((bufWidth & 0x3F) == 0);

	auto bltBuf = make_convertible<CGSHandler::BITBLTBUF>(0);
	bltBuf.nDstPsm = psm;
	bltBuf.nDstPtr = bufPtr / 0x100;
	bltBuf.nDstWidth = bufWidth / 0x40;
	return bltBuf;
}

static CGSHandler::TRXREG MakeTrxReg(uint32 width, uint32 height)
{
	auto trxReg = make_convertible<CGSHandler::TRXREG>(0);
	trxReg.nRRW = width;
	trxReg.nRRH = height;
	return trxReg;
}

static CGSHandler::TRXPOS MakeDstTrxPos(uint32 x, uint32 y)
{
	auto trxPos = make_convertible<CGSHandler::TRXPOS>(0);
	trxPos.nDSAX = x;
	trxPos.nDSAY = y;
	return trxPos;
}

template <uint32 psm, typename Storage>
static void SinglePageTransferTest()
{
	uint32 bufPtr = 0x300000;
	uint32 bufWidth = Storage::PAGEWIDTH;

	auto bltBuf = MakeDstBltBuf(psm, bufPtr, bufWidth);
	auto trxReg = MakeTrxReg(Storage::PAGEWIDTH, Storage::PAGEHEIGHT);
	auto trxPos = make_convertible<CGSHandler::TRXPOS>(0);

	auto [transferAddress, transferSize] = CGSHandler::GetTransferInvalidationRange(bltBuf, trxReg, trxPos);

	TEST_VERIFY(transferAddress == bufPtr);
	TEST_VERIFY(transferSize == CGsPixelFormats::PAGESIZE);
}

static void SimpleOffsetYTransferTest()
{
	//In this test, the transfer position is skipping a few pages
	//transferAddress should start at (bufWidth / 64) * 2
	//This is used for movie playback in many games

	uint32 bufPtr = 0x100000;
	uint32 bufWidth = 640;
	uint32 pageCountX = bufWidth / CGsPixelFormats::STORAGEPSMCT32::PAGEWIDTH;
	uint32 pageOffsetY = 2;

	auto bltBuf = MakeDstBltBuf(CGSHandler::PSMCT32, bufPtr, bufWidth);
	auto trxReg = MakeTrxReg(16, 16);
	auto trxPos = MakeDstTrxPos(0, CGsPixelFormats::STORAGEPSMCT32::PAGEHEIGHT * pageOffsetY);

	auto [transferAddress, transferSize] = CGSHandler::GetTransferInvalidationRange(bltBuf, trxReg, trxPos);

	TEST_VERIFY(transferAddress == (bufPtr + (CGsPixelFormats::PAGESIZE * pageCountX * pageOffsetY)));
	//TEST_VERIFY(transferSize == CGsPixelFormats::PAGESIZE);
}

static void ZeroBufWidthTransferTest()
{
	//This is done by DBZ: Tenkaichi 2
	//This transfer should touch a page at least

	uint32 bufPtr = 0x380000;
	uint32 bufWidth = 0;

	auto bltBuf = MakeDstBltBuf(CGSHandler::PSMCT32, bufPtr, bufWidth);
	auto trxReg = MakeTrxReg(16, 16);
	auto trxPos = make_convertible<CGSHandler::TRXPOS>(0);

	auto [transferAddress, transferSize] = CGSHandler::GetTransferInvalidationRange(bltBuf, trxReg, trxPos);

	TEST_VERIFY(transferAddress == bufPtr);
	TEST_VERIFY(transferSize == CGsPixelFormats::PAGESIZE);
}

void CGsTransferInvalidationTest::Execute()
{
	SinglePageTransferTest<CGSHandler::PSMCT32, CGsPixelFormats::STORAGEPSMCT32>();
	SinglePageTransferTest<CGSHandler::PSMCT16, CGsPixelFormats::STORAGEPSMCT16>();
	SinglePageTransferTest<CGSHandler::PSMT8, CGsPixelFormats::STORAGEPSMT8>();
	SimpleOffsetYTransferTest();
	ZeroBufWidthTransferTest();
}
