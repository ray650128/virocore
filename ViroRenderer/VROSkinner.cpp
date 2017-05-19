//
//  VROSkinner.cpp
//  ViroRenderer
//
//  Created by Raj Advani on 5/9/17.
//  Copyright © 2017 Viro Media. All rights reserved.
//

#include "VROSkinner.h"
#include "VROSkeleton.h"
#include "VROBone.h"
#include "VROMatrix4f.h"
#include "VROMath.h"

VROSkinner::VROSkinner(std::shared_ptr<VROSkeleton> skeleton,
                       VROMatrix4f geometryBindTransform,
                       std::vector<VROMatrix4f> bindTransforms,
                       std::shared_ptr<VROGeometrySource> boneIndices,
                       std::shared_ptr<VROGeometrySource> boneWeights) :
    _skeleton(skeleton),
    _boneIndices(boneIndices),
    _boneWeights(boneWeights) {
    
    VROMatrix4f inverseGeometryBindTransform = geometryBindTransform.invert();
        
    for (VROMatrix4f &transform : bindTransforms) {
        VROMatrix4f boneBindTransform = transform.multiply(geometryBindTransform);
        _bindTransforms.push_back(boneBindTransform);
        _inverseBindTransforms.push_back(boneBindTransform.invert());
    }
}

VROMatrix4f VROSkinner::getModelTransform(int boneIndex) {
    /*
     This carries out the series of transforms described in the top-notes
     of the header:
     
     Model space, original position --> [bindTransform]                 --> Bone space, bind position
     Bone space, bind position      --> [boneTransform]                 --> Bone space, animated position
     Bone space, animated position  --> [inverseBindTransform]          --> Model space, animated position
     */
    const std::shared_ptr<VROBone> &bone = _skeleton->getBone(boneIndex);
    return _inverseBindTransforms[boneIndex].multiply(bone->getTransform()).multiply(_bindTransforms[boneIndex]);
}
