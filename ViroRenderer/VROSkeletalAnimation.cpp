//
//  VROSkeletalAnimation.cpp
//  ViroRenderer
//
//  Created by Raj Advani on 5/16/17.
//  Copyright © 2017 Viro Media. All rights reserved.
//

#include "VROSkeletalAnimation.h"
#include "VROTransaction.h"
#include "VROLog.h"
#include "VROAnimationMatrix4f.h"
#include "VROSkeleton.h"
#include "VROShaderModifier.h"
#include "VROBone.h"
#include <sstream>
#include <map>

static std::shared_ptr<VROShaderModifier> sSkeletalAnimationShaderModifier;

std::shared_ptr<VROShaderModifier> VROSkeletalAnimation::createSkeletalAnimationShaderModifier() {
    /*
     Modifier that performs skeletal animation in the vertex shader.
     */
    if (!sSkeletalAnimationShaderModifier) {
        std::vector<std::string> modifierCode =  {
            "vec4 pos_h = vec4(_geometry.position, 1.0);",
            "vec4 pos_blended = (bone_matrices[_geometry.bone_indices.x] * pos_h) * _geometry.bone_weights.x + "
                               "(bone_matrices[_geometry.bone_indices.y] * pos_h) * _geometry.bone_weights.y + "
                               "(bone_matrices[_geometry.bone_indices.z] * pos_h) * _geometry.bone_weights.z + "
                               "(bone_matrices[_geometry.bone_indices.w] * pos_h) * _geometry.bone_weights.w;",
            "_geometry.position = pos_blended.xyz;"
        };
        sSkeletalAnimationShaderModifier = std::make_shared<VROShaderModifier>(VROShaderEntryPoint::Geometry,
                                                                               modifierCode);
    }
    
    return sSkeletalAnimationShaderModifier;
}

std::shared_ptr<VROExecutableAnimation> VROSkeletalAnimation::copy() {
    pabort("Skeletal animations may not be copied");
}

void VROSkeletalAnimation::execute(std::shared_ptr<VRONode> node, std::function<void()> onFinished) {
    std::weak_ptr<VROSkeletalAnimation> shared_w = shared_from_this();
    
    /*
     Build the key frame animation data.
     */
    std::map<int, std::vector<float>> boneKeyTimes;
    std::map<int, std::vector<VROMatrix4f>> boneKeyValues;

    for (std::unique_ptr<VROSkeletalAnimationFrame> &frame : _frames) {
        passert (frame->boneIndices.size() == frame->boneTransforms.size());
        
        for (int i = 0; i < frame->boneIndices.size(); i++) {
            int boneIndex = frame->boneIndices[i];
            VROMatrix4f &transform = frame->boneTransforms[i];
            
            boneKeyValues[boneIndex].push_back(transform);
            boneKeyTimes[boneIndex].push_back(frame->time);
        }
    }
    
    VROTransaction::begin();
    VROTransaction::setAnimationDuration(_duration / 1000);
    VROTransaction::setTimingFunction(VROTimingFunctionType::Linear);
    
    for (auto kv : boneKeyTimes) {
        int boneIndex = kv.first;
        std::shared_ptr<VROBone> bone = _skeleton->getBone(boneIndex);
        
        std::vector<float> &keyTimes = kv.second;
        std::vector<VROMatrix4f> &keyValues = boneKeyValues[boneIndex];
        std::shared_ptr<VROAnimation> animation = std::make_shared<VROAnimationMatrix4f>([shared_w, boneIndex](VROAnimatable *const animatable, VROMatrix4f m) {
            std::shared_ptr<VROSkeletalAnimation> shared = shared_w.lock();
            if (!shared) {
                return;
            }
            ((VROBone *)animatable)->setTransform(m);
        }, keyTimes, keyValues);
        
        bone->animate(animation);
    }
    
    _transaction = VROTransaction::commit();
}

void VROSkeletalAnimation::pause() {
    if (_transaction) {
        VROTransaction::pause(_transaction);
    }
}

void VROSkeletalAnimation::resume() {
    if (_transaction) {
        VROTransaction::resume(_transaction);
    }
}

void VROSkeletalAnimation::terminate() {
    if (_transaction) {
        VROTransaction::terminate(_transaction);
        _transaction.reset();
    }
}

std::string VROSkeletalAnimation::toString() const {
    std::stringstream ss;
    ss << "[skeletal: " << _name << "]";
    return ss.str();
}
