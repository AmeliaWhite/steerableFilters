#ifndef BACKEND_H
#define BACKEND_H

#include <string>

#include "Image.h"

class Backend
{
public:
    Backend() {}
    virtual ~Backend() {}

    virtual bool LoadImage(Image& image, const std::string& name) = 0;

    virtual bool LoadInterImage(Image& image, const std::string& name, const std::string& type = "") = 0;
    virtual bool LoadInterDImage(DImage& data, const std::string& name, const std::string& type = "") = 0;
    virtual bool SaveInterImage(Image& image, const std::string& name, const std::string& type = "") = 0;
    virtual bool SaveInterDImage(DImage& data, const std::string& name, const std::string& type = "") = 0;

    virtual bool SaveCounts(const std::string& name, const std::string& type = "") = 0;
};

#endif // BACKEND_H
