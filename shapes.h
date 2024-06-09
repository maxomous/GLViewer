#pragma once

#include <string>
#include <functional>
#include <MaxLib.h>
#include "glcore/glcore.h"


namespace GLViewer {

// A generic shape container conatining a list of vertices
class Shape
{
public:     
    
    Shape(uint glPrimitive);
    Shape(uint glPrimitive, std::function<void(Shape& shape)> cb_AddVertices);

    void AddVertex(const glm::vec3& vertex);
    void AddShape(const Shape& shape);
    // returns a new transformed Shape of Shape 
    // translate, scale & rotate about x axis and then z axis
    Shape Transform(const glm::vec3& translate = { 0.0f, 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f }, const glm::vec2& rotate = { 0.0f, 0.0f }) const;
    // returns vertex by index (e.g. shape[2])
    const glm::vec3& operator [](size_t i) const;
    // returns number of vertices
    size_t Size() const;
    
    // Define begin() and end() for range-based for loops
    std::vector<glm::vec3>::iterator begin();
    std::vector<glm::vec3>::iterator end();
    
    uint primitive;
private:
    //void AddIndices(uint indices);
    //void AddNormal(const glm::vec3& normal);
    //void AddTextureCoord(const glm::vec2& textureCoord);
    
    std::vector<glm::vec3> m_Vertices;
    //std::vector<uint> m_Indices;
    //std::vector<glm::vec3> m_Normals;
    //std::vector<glm::vec2> m_TextureCoords;
};


// A list of 3d shape primitives with size 1, centred about the origin
class Shapes
{
public:
    static const Shape& Wireframe_Circle()     { return wireframe_Circle; }
    static const Shape& Wireframe_Square()     { return wireframe_Square; }
    static const Shape& Wireframe_Cylinder()   { return wireframe_Cylinder; }
    static const Shape& Wireframe_Cube()       { return wireframe_Cube; }
    static const Shape& Face_Circle()          { return face_Circle; }
    static const Shape& Face_Square()          { return face_Square; }
    static const Shape& Body_Cylinder()        { return body_Cylinder; }
    static const Shape& Body_Cube()            { return body_Cube; }
    static const Shape& Body_Sphere()          { return body_Sphere; }
    
private:
    inline static const float DEFAULT_ANGLE_INCREMENT = 10.0f;
    
    static Shape wireframe_Circle;
    static Shape wireframe_Square;
    static Shape wireframe_Cylinder;
    static Shape wireframe_Cube;
    static Shape face_Circle;
    static Shape face_Square;
    static Shape body_Cylinder;
    static Shape body_Cube;
    static Shape body_Sphere;

};

} // End namespace GLViewer
