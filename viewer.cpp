#include <iostream>
#include "viewer.h"
using namespace std;
using namespace MaxLib;




void DynamicBuffer::AddCursor(const glm::vec2& position, const glm::vec3& colour, float size)
{
    AddData(GL_LINES, [&]() {
        glm::vec3 p = glm::vec3(position.x,  position.y, 0.0f);
        AddVertex(p + glm::vec3(0.0f,  -size,  0.0f), colour);
        AddVertex(p + glm::vec3(0.0f,   size,  0.0f), colour);
        AddVertex(p + glm::vec3(-size,  0.0f,  0.0f), colour);
        AddVertex(p + glm::vec3(size,   0.0f,  0.0f), colour);
    });
}

// Position + glmVec3(activeCoordSys)
void DynamicBuffer::AddGrid(const glm::vec3& position, const glm::vec3& color, const glm::vec2& size, float spacing)
{   
    if(spacing <= 0)
        return;

    AddData(GL_LINES, [&]() {
        glm::vec2 gridOrientation = glm::vec2(Geom::Sign(size.x), Geom::Sign(size.y));
        // X lines
        for (float i = 0.0f; i <= fabs(size.x); i += spacing) {
            AddVertex(position + glm::vec3(gridOrientation.x * i, 0.0f, 0.0f), color);
            AddVertex(position + glm::vec3(gridOrientation.x * i, size.y, 0.0f), color);
        }
        // Y lines
        for (float j = 0.0f; j <= fabs(size.y); j += spacing) {
            AddVertex(position + glm::vec3(0.0f, gridOrientation.y * j, 0.0f), color);
            AddVertex(position + glm::vec3(size.x, gridOrientation.y * j, 0.0f), color);
        }
    });
}


void DynamicBuffer::AddAxes(const glm::vec3& position, float size)
{    
    AddData(GL_LINES, [&]() {
        // draw axis, xyz axes are rgp respectively
        AddVertex(position + glm::vec3(0.0f,      0.0f,       0.0f),  { 1.0f, 0.0f, 0.0f });
        AddVertex(position + glm::vec3(size,      0.0f,       0.0f),  { 1.0f, 0.0f, 0.0f });
        AddVertex(position + glm::vec3(0.0f,      0.0f,       0.0f),  { 0.0f, 1.0f, 0.0f });
        AddVertex(position + glm::vec3(0.0f,      size,       0.0f),  { 0.0f, 1.0f, 0.0f });
        AddVertex(position + glm::vec3(0.0f,      0.0f,       0.0f),  { 0.0f, 0.0f, 1.0f });
        AddVertex(position + glm::vec3(0.0f,      0.0f,       size),  { 0.0f, 0.0f, 1.0f });
    });
} 


void DynamicBuffer::AddShape(const Shape& shape, const glm::vec3& colour, const glm::vec3& translate, const glm::vec3& scale, const glm::vec2& rotate) 
{   
    AddData(shape.primitive, [&]() {
        for(size_t i = 0; i < shape.Size(); i++) {
            glm::vec3 v = glm::Transform(shape[i], translate, scale, rotate);
            AddVertex(move(v), colour, isOutline);
        }
    });
} 


/ TODO

void DynamicBuffer::AddVertexListAsLines(const std::vector<VertexList>* dynamicVertexLists, const glm::vec3& zeroPosition)
{
    AddData(GL_LINES, [&]() {
        for (size_t i = 0; i < dynamicVertexLists->size(); i++) {
            auto& vertices = (*dynamicVertexLists)[i].position;
            auto& colour = (*dynamicVertexLists)[i].colour;
            // add each vertex as lines
            for (size_t j = 1; j < vertices.size(); j++) {
                AddVertex(zeroPosition + vertices[j-1], colour);
                AddVertex(zeroPosition + vertices[j], colour);
            }
        }
    });
}

void DynamicBuffer::AddVertexListAsPoints(const std::vector<VertexList>* dynamicVertexLists, const glm::vec3& zeroPosition)
{
    AddData(GL_POINTS, [&]() {
        // 
        for (size_t i = 0; i < dynamicVertexLists->size(); i++) {
            auto& vertices = (*dynamicVertexLists)[i].position;
            auto& colour = (*dynamicVertexLists)[i].colour;
            // add each vertex as points
            for (size_t j = 0; j < vertices.size(); j++) {
                AddVertex(zeroPosition + vertices[j], colour);
            }
        }
    });
}





    
   
DynamicBuffer::DynamicBuffer(GLenum primitiveType, int maxVertices, int maxIndices)
    : m_PrimitiveType(primitiveType)
{ 
    std::cout << "This should only have been called once per buffer"
    // Initialise
    Resize(maxVertices, maxIndices);
}
  
void DynamicBuffer::Resize(int maxVertices, int maxIndices)
{     
    m_MaxVertexCount = maxVertices;
    m_MaxIndexCount = maxIndices;
    
    std::vector<uint> indices;
    indices.reserve(m_MaxIndexCount);
    for (uint i = 0; i < m_MaxIndexCount; i++)
        indices.push_back(i);
    m_Vertices.reserve(m_MaxVertexCount);
    
    m_Shader.reset(new Shader(Viewer_BasicVertexShader, Viewer_BasicFragmentShader));
    // make dynamic vertex buffer
    m_VertexBuffer.reset(new VertexBuffer(BufferType::Dynamic, m_MaxVertexCount * sizeof(Vertex)));
    VertexBufferLayout layout;
    
    layout.Push<float>(m_Shader->GetAttribLocation("in_Position"), 3);
    layout.Push<float>(m_Shader->GetAttribLocation("in_Colour"), 3);
    
    m_VAO.reset(new VertexArray());
    m_VAO->AddBuffer(*m_VertexBuffer, layout);
    
    m_IndexBuffer.reset(new IndexBuffer(m_MaxIndexCount, indices.data()));
    
}
void DynamicBuffer::ClearVertices()
{
    m_Vertices.clear();
    m_ElementData.clear();
}
    
// This should only be called by higher level Add functions m_ElementData.emplace_back() must be called after adding vertices
void DynamicBuffer::AddVertex(const glm::vec3& position, const glm::vec3& colour)
{
    m_Vertices.emplace_back(position, colour); 
} 

// Wrapper for creating a elementdata class when adding vertices
void DynamicBuffer::AddData(uint primitive, std::function<void()> cb_AddVertices) 
{
    uint start = m_Vertices.size();
    // Add vertices in from callback
    cb_AddVertices();
    uint end = m_Vertices.size();
    // Create element data with primitive type, start position and count
    m_ElementData.emplace_back({ primitive, start, end - start });
}




   
void DynamicBuffer::Update() 
{
    m_VertexBuffer->DynamicUpdate(0, m_Vertices.size() * sizeof(Vertex), m_Vertices.data());
}

void DynamicBuffer::Render(glm::mat4& proj, glm::mat4& view) 
{    
    m_Shader->Bind();
    m_Shader->SetUniformMat4f("u_MVP", proj * view * glm::mat4(1.0f));
    
    if(m_Vertices.Size() > m_MaxIndexCount || m_Vertices.Size() > m_MaxVertexCount) {
        Log::Info("Too many vertices to display, resizing buffer to %d vertices, %d indices", m_MaxVertexCount*2, m_MaxIndexCount*2);
        // double size of buffer
        Resize(m_MaxVertexCount*2, m_MaxIndexCount*2);
    }
    
    // set point size
    glPointSize(pointSize);
    // set line width
    glLineWidth(lineWidth);
    // set depth function for bodies
    glDepthFunc(depthFunction); // GL_NEVER, GL_LESS etc.   
        
    // Let opengl know how where and how to open each data chunk
    Renderer::Draw(*m_VAO, *m_IndexBuffer, *m_Shader, m_ElementData);
}

     
Viewer::Viewer() 
  : m_Camera(Window::GetWidth(), Window::GetHeight(), glm::vec3(0.0f, 0.0f, 0.0f), 80.0f)
{     
    
    auto WindowResizeEvent = [&](Event_WindowResize data) {
        m_Camera.SetViewport(0, 0, data.Width, data.Height);
    };
    auto MouseScrollEvent = [&](Event_MouseScroll data) {
        if(!ActiveItem::IsViewportHovered(m_Camera))
            return;
        m_Camera.ProcessMouseScroll(data.OffsetY);
    };
    auto MouseDragEvent = [&](Event_MouseMove data) {
        // we can just use the data already sent to mouse
        (void)data;
        // ignore if a window is hovered over
        if(ImGui::GetIO().WantCaptureMouse)
            return;
        
        // rotate on mouse click
        if(Mouse::IsLeftClicked() && ActiveItem::IsViewport(m_Camera)) {
            m_Camera.ChangeDirection(Mouse::GetPositionDif());
        } 
        // pan on mouse middle click
        else if(Mouse::IsMiddleClicked() && ActiveItem::IsViewport(m_Camera)) { 
            m_Camera.Move(m_Camera.GetWorldVector(Mouse::GetPositionDif()));		
        } 
    };
    auto KeyboardEvent = [&](Event_KeyInput data) {
        //bool ctrl = data.Modifier & GLFW_MOD_CONTROL;
        
        if(data.Action == GLFW_PRESS || data.Action == GLFW_REPEAT)  // GLFW_RELEASE / GLFW_REPEAT
        {
           /* switch (data.Key)
            {
                case GLFW_KEY_W:
                    break;
            }*/
        } 
    };
    //~ auto AddVertexLists = [&](Event_Viewer_AddLineLists data) {
        //~ m_DynamicLineLists = data.dynamicLineLists;
    //~ };
    //~ auto AddDynamicPointLists = [&](Event_Viewer_AddPointLists data) {
        //~ m_DynamicPointLists = data.dynamicPointLists;
    //~ };
    
    //~ auto Get2DModeEvent = [&](Event_Get2DMode data) {
        //~ data.is2DMode = m_Camera.Is2DMode();
    //~ };
    
    //~ auto Set2DModeEvent = [&](Event_Set2DMode data) {
        //~ m_Camera.Set2DMode(data.isTrue);
    //~ };
    
    
    
   /* auto UpdateCameraEvent = [&](Event_SettingsUpdated data) {
        if(data.type != Event_SettingType::CoordSystems)
            return;
        m_Camera.SetCentre(data.position);
        // data.angle
        grblVals.coords.homeCoords[0], 0.0f 
    };
    */
    event_WindowResize          = make_unique<EventHandler<Event_WindowResize>>(WindowResizeEvent);
    event_MouseScroll           = make_unique<EventHandler<Event_MouseScroll>>(MouseScrollEvent);
    event_MouseDrag             = make_unique<EventHandler<Event_MouseMove>>(MouseDragEvent);
    event_Keyboard              = make_unique<EventHandler<Event_KeyInput>>(KeyboardEvent);
    //~ event_AddLineLists          = make_unique<EventHandler<Event_Viewer_AddLineLists>>(AddVertexLists); 
    //~ event_AddPointLists         = make_unique<EventHandler<Event_Viewer_AddPointLists>>(AddDynamicPointLists); 
    //~ event_Get2DMode             = make_unique<EventHandler<Event_Get2DMode>>(Get2DModeEvent);
    //~ event_Set2DMode             = make_unique<EventHandler<Event_Set2DMode>>(Set2DModeEvent);
    //event_UpdateCamera = make_unique<EventHandler<Event_SettingsUpdated>>(UpdateCameraEvent);
      
    // dont draw triangles facing the wrong way 
    glEnable(GL_CULL_FACE);  
    // dont draw vertices outside of our visible depth
    glEnable(GL_DEPTH_TEST);
    
    m_Camera.SetNearFar(0.1f, 5000.0f);
    m_Camera.SetZoomMinMax(1.0f, 3000.0f);
    m_Camera.SetZoom(2000.0f);
    
    // 3D shapes
    // Tool (move upward so that bottom of cylinder is on z0)
    m_Shape_Tool = Shapes::Body_Cylinder().Transform({ 0.0f, 0.0f, 0.5f }); 
    // Tool WireFrame
    m_Shape_Tool_Wireframe = Shapes::Wireframe_Cylinder().Transform({ 0.0f, 0.0f, 0.5f }); 
      
    // Tool Holder
    float height = 0.0f;
    // spindle dimensions
    glm::vec3 dim_Nut       = { 34.0f, 34.0f, 23.0f };
    glm::vec3 dim_Shank     = { 23.0f, 23.0f, 20.0f };
    glm::vec3 dim_Shoulder  = { 65.0f, 65.0f, 12.0f };
    glm::vec3 dim_Body      = { 80.0f, 80.0f, 228.0f };
    // cylinder moved up, so that z = 0
    Shape cylinder = Shapes::Body_Cylinder().Transform({ 0.0f, 0.0f, 0.5f });
    // make spindle shape 
    m_Shape_ToolHolder.AddShape(cylinder.Transform({ 0.0f, 0.0f, height },                   dim_Nut)); 
    m_Shape_ToolHolder.AddShape(cylinder.Transform({ 0.0f, 0.0f, height += dim_Nut.z },      dim_Shank)); 
    m_Shape_ToolHolder.AddShape(cylinder.Transform({ 0.0f, 0.0f, height += dim_Shank.z },    dim_Shoulder)); 
    m_Shape_ToolHolder.AddShape(cylinder.Transform({ 0.0f, 0.0f, height += dim_Shoulder.z }, dim_Body)); 
    // spindle wireframe
    height = 0.0f;
    // cylinder moved up, so that z = 0
    Shape cylinder_Wireframe = Shapes::Wireframe_Cylinder().Transform({ 0.0f, 0.0f, 0.5f });
    m_Shape_ToolHolder_Wireframe.AddShape(cylinder_Wireframe.Transform({ 0.0f, 0.0f, height },                   dim_Nut)); 
    m_Shape_ToolHolder_Wireframe.AddShape(cylinder_Wireframe.Transform({ 0.0f, 0.0f, height += dim_Nut.z },      dim_Shank)); 
    m_Shape_ToolHolder_Wireframe.AddShape(cylinder_Wireframe.Transform({ 0.0f, 0.0f, height += dim_Shank.z },    dim_Shoulder)); 
    m_Shape_ToolHolder_Wireframe.AddShape(cylinder_Wireframe.Transform({ 0.0f, 0.0f, height += dim_Shoulder.z }, dim_Body)); 
    
    
}
 

glm::vec3 Viewer::GetWorldPosition(glm::vec2 px) 
{
    return m_Camera.GetWorldPosition(px);
}             

void Viewer::SetPath(size_t size, std::function<glm::vec3(size_t)> cb_Position, std::function<glm::vec3(size_t)> cb_Colour)
//void Viewer::SetPath(std::vector<glm::vec3>& cb_Positions, std::vector<glm::vec3>& cb_Colours)
{ 
    // determine number of vertices to draw (e.g. 3 points is 2 lines, so 4 vertices)
    size_t nVertices = 0;
    if(size > 1) 
        nVertices = (size-1) * 2;
        
    // make vertices
    vector<Vertex> vertices;
    vertices.reserve(nVertices);
    // add as lines
    for (size_t i = 0; i < size- 1; i++) {
        //vertices.emplace_back( cb_Positions[i], cb_Colours[i+1] );
        //vertices.emplace_back( cb_Positions[i+1], cb_Colours[i+1] );
        vertices.emplace_back(cb_Position(i), cb_Colour(i+1));
        vertices.emplace_back(cb_Position(i+1), cb_Colour(i+1));
    }
    // make indices 
    std::vector<uint> indices;
    indices.reserve(nVertices); // 2 points per line
    for (size_t i = 0; i < nVertices; i++) {
        indices.emplace_back(i);
    }
    
    m_Shader.reset(new Shader(Viewer_BasicVertexShader, Viewer_BasicFragmentShader));
   
    m_VertexBuffer.reset(new VertexBuffer(BufferType::Static, vertices.size() * sizeof(Vertex), vertices.data()));
    VertexBufferLayout layout;
    
    layout.Push<float>(m_Shader->GetAttribLocation("in_Position"), 3);
    layout.Push<float>(m_Shader->GetAttribLocation("in_Colour"), 3);
    
    m_VAO.reset(new VertexArray());
    m_VAO->AddBuffer(*m_VertexBuffer, layout);
    
    m_IndexBuffer.reset(new IndexBuffer(indices.size(), indices.data()));
    
    m_DrawCount = m_DrawMax = m_IndexBuffer->GetCount();
    
    m_Initialised = true;
}

void Viewer::Clear()
{
    m_Shader.reset(new Shader(Viewer_BasicVertexShader, Viewer_BasicFragmentShader));
    m_VertexBuffer.reset(new VertexBuffer(BufferType::None, 0, nullptr));
    m_VAO.reset(new VertexArray());
    m_IndexBuffer.reset(new IndexBuffer(0, nullptr));
    
    m_DrawCount = m_DrawMax = 0;
    m_Initialised = false;
}

void Viewer::Draw2DText(const char* label, glm::vec3 position)
{
    pair<bool, glm::vec2> labelPos = m_Camera.GetScreenCoords(position);
    // centre letters
    ImVec2 charSize = ImGui::CalcTextSize(label);
    glm::vec2 charOffset = { charSize.x/2, charSize.y/2 };
    
    if(labelPos.first) {
        glm::vec2 pos2D = Window::InvertYCoord(labelPos.second) - charOffset;
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        drawList->AddText(ImVec2(pos2D.x, pos2D.y), IM_COL32(255, 255, 255, 255), label);
    }
}

void Viewer::Draw2DAxesLabels(glm::vec3 position, float axisLength)
{
    Draw2DText("X", glm::vec3(axisLength, 0.0f, 0.0f) + position);
    Draw2DText("Y", glm::vec3(0.0f, axisLength, 0.0f) + position);
    Draw2DText("Z", glm::vec3(0.0f, 0.0f, axisLength) + position);
}

        
void Viewer::Update()
{
    //~ GRBLVals& grblVals = settings.grblVals;
    //~ float axisSize = settings.p.viewer.axis.Size;
    //~ const glm::vec3& zeroPos = glmVec3(grblVals.ActiveCoordSys());
    //~ const glm::vec3& MPos = glmVec3(grblVals.status.MPos);
    
    //~ // Add axis letters
    //~ Draw2DAxesLabels(zeroPos, axisSize);
    //~ Draw2DText("H", glmVec3(grblVals.coords.homeCoords[0]));
    //~ // Reset buffer
    //~ m_DynamicPoints.ClearVertices();
    //~ m_DynamicLines.ClearVertices();
    m_DynamicBodies.ClearVertices();
    
//~ // -------------LINES----------------
    //~ // this could be in static buffer...
    m_DynamicLines.AddGrid({0,0,0}, { 1.0f, 0.0f, 0.0f }, {500.0f, 1000.0f}, 100);
    //m_DynamicLines.AddGrid(settings);
    
    //~ // add shape and offset path
    //~ if(m_DynamicLineLists) {
        //~ m_DynamicLines.AddVertexListAsLines(m_DynamicLineLists, zeroPos);
    //~ }    
    //~ if(m_DynamicPointLists) {
        //~ m_DynamicPoints.AddVertexListAsPoints(m_DynamicPointLists, zeroPos);
    //~ }
    //~ // Draw coord system axis
    //~ m_DynamicLines.AddAxes(axisSize, zeroPos);
    //~ // add user cursor
    //~ if(auto cursorPos = settings.p.sketch.cursor.Position_WorldCoords) {
        //~ m_DynamicLines.AddCursor(settings, *cursorPos);
    //~ }
           
//~ // -------------Bodies----------------
    //~ glm::vec3 scaleTool = glmVec3(settings.p.toolSettings.tools.GetToolScale());
    
//~ // Draw Current Position
// Draw Outlines 
    glm::vec3 MPos = {0,0,0};
    glm::vec3 scaleTool = {100.0f, 100.0f, 100.0f};
    // tool 
    //~ m_DynamicBodies.AddShapeOutline(m_Shape_Tool_Wireframe,         settings.p.viewer.spindle.colours.toolOutline,  MPos, scaleTool);
    m_DynamicBodies.AddShape(m_Shape_Tool_Wireframe,         {1.0f, 1.0f, 1.0f},  MPos, scaleTool);
    //~ // tool holder (above tool)
    //~ if(settings.p.viewer.spindle.visibility) {
        //~ m_DynamicBodies.AddShapeOutline(m_Shape_ToolHolder_Wireframe, settings.p.viewer.spindle.colours.toolHolderOutline, MPos + glm::vec3(0.0f, 0.0f, scaleTool.z));
    //~ }
// Draw Faces
    // tool
    m_DynamicBodies.AddShape(m_Shape_Tool, {0.0f, 1.0f, 0.0f}, MPos, scaleTool);
    //~ // tool holder (above tool)
    //~ if(settings.p.viewer.spindle.visibility) {
        //~ m_DynamicBodies.AddShape(m_Shape_ToolHolder, settings.p.viewer.spindle.colours.toolHolder, MPos + glm::vec3(0.0f, 0.0f, scaleTool.z));
    //~ }
    
    m_DynamicLines.Update();
    m_DynamicBodies.Update();
    //~ m_DynamicPoints.Update();
}



void Viewer::Render()
{    
    m_Proj = m_Camera.GetProjectionMatrix();
    m_View = m_Camera.GetViewMatrix();
    // Clear old data
    Renderer::Clear();
    // Render each of the buffers
    for(auto& buffer : m_Buffers) {
        buffer.Render(m_Proj, m_View);
    }
    
    //~ DrawPath();
}

// TODO: I think this is static buffer
void Viewer::DrawPath()     StaticBuffer::Render
{ 
    //~ if(!m_Initialised || !m_Show)
        //~ return;
    
    //~ m_Shader->Bind();
    //~ m_Shader->SetUniformMat4f("u_MVP", m_Proj * m_View * glm::mat4(1.0f));
    
    //~ Renderer::Draw(GL_LINES, *m_VAO, *m_IndexBuffer, *m_Shader, 0, (uint)m_DrawCount);
}
  
void Viewer::ImGuiRender()  
{ 
    // begin new imgui window
    static ImGuiModules::ImGuiWindow window("Viewer"); // default size
    if(!window.Begin()) return;

    ImGui::TextUnformatted("GCode Viewer"); 
    ImGui::Separator();
        
    ImGui::TextUnformatted("General"); ImGui::Indent(); 
        //~ ImGui:SliderFloat("Point Size", &settings.p.sketch.point.size, 0.0f, 100.0f);

        ImGui::SliderFloat("Line Width of Lines", &m_LineWidth_Lines, 0.0f, 20.0f);
        ImGui::SliderFloat("Line Width of Bodies", &m_LineWidth_Bodies, 0.0f, 20.0f);
        //ImGui::Combo("Depth Function", &m_DepthFunction, "Never\0<\0=\0<=\0>\0!=\0>=\0Always\0\0");
        static int imgui_wireframe = 0;
        if(ImGui::Combo("Edges", &imgui_wireframe, "Show Edges\0Show Hidden Edges\0Hide Edges\0\0")) { // <=  Always
            if(imgui_wireframe == 0) { m_DepthFunction = 3; } // <=
            if(imgui_wireframe == 1) { m_DepthFunction = 7; } // Always
            if(imgui_wireframe == 2) { m_DepthFunction = 0; } // Never
        }
        ImGui::Separator();
        
        static int toolShape = 2;
        if(ImGui::Combo("Tool Shape", &toolShape, "Circle\0Square\0Cylinder\0Cube\0Sphere\0\0")) {
            if (toolShape == 0) { 
                m_Shape_Tool = Shapes::Face_Circle(); 
                m_Shape_Tool_Wireframe = Shapes::Wireframe_Circle();
            }
            else if (toolShape == 1) { 
                m_Shape_Tool = Shapes::Face_Square(); 
                m_Shape_Tool_Wireframe = Shapes::Wireframe_Square();
            }
            else if (toolShape == 2) { 
                m_Shape_Tool = Shapes::Body_Cylinder().Transform({ 0.0f, 0.0f, 0.5f }); 
                m_Shape_Tool_Wireframe = Shapes::Wireframe_Cylinder().Transform({ 0.0f, 0.0f, 0.5f }); 
            }
            else if (toolShape == 3) { 
                m_Shape_Tool = Shapes::Body_Cube().Transform({ 0.0f, 0.0f, 0.5f }); 
                m_Shape_Tool_Wireframe = Shapes::Wireframe_Cube().Transform({ 0.0f, 0.0f, 0.5f });
            }
            else if (toolShape == 4) { 
                m_Shape_Tool = Shapes::Body_Sphere().Transform({ 0.0f, 0.0f, 0.5f }); 
                m_Shape_Tool_Wireframe = Shapes::Wireframe_Circle().Transform({ 0.0f, 0.0f, 0.5f }, { 1.0f, 1.0f, 1.0f });
            }
        } 
    ImGui::Unindent();  ImGui::Separator();
        
    //~ ImGui::TextUnformatted("Cursor"); ImGui::Indent(); 
        //~ ImGuiCustomModules::Text("2D Raw Position", settings.p.sketch.cursor.Position_Raw);
        //~ ImGuiCustomModules::Text("2D World Position", settings.p.sketch.cursor.Position_WorldCoords);
        //~ ImGuiCustomModules::Text("2D Snapped Position", settings.p.sketch.cursor.Position_Snapped);
        //~ ImGuiCustomModules::Text("2D Clicked Position", settings.p.sketch.cursor.Position_Clicked);
        
        //~ if(ImGui::SliderFloat("Cursor Size", &settings.p.sketch.cursor.Size, 0.0f, 100.0f))  {
            //~ settings.p.sketch.cursor.Size_Scaled = ScaleToPx(settings.p.sketch.cursor.Size);
        //~ }
        //~ ImGui::SameLine();
        //~ ImGui::Text("%g Scaled", settings.p.sketch.cursor.Size_Scaled);
        
        //~ ImGui::SliderFloat("Cursor Snap Distance", &settings.p.sketch.cursor.SnapDistance, 0.1f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
        //~ ImGui::SameLine();
        //~ ImGui::Text("%g Scaled", settings.p.sketch.cursor.SnapDistance_Scaled);
    //~ ImGui::Unindent();  ImGui::Separator();
         
    ImGui::TextUnformatted("Tool Path"); ImGui::Indent(); 
        ImGui::SliderInt("Vertices", &m_DrawCount, 0, m_DrawMax); 
        ImGui::Checkbox("Show##toolpath", &m_Show);
        ImGui::SameLine();
        if(ImGui::Button("Clear")) { Clear(); }
    ImGui::Unindent();  ImGui::Separator();
        
    //~ ImGui::TextUnformatted("Axis"); ImGui::Indent();  
        //~ ImGui::SliderFloat("Axis Size", &settings.p.viewer.axis.Size, 0.0f, 500.0f);
    //~ ImGui::Unindent();  ImGui::Separator();
    
    //~ ImGui::TextUnformatted("Grid"); ImGui::Indent();    
        //~ ImGui::SliderFloat3("Position", &settings.p.viewer.grid.Position[0], -3000.0f, 3000.0f);
        //~ ImGui::SameLine();
        //~ // TODO make it direct to vec3
        //~ ImGuiCustomModules::HereButton(settings.grblVals, settings.p.viewer.grid.Position);
        //~ ImGui::SliderFloat2("Size", &settings.p.viewer.grid.Size[0], -3000.0f, 3000.0f);
        //~ ImGui::SliderFloat("Spacing", &settings.p.viewer.grid.Spacing, 0.0f, 1000.0f);
    //~ ImGui::Unindent();  ImGui::Separator();
    
    //~ ImGui::TextUnformatted("Tool Holder"); ImGui::Indent();  
        //~ ImGui::Checkbox("Show##toolholder", &settings.p.viewer.spindle.visibility);
    //~ ImGui::Unindent();  ImGui::Separator();
    
    //~ ImGui::TextUnformatted("Colours"); ImGui::Indent();
        //~ ImGuiColorEditFlags colourFlags = ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoInputs;
        
        //~ ImGui::TextUnformatted("General"); ImGui::Indent();
            //~ ImGui::ColorEdit3("Background", &settings.p.viewer.general.BackgroundColour[0], colourFlags);
            //~ ImGui::ColorEdit3("Grid", &settings.p.viewer.grid.Colour[0], colourFlags);
            //~ ImGui::ColorEdit3("Cursor", &settings.p.sketch.cursor.Colour[0], colourFlags);
            //~ ImGui::Unindent();            
        
        //~ ImGui::TextUnformatted("Tool / Holder"); ImGui::Indent();
            //~ ImGui::ColorEdit3("Tool", &settings.p.viewer.spindle.colours.tool[0], colourFlags);
            //~ ImGui::ColorEdit3("Tool Outline", &settings.p.viewer.spindle.colours.toolOutline[0], colourFlags);
            //~ ImGui::ColorEdit3("Tool Holder", &settings.p.viewer.spindle.colours.toolHolder[0], colourFlags);
            //~ ImGui::ColorEdit3("Tool Holder Outline", &settings.p.viewer.spindle.colours.toolHolderOutline[0], colourFlags);
            //~ ImGui::Unindent();            
            
        //~ ImGui::TextUnformatted("Toolpath"); ImGui::Indent();
            //~ ImGui::ColorEdit3("Feed", &settings.p.viewer.toolpath.Colour_Feed[0], colourFlags);
            //~ ImGui::ColorEdit3("Feed Z", &settings.p.viewer.toolpath.Colour_FeedZ[0], colourFlags);
            //~ ImGui::ColorEdit3("Rapid", &settings.p.viewer.toolpath.Colour_Rapid[0], colourFlags);
            //~ ImGui::ColorEdit3("Home", &settings.p.viewer.toolpath.Colour_Home[0], colourFlags);
            //~ ImGui::Unindent();            
        
        //~ ImGui::TextUnformatted("Sketch"); ImGui::Indent();
            //~ ImGui::ColorEdit3("Points", &settings.p.sketch.point.colour[0], colourFlags);
            //~ ImGui::ColorEdit3("Active Point", &settings.p.sketch.point.colourActive[0], colourFlags);
            //~ ImGui::ColorEdit3("Lines", &settings.p.sketch.line.colour[0], colourFlags);
            //~ ImGui::ColorEdit3("Lines (Disabled)", &settings.p.sketch.line.colourDisabled[0], colourFlags);
            //~ ImGui::Unindent();            
        
    //~ ImGui::Unindent();  ImGui::Separator();

    
    
    window.End();
}


