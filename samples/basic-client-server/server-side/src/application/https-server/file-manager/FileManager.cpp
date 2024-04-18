
#include "FileManager.hpp"

#include "gzip/compress.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <cstring>
#include <ctime>

namespace fs = std::filesystem;

FileManager::FileManager(const std::string& basePath) : _basePath(basePath) {
  _buildTypesMap();
  _buildFileCache();
}

std::optional<std::reference_wrapper<const FileCacheEntry>>
FileManager::getFile(const std::string& filename) const {
  auto it = _fileCache.find(filename);
  if (it != _fileCache.end()) {
    return *(it->second.get());
  }
  return std::nullopt;
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

  // this allows the browser "streaming compilation", if supported
  _typesMap[".wasm"] = "application/wasm";
}

void
FileManager::_buildFileCache() {
  const fs::path baseDir{_basePath};
  for (auto const& dir_entry : fs::recursive_directory_iterator{baseDir}) {
    const std::string filePath = dir_entry.path();

    const std::string requestPath = filePath.substr(_basePath.size());
    std::shared_ptr<FileCacheEntry> fileCachePtr = _loadFile(filePath, requestPath);

    if (!fileCachePtr) {
      continue;
    }

#if 1
    std::cout << "CACHING FILE" << std::endl;
    std::cout << " => disk path     : " << filePath << std::endl;
    std::cout << " => web path      : " << requestPath << std::endl;
    std::cout << " => type          : " << fileCachePtr->type << std::endl;
    std::cout << " => compression   : " << std::fixed << std::setprecision(1) << fileCachePtr->compressionRatio << "x"
              << std::endl;
    std::cout << " => last modified : " << fileCachePtr->lastModified << std::endl;
#endif

    _fileCache[requestPath] = fileCachePtr;

    //
    // handle the "index.html" case

    const std::size_t pathIndex = requestPath.find_last_of("/");
    if (pathIndex == std::string::npos) {
      continue;
    }

    const std::string filename = requestPath.substr(pathIndex);
    if (filename != "/index.html") {
      continue;
    }

    const std::string subKey = requestPath.substr(0, pathIndex + 1);

    std::cout << " => file was indexed: " << filePath << " -> " << subKey << std::endl;

    _fileCache[subKey] = fileCachePtr;
  }
}

std::shared_ptr<FileCacheEntry>
FileManager::_loadFile(const std::string& filename, const std::string& requestPath) {

  // filename exist and is file
  if (fs::is_regular_file(filename) == false) {
    return nullptr;
  }

  // file can be opened (file permission failure can still happen)
  std::ifstream iStr(filename);
  if (iStr.fail()) {
    return nullptr;
  }

  // get file content

  std::stringstream sstr;
  sstr << iStr.rdbuf();
  const std::string fileContent = sstr.str();

  // make new file cache entry

  auto newCache = std::make_shared<FileCacheEntry>();

  // set content type

  const std::string extension = filename.substr(filename.find_last_of("."));
  auto it = _typesMap.find(extension);
  if (it != _typesMap.end()) {
    newCache->type = it->second;
  } else {
    newCache->type = "text/plain";
  }

  // file content (attempting gzip)

  const std::string compressedFileContent = gzip::compress(fileContent.c_str(), fileContent.size(), Z_BEST_COMPRESSION);

  if (compressedFileContent.size() < fileContent.size()) {
    // compressed file content is smaller: keep
    newCache->compressionRatio = float(fileContent.size()) / float(compressedFileContent.size());
    newCache->compressedFileContent = std::move(compressedFileContent);
  } else {
    // compressed file content is larger: ignore
    newCache->compressionRatio = 0.0f;
  }
  newCache->fileContent = std::move(fileContent);

  // last-modified

  _getLastModifiedTime(filename, "%a, %d %b %Y %X %Z", newCache->lastModified);

  // compute the logContent
  {
    std::stringstream subSstr;
    subSstr << " [size=" << std::setw(8) << fileContent.size() << "b,";
    if (newCache->compressionRatio > 0.0f) {
      subSstr << " sent=" << std::setw(8) << newCache->compressedFileContent.size() << "b,";
    } else {
      subSstr << " sent=" << std::setw(8) << fileContent.size() << "b,";
    }
    subSstr << " compression=" << std::setw(3) << std::fixed << std::setprecision(1) << newCache->compressionRatio
            << "x]";
    subSstr << " " << newCache->lastModified;
    subSstr << " " << std::setw(20) << newCache->type << " " << requestPath;

    newCache->logContent = subSstr.str();
  }

  return newCache;
}

void
FileManager::_getLastModifiedTime(
  const std::string_view& filename, const std::string_view& format, std::string& outLastModified) {
  fs::file_time_type file_time = fs::last_write_time(filename);

  auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
    file_time - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
  std::time_t tt = std::chrono::system_clock::to_time_t(sctp);

  std::tm now_tm = *std::localtime(&tt);

  constexpr std::size_t k_bufferMaxSize = 128;
  char bufferData[k_bufferMaxSize];
  const size_t bufferSize = std::strftime(bufferData, k_bufferMaxSize, format.data(), &now_tm);
  bufferData[bufferSize] = '\0';

  outLastModified = std::string(bufferData, bufferSize);
}
