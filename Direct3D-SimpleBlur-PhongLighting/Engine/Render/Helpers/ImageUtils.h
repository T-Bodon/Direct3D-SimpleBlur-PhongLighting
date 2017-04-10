#pragma once

#include <wincodec.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <d2d1_2.h>
#include <unordered_map>

#include <Shcore.h>

#include <Shlwapi.h>
#pragma comment( lib, "Shlwapi.lib" )

typedef GUID GUID_ContainerFormat;
typedef GUID GUID_WICPixelFormat;

// rotations are clock-wise
// exif flags starts from 1
enum class ExifRotationFlag : uint16_t {
  NoTransform = 1,
  FlipHorizontal = 2,
  Rotate180 = 3,
  FlipVertcal = 4,
  Rotate90FlipHorizontal = 5,
  Rotate270 = 6,
  Rotate90FlipVertcal = 7,
  Rotate90 = 8,
};

enum class ImageContainer {
  PNG, 
  JPG, 
  BMP
};

struct ImagePixels {
  DirectX::XMUINT2 Size;
  std::vector<uint8_t> Data;
};

class ImageUtils {
public:
  ImageUtils();
  ~ImageUtils();

  DirectX::XMUINT2 GetFrameSize(IWICBitmapSource* _frame) const;

  // Decoding:
  Microsoft::WRL::ComPtr<IWICBitmapDecoder> CreateDecoder(const Microsoft::WRL::ComPtr<IStream>& _stream) const;
  Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> CreateFrameForDecode(IWICBitmapDecoder* _decoder, 
    uint32_t _idx = 0) const;
  void DecodePixels(IWICBitmapSource* _frame, uint32_t _pixelsByteSize, void* _pixels, 
    const WICRect* _rect = nullptr) const;

  Microsoft::WRL::ComPtr<IWICBitmap> LoadToMemory(IWICBitmapSource* _frame) const;

  // Transforms:
  Microsoft::WRL::ComPtr<IWICFormatConverter> ConvertPixelFormatInMemory(IWICBitmapSource* _frame, 
    const GUID_WICPixelFormat& _destFmt) const;
  Microsoft::WRL::ComPtr<IWICBitmapScaler> Scale(IWICBitmapSource* _frame, const DirectX::XMUINT2& _destSize,
    WICBitmapInterpolationMode _interpolationMode = WICBitmapInterpolationMode::WICBitmapInterpolationModeLinear) const;
  Microsoft::WRL::ComPtr<IWICBitmapFlipRotator> RotateFlipInMemory(IWICBitmapSource* _frame, 
    WICBitmapTransformOptions _options) const;

  static Microsoft::WRL::ComPtr<ID2D1Bitmap1> LoadBmpFromPath(const Microsoft::WRL::ComPtr<IStream>& _stream, 
    ID2D1DeviceContext* _d2dCtx);

  static ImagePixels LoadDxPBgra32Image(const Microsoft::WRL::ComPtr<IStream>& _stream, 
    uint32_t _maxSize);

  ExifRotationFlag GetExifRotation(IWICBitmapFrameDecode* _frame) const;
  WICBitmapTransformOptions GetInversedRotationFlipOptions(ExifRotationFlag _v) const;
  uint32_t GetFrameByteSize(IWICBitmapSource* _frame) const;
  uint32_t GetPixelBitSize(IWICBitmapSource* _frame) const;
  uint32_t GetFrameStride(IWICBitmapSource* _frame) const;

private:
  Microsoft::WRL::ComPtr<IWICImagingFactory2>  wicFactory;

  static WICBitmapTransformOptions RotationFlipOptionsFromExifInversed(ExifRotationFlag v);

  unsigned int GetFrameBitSizeFromFormat(GUID _format) const;
};