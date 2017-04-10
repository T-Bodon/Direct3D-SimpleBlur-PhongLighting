#include "CResourceLoader.h"

#include <Windows.h>
#include <fstream>
#include <string>
#include <sstream>

std::wstring helpers::CShaderLoader::GetPackagePath() {
  std::wstring packagePath;
  WCHAR exePath[MAX_PATH];
  GetModuleFileNameW(nullptr, exePath, MAX_PATH);
  packagePath = exePath;
  packagePath = packagePath.substr(0, packagePath.find_last_of('\\') + 1);
  return packagePath;
}

std::vector<uint8_t> helpers::CShaderLoader::LoadPackageFile(const std::wstring & _path) {
  std::vector<uint8_t> fileData;

  auto file = CreateFileW(_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

  BOOL res = false;

  if (file != INVALID_HANDLE_VALUE) {
    DWORD readed;
    LARGE_INTEGER pos, newPos, fileSize;

    pos.QuadPart = 0;
    newPos.QuadPart = 0;

    SetFilePointerEx(file, pos, &newPos, FILE_END);
    fileSize = newPos;
    SetFilePointerEx(file, pos, &newPos, FILE_BEGIN);

    fileData.resize(static_cast<size_t>(fileSize.QuadPart));

    res = ReadFile(file, fileData.data(), (DWORD)fileData.size(), &readed, nullptr);

    CloseHandle(file);
  }

  if (res == FALSE)
    return std::vector<uint8_t>();
  else
    return fileData;
}




helpers::FaceVertex::FaceVertex() {
}

helpers::FaceVertex::FaceVertex(const uint32_t _vIdx, const uint32_t _tIdx, const uint32_t _nIdx) 
  : mVertexIdx(_vIdx), mTextIdx(_tIdx), mNormalIdx(_nIdx) {
}

helpers::Face::Face(const FaceVertex _v1, const FaceVertex _v2, const FaceVertex _v3) {
  face.resize(3);
  face[0] = _v1;
  face[1] = _v2;
  face[2] = _v3;
}

helpers::CSimpleObjLoader::CSimpleObjLoader() {
}

helpers::CSimpleObjLoader::~CSimpleObjLoader() {
}

std::vector<DirectX::XMFLOAT3> * helpers::CSimpleObjLoader::GetVBuff() {
  return &mVBuff;
}

std::vector<DirectX::XMFLOAT2> * helpers::CSimpleObjLoader::GetVtBuff() {
  return &mVtBuff;
}

std::vector<DirectX::XMFLOAT3>* helpers::CSimpleObjLoader::GetVnBuff() {
  return &mVnBuff;
}

std::vector<helpers::Face> * helpers::CSimpleObjLoader::GetFaces() {
  return &mFaces;
}

void helpers::CSimpleObjLoader::LoadObj(
  const wchar_t * _path
)
{
  std::ifstream objFile(_path);

  std::string tmpBuffer;
  if (objFile.is_open())   {
    while (std::getline(objFile, tmpBuffer)) {
      if (tmpBuffer.substr(0, 2) == "v ")  {
        std::stringstream s(tmpBuffer.substr(2));
        DirectX::XMFLOAT3 tmp;
        s >> tmp.x;
        s >> tmp.y;
        s >> tmp.z;

        mVBuff.push_back(tmp);
      }

      if (tmpBuffer.substr(0, 3) == "vt ") {
        std::stringstream s(tmpBuffer.substr(3));
        DirectX::XMFLOAT2 tmp;
        s >> tmp.x;
        s >> tmp.y;

        mVtBuff.push_back(tmp);
      }

      if (tmpBuffer.substr(0, 3) == "vn ") {
        std::stringstream s(tmpBuffer.substr(3));
        DirectX::XMFLOAT3 tmp;
        s >> tmp.x;
        s >> tmp.y;
        s >> tmp.z;

        mVnBuff.push_back(tmp);
      }

      if (tmpBuffer.substr(0, 2) == "f ") {
        int v = 0, t = 0, n = 0;

        auto sub = tmpBuffer.substr(2);
        auto res = sub.find(L'/');
        v = atoi(sub.substr(0, res).data());
        sub.erase(0, res + 1);

        res = sub.find(L'/');
        t = atoi(sub.substr(0, res).data());
        sub.erase(0, res + 1);

        res = sub.find(L' ');
        n = atoi(sub.substr(0, res).data());
        sub.erase(0, res + 1);

        FaceVertex v1(v - 1, t - 1, n - 1);


        res = sub.find(L'/');
        v = atoi(sub.substr(0, res).data());
        sub.erase(0, res + 1);

        res = sub.find(L'/');
        t = atoi(sub.substr(0, res).data());
        sub.erase(0, res + 1);

        res = sub.find(L' ');
        n = atoi(sub.substr(0, res).data());
        sub.erase(0, res + 1);

        FaceVertex v2(v - 1, t - 1, n - 1);


        res = sub.find(L'/');
        v = atoi(sub.substr(0, res).data());
        sub.erase(0, res + 1);

        res = sub.find(L'/');
        t = atoi(sub.substr(0, res).data());
        sub.erase(0, res + 1);

        res = sub.find(L'/');
        n = atoi(sub.substr(0, res).data());

        FaceVertex v3(v - 1, t - 1, n - 1);
        mFaces.push_back(Face(v1, v2, v3));
      }
    }
  }

  std::vector<DirectX::XMFLOAT3> mVBuff2;
  std::vector<DirectX::XMFLOAT2> mVtBuff2;
  std::vector<DirectX::XMFLOAT3> mVnBuff2;

  for (auto &i : mFaces) {
      for (auto &j : i.face) {
          mVBuff2.push_back(mVBuff[j.mVertexIdx]);

          if (j.mTextIdx != -1) {
              mVtBuff2.push_back(mVtBuff[j.mTextIdx]);
          }
          else {
              mVtBuff2.push_back(DirectX::XMFLOAT2(0, 0));
          }
          
          mVnBuff2.push_back(mVnBuff[j.mNormalIdx]);
      }
  }

  mVBuff = mVBuff2;
  mVtBuff = mVtBuff2;
  mVnBuff = mVnBuff2;
}
