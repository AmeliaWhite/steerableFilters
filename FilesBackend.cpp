#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "FilesBackend.h"

#include <iostream>
#include <fstream>

FilesBackend::FilesBackend() : directory(".")
{
}

FilesBackend::FilesBackend(const std::string& dir) : directory(dir)
{
}

FilesBackend::~FilesBackend()
{
}

bool FilesBackend::LoadImage(Image& image, const std::string& name)
{
    image.load(name.c_str());
}

std::string FilesBackend::construct_path(const std::string& name, const std::string& type, const std::string& ext)
{
    size_t pind = name.find_last_of('.');
    std::string namenoext = (pind == std::string::npos) ? name : name.substr(0, pind);

    // convert slashes to dashes
    // XXX: not portable, if this were ever to port to windows it would be the wrong character
    for (std::string::iterator it = namenoext.begin(); it != namenoext.end(); ++it) {
        if (*it == '/')
            *it = '-';
    }

    return directory + "/" + namenoext + type + "." + ext;
}

bool FilesBackend::file_exists(const std::string& filename)
{
}

bool FilesBackend::LoadInterImage(Image& image, const std::string& name, const std::string& type)
{
    std::string filename = construct_path(name, type, "bmp");
    if (file_exists(filename)) {
        image.load(filename.c_str());
        return true;
    }

    return false;
}

bool FilesBackend::LoadInterDImage(DImage& data, const std::string& name, const std::string& type)
{
    std::string filename = construct_path(name, type, "bin");

    try {
        std::ifstream data_file(filename.c_str(), std::ios::binary);
        size_t dimx, dimy;

        data_file.read((char *) &dimx, sizeof(dimx));
        data_file.read((char *) &dimy, sizeof(dimy));

        data.assign(dimx, dimy);
        data_file.read((char *) data.ptr(), data.size() * sizeof(double));
    } catch(...) {
        return false;
    }

    return true;
}

bool FilesBackend::SaveInterImage(Image& image, const std::string& name, const std::string& type)
{
    std::string filename = construct_path(name, type, "bmp");
    image.save(filename.c_str());
    return true;
}

bool FilesBackend::SaveInterDImage(DImage& data, const std::string& name, const std::string& type)
{
    std::string filename = construct_path(name, type, "bin");

    try {
        std::ofstream data_file(filename.c_str(), std::ios::binary);
        size_t dimx = data.dimx(), dimy = data.dimy();

        data_file.write((char *) &dimx, sizeof(dimx));
        data_file.write((char *) &dimy, sizeof(dimy));

        data_file.write((char *) data.ptr(), data.size() * sizeof(double));
    } catch(...) {
        return false;
    }

    return true;
}

bool FilesBackend::SaveCounts(const std::string& name, const std::string& type)
{
}
