//
//  VROView.h
//  ViroRenderer
//
//  Created by Raj Advani on 12/15/15.
//  Copyright © 2015 Viro Media. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import "VRORenderDelegate.h"
#import "VROScreenUIView.h"
#import <memory>

@class VROSceneController;
class VROScene;
enum class VROTimingFunctionType;

@interface VROView : MTKView <MTKViewDelegate>

@property (nonatomic, unsafe_unretained) IBOutlet id <VRORenderDelegate> renderDelegate;

@property (nonatomic, readonly) VROScreenUIView *HUD;
@property (readwrite, nonatomic) VROSceneController *sceneController;

- (instancetype)initWithFrame:(CGRect)frame;
- (VRORenderContext *)renderContext;

- (void)setSceneController:(VROSceneController *)sceneController animated:(BOOL)animated;
- (void)setSceneController:(VROSceneController *)sceneController duration:(float)seconds
            timingFunction:(VROTimingFunctionType)timingFunctionType;

- (void)setPosition:(VROVector3f)position;
- (void)setBaseRotation:(VROQuaternion)rotation;
- (float)worldPerScreenAtDepth:(float)distance;

@end
