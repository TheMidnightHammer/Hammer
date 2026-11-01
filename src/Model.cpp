/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#define TINYOBJLOADER_IMPLEMENTATION
#include "EngineCommon.h"

HammerModel::HammerModel(const std::string& path){
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::string warn_and_err;
    bool ret = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn_and_err,
        path.c_str(),
        nullptr,
        true
    );

    if (!ret) {
        throw std::runtime_error("HammerEngine: Failed to load OBJ: " + warn_and_err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            // Position
            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            // UV Coordinates
            if (index.texcoord_index >= 0) {
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            // Normals
            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            } else {
                // Fallback if the OBJ file lacks normal data
                vertex.normal = {0.0f, 1.0f, 0.0f};
            }

            vertex.color = {1.0f, 1.0f, 1.0f};

            // Deduplication: only add vertex if it's new
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertexData.size());
                vertexData.push_back(vertex);
            }

            indexData.push_back(uniqueVertices[vertex]);
        }
    }
}
