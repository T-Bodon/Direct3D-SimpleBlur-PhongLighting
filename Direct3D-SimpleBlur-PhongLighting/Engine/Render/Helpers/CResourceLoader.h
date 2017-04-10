#pragma once
#include "Dx.h"

#include <vector>
#include <wincodec.h>
#include <Shlwapi.h>
#pragma comment( lib, "Shlwapi.lib" )

namespace helpers {
  class CShaderLoader {
  public:
    static std::wstring GetPackagePath();

    static std::vector<uint8_t> LoadPackageFile(
      const std::wstring & _path
    );
  };

  struct FaceVertex {
    uint32_t mVertexIdx;
    uint32_t mTextIdx;
    uint32_t mNormalIdx;
    FaceVertex();
    FaceVertex(const uint32_t _vIdx, const uint32_t _tIdx, const uint32_t _nIdx);
  };

  struct Face {
    std::vector<FaceVertex> face;
    Face(const FaceVertex _v1, const FaceVertex _v2, const FaceVertex _v3);
  };

  class CSimpleObjLoader {
  public:
    CSimpleObjLoader();
    ~CSimpleObjLoader();

    void LoadObj(const wchar_t* _path);

    std::vector<DirectX::XMFLOAT3> * GetVBuff();
    std::vector<DirectX::XMFLOAT2> * GetVtBuff();
    std::vector<DirectX::XMFLOAT3> * GetVnBuff();
    std::vector<Face> * GetFaces();

  private:
    std::vector<DirectX::XMFLOAT3> mVBuff;
    std::vector<DirectX::XMFLOAT2> mVtBuff;
    std::vector<DirectX::XMFLOAT3> mVnBuff;
    std::vector<Face> mFaces;
  };
}