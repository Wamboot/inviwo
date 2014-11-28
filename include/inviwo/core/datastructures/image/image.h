/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2012-2014 Inviwo Foundation
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
 * Contact: Erik Sund�n
 *
 *********************************************************************************/

#ifndef IVW_IMAGE_H
#define IVW_IMAGE_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/datastructures/datagroup.h>
#include <inviwo/core/datastructures/image/layer.h>
#include <inviwo/core/datastructures/image/imagetypes.h>

namespace inviwo {

class ImageRepresentation;
class ImageInport;

class IVW_CORE_API Image : public DataGroup {
public:
    Image(uvec2 dimensions = uvec2(32, 32), ImageType type = COLOR_DEPTH,
          const DataFormatBase* format = DataVec4UINT8::get(), bool allowMissingLayers = false);
    Image(Layer* colorLayer, ImageType type = COLOR_DEPTH, bool allowMissingLayers = false);
    Image(const Image&);
    Image& operator=(const Image& that);
    virtual Image* clone() const;
    virtual ~Image();
    virtual std::string getDataInfo() const;

    void deinitialize();
    void initialize(uvec2 dimensions, const DataFormatBase*, Layer* colorLayer = NULL);

    uvec2 getDimension() const;

    size_t addColorLayer(Layer*);

    const Layer* getLayer(LayerType, size_t idx = 0) const;
    Layer* getLayer(LayerType, size_t idx = 0);

    const Layer* getColorLayer(size_t idx = 0) const;
    Layer* getColorLayer(size_t idx = 0);

    size_t getNumberOfColorLayers() const;

    const Layer* getDepthLayer() const;
    Layer* getDepthLayer();

    const Layer* getPickingLayer() const;
    Layer* getPickingLayer();

    void resize(uvec2 dimensions);
    void resizeRepresentations(Image* targetImage, uvec2 targetDim);

    ImageType getImageType() const;
    const DataFormatBase* getDataFormat() const;

protected:
    std::vector<Layer*> colorLayers_;   //< owning pointer
    Layer* depthLayer_;                 //< owning pointer      
    Layer* pickingLayer_;               //< owning pointer

private:
    bool allowMissingLayers_;
    ImageType imageType_;
};

} // namespace

#endif // IVW_IMAGE_H
