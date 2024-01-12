#include "FolderWatcher.h"
#include <filesystem>
#include <unordered_set>
#include <chrono>

namespace fs = std::filesystem;

FolderWatcher::FolderWatcher(const std::string& path) : folderPath(path), watching(false) {}

FolderWatcher::~FolderWatcher() {
    stopWatching();
}

void FolderWatcher::addObserver(std::function<void(const std::string&)> callback) {
    observers.push_back(callback);
}

void FolderWatcher::startWatching() {
    watching = true;
    watcherThread = std::thread(&FolderWatcher::watchFolder, this);
}

void FolderWatcher::stopWatching() {
    if (watching) {
        watching = false;
        watcherThread.join();
    }
}

void FolderWatcher::watchFolder() {}
