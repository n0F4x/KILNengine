module Material;

// size: 128
public struct Material {
// size: 16
    float4 pbrMetallicRoughnessBaseColorFactor;

// size: 4
    uint pbrMetallicRoughnessBaseColorTextureIndex;
// size: 4
    uint pbrMetallicRoughnessBaseColorTextureTexCoord;
// size: 4
    float pbrMetallicRoughnessMetallicFactor;
// size: 4
    float pbrMetallicRoughnessRoughnessFactor;

// size: 4
    uint pbrMetallicRoughnessMetallicRoughnessTextureIndex;
// size: 4
    uint pbrMetallicRoughnessMetallicRoughnessTextureTexCoord;
// size: 4
    uint normalTextureIndex;
// size: 4
    uint normalTextureTexCoord;

// size: 4
    float normalTextureScale;
// size: 4
    uint occlusionTextureIndex;
// size: 4
    uint occlusionTextureTexCoord;
// size: 4
    float occlusionTextureScale;

// size: 4
    uint emissiveTextureIndex;
// size: 4
    uint emissiveTextureTexCoord;
// size: 8
    float2 _padding0;

// size: 12
    float3 emissiveFactor;
// size: 4
    float alphaCutoff;

// size: 16
    float4 _padding1;

// size: 16
    float4 _padding2;
};
