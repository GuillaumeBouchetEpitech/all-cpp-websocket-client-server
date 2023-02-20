
#include "FileManager.hpp"

#include "gzip/compress.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <cstring>
#include <ctime>

namespace {

template <typename TP>
std::time_t
to_time_t(TP tp) {
  using namespace std::chrono;
  auto sctp = time_point_cast<system_clock::duration>(
    tp - TP::clock::now() + system_clock::now());
  return system_clock::to_time_t(sctp);
}

} // namespace

namespace fs = std::filesystem;

FileManager::FileManager(const std::string& basePath) : _basePath(basePath) {
  _buildTypesMap();
  _buildFileCache();
}

const FileCacheResult*
FileManager::getFile(const std::string& filename) {
  auto it = _fileCache.find(filename);
  if (it != _fileCache.end()) {

    // std::cout << filename << " -> " << it->second->fileExist << std::endl;

    if (it->second->fileExist)
      return it->second.get();

    // std::cout << filename << " -> ??? 1" << std::endl;

    return nullptr;
  }

  // std::cout << filename << " -> ??? 2" << std::endl;

  return nullptr;
}

void
FileManager::_buildTypesMap() {
  _typesMap[".ico"] = "image/x-icon";
  _typesMap[".html"] = "text/html";
  _typesMap[".js"] = "text/javascript";
  _typesMap[".json"] = "application/json";
  _typesMap[".css"] = "text/css";
  _typesMap[".png"] = "image/png";
  _typesMap[".jpg"] = "image/jpeg";
  _typesMap[".wav"] = "audio/wav";
  _typesMap[".mp3"] = "audio/mpeg";
  _typesMap[".wav"] = "audio/wav";
  _typesMap[".svg"] = "image/svg+xml";
  _typesMap[".pdf"] = "application/pdf";
  _typesMap[".doc"] = "application/msword";
  _typesMap[".wasm"] = "application/wasm";
}

void
FileManager::_buildFileCache() {
  const fs::path baseDir{_basePath};
  for (auto const& dir_entry : fs::recursive_directory_iterator{baseDir}) {
    const std::string currPath = dir_entry.path();

    std::shared_ptr<FileCacheResult> fileCachePtr = _loadFile(currPath);

    if (fileCachePtr && fileCachePtr->fileExist) {
      const std::string key = currPath.substr(_basePath.size());

      std::cout << "CACHING FILE" << std::endl;
      std::cout << " => disk path     : " << currPath << std::endl;
      std::cout << " => web path      : " << key << std::endl;
      std::cout << " => type          : " << fileCachePtr->type << std::endl;
      std::cout << " => compression   : " << fileCachePtr->compressionRatio
                << std::endl;
      std::cout << " => last modified : " << fileCachePtr->lastModified
                << std::endl;

      _fileCache[key] = fileCachePtr;

      // handle the "index.html" case
      const std::size_t pathIndex = key.find_last_of("/");
      if (pathIndex != std::string::npos) {
        const std::string filename = key.substr(pathIndex);
        if (filename == "/index.html") {
          const std::string subKey = key.substr(0, pathIndex + 1);

          std::cout << " => file was indexed: " << currPath << " -> " << subKey
                    << std::endl;

          _fileCache[subKey] = fileCachePtr;
        }
      }
    }
  }
}

std::shared_ptr<FileCacheResult>
FileManager::_loadFile(const std::string& filename) {
  if (fs::is_regular_file(filename) == false) {
    return nullptr;
  }

  std::shared_ptr<FileCacheResult> newCache =
    std::make_shared<FileCacheResult>();

  // file exist

  std::ifstream iStr(filename);
  if (iStr.fail()) {
    newCache->fileExist = false;
    return nullptr;
  }

  newCache->fileExist = true;

  // content type

  const std::string extension = filename.substr(filename.find_last_of("."));
  auto it = _typesMap.find(extension);
  if (it != _typesMap.end()) {
    newCache->type = it->second;
  } else {
    newCache->type = "text/plain";
  }

  // file content

  std::stringstream sstr;
  sstr << iStr.rdbuf();
  const std::string fileContent = sstr.str();

  // file content (gzip)

  const std::string compressedContent =
    gzip::compress(fileContent.c_str(), fileContent.size(), Z_BEST_COMPRESSION);

  if (compressedContent.size() < fileContent.size()) {
    newCache->compressionRatio =
      float(fileContent.size()) / float(compressedContent.size());
    newCache->compressed = std::move(compressedContent);
  } else {
    newCache->compressionRatio = 0.0f;
  }
  newCache->fileContent = std::move(fileContent);

  // last-modified

  fs::file_time_type file_time = fs::last_write_time(filename);
  std::time_t tt = to_time_t(file_time);
  std::tm now_tm = *std::localtime(&tt);

  constexpr std::size_t bufferSize = 128;
  char bufferData[bufferSize];
  // format: Sun, 13 Nov 2022 11:48:27 GMT
  const size_t total =
    std::strftime(bufferData, bufferSize, "%a, %d %b %Y %X %Z", &now_tm);
  bufferData[total] = '\0';

  newCache->lastModified = std::string(bufferData, total);

  return newCache;
}
