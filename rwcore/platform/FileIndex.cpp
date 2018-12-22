#include "platform/FileIndex.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <sstream>

#include "platform/FileHandle.hpp"
#include "loaders/LoaderIMG.hpp"

#include "rw/debug.hpp"

std::string FileIndex::normalizeFilePath(const std::string &filePath) {
    std::ostringstream oss;
    std::transform(filePath.cbegin(), filePath.cend(), std::ostreambuf_iterator<char>(oss), [](char c) {
        if (c == '\\') {
            return '/';
        }
        return static_cast<char>(std::tolower(c));
    });
    return oss.str();
}

void FileIndex::indexTree(const rwfs::path &path) {
    // Remove the trailing "/" or "/." from base_path. Boost 1.66 and c++17 have different lexically_relative behavior.
    rwfs::path basePath = (path / ".").lexically_normal();
    basePath = basePath.parent_path();

    for (const rwfs::path &path :
         rwfs::recursive_directory_iterator(basePath)) {
        if (!rwfs::is_regular_file(path)) {
            continue;
        }
        auto relPath = path.lexically_relative(basePath);
        std::string relPathName = normalizeFilePath(relPath.string());
        indexedData_[relPathName] = {IndexedDataType::FILE, path.string(), ""};

        auto filename = normalizeFilePath(path.filename().string());
        indexedData_[filename] = {IndexedDataType::FILE, path.string(), ""};
    }
}

const FileIndex::IndexedData *FileIndex::getIndexedDataAt(const std::string &filePath) const {
    auto normPath = normalizeFilePath(filePath);
    return &indexedData_.at(normPath);
}

rwfs::path FileIndex::findFilePath(const std::string &filePath) const {
    return getIndexedDataAt(filePath)->path;
}

FileContentsInfo FileIndex::openFileRaw(const std::string &filePath) const {
    const auto *indexData = getIndexedDataAt(filePath);
    std::ifstream dfile(indexData->path, std::ios::binary);
    if (!dfile.is_open()) {
        throw std::runtime_error("Unable to open file: " + filePath);
    }

#ifdef RW_DEBUG
    if (indexData->type != IndexedDataType::FILE) {
        RW_MESSAGE("Reading raw data from archive \"" << filePath << "\"");
    }
#endif

    dfile.seekg(0, std::ios::end);
    size_t length;
    if (auto tmp_length = dfile.tellg(); tmp_length < 0) {
        throw std::runtime_error("Unable to get file size: " + filePath);
    } else {
        length = static_cast<size_t>(tmp_length);
    }
    dfile.seekg(0);
    auto data = std::make_unique<char[]>(length);
    dfile.read(data.get(), length);

    return {std::move(data), length};
}

void FileIndex::indexArchive(const std::string &archive) {
    rwfs::path path = findFilePath(archive);

    LoaderIMG img;
    if (!img.load(path.string())) {
        throw std::runtime_error("Failed to load IMG archive: " + path.string());
    }

    for (size_t i = 0; i < img.getAssetCount(); ++i) {
        auto &asset = img.getAssetInfoByIndex(i);

        if (asset.size == 0) continue;

        std::string assetName = normalizeFilePath(asset.name);

        indexedData_[assetName] = {IndexedDataType::ARCHIVE, path.string(), asset.name};
    }
}

FileContentsInfo FileIndex::openFile(const std::string &filePath) {
    auto cleanFilePath = normalizeFilePath(filePath);
    auto indexedDataPos = indexedData_.find(cleanFilePath);

    if (indexedDataPos == indexedData_.end()) {
        return {nullptr, 0};
    }

    const auto &indexedData = indexedDataPos->second;

    std::unique_ptr<char[]> data = nullptr;
    size_t length = 0;

    if (indexedData.type == IndexedDataType::ARCHIVE) {
        LoaderIMG img;

        if (!img.load(indexedData.path)) {
            throw std::runtime_error("Failed to load IMG archive: " + indexedData.path);
        }

        LoaderIMGFile file;
        auto filename = rwfs::path(indexedData.assetData).filename().string();
        if (img.findAssetInfo(filename, file)) {
            length = file.size * 2048;
            data = img.loadToMemory(filename);
        }
    } else {
        std::ifstream dfile(indexedData.path, std::ios::binary);
        if (!dfile.is_open()) {
            throw std::runtime_error("Unable to open file: " + indexedData.path);
        }

        dfile.seekg(0, std::ios::end);
        if (auto tmp_length = dfile.tellg(); tmp_length < 0) {
            throw std::runtime_error("Unable to get file size: " + filePath);
        } else {
            length = static_cast<size_t>(tmp_length);
        }

        dfile.seekg(0);
        data = std::make_unique<char[]>(length);
        dfile.read(data.get(), length);
    }

    return {std::move(data), length};
}
