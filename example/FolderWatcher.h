#include <string>
#include <vector>
#include <thread>
#include <functional>

class FolderWatcher {
public:
    FolderWatcher(const std::string& folderPath);
    ~FolderWatcher();

    void addObserver(std::function<void(const std::string&)> callback);
    void startWatching();
    void stopWatching();

private:
    std::string folderPath;
    std::vector<std::function<void(const std::string&)>> observers;
    std::thread watcherThread;
    bool watching;
    
    void watchFolder();
};
