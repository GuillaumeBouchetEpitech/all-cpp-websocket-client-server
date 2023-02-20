
#pragma once

#include <memory>
#include <unordered_map>

struct FileCacheResult {
  bool fileExist = false;
  std::string fileContent;
  std::string compressed;
  std::string type;
  float compressionRatio = 0.0f;
  std::string lastModified;
};

class FileManager {
private:
  std::string _basePath;

  std::unordered_map<std::string, std::string> _typesMap;

  std::unordered_map<std::string, std::shared_ptr<FileCacheResult>> _fileCache;

public:
  FileManager(const std::string& basePath);
  ~FileManager() = default;

public:
  const FileCacheResult* getFile(const std::string& filename);

private:
  void _buildTypesMap();
  void _buildFileCache();
  std::shared_ptr<FileCacheResult> _loadFile(const std::string& filename);
};
