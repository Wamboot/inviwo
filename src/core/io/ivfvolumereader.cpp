/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2013-2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Contact: Peter Steneteg
 *
 *********************************************************************************/

#include <inviwo/core/io/ivfvolumereader.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <inviwo/core/datastructures/volume/volumetypeclassification.h>
#include "inviwo/core/datastructures/volume/volumedisk.h"
#include "inviwo/core/util/urlparser.h"

namespace inviwo {

IvfVolumeReader::IvfVolumeReader()
    : DataReaderType<Volume>()
    , rawFile_("")
    , filePos_(0)
    , littleEndian_(true)
    , dimension_(uvec3(0))
    , format_(NULL) {
    addExtension(FileExtension("ivf", "Inviwo ivf file format"));
}

IvfVolumeReader::IvfVolumeReader(const IvfVolumeReader& rhs)
    : DataReaderType<Volume>(rhs)
    , rawFile_(rhs.rawFile_)
    , filePos_(0)
    , littleEndian_(rhs.littleEndian_)
    , dimension_(rhs.dimension_)
    , format_(rhs.format_) {
}

IvfVolumeReader& IvfVolumeReader::operator=(const IvfVolumeReader& that) {
    if (this != &that) {
        rawFile_ = that.rawFile_;
        filePos_ = that.filePos_;
        littleEndian_ = that.littleEndian_;
        dimension_ = that.dimension_;
        format_ = that.format_;
        DataReaderType<Volume>::operator=(that);
    }

    return *this;
}

IvfVolumeReader* IvfVolumeReader::clone() const {
    return new IvfVolumeReader(*this);
}

Volume* IvfVolumeReader::readMetaData(std::string filePath)  {
    if (!URLParser::fileExists(filePath)) {
        std::string newPath = URLParser::addBasePath(filePath);

        if (URLParser::fileExists(newPath)) {
            filePath = newPath;
        } else {
            throw DataReaderException("Error could not find input file: " + filePath);
        }
    }

    std::string fileDirectory = URLParser::getFileDirectory(filePath);
    std::string fileExtension = URLParser::getFileExtension(filePath);
    Volume* volume = new UniformRectiLinearVolume();
    IvwDeserializer d(filePath);
    d.deserialize("RawFile", rawFile_);
    rawFile_ = fileDirectory + rawFile_;
    std::string formatFlag("");
    d.deserialize("Format", formatFlag);
    format_ = DataFormatBase::get(formatFlag);
    mat4 basisAndOffset;
    d.deserialize("BasisAndOffset", basisAndOffset);
    volume->setBasisAndOffset(basisAndOffset);
    mat4 worldTransform;
    d.deserialize("WorldTransform", worldTransform);
    volume->setWorldTransform(worldTransform);
    d.deserialize("Dimension", dimension_);
    volume->setDimension(dimension_);

    d.deserialize("DataRange", volume->dataMap_.dataRange);
    d.deserialize("ValueRange", volume->dataMap_.valueRange);
    d.deserialize("Unit", volume->dataMap_.valueUnit);

    volume->getMetaDataMap()->deserialize(d);
    littleEndian_ = volume->getMetaData<BoolMetaData>("LittleEndian", littleEndian_);
    VolumeDisk* vd = new VolumeDisk(filePath, dimension_, format_);
    vd->setDataReader(this);
    volume->addRepresentation(vd);
    return volume;
}

void IvfVolumeReader::readDataInto(void* destination) const {
    std::fstream fin(rawFile_.c_str(), std::ios::in | std::ios::binary);

    if (fin.good()) {
        std::size_t size = dimension_.x*dimension_.y*dimension_.z*(format_->getBytesAllocated());
        fin.seekg(filePos_);
        fin.read((char*)destination, size);

        if (!littleEndian_ && format_->getBytesAllocated() > 1) {
            std::size_t bytes = format_->getBytesAllocated();
            char* temp = new char[bytes];

            for (std::size_t i = 0; i < size; i += bytes) {
                for (std::size_t j = 0; j < bytes; j++)
                    temp[j] = static_cast<char*>(destination)[i + j];

                for (std::size_t j = 0; j < bytes; j++)
                    static_cast<char*>(destination)[i + j] = temp[bytes - j - 1];
            }

            delete temp;
        }
    } else
        throw DataReaderException("Error: Could not read from raw file: " + rawFile_);

    fin.close();
}

void* IvfVolumeReader::readData() const {
    std::size_t size = dimension_.x * dimension_.y * dimension_.z * (format_->getBytesAllocated());
    char* data = new char[size];

    if (data) {
        readDataInto(data);
    } else {
        throw DataReaderException("Error: Could not allocate memory for loading raw file: " +
                                  rawFile_);
    }
    return data;
}

} // namespace
