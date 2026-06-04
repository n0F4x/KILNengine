module;

#include "modules/Camera.slang"
#include "modules/DrawCommand.slang"
#include "modules/DrawCommandGenerationPushConstants.slang"
#include "modules/Frustum.slang"
#include "modules/FrustumCullingPushConstants.slang"
#include "modules/Index.slang"
#include "modules/InstanceIndexGenerationPushConstants.slang"
#include "modules/Material.slang"
#include "modules/Plane.slang"
#include "modules/Primitive.slang"
#include "modules/SBV.slang"
#include "modules/Scene.slang"

export module examples.simple_scene.shaders;

namespace demo::shaders {

export using ::Camera;
export using ::DrawCommand;
export using ::DrawCommandGenerationPushConstants;
export using ::Frustum;
export using ::FrustumCullingPushConstants;
export using ::Index;
export using ::InstanceIndexGenerationPushConstants;
export using ::Material;
export using ::Plane;
export using ::Primitive;
export using ::SBV;
export using ::Scene;

}   // namespace demo::shaders
