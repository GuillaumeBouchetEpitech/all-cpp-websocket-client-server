
#pragma once

#include <memory>
#include <unordered_map>
#include <optional>

struct FileCacheEntry {
  std::string fileContent;
  std::string compressedFileContent;
  std::string type;
  float compressionRatio = 0.0f;
  std::string lastModified;
  std::string logContent;
};

class FileManager {
private:
  std::string _basePath;

  std::unordered_map<std::string, std::string> _typesMap;

  std::unordered_map<std::string, std::shared_ptr<FileCacheEntry>> _fileCache;

public:
  FileManager(const std::string& basePath);
  ~FileManager() = default;

public:
  std::optional<std::reference_wrapper<const FileCacheEntry>> getFile(const std::string& filename) const;

private:
  void _buildTypesMap();
  void _buildFileCache();
  std::shared_ptr<FileCacheEntry> _loadFile(const std::string& filename, const std::string& cacheKey);

private:
  static void _getLastModifiedTime(
    const std::string_view& filename,
    const std::string_view& format,
    std::string& outLastModified
  );
};
