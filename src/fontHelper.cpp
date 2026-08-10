/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#include "../include/HammerEngine/HammerEngine.h"

Char HammerFont::charCoor(int index, int totalChars, float xOffset, float yOffset, float charWidth, float charHeight, const char name) {
    float textureWidth = 2048.0f;
    
    float slotWidth = floor(textureWidth / (float)totalChars); 
    
    float pixelLeft = (float)index * slotWidth;
    float pixelRight = pixelLeft + slotWidth;

    // Add a 0.5-pixel padding to stop Linear Filtering from doint texture bleeding
    float uvPadding = 0.5f; 
    
    float uMin = (pixelLeft + uvPadding) / textureWidth;
    float uMax = (pixelRight - uvPadding) / textureWidth;
    float vMin = 0.0f;
    float vMax = 1.0f;

    float xMin = xOffset;
    float xMax = xOffset + charWidth;
    float yMin = yOffset;
    float yMax = yOffset + charHeight;

    std::vector<Vertex> vertices = {
        {{xMin, yMin, 0.0f}, {1.0f, 1.0f, 1.0f}, {uMin, vMax}, {0.0f, 0.0f, 0.0f}},
        {{xMax, yMin, 0.0f}, {1.0f, 1.0f, 1.0f}, {uMax, vMax}, {0.0f, 0.0f, 0.0f}},
        {{xMax, yMax, 0.0f}, {1.0f, 1.0f, 1.0f}, {uMax, vMin}, {0.0f, 0.0f, 0.0f}},
        {{xMin, yMax, 0.0f}, {1.0f, 1.0f, 1.0f}, {uMin, vMin}, {0.0f, 0.0f, 0.0f}}
    };

    std::vector<uint32_t> indices = {
        0, 1, 2, 
        2, 3, 0
    };

    Char currentChar;
    currentChar.vertices = vertices;
    currentChar.indices = indices;
    currentChar.charName = name;

    return currentChar;
}

void HammerFont::convertTextToVertexAndIndex(std::vector<Vertex>* verticesToAdd, std::vector<uint32_t>* indicesToAdd, std::string textToConvertToVertex){
    float charXOffset = 0.0f;
    float charYOffset = 0.0f;

    for(char currentLookingChar : textToConvertToVertex){
        if(currentLookingChar == '\n'){
            charYOffset -= 3.5f;
            charXOffset = 0.0f;
            continue;
        }
        for(int i = 0; i < 95; i++){
            if(allCharVectors[i].charName == currentLookingChar) {
                if(currentLookingChar == ' '){
                    charXOffset += 1.0f;
                } else {

                    uint32_t bVertexOffset = static_cast<uint32_t>(verticesToAdd->size());

                    Char myCharToAdd;
                    myCharToAdd.vertices = allCharVectors[i].vertices;
                    myCharToAdd.indices = allCharVectors[i].indices;

                    myCharToAdd.vertices[0].pos += glm::vec3(charXOffset, charYOffset, 0); 
                    myCharToAdd.vertices[1].pos += glm::vec3(charXOffset, charYOffset, 0); 
                    myCharToAdd.vertices[2].pos += glm::vec3(charXOffset, charYOffset, 0); 
                    myCharToAdd.vertices[3].pos += glm::vec3(charXOffset, charYOffset, 0);
                    charXOffset += 1.0f;

                    verticesToAdd->insert(verticesToAdd->end(), myCharToAdd.vertices.begin(), myCharToAdd.vertices.end());

                    for (uint32_t index : allCharVectors[i].indices) {
                        indicesToAdd->push_back(index + bVertexOffset);
                    }
                }
                break;
            }
        }
    }
}
