
#pragma once
#include <MaxLib.h>
#include "glcore/glcore.h"
#include "shapes.h"

    
// basic shaders for just position and colour. No lighting. No normals, No texture coords
static const std::string Viewer_BasicVertexShader = R"(
// #version 140
#version 300 es

in vec3 in_Position;
in vec3 in_Colour;

out vec3 v_Colour;

uniform mat4 u_MVP;

void main(void)
{
	gl_Position = u_MVP * vec4(in_Position, 1.0f);
    v_Colour = in_Colour;
}
)";

static std::string Viewer_BasicFragmentShader = R"(
// #version 140
#version 300 es

precision highp float; // needed only for version 1.30

in vec3 v_Colour;
out vec4 out_Colour;

void main(void)
{
	out_Colour = vec4(v_Colour, 1.0f);
}
)";


inline glm::vec2 glmVec2(const MaxLib::Geom::Vec2& v) { return { v.x, v.y }; }
inline glm::vec3 glmVec3(const MaxLib::Geom::Vec3& v) { return { v.x, v.y, v.z }; }
    


struct Vertex {
    Vertex(glm::vec3 pos, glm::vec3 col) : position(pos), colour(col) {}
    glm::vec3 position;
    glm::vec3 colour;
};

// contains a list of points/lines + a colour
struct VertexList {
    VertexList() {}
    VertexList(const glm::vec3& col) : colour(col) {}
    std::vector<glm::vec3> position; 
    glm::vec3 colour; 
};


/*
        
        TODO: 
        
        Viewer Dynamic Buffer:
            The raw value Viewer needs is a Vertex (glm::vec3 position; glm::vec3 colour; )
            currently we are using a VertexList (std::vector<glm::vec3> positions; glm::vec3 colour; )
            We want to tell the dynamic buffer, this is where the data is.. e.g:
            - allow a custom vector as a parameter like imgui does
            - AddVertexPtr() Callback function to show where the point is to render and how to render it. emplace_back(glm::vec3 pos, glm::vec3 colour)
            - maybe a template so you can pass anything?


*/ 

// Forward declare
//class Viewer;




     
class Buffer
{
public:
    
private:
    
};


class DynamicBuffer : public Buffer
{
public:
    void AddCursor(const glm::vec2& position, const glm::vec3& colour, float size);
    void AddGrid(const glm::vec3& position, const glm::vec3& color, const glm::vec2& size, float spacing);
    void AddAxes(const glm::vec3& position, float size);
    void AddShape(const Shape& shape, const glm::vec3& colour, const glm::vec3& translate = { 0.0f, 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f }, const glm::vec2& rotate = { 0.0f, 0.0f });
    void AddVertexListAsLines(const std::vector<VertexList>* dynamicVertexLists, const glm::vec3& zeroPosition);
    void AddVertexListAsPoints(const std::vector<VertexList>* dynamicVertexLists, const glm::vec3& zeroPosition);
    
    float pointSize = 1.0f;
    float lineWidth = 1.0f;
    GLenum depthFunction = GL_LEQUAL; 
    
private:
    
    std::vector<Renderer::ElementData> m_ElementData;


    GLenum m_PrimitiveType;     / TODO REMOVE
    uint m_MaxVertexCount;
    uint m_MaxIndexCount;
    
    std::vector<Vertex> m_Vertices;
    
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    
    DynamicBuffer(GLenum primitiveType, int maxVertices, int maxIndices);
    
    void AddData(uint primitive, std::function<void()> cb_AddVertices) 
    void AddVertex(const glm::vec3& position, const glm::vec3& colour); // must call a
    void Resize(int maxVertices, int maxIndices);
    void ClearVertices();
    void Update();
    void Render(glm::mat4& proj, glm::mat4& view);
    
    friend class Viewer;
}; 


class Viewer
{
public:
    Viewer();

    // T: StaticBuffer, DynamicBuffer, StreamBuffer
    // primitiveType: GL_POINTS, GL_LINES, GL_TRIANGLES etc
    template<typename T>
    T& CreateBuffer(GLenum primitiveType, int maxVertices = 1024, int maxIndices = 1024) {
        return m_Buffers.Add<T>({ primitiveType, maxVertices, maxIndices });
    }
    
    // A array of buffers (static, dynamic or stream)
    MaxLib::Vector::Vector_Ptrs<Buffer> m_Buffers;


    glm::vec3 GetWorldPosition(glm::vec2 px);
    float ScaleToPx(float size) { return size * (m_Camera.GetZoom() / Window::GetHeight()); } 
    
    std::pair<bool, glm::vec2> GetScreenCoords(glm::vec3 position) { return m_Camera.GetScreenCoords(position); }
    
    void Draw2DText(const char* label, glm::vec3 position);
    
    void SetPath(size_t size, std::function<glm::vec3(size_t)> cb_position, std::function<glm::vec3(size_t)> cb_colour);
    void Clear();    
    
    void Update();
    void Render();
    void ImGuiRender();
    
private:
    bool m_Initialised = false;
    bool m_Show = true;
    
    Camera_CentreObject m_Camera;
    
    std::unique_ptr<EventHandler<Event_WindowResize>> event_WindowResize;
    std::unique_ptr<EventHandler<Event_MouseScroll>> event_MouseScroll;
    std::unique_ptr<EventHandler<Event_MouseMove>> event_MouseDrag;
    std::unique_ptr<EventHandler<Event_KeyInput>> event_Keyboard;
    //~ std::unique_ptr<EventHandler<Event_Viewer_AddLineLists>> event_AddLineLists;
    //~ std::unique_ptr<EventHandler<Event_Viewer_AddPointLists>> event_AddPointLists;
    //~ std::unique_ptr<EventHandler<Event_Get2DMode>> event_Get2DMode;
    //~ std::unique_ptr<EventHandler<Event_Set2DMode>> event_Set2DMode;
    //std::unique_ptr<EventHandler<Event_UpdateCamera>> event_UpdateCamera;
     
    // tool
    Shape m_Shape_Tool(GL_TRIANGLES);
    Shape m_Shape_ToolHolder(GL_TRIANGLES);
    Shape m_Shape_Tool_Wireframe(GL_LINES);
    Shape m_Shape_ToolHolder_Wireframe(GL_LINES);
    
    DynamicBuffer m_DynamicPoints = { GL_POINTS, 512, 512 };
    DynamicBuffer m_DynamicLines = { GL_LINES, 1024, 1024 };
    
TODO:   m_DynamicLines.depthFunction = GL_ALWAYS;
    
    DynamicBuffer m_DynamicBodies = { GL_TRIANGLES, 4096, 4096 };
    
    std::vector<VertexList>* m_DynamicLineLists = nullptr;
    std::vector<VertexList>* m_DynamicPointLists = nullptr;
    //std::vector<glm::vec2> m_Shape;
    //std::vector<glm::vec2> m_ShapeOffset;
    //bool m_ShapeIsLoop = false;
    
    
    // static buffer for path
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    
    glm::mat4 m_Proj;
    glm::mat4 m_View;
    
    int m_DrawCount = 0;
    int m_DrawMax = 0;
    
    // Create material picker and set initial value
    //~ MaterialPicker m_MaterialPicker;
    //~ Material m_Material = m_MaterialPicker.GetCurrentMaterial();
    
    float m_LineWidth_Lines = 1.0f;
    float m_LineWidth_Bodies = 1.0f;
    int m_DepthFunction = 3;
    
    void Draw2DAxesLabels(glm::vec3 position, float axisLength);
    void DrawPath();

};

