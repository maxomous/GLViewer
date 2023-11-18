#include "shapes.h"



Shape::Shape(uint glPrimitive) 
  : primitive(glPrimitive)
{ 
    cb_AddVertices(*this); 
}

Shape::Shape(uint glPrimitive, std::function<void(Shape& shape)> cb_AddVertices) 
  : primitive(glPrimitive)
{ 
    cb_AddVertices(*this); 
}

void Shape::AddVertex(const glm::vec3& vertex) 
{ 
    m_Vertices.push_back(vertex); 
}
void Shape::AddShape(const Shape& shape) 
{                      
    for(size_t i = 0; i < shape.Size(); i++) {
        AddVertex(shape[i]);
    }
}
// returns a new transformed Shape of Shape 
// translate, scale & rotate about x axis and then z axis
Shape Shape::Transform(const glm::vec3& translate, const glm::vec3& scale, const glm::vec2& rotate) const
{
    Shape newShape(primitive);
    for(const glm::vec3& vertex : m_Vertices) {
        glm::vec3 v = glm::Transform(vertex, translate, scale, rotate);
        newShape.AddVertex(std::move(v));
    }
    return newShape;
}   
// returns vertex by index (e.g. shape[2])
const glm::vec3& Shape::operator[](size_t i) const    
{ 
    assert(i < m_Vertices.size()); return m_Vertices[i]; 
}
// returns number of vertices
size_t Shape::Size() const  { return m_Vertices.size(); } 
std::vector<glm::vec3>::iterator Shape::begin()         { return m_Vertices.begin(); }
std::vector<glm::vec3>::iterator Shape::end()           { return m_Vertices.end(); }


Shape Shapes::wireframe_Circle = Shape(GL_LINES, [&](Shape& shape) 
{
    using namespace MaxLib::Geom;
    
    for (float th = 0.0f; th <= 360.0f; th += DEFAULT_ANGLE_INCREMENT) {
        float th2 = th + DEFAULT_ANGLE_INCREMENT;
        shape.AddVertex({ 0.5f * Cos(th),   -0.5f * Sin(th),    0.0f });
        shape.AddVertex({ 0.5f * Cos(th2),  -0.5f * Sin(th2),   0.0f });
    }
});

Shape Shapes::wireframe_Square = Shape(GL_LINES, [&](Shape& shape) 
{
    shape.AddVertex({ -0.5f, -0.5f, 0.0f });
    shape.AddVertex({  0.5f, -0.5f, 0.0f });
    
    shape.AddVertex({  0.5f, -0.5f, 0.0f });
    shape.AddVertex({  0.5f,  0.5f, 0.0f });
    
    shape.AddVertex({  0.5f,  0.5f, 0.0f });
    shape.AddVertex({ -0.5f,  0.5f, 0.0f });
    
    shape.AddVertex({ -0.5f,  0.5f, 0.0f });
    shape.AddVertex({ -0.5f, -0.5f, 0.0f });
});

Shape Shapes::wireframe_Cylinder = Shape(GL_LINES, [&](Shape& shape) 
{
    // outline bottom
    shape.AddShape(wireframe_Circle.Transform({ 0.0f, 0.0f, -0.5f }));
    // outline top
    shape.AddShape(wireframe_Circle.Transform({ 0.0f, 0.0f, 0.5f }));
});

Shape Shapes::wireframe_Cube = Shape(GL_LINES, [&](Shape& shape) 
{
    // bottom
    shape.AddShape(wireframe_Square.Transform({ 0.0f, 0.0f, -0.5f }));
    // top
    shape.AddShape(wireframe_Square.Transform({ 0.0f, 0.0f, 0.5f }));
    // join between bottom and top
    shape.AddVertex({ -0.5f, -0.5f, -0.5f });
    shape.AddVertex({ -0.5f, -0.5f,  0.5f });
    
    shape.AddVertex({  0.5f, -0.5f, -0.5f });
    shape.AddVertex({  0.5f, -0.5f,  0.5f });
    
    shape.AddVertex({  0.5f,  0.5f, -0.5f });
    shape.AddVertex({  0.5f,  0.5f,  0.5f });
    
    shape.AddVertex({ -0.5f,  0.5f, -0.5f });
    shape.AddVertex({ -0.5f,  0.5f,  0.5f });
});

Shape Shapes::face_Circle = Shape(GL_TRIANGLES, [&](Shape& shape) 
{
    using namespace MaxLib::Geom;
    // circle facing up
    for (float th = 0.0f; th <= 360.0f; th += DEFAULT_ANGLE_INCREMENT) {
        float th2 = th + DEFAULT_ANGLE_INCREMENT;
        shape.AddVertex({ 0.5f * Cos(th),   -0.5f * Sin(th),    0.0f });
        shape.AddVertex({ 0.0f,              0.0f,              0.0f });
        shape.AddVertex({ 0.5f * Cos(th2),  -0.5f * Sin(th2),   0.0f });
    }
});

Shape Shapes::face_Square = Shape(GL_TRIANGLES, [&](Shape& shape) 
{
    shape.AddVertex({ -0.5f, -0.5f, 0.0f });
    shape.AddVertex({  0.5f,  0.5f, 0.0f });
    shape.AddVertex({ -0.5f,  0.5f, 0.0f });
                             
    shape.AddVertex({ -0.5f, -0.5f, 0.0f });
    shape.AddVertex({  0.5f, -0.5f, 0.0f });
    shape.AddVertex({  0.5f,  0.5f, 0.0f });
});

Shape Shapes::body_Cylinder = Shape(GL_TRIANGLES, [&](Shape& shape) 
{
    using namespace MaxLib::Geom;
    // top face - circle facing up
    shape.AddShape(face_Circle.Transform({ 0.0f, 0.0f, 0.5f }));
    // bottom face - circle facing down 
    shape.AddShape(face_Circle.Transform({ 0.0f, 0.0f, -0.5f }, glm::vec3(1.0f), { 180.0f, 0.0f }));
    
    // walls
    for (float th = 0.0f; th <= 360.0f; th += DEFAULT_ANGLE_INCREMENT) {
        float th2 = th + DEFAULT_ANGLE_INCREMENT;
        shape.AddVertex({ 0.5f * Cos(th),  -0.5f * Sin(th),  -0.5f });
        shape.AddVertex({ 0.5f * Cos(th),  -0.5f * Sin(th),   0.5f });
        shape.AddVertex({ 0.5f * Cos(th2), -0.5f * Sin(th2), -0.5f });
        shape.AddVertex({ 0.5f * Cos(th),  -0.5f * Sin(th),   0.5f });
        shape.AddVertex({ 0.5f * Cos(th2), -0.5f * Sin(th2),  0.5f });
        shape.AddVertex({ 0.5f * Cos(th2), -0.5f * Sin(th2), -0.5f });
    }
});

Shape Shapes::body_Cube = Shape(GL_TRIANGLES, [&](Shape& shape) 
{
    // top and bottom faces
    shape.AddShape(face_Square.Transform({  0.0f,  0.0f,  0.5f }));
    shape.AddShape(face_Square.Transform({  0.0f,  0.0f, -0.5f }, glm::vec3(1.0f), { 180.0f, 0.0f }));
    // front and back faces                       
    shape.AddShape(face_Square.Transform({  0.0f, -0.5f,  0.0f }, glm::vec3(1.0f), { 90.0f, 0.0f })); 
    shape.AddShape(face_Square.Transform({  0.0f,  0.5f,  0.0f }, glm::vec3(1.0f), { 270.0f, 0.0f }));
    // sides                                      
    shape.AddShape(face_Square.Transform({  0.5f,  0.0f,  0.0f }, glm::vec3(1.0f), { 90.0f, 90.0f })); 
    shape.AddShape(face_Square.Transform({ -0.5f,  0.0f,  0.0f }, glm::vec3(1.0f), { 90.0f, 270.0f })); 
    
    /*  uint cubeIndices[] = 
        {
            // face 1 (xy)
            0, 1, 2,
            2, 3, 0,
        }
    */
});

// build vertices of sphere with smooth shading using parametric equation
// x = r * cos(u) * cos(v)
// y = r * cos(u) * sin(v)
// z = r * sin(u)
// where u: stack(latitude) angle (-90 <= u <= 90)
//       v: sector(longitude) angle (0 <= v <= 360)
Shape Shapes::body_Sphere = Shape(GL_TRIANGLES, [&](Shape& shape) {
    // Set the size of the sphere
    float radius = 0.5f;

    // Calculate the number of slices and stacks based on the given increment angle
    int slices = static_cast<int>(360.0f / DEFAULT_ANGLE_INCREMENT);
    int stacks = static_cast<int>(180.0f / DEFAULT_ANGLE_INCREMENT);

    for (int stack = 0; stack < stacks; ++stack) {
        float phi1 = glm::pi<float>() * static_cast<float>(stack) / static_cast<float>(stacks);
        float phi2 = glm::pi<float>() * static_cast<float>(stack + 1) / static_cast<float>(stacks);

        for (int slice = 0; slice < slices; ++slice) {
            float theta1 = 2.0f * glm::pi<float>() * static_cast<float>(slice) / static_cast<float>(slices);
            float theta2 = 2.0f * glm::pi<float>() * static_cast<float>(slice + 1) / static_cast<float>(slices);

            // Vertices of the triangle
            glm::vec3 v1(radius * std::sin(phi1) * std::cos(theta1),
                         radius * std::sin(phi1) * std::sin(theta1),
                         radius * std::cos(phi1));

            glm::vec3 v2(radius * std::sin(phi1) * std::cos(theta2),
                         radius * std::sin(phi1) * std::sin(theta2),
                         radius * std::cos(phi1));

            glm::vec3 v3(radius * std::sin(phi2) * std::cos(theta1),
                         radius * std::sin(phi2) * std::sin(theta1),
                         radius * std::cos(phi2));

            glm::vec3 v4(radius * std::sin(phi2) * std::cos(theta2),
                         radius * std::sin(phi2) * std::sin(theta2),
                         radius * std::cos(phi2));

            // Add the first triangle
            shape.AddVertex(v1);
            shape.AddVertex(v2);
            shape.AddVertex(v3);

            // Add the second triangle
            shape.AddVertex(v2);
            shape.AddVertex(v4);
            shape.AddVertex(v3);
        }
    }
});


