#ifndef FILESBACKEND_H
#define FILESBACKEND_H

#include "Backend.h"

class FilesBackend : public Backend
{
public:
    FilesBackend();
    FilesBackend(const std::string& dir);
    virtual ~FilesBackend();

    virtual bool LoadImage(Image& image, const std::string& name);

    virtual bool LoadInterImage(Image& image, const std::string& name, const std::string& type = "");
    virtual bool LoadInterDImage(DImage& data, const std::string& name, const std::string& type = "");
    virtual bool SaveInterImage(Image& image, const std::string& name, const std::string& type = "");
    virtual bool SaveInterDImage(DImage& data, const std::string& name, const std::string& type = "");

    virtual bool SaveCounts(const std::string& name, const std::string& type = "");

private:
    std::string construct_path(const std::string& name, const std::string& type, const std::string& ext);
    bool file_exists(const std::string& filename);

private:
    std::string directory;
};

#endif // FILESBACKEND_H
