#pragma once

#include <functional>
#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <vector>

using namespace std;


class FolderWatcher {
public:
    struct FileChangeInfo {
        enum ChangeType { ADDED, DELETED, MODIFIED };
        std::string fileName;
        ChangeType changeType;
    };

    using Observer = std::function<void(const FileChangeInfo&)>;

    void subscribe(const Observer& observer);
    void startWatching(const std::string& folderPath);
    void notifyObservers(const FileChangeInfo& changeInfo);
    
    void setFolderPath(const string& path) {
        folderPath = path;
    }

    const string& getFolderPath() const {
        return folderPath;
    }

private:
    std::vector<Observer> observers;
    FileChangeInfo lastChange;
    string folderPath;

};

class Observer {
public:
    void update();
};
