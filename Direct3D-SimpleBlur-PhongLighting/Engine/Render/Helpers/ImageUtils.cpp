#include "ImageUtils.h"
#include <algorithm>

ImageUtils::ImageUtils() {
  auto hr = CoCreateInstance(
    CLSID_WICImagingFactory2,
    nullptr,
    CLSCTX_INPROC_SERVER,
    IID_PPV_ARGS(this->wicFactory.ReleaseAndGetAddressOf())
  );
}

ImageUtils::~ImageUtils() {
}

DirectX::XMUINT2 ImageUtils::GetFrameSize(IWICBitmapSource* _frame) const {
  HRESULT hr = S_OK;
  DirectX::XMUINT2 size;

  hr = _frame->GetSize(&size.x, &size.y);

  return size;
}

Microsoft::WRL::ComPtr<IWICBitmapDecoder> ImageUtils::CreateDecoder(const Microsoft::WRL::ComPtr<IStream>& _stream) const {
  HRESULT hr = S_OK;
  Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;

  hr = this->wicFactory->CreateDecoderFromStream(
    _stream.Get(),
    nullptr,
    WICDecodeOptions::WICDecodeMetadataCacheOnDemand,
    decoder.ReleaseAndGetAddressOf());

  return decoder;
}

Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> ImageUtils::CreateFrameForDecode(IWICBitmapDecoder* _decoder, uint32_t _idx) const {
  HRESULT hr = S_OK;
  Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;

  hr = _decoder->GetFrame(_idx, frame.GetAddressOf());

  return frame;
}

void ImageUtils::DecodePixels(IWICBitmapSource * frame, uint32_t pixelsByteSize, 
  void * pixels, const WICRect * rect) const {
  HRESULT hr = S_OK;
  uint32_t frameStride;
  if (rect) {
    frameStride = rect->Width * 4;
  }
  else {
    frameStride = this->GetFrameStride(frame);
  }

  hr = frame->CopyPixels(rect, frameStride, pixelsByteSize, static_cast<BYTE *>(pixels));
}

Microsoft::WRL::ComPtr<IWICBitmap> ImageUtils::LoadToMemory(IWICBitmapSource* _frame) const {
  HRESULT hr = S_OK;
  Microsoft::WRL::ComPtr<IWICBitmap> bitmap;

  hr = this->wicFactory->CreateBitmapFromSource(
    _frame,
    WICBitmapCreateCacheOption::WICBitmapCacheOnDemand,
    bitmap.GetAddressOf());

  return bitmap;
}

Microsoft::WRL::ComPtr<IWICFormatConverter> ImageUtils::ConvertPixelFormatInMemory(IWICBitmapSource* _frame,
  const GUID_WICPixelFormat& _destFmt) const {
  HRESULT hr = S_OK;
  Microsoft::WRL::ComPtr<IWICFormatConverter> transformedFrame;
  Microsoft::WRL::ComPtr<IWICBitmap> tmpBitmap = this->LoadToMemory(_frame);

  hr = this->wicFactory->CreateFormatConverter(transformedFrame.GetAddressOf());

  hr = transformedFrame->Initialize(tmpBitmap.Get(), _destFmt,
    WICBitmapDitherType::WICBitmapDitherTypeNone,
    nullptr, 0.f,
    WICBitmapPaletteType::WICBitmapPaletteTypeCustom);

  return transformedFrame;
}

Microsoft::WRL::ComPtr<IWICBitmapScaler> ImageUtils::Scale(IWICBitmapSource* _frame, const DirectX::XMUINT2& _destSize, 
  WICBitmapInterpolationMode _interpolationMode) const {
  HRESULT hr = S_OK;
  Microsoft::WRL::ComPtr<IWICBitmapScaler> transformedFrame;

  auto tmpSize = _destSize;

  if (_destSize.x == 0) {
    tmpSize.x = 1;
  }
  if (_destSize.y == 0) {
    tmpSize.y = 1;
  }

  hr = this->wicFactory->CreateBitmapScaler(transformedFrame.GetAddressOf());

  hr = transformedFrame->Initialize(_frame, tmpSize.x, tmpSize.y, _interpolationMode);

  return transformedFrame;
}

Microsoft::WRL::ComPtr<IWICBitmapFlipRotator> ImageUtils::RotateFlipInMemory(IWICBitmapSource* _frame, 
  WICBitmapTransformOptions _options) const {
  HRESULT hr = S_OK;
  Microsoft::WRL::ComPtr<IWICBitmapFlipRotator> transformedFrame;

  Microsoft::WRL::ComPtr<IWICBitmap> tmpBitmap = this->LoadToMemory(_frame);

  hr = this->wicFactory->CreateBitmapFlipRotator(transformedFrame.GetAddressOf());

  hr = transformedFrame->Initialize(tmpBitmap.Get(), _options);

  return transformedFrame;
}

Microsoft::WRL::ComPtr<ID2D1Bitmap1> ImageUtils::LoadBmpFromPath(const Microsoft::WRL::ComPtr<IStream>& _stream, 
  ID2D1DeviceContext* _d2dCtx) {
  HRESULT hr = S_OK;
  ImagePixels pixels;
  ImageUtils imgUtils;

  auto dec = imgUtils.CreateDecoder(_stream);
  Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> imgDecoderFrame = imgUtils.CreateFrameForDecode(dec.Get());
  Microsoft::WRL::ComPtr<IWICFormatConverter> newFrame = imgUtils.ConvertPixelFormatInMemory(imgDecoderFrame.Get(), 
    GUID_WICPixelFormat32bppPBGRA);
  DirectX::XMUINT2 imgFrameSize = imgUtils.GetFrameSize(newFrame.Get());

  float x, y;
  _d2dCtx->GetDpi(&x, &y);

  D2D1_BITMAP_PROPERTIES1 bitmapProps;
  bitmapProps.dpiX = x;
  bitmapProps.dpiY = y;
  bitmapProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
  bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
  bitmapProps.colorContext = nullptr;

  Microsoft::WRL::ComPtr<ID2D1Bitmap1> res;

  HRESULT hres = _d2dCtx->CreateBitmapFromWicBitmap(newFrame.Get(), &bitmapProps, res.GetAddressOf());

  return res;
}

ImagePixels ImageUtils::LoadDxPBgra32Image(const Microsoft::WRL::ComPtr<IStream>& _stream, 
  uint32_t _maxSize) {
  HRESULT hr = S_OK;
  ImagePixels pixels;
  ImageUtils imgUtils;
  WICBitmapTransformOptions rotateFlip;
  auto dec = imgUtils.CreateDecoder(_stream);
  Microsoft::WRL::ComPtr<IWICBitmapSource> frame = imgUtils.CreateFrameForDecode(dec.Get());

  try {
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> decodeFrame;
    frame.As(&decodeFrame);
    auto exifRotation = imgUtils.GetExifRotation(decodeFrame.Get());
    rotateFlip = imgUtils.GetInversedRotationFlipOptions(exifRotation);
  }
  catch (...) {
    rotateFlip = WICBitmapTransformOptions::WICBitmapTransformRotate0;
  }

  pixels.Size = imgUtils.GetFrameSize(frame.Get());

  DirectX::XMFLOAT2 tmpVec((float)pixels.Size.x, (float)pixels.Size.y);
  float scaleFactor = 1.0f;
  if ((float)_maxSize < (std::max)(tmpVec.x, tmpVec.y)) {
    scaleFactor = (float)_maxSize / (std::max)(tmpVec.x, tmpVec.y);
  }

  tmpVec.x *= scaleFactor;
  tmpVec.y *= scaleFactor;
  pixels.Size = DirectX::XMUINT2((uint32_t)tmpVec.x, (uint32_t)tmpVec.y);
  frame = imgUtils.Scale(frame.Get(), pixels.Size);

  if (rotateFlip != WICBitmapTransformOptions::WICBitmapTransformRotate0) {
    frame = imgUtils.RotateFlipInMemory(frame.Get(), rotateFlip);
  }

  frame = imgUtils.ConvertPixelFormatInMemory(frame.Get(), GUID_WICPixelFormat32bppPBGRA);
  pixels.Size = imgUtils.GetFrameSize(frame.Get());

  auto frameByteSize = imgUtils.GetFrameByteSize(frame.Get());

  pixels.Data.resize(frameByteSize);
  imgUtils.DecodePixels(frame.Get(), frameByteSize, pixels.Data.data());

  return pixels;
}

ExifRotationFlag ImageUtils::GetExifRotation(IWICBitmapFrameDecode* _frame) const {
  HRESULT hr = S_OK;
  ExifRotationFlag exifRotationFlag;
  Microsoft::WRL::ComPtr<IWICMetadataQueryReader> mdReader;
  GUID_ContainerFormat containerFormat;
  PROPVARIANT pv;

  pv.vt = VT_EMPTY;

  hr = _frame->GetMetadataQueryReader(mdReader.GetAddressOf());

  hr = mdReader->GetContainerFormat(&containerFormat);

  if (containerFormat == GUID_ContainerFormatJpeg) {
    hr = mdReader->GetMetadataByName(L"/app1/ifd/{ushort=274}", &pv);

    exifRotationFlag = static_cast<ExifRotationFlag>(pv.uiVal);
  }
  else {
    exifRotationFlag = ExifRotationFlag::NoTransform;
  }

  return exifRotationFlag;
}

WICBitmapTransformOptions ImageUtils::GetInversedRotationFlipOptions(ExifRotationFlag _v) const {
  return ImageUtils::RotationFlipOptionsFromExifInversed(_v);
}

uint32_t ImageUtils::GetFrameByteSize(IWICBitmapSource* _frame) const {
  uint32_t byteSize = 0;
  auto frameSize = ImageUtils::GetFrameSize(_frame);
  auto pixelBitSize = this->GetPixelBitSize(_frame);

  byteSize = (frameSize.x * frameSize.y) * (pixelBitSize / 8);

  return byteSize;
}

uint32_t ImageUtils::GetPixelBitSize(IWICBitmapSource* _frame) const {
  HRESULT hr = S_OK;
  uint32_t pixelBitSize = 0;
  GUID_WICPixelFormat pixelFormat;

  hr = _frame->GetPixelFormat(&pixelFormat);
  
  pixelBitSize = GetFrameBitSizeFromFormat(pixelFormat);


  return pixelBitSize;
}

uint32_t ImageUtils::GetFrameStride(IWICBitmapSource* _frame) const {
  uint32_t stride = 0;
  auto frameSize = this->GetFrameSize(_frame);
  auto pixelBitSize = this->GetPixelBitSize(_frame);

  // formula from https://msdn.microsoft.com/en-us/library/windows/desktop/ee690179%28v=vs.85%29.aspx Codec Developer Remarks
  stride = (frameSize.x * pixelBitSize + 7) / 8;

  return stride;
}

WICBitmapTransformOptions ImageUtils::RotationFlipOptionsFromExifInversed(ExifRotationFlag _v) {
  // WICBitmapTransformOptions rotations is clock-wise

  static const WICBitmapTransformOptions Opts[8] = {
    WICBitmapTransformOptions::WICBitmapTransformRotate0,			// 1
    WICBitmapTransformOptions::WICBitmapTransformFlipHorizontal,	// 2
    WICBitmapTransformOptions::WICBitmapTransformRotate180,			// 3
    WICBitmapTransformOptions::WICBitmapTransformFlipVertical,		// 4

    (WICBitmapTransformOptions)
    (WICBitmapTransformOptions::WICBitmapTransformRotate90 |
      WICBitmapTransformOptions::WICBitmapTransformFlipHorizontal),	// 5

    WICBitmapTransformOptions::WICBitmapTransformRotate90,			// 6

    (WICBitmapTransformOptions)
    (WICBitmapTransformOptions::WICBitmapTransformRotate90 |
      WICBitmapTransformOptions::WICBitmapTransformFlipVertical),		// 7

    WICBitmapTransformOptions::WICBitmapTransformRotate270			// 8
  };

  return Opts[static_cast<uint16_t>(_v) - 1];
}

unsigned int ImageUtils::GetFrameBitSizeFromFormat(GUID _format) const {
  if (_format == GUID_WICPixelFormat32bppPBGRA ||
    _format == GUID_WICPixelFormat32bppPRGBA ||
    _format == GUID_WICPixelFormat32bppBGRA ||
    _format == GUID_WICPixelFormat32bppRGBA) {
    return 32;
  }

  if (_format == GUID_WICPixelFormat24bppBGR ||
    _format == GUID_WICPixelFormat24bppRGB) {
    return 24;
  }

  if (_format == GUID_WICPixelFormat1bppIndexed ||
    _format == GUID_WICPixelFormatBlackWhite) {
    return 1;
  }

  if (_format == GUID_WICPixelFormat2bppIndexed ||
    _format == GUID_WICPixelFormat2bppGray) {
    return 2;
  }

  if (_format == GUID_WICPixelFormat4bppIndexed ||
    _format == GUID_WICPixelFormat4bppGray) {
    return 4;
  }

  if (_format == GUID_WICPixelFormat8bppIndexed ||
    _format == GUID_WICPixelFormat8bppGray ||
    _format == GUID_WICPixelFormat8bppAlpha ) {
    return 8;
  }

  if (_format == GUID_WICPixelFormat16bppBGR555 ||
    _format == GUID_WICPixelFormat16bppBGR565 ||
    _format == GUID_WICPixelFormat16bppBGRA5551 ||
    _format == GUID_WICPixelFormat16bppGray) {
    return 16;
  }

  return 0;
}