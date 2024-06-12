#include <iostream>
#include <algorithm>
#include "viewer.h"
using namespace std;
using namespace MaxLib;


namespace GLViewer {

// Global counter for buffers
static uint m_Counter_GeometryID = 0;
static uint m_Counter_TextID = 0;
static uint m_Counter_ImageID = 0;

// Constructor - sets type & id
GeometryBuffer::GeometryBuffer(BufferType type) 
    : m_Type(type), m_ID(++m_Counter_GeometryID) 
{}

void GeometryBuffer::Reserve(size_t nVertices) {
    m_Vertices.reserve(nVertices);
    m_Indices.reserve(nVertices);
}

// Draws a '+' style Cursor of size 1, use transform to set size, position & rotation
void GeometryBuffer::AddCursor(const glm::vec3& colour, const Transform& transform)
{
    AddData(GL_LINES, [&](GeometryBuffer& b) {
        b.AddVertexData({ 0.0f, -1.0f, 0.0f }, colour, transform);
        b.AddVertexData({ 0.0f,  1.0f, 0.0f }, colour, transform);
        b.AddVertexData({ -1.0f, 0.0f, 0.0f }, colour, transform);
        b.AddVertexData({  1.0f, 0.0f, 0.0f }, colour, transform);
    });
}


// Draws a grid with size and spacing as paramters, use transform to set position & rotate if required 
void GeometryBuffer::AddGrid(const glm::vec2& size, float spacing, const glm::vec3& colour, const Transform& transform)
{   
    if(spacing <= 0)
        return;

    AddData(GL_LINES, [&](GeometryBuffer& b) {
        glm::vec2 sign = glm::vec2(Geom::Sign(size.x), Geom::Sign(size.y));
        // X lines
        for (float i = 0.0f; i <= fabs(size.x); i += spacing) {
            b.AddVertexData({ sign.x * i , 0.0f, 0.0f  }, colour, transform);
            b.AddVertexData({ sign.x * i, size.y, 0.0f }, colour, transform);
        }
        // Y lines
        for (float j = 0.0f; j <= fabs(size.y); j += spacing) {
            b.AddVertexData({ 0.0f, sign.y * j, 0.0f  }, colour, transform);
            b.AddVertexData({ size.x, sign.y * j, 0.0f }, colour, transform);
        }
    });
}


// Draws a axis of length 1, use transform to set size and position 
void GeometryBuffer::AddAxes(const Transform& transform)
{    
    AddData(GL_LINES, [&](GeometryBuffer& b) {
        // draw axis, xyz axes are rgb respectively
        b.AddVertexData({ 0.0f, 0.0f, 0.0f}, Colour::Red, transform);
        b.AddVertexData({ 1.0f, 0.0f, 0.0f}, Colour::Red, transform);
        b.AddVertexData({ 0.0f, 0.0f, 0.0f}, Colour::Green, transform);
        b.AddVertexData({ 0.0f, 1.0f, 0.0f}, Colour::Green, transform);
        b.AddVertexData({ 0.0f, 0.0f, 0.0f}, Colour::Blue, transform);
        b.AddVertexData({ 0.0f, 0.0f, 1.0f}, Colour::Blue, transform);
    });
    cout << "mAKE SURE AXIS IS RGB IN CORRECT ORDER" << endl;
} 



void GeometryBuffer::AddShape(const Shape& shape, const glm::vec3& colour, const Transform& transform) 
{   
    AddData(shape.primitive, [&](GeometryBuffer& b) {
        for(size_t i = 0; i < shape.Size(); i++) {
            b.AddVertexData(shape[i], colour, transform);
        }
    });
} 

// Adds vertex of any type primitive (GL_POINTS, GL_LINES etc), one colour
void GeometryBuffer::AddVertex(uint primitive, const Vertex& vertex, const Transform& transform)
{
    AddData(primitive, [&](GeometryBuffer& b) {
        b.AddVertexData(vertex.position, vertex.colour, transform);
    });
}

// Adds vertices of any type primitive (GL_POINTS, GL_LINES etc), one colour
void GeometryBuffer::AddVertices(uint primitive, const Vertices& vertices, const Transform& transform)
{
    AddData(primitive, [&](GeometryBuffer& b) {
        for(auto& v : vertices.position) {
            b.AddVertexData(v, vertices.colour, transform);
        }
    });
}

// Adds a list of vertices of any type primitive (GL_POINTS, GL_LINES etc), each has their own colour.
void GeometryBuffer::AddVerticesList(uint primitive, const std::vector<Vertices>& verticesList, const Transform& transform)
{
    AddData(primitive, [&](GeometryBuffer& b) {
        for(auto& vertices : verticesList) {
            for(auto& vertex : vertices.position) {
                b.AddVertexData(vertex, vertices.colour, transform);
            }
        }
    });
} 


// TODO: this function should be replaced when indices are pre specified, 
// Creates a list of ascending numbers between 1 and size
std::vector<uint> GeometryBuffer::CreateBasicIndicesToMatchVertices(uint size) {
    std::vector<uint> indices;
    indices.reserve(size);
    for (uint i = 0; i < size; i++)
        indices.push_back(i);
    return indices;
}
  

bool GeometryBuffer::SetMaxVertices() {
    
    bool needsUpdate = false;
    
    //~ cout << endl << "nVertices: " << m_Vertices.size() << endl;
    while(m_Vertices.size() > m_MaxVertices) {
        m_MaxVertices *= 2;
        //~ cout << "Increasing buffer size to: " << m_MaxVertices << endl;
        needsUpdate = true;
    }
    // Shrink buffer if far larger than the number of vertices  i.e. if buffer is 10x bigger
    uint scaleFactor = 10;
    // Minimum size of buffer
    uint minSize = 256;
    while((m_Vertices.size() * scaleFactor < m_MaxVertices) && (m_MaxVertices > minSize)) {
        m_MaxVertices /= 2;
        //~ cout << "Decreasing buffer size to: " << m_MaxVertices << endl;
        needsUpdate = true;
    }
    return needsUpdate;
}
    

// The size of the buffers is linked to m_MaxVertices rather than m_Vertices.size().
// This allows us to continue adding data upto m_MaxVertices without needing to reallocate the memory
// 
void GeometryBuffer::Resize()
{ 
    // Set Max GeometryBuffer size
    bool needsUpdate = SetMaxVertices();
    // Return if we dont need to update
    if(!needsUpdate && m_Initialised) { return; }
    
    // Print to terminal
    Log::Info("Resizing buffer to %d vertices (nVertices = %d)", m_MaxVertices, m_Vertices.size());
    
    // Make shader
    m_Shader.reset(new Shader(Shaders::BasicVertex, Shaders::BasicFragment));
    
    // Make static vertex buffer
    if(m_Type == BufferType::Static) {        
        m_VertexBuffer.reset(new VertexBuffer(m_Type, m_MaxVertices * sizeof(Vertex), m_Vertices.data()));
    } // Make dynamic / stream vertex buffer
    else if((m_Type == BufferType::Dynamic) || (m_Type == BufferType::Stream)) {
        m_VertexBuffer.reset(new VertexBuffer(m_Type, m_MaxVertices * sizeof(Vertex)));
    } // Shouldn't reach
    else { assert(0 && "Unknown GeometryBuffer Type"); }
    
    // Make vertex layout
    VertexBufferLayout layout;
    layout.Push<float>(m_Shader->GetAttribLocation("in_Position"), 3);
    layout.Push<float>(m_Shader->GetAttribLocation("in_Colour"), 3);
    
    // Make Vertex Array Object
    m_VAO.reset(new VertexArray());
    m_VAO->AddBuffer(*m_VertexBuffer, layout);
    
    // Create incrementing list of indices between 1 and maxVertices   TODO: this should be replaced by indices
    std::vector<uint> indices = CreateBasicIndicesToMatchVertices(m_MaxVertices);
    // Make index buffer
    m_IndexBuffer.reset(new IndexBuffer(m_MaxVertices, indices.data())); // size can be more than data.size() if required
    // Set initialised flag
    m_Initialised = true;
}

void GeometryBuffer::ClearAll()
{
    m_Vertices.clear();
    m_ElementData.clear();
}
    
// This should only be called by higher level Add functions m_ElementData.emplace_back() must be called after adding vertices
void GeometryBuffer::AddVertexData(const glm::vec3& position, const glm::vec3& colour, const Transform& transform)
{
    glm::vec3 v = transform.TransformVertex(position);
    m_Vertices.emplace_back(std::move(v), colour);  
    // TODO: Currently we create 1 indices per vertex, we can send a lot less data if instead indices are defined correctly 
    m_Indices.push_back(m_Indices.size() + 1);       
} 

// Wrapper for creating a elementdata class when adding vertices
// Draws number of vertices equal to drawcount, unless drawCount <= -1, in which case all vertices will be drawn
void GeometryBuffer::AddData(uint primitive, std::function<void(GeometryBuffer&)> cb_AddVertices, int drawCount) 
{
    uint start = m_Vertices.size();
    // Add vertices in from callback
    cb_AddVertices(*this);
    uint end = m_Vertices.size();
    // Draw user defined number of vertices, or all vertices if -1
    uint count = (drawCount > -1) ? min((end - start), (uint)drawCount) : (end - start);
    // Create element data with primitive type, start position and count
    m_ElementData.emplace_back(primitive, start, count); 
}


void GeometryBuffer::Render(const glm::mat4& proj, const glm::mat4& view) 
{   
    if(!m_Initialised)
        return; 
    
    m_Shader->Bind();
    m_Shader->SetUniformMat4f("u_MVP", proj * view * glm::mat4(1.0f));
    
    // set point size
    glPointSize(pointSize);
    // set line width
    glLineWidth(lineWidth);
    // set depth function for bodies
    glDepthFunc(depthFunction); // GL_NEVER, GL_LESS etc.   
        
    // Let opengl know where and how to open each data chunk
    Renderer::Draw(*m_VAO, *m_IndexBuffer, *m_Shader, m_ElementData);
}


  
void GeometryBuffer::DynamicUpdate() 
{
    // return if no vertices
    if(m_Vertices.empty()) { return; }
    // TODO: We should only update the bits of data we need to
    m_VertexBuffer->DynamicUpdate(0, m_Vertices.size() * sizeof(Vertex), m_Vertices.data());
}
   
  
// Doesn't require maxVertices / maxIndices as it 
StaticBuffer::StaticBuffer(std::function<void(StaticBuffer&)> cb_AddData) 
    : GeometryBuffer(BufferType::Static)
{
    //SetVertices<StaticBuffer>(cb_AddData, true); // should update to intialise shaders, vb, ib etc
    SetVertices(cb_AddData); // should update to intialise shaders, vb, ib etc
}
    
DynamicBuffer::DynamicBuffer()
    : GeometryBuffer(BufferType::Dynamic)
{}
  
StreamBuffer::StreamBuffer()
    : GeometryBuffer(BufferType::Stream)
{}
  
  
  
RenderableImage::RenderableImage(Viewer* viewer, ImageTexture texture, const glm::vec2& position2d, const glm::vec2& size)
    : m_Texture(texture), m_Position(position2d), m_Size(size), m_Valid(true)
{
    (void)viewer;
}

RenderableImage::RenderableImage(Viewer* viewer, ImageTexture texture, const glm::vec3& position3d, const glm::vec2& size) 
    : m_Texture(texture), m_Size(size)
{
    // convert world coords to screen coords
    if(std::optional<glm::vec2> screenCoords = viewer->WorldToScreenCoords(position3d)) {
        // top left of image
        m_Position = *screenCoords - (m_Size / 2.0f);
        m_Valid = true;
    }
}
    
void RenderableImage::Render() 
{			  
    if(!m_Valid) { return; }
    // add to ImGui draw list
    ImGui::GetBackgroundDrawList()->AddImage((void*)(intptr_t)m_Texture.textureID, ImVec2(m_Position.x, m_Position.y), ImVec2(m_Position.x + m_Size.x, m_Position.y + m_Size.y));
}





ImageBuffer::ImageBuffer(Viewer* viewer) 
    : m_Parent(viewer), m_ID(++m_Counter_ImageID)
{}
    
    
void ImageBuffer::Add2DImage(ImageTexture texture, const glm::vec2& position2d, const glm::vec2& size) 
{
    m_Images.emplace_back(m_Parent, texture, position2d, size);
}

void ImageBuffer::Add3DImage(ImageTexture texture, const glm::vec3& position3d, const glm::vec2& size) 
{
    m_Images.emplace_back(m_Parent, texture, position3d, size);
}


void ImageBuffer::ClearAll() 
{ 
    m_Images.erase(m_Images.begin(), m_Images.end()); 
}

void ImageBuffer::SetData(std::function<void(ImageBuffer&)> cb_AddData)
{   // Clear old data
    ClearAll();
    // Add vertices via callback
    cb_AddData(*this);  
}

void ImageBuffer::Render() 
{
    // add each image to imgui draw list
    for(RenderableImage& image : m_Images) {
        image.Render();        
    } 
}
    




// Text at 3D (World) Position, text is aligned centre by default (0, 0 if top left)
RenderableText::RenderableText(Viewer* viewer, const std::string& text, const glm::vec3& pos3d, const glm::vec2& textAlign)
    : m_Label(text)
{
    // Get 2d screen position of 3d point (pair<bool, glm::vec2>)
    if(std::optional<glm::vec2> pos2d = viewer->WorldToScreenCoords(pos3d)) {
        ImVec2 textSize = ImGui::CalcTextSize(m_Label.c_str()); 
        // Align the text
        m_Position = *pos2d - textAlign * glm::vec2(textSize.x, textSize.y);
        m_Valid = true;  
    }
    
}

// Text at 2D (Window) Position, text is aligned centre by default
RenderableText::RenderableText(Viewer* viewer, const std::string& text, const glm::vec2& pos2d, const glm::vec2& textAlign)
    : m_Label(text), m_Valid(true)
{
    (void)viewer; 
    // Send text to imgui draw list (first align the text)
    ImVec2 textSize = ImGui::CalcTextSize(m_Label.c_str());
    m_Position = pos2d - textAlign * glm::vec2(textSize.x, textSize.y);  
}

// TODO: Viewer in render only
void RenderableText::Render() {  
    if(!m_Valid) { return; }
    // Send text to imgui draw list
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(m_Position.x, m_Position.y), IM_COL32(255, 255, 255, 255), m_Label.c_str());
} 




TextBuffer::TextBuffer(Viewer* viewer) 
    : m_Parent(viewer), m_ID(++m_Counter_TextID)
{}

void TextBuffer::Add2DText(const std::string& label, const glm::vec2& p)
{
    m_Texts.emplace_back(m_Parent, label, p, glm::vec2(0.5f, 0.5f));
}

void TextBuffer::Add3DText(const std::string& label, const glm::vec3& position)
{
    m_Texts.emplace_back(m_Parent, label, position, glm::vec2(0.5f, 0.5f));
}

void TextBuffer::Add3DAxisLabels(const Transform& transform)
{
    Add3DText("X", transform.TransformVertex(glm::vec3(1.0f, 0.0f, 0.0f)));
    Add3DText("Y", transform.TransformVertex(glm::vec3(0.0f, 1.0f, 0.0f)));
    Add3DText("Z", transform.TransformVertex(glm::vec3(0.0f, 0.0f, 1.0f)));
}

void TextBuffer::ClearAll() 
{ 
    m_Texts.erase(m_Texts.begin(), m_Texts.end()); 
}
void TextBuffer::SetData(std::function<void(TextBuffer&)> cb_AddData)
{   // Clear old data
    ClearAll();
    // Add vertices via callback
    cb_AddData(*this);  
}

void TextBuffer::Render() 
{
    // add each text to imgui draw list
    for(RenderableText& text : m_Texts) {
        text.Render();
    } 
}

    

// Create a new static buffer
// Example: GeometryBufferID buffer = buffers.CreateBuffer<StaticBuffer>([](StaticBuffer& buffer){ buffer.Addxxx() });
GeometryBufferID Buffers::CreateStaticBuffer(std::function<void(StaticBuffer&)> cb_AddData) {
    return m_GeometryBuffers.Add<StaticBuffer>(cb_AddData).m_ID;
}
// Create a new dynamic buffer
// Example: GeometryBufferID buffer = buffers.CreateBuffer<DynamicBuffer>();
GeometryBufferID Buffers::CreateDynamicBuffer() {
    return m_GeometryBuffers.Add<DynamicBuffer>().m_ID;
}

// Create a new Stream buffer
// Example: GeometryBufferID buffer = buffers.CreateBuffer<StreamBuffer>();
GeometryBufferID Buffers::CreateStreamBuffer() {
    return m_GeometryBuffers.Add<StreamBuffer>().m_ID;
}

// Create a new Text buffer
TextBufferID Buffers::CreateTextBuffer() {
    return m_TextBuffers.Add(m_Parent).m_ID;
}
// Create a new Image buffer
ImageBufferID Buffers::CreateImageBuffer() {
    return m_ImageBuffers.Add(m_Parent).m_ID;
}


// Deletes a buffer, do not try to access this buffer after deletion
void Buffers::DeleteGeometryBuffer(GeometryBufferID bufferID) {
    // Delete buffer if the ID matches. Go backwards through list so we always have correct index
    for(int i = m_GeometryBuffers.Size()-1; i >= 0; i--) {
        if(m_GeometryBuffers[(size_t)i].m_ID.id == bufferID.id) { m_GeometryBuffers.Remove((size_t)i); }
    }
}
// Deletes a buffer, do not try to access this buffer after deletion
void Buffers::DeleteTextBuffer(TextBufferID bufferID) {
    // Delete buffer if the ID matches. Go backwards through list so we always have correct index
    for(int i = m_TextBuffers.Size()-1; i >= 0; i--) {
        if(m_TextBuffers[(size_t)i].m_ID.id == bufferID.id) { m_TextBuffers.Remove((size_t)i); }
    }
}
// Deletes a buffer, do not try to access this buffer after deletion
void Buffers::DeleteImageBuffer(ImageBufferID bufferID) {
    // Delete buffer if the ID matches. Go backwards through list so we always have correct index
    for(int i = m_ImageBuffers.Size()-1; i >= 0; i--) {
        if(m_ImageBuffers[(size_t)i].m_ID.id == bufferID.id) { m_ImageBuffers.Remove((size_t)i); }
    }
}

void Buffers::DeleteAllBuffers() {
    // Delete each buffer. Go backwards through list so we always have correct index
    while(m_GeometryBuffers.Size()) { m_GeometryBuffers.Remove(0); }
    while(m_TextBuffers.Size())     { m_TextBuffers.Remove(0); }
    while(m_ImageBuffers.Size())    { m_ImageBuffers.Remove(0); }
}

// Renders each of the buffers per frame
void Buffers::RenderBuffers(const glm::mat4& proj, const glm::mat4& view) {
    // Render each of the GeometryBuffers
    for(auto& buffer : m_GeometryBuffers)   { buffer->Render(proj, view); }   
    // Render each of the TextBuffers
    for(auto& buffer : m_TextBuffers)       { buffer->Render(); }   
    // Render each of the ImageBuffers
    for(auto& buffer : m_ImageBuffers)      { buffer->Render(); }   
}


     
Viewer::Viewer() 
  : camera(Window::GetWidth(), Window::GetHeight(), glm::vec3(0.0f, 0.0f, 0.0f), 80.0f), buffers(this)
    
{ 
    auto WindowResizeEvent = [&](Event_WindowResize data) {
        camera.SetViewport(0, 0, data.Width, data.Height);
    };
    auto MouseScrollEvent = [&](Event_MouseScroll data) {
        if(!ActiveItem::IsViewportHovered(camera))
            return;
        camera.ProcessMouseScroll(data.OffsetY);
    };
    auto MouseDragEvent = [&](Event_MouseMove data) {
        // we can just use the data already sent to mouse
        (void)data;
        // ignore if a window is hovered over
        if(ImGui::GetIO().WantCaptureMouse)
            return;
        
        // rotate on mouse click
        if(Mouse::IsLeftClicked() && ActiveItem::IsViewport(camera)) {
            camera.ChangeDirection(Mouse::GetPositionDif());
        } 
        // pan on mouse middle click
        else if(Mouse::IsMiddleClicked() && ActiveItem::IsViewport(camera)) { 
            camera.Move(camera.GetWorldVector(Mouse::GetPositionDif()));		
        } 
    };
    
    event_WindowResize          = make_unique<EventHandler<Event_WindowResize>>(WindowResizeEvent);
    event_MouseScroll           = make_unique<EventHandler<Event_MouseScroll>>(MouseScrollEvent);
    event_MouseDrag             = make_unique<EventHandler<Event_MouseMove>>(MouseDragEvent);

	// blending (allows translucence)
	GLCall(glEnable(GL_BLEND));
	// what to do with source (overlapping item) / what to do with destination (item we are overlapping)
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	// dont draw triangles facing the wrong way 
	glEnable(GL_CULL_FACE);  
	// dont draw vertices outside of our visible depth
	glEnable(GL_DEPTH_TEST);
}
 
void Viewer::Render()
{    
    m_Proj = camera.GetProjectionMatrix();
    m_View = camera.GetViewMatrix();
    // Clear old render data
    Renderer::Clear();
    // Render each of the buffers 
    buffers.RenderBuffers(m_Proj, m_View);
}


void Viewer::RenderLoop(GLSystem& glsys, std::function<void()> cb_Configure_GL, std::function<void()> cb_Draw_ImGui, std::function<void()> cb_Set_DynamicBuffers)
{
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(glsys.GetWindow()))
    {    
        // Poll for and process glfw events 
        GLCall(glfwPollEvents());
        // Configure OpenGL
        cb_Configure_GL();
       
        // Draw ImGui
        glsys.imgui_NewFrame();
        cb_Draw_ImGui();

        // Render viewer buffers
        cb_Set_DynamicBuffers();
        Render(); 
        
        // Render ImGui
        glsys.imgui_Render();
        
        // Swap front and back buffers 
        GLCall(glfwSwapBuffers(glsys.GetWindow()));
    }
}
    
    
} // End namespace GLViewer

