#include <iostream>
#include <string>
#include <functional>
#include <MaxLib.h>
#include "viewer.h"

using namespace std;
using namespace MaxLib;
using namespace MaxLib::Geom;


//~ namespace GLViewer { 
       
    
#define GUI_WINDOW_NAME     "GLViewer"
#define GUI_WINDOW_W        1280
#define GUI_WINDOW_H        720
#define GUI_WINDOW_WMIN     200
#define GUI_WINDOW_HMIN     200

#define GUI_CONFIG_FILE     "uiconfig.ini"

#define GUI_IMG_ICON        "/img/icon.png"


ImFont* font_medium;
ImageTexture img_Add;

    
void imgui_Settings()
{ 
    ImGuiIO& io = ImGui::GetIO();
       
    // Load Fonts (primary first)
    font_medium = io.Fonts->AddFontFromMemoryCompressedTTF(geomanist_compressed_data, geomanist_compressed_size, 17.0f);
    if(!font_medium)
        cout << "Error: Could not find font: Geomanist 17" << endl;
       
    // Images
    img_Add.Init(File::ThisDir("img/img_add.png").c_str());
}

//~ // Updates the dynamic vertex buffer in viewer
//~ class Updater
//~ {
//~ public:

    //~ struct RenderImage
    //~ {
        //~ RenderImage(ImageTexture t, const ImVec2& p) : texture(t), position(p) {}
        //~ RenderImage(ImageTexture t, const Vec2& p) : RenderImage(t, ImVec2(p.x, p.y)) {}
        //~ ImageTexture texture;
        //~ ImVec2 position; 
    //~ };

    //~ struct RenderText
    //~ {
        //~ RenderText(std::string str, const ImVec2& p) : value(str), position(p) {}
        //~ RenderText(std::string str, const Vec2& p) : RenderText(str, ImVec2(p.x, p.y)) {}
        //~ std::string value;
        //~ ImVec2 position; 
    //~ };



    //~ void HandleUpdateFlag(Settings& settings) 
    //~ {
        //~ // return if no update required
        //~ if(settings.GetUpdateFlag() == SqeakUpdate::None) { return; }
        
        //~ if(settings.GetUpdateFlag() & SqeakUpdate::Viewer) { 
            //~ std::cout << "Updating Sketch" << std::endl;  
            //~ // update 
            //~ UpdateViewerFromSketch();
            
            //~ // TODO:
            //~ //functions.Update();
        //~ }
        
        //~ // reset the update flag
        //~ settings.ResetUpdateFlag();
    //~ }
    
    //~ const std::vector<RenderImage>& Images() const { return m_Images; }
    //~ const std::vector<RenderText>& Texts() const { return m_Texts; }
    
//~ private:

    //~ // TODO: These should all go directly to viewer instead
    //~ std::vector<VertexList> m_ViewerLineLists;
    //~ std::vector<VertexList> m_ViewerPointLists;
    //~ std::vector<RenderImage> m_Images;
    //~ std::vector<RenderText> m_Texts;
    
    //~ void ClearViewer()
    //~ {
        //~ // line lists
        //~ Event<Event_Viewer_AddLineLists>::Dispatch( { nullptr } );
        //~ // points lists
        //~ Event<Event_Viewer_AddPointLists>::Dispatch( { nullptr } );
        //~ //
        //~ Event<Event_Update3DModelFromVector>::Dispatch( { vector<string>(/*empty*/) } );
    //~ }
    
    
    //~ void UpdateViewerFromSketch()
    //~ { 
        //~ // make a list of points / lines which is sent to viewer
        //~ m_ViewerPointLists.clear();
        //~ m_ViewerLineLists.clear();
        //~ m_Images.clear();
        //~ m_Texts.clear();
        
        //~ RenderSketcher();
        
        //~ // dispatch points / lines
        //~ Event<Event_Viewer_AddPointLists>::Dispatch( { &m_ViewerPointLists } );
        //~ Event<Event_Viewer_AddLineLists>::Dispatch( { &m_ViewerLineLists } );
    //~ }
    
    //~ // TODO: This should instead put all values directly on viewer
        
    //~ void RenderSketcher() 
    //~ {    
        //~ using namespace Sketch;
        
        //~ auto CopyVertices = [](std::vector<VertexList>& list, const vector<Geometry>& data, const glm::vec3& colour) {
            
            //~ for(const Geometry& geometry : data) {
                
                //~ VertexList vertices(colour);
                
                //~ geometry.size();
                
                //~ for(const Vec2& p : geometry) {
                    //~ vertices.position.emplace_back(p.x, p.y, 0.0f);
                //~ }
                //~ list.emplace_back(std::move(vertices));     
            //~ }
        //~ };
        
        
        //~ auto CopyImages = [&](const vector<Sketch::RenderData::Image>& images) {
            //~ // Copy all of the images with position to m_Images
            //~ // TODO: This should go directly to viewer
            //~ for(const Sketch::RenderData::Image& image : images) 
            //~ {
                //~ GUISettings& s = settings.guiSettings;
                //~ if(image.type == Sketch::RenderData::Image::Type::Coincident)   { m_Images.emplace_back(s.img_Sketch_Constraint_Coincident, image.position); }
                //~ if(image.type == Sketch::RenderData::Image::Type::Midpoint)     { m_Images.emplace_back(s.img_Sketch_Constraint_Midpoint, image.position); }
                //~ if(image.type == Sketch::RenderData::Image::Type::Vertical)     { m_Images.emplace_back(s.img_Sketch_Constraint_Vertical, image.position); }
                //~ if(image.type == Sketch::RenderData::Image::Type::Horizontal)   { m_Images.emplace_back(s.img_Sketch_Constraint_Horizontal, image.position); }
                //~ if(image.type == Sketch::RenderData::Image::Type::Parallel)     { m_Images.emplace_back(s.img_Sketch_Constraint_Parallel, image.position); }
                //~ if(image.type == Sketch::RenderData::Image::Type::Perpendicular){ m_Images.emplace_back(s.img_Sketch_Constraint_Perpendicular, image.position); }
                //~ if(image.type == Sketch::RenderData::Image::Type::Tangent)      { m_Images.emplace_back(s.img_Sketch_Constraint_Tangent, image.position); }
                //~ if(image.type == Sketch::RenderData::Image::Type::Equal)        { m_Images.emplace_back(s.img_Sketch_Constraint_Equal, image.position); }
                //~ if(image.type == Sketch::RenderData::Image::Type::Distance)     { m_Images.emplace_back(s.img_Sketch_Constraint_Distance, image.position); }
                //~ if(image.type == Sketch::RenderData::Image::Type::Radius)       { m_Images.emplace_back(s.img_Sketch_Constraint_Radius, image.position); }
                //~ if(image.type == Sketch::RenderData::Image::Type::Angle)        { m_Images.emplace_back(s.img_Sketch_Constraint_Angle, image.position); }
            //~ }
     
        //~ };
        
        //~ auto CopyTexts = [&](const vector<Sketch::RenderData::Text>& texts) {
            //~ // Copy all of the texts with position to m_Texts
            //~ // TODO: This should go directly to viewer
            //~ for(const Sketch::RenderData::Text& text : texts) {
                //~ m_Texts.emplace_back(text.value, text.position);
            //~ }
     
        //~ };
        
        //~ auto CopyData = [&](const Sketch::RenderData::Data& data, const glm::vec3& colourPoints, const glm::vec3& colourLines) {
            //~ CopyVertices(m_ViewerPointLists, data.points, colourPoints);
            //~ CopyVertices(m_ViewerLineLists, data.linestrings, colourLines);
            //~ CopyImages(data.images);
            //~ CopyTexts(data.texts);
        //~ };
        
        
        
        
        //~ const Sketch::RenderData& renderData = sketcher.Renderer().GetRenderData();
        
        //~ CopyData(renderData.elements.unselected,        settings.p.sketch.point.colour,     settings.p.sketch.line.colour);
        //~ CopyData(renderData.elements.failed,            { 1.0f, 0.0f, 0.0f },               { 1.0f, 0.0f, 0.0f });
        //~ CopyData(renderData.elements.hovered,           { 0.568f, 0.019f, 0.940f },         { 0.6f, 0.8f, 0.8f });
        //~ CopyData(renderData.elements.selected,          { 1.0f, 1.0f, 1.0f },               { 1.0f, 1.0f, 1.0f });
        
        //~ CopyData(renderData.constraints.unselected,     settings.p.sketch.point.colour,     settings.p.sketch.line.colour);
        //~ CopyData(renderData.constraints.failed,         { 1.0f, 0.0f, 0.0f },               { 1.0f, 0.0f, 0.0f });
        //~ CopyData(renderData.constraints.hovered,        { 0.568f, 0.019f, 0.940f },         { 0.6f, 0.8f, 0.8f });
        //~ CopyData(renderData.constraints.selected,       { 1.0f, 1.0f, 1.0f },               { 1.0f, 1.0f, 1.0f });
        
        //~ CopyData(renderData.preview,                    settings.p.sketch.point.colour,     settings.p.sketch.line.colour);
        //~ const Sketch::RenderData::Data& cursor = renderData.cursor;
        //~ // Change colour of selection box if left of click position
        //~ if(cursor.points.size() == 1) {
            //~ if(cursor.points[0].size() == 4) {
                //~ // change colour depending on whether selection box is to left or right
                //~ const glm::vec3& selectionBoxColour = ((cursor.points[0][2].x - cursor.points[0][0].x) < 0) ? glm::vec3(0.1f, 0.3f, 0.8f) : glm::vec3(0.306f, 0.959f, 0.109f);
                //~ CopyVertices(m_ViewerLineLists, cursor.linestrings, selectionBoxColour);
            //~ }
        //~ }
        
    //~ }
    
//~ };
      



void DrawDockSpace()
{
    // fullscreen dockspace
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    float padding = 15.0f;
    float toolbarHeight = 150.0f;
    // create the dockspace below the main tool bar
    ImGui::SetNextWindowPos(viewport->WorkPos + ImVec2(padding, toolbarHeight + 2.0f*padding));
    ImGui::SetNextWindowSize(viewport->WorkSize - ImVec2(padding*2.0f, toolbarHeight + 3.0f*padding));
    ImGui::SetNextWindowSize(viewport->WorkSize - ImVec2(padding*2.0f, toolbarHeight + 3.0f*padding));
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse 
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    // remove window padding as this is controlled by position + size
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", NULL, window_flags);
    ImGui::PopStyleVar();
    
    /*
    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    
    if (ImGui::DockBuilderGetNode(dockspace_id) == NULL) 
    {
        ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None); // Add empty node
        //ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(200.0f, 400.0f));

        ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);
        ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);

        ImGui::DockBuilderDockWindow("Stats", dock_id_left);
        ImGui::DockBuilderDockWindow("Jog Controller", dock_id_right);
        ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
        
        ImGui::DockBuilderFinish(dockspace_id);
    }
    //DockBuilderDockWindow() or SetNextWindowDockId()
    */
     
    // Submit the DockSpace
    ImGui::DockSpace(ImGui::GetID("DockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode);
    
}    
      

      
void DrawFrames(Viewer& viewer)
{ 
    // draw ImGui windows
    DrawDockSpace();
    
	// show ImGui Demo 
	ImGui::ShowDemoWindow(NULL);
	// Draw Viewer Widgets
	viewer.ImGuiRender();
}



int main()
{
    // GLFW Config Version
    auto cb_GLFW_ConfigVersion = []() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    };
    // GLFW Config General
    auto cb_GLFW_Config = [](GLFWwindow* window) {
        // set minimum window size
        glfwSetWindowSizeLimits(window, GUI_WINDOW_WMIN, GUI_WINDOW_HMIN, GLFW_DONT_CARE, GLFW_DONT_CARE); 
    };

    // ImGui Config
    auto cb_imgui_Config = [](GLFWwindow* window) {
        // Style
        ImGui::StyleColorsDark();
        // Get IO
        ImGuiIO& io = ImGui::GetIO();
        
        // ImGui ini File
        static string iniFile = File::ThisDir(GUI_CONFIG_FILE);
        io.IniFilename = iniFile.c_str();
        
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigDockingAlwaysTabBar = true;
        
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowMenuButtonPosition  = ImGuiDir_Right;   // icon in menu of window
        //style.ColorButtonPosition       = ImGuiDir_Left;    // colour icon side for changing colours 
        style.ScrollbarRounding         = 3.0f;             // scroll bars
        style.FrameRounding             = 2.0f;             // frames i.e. buttons, textboxes etc.
        
        // Load icon
        static string iconLocation = File::ThisDir(GUI_IMG_ICON);
        if(!LoadIconFromFile(window, iconLocation.c_str()))
            Log::Error(string("Could not find icon: ") + iconLocation);
    };
    
    GLSystem glsys(GUI_WINDOW_W, GUI_WINDOW_H, GUI_WINDOW_NAME, "#version 300 es", cb_GLFW_ConfigVersion, cb_GLFW_Config, cb_imgui_Config); // glsl version   
	// blending (allows translucence)
	GLCall(glEnable(GL_BLEND));
	// what to do with source (overlapping item) / what to do with destination (item we are overlapping)
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    // set images / fonts
    imgui_Settings();
    
    // Create Viewer instance
    Viewer viewer;
    //Frames frames;
    
    //Updater updater;
        
    
    //~ // TODO: these should be owned by viewer, combine with viewer.Draw2DText();
    //~ auto ScreenToWorldCoords = [&](glm::vec2 p) {

        //~ glm::vec2 screenCoords = Window::InvertYCoord(p) + glm::vec2(0.0f, 1.0f); // TODO DO WE NEED TO OFFSET ???
        //~ Vec3 WCO = settings.grblVals.status.WCO;
        //~ glm::vec3 returnCoords = viewer.GetWorldPosition(screenCoords) - glm::vec3({ WCO.x, WCO.y, WCO.z });
        //~ return returnCoords;        
    //~ };
    
    //~ auto WorldToScreenCoords = [&](glm::vec2 p) -> std::optional<glm::vec2> {

        //~ Vec3 WCO = settings.grblVals.status.WCO;
        //~ pair<bool, glm::vec2> screenCoords = viewer.GetScreenCoords(glm::vec3({ p.x, p.y, 0.0f }) + glm::vec3({ WCO.x, WCO.y, WCO.z }));
        //~ return (screenCoords.first) ? std::optional<glm::vec2>{ screenCoords.second } : std::nullopt;
    //~ };


    

    //~ auto RenderImguiDrawList = [&]() {
            
        //~ ImVec2 offset = { 0.0f, -20.0f };
        //~ ImVec2 size = { 20.0f, 20.0f };
        //~ ImVec2 halfSize = size / 2.0f;
        //~ ImDrawList* drawList = ImGui::GetWindowDrawList();

        //~ // add each image to imgui draw list
        //~ for(const Updater::RenderImage& image : updater.Images()) {
            //~ // convert world coords to screen coords
            //~ if(std::optional<glm::vec2> screenCoords = WorldToScreenCoords({ image.position.x, image.position.y })) {
                //~ // invert y
                //~ glm::vec2 p = Window::InvertYCoord(*screenCoords);
                //~ // centre image about position
                //~ ImVec2 position = ImVec2{ p.x, p.y } - halfSize + offset;
                //~ // add to ImGui draw list
                //~ drawList->AddImage((void*)(intptr_t)image.texture.textureID, position, position + size);
            //~ }            
        //~ } 
        
        //~ Vec3 WCO = settings.grblVals.status.WCO;
        //~ // add each text to imgui draw list
        //~ for(const Updater::RenderText& text : updater.Texts()) 
        //~ {
            //~ viewer.Draw2DText(text.value.c_str(), {  WCO.x + text.position.x, WCO.y + text.position.y, WCO.z + 0.0f });
        //~ } 

    //~ };
    
    
        
        
    //~ Event<Event_Update3DModelFromVector>::RegisterHandler([&viewer](Event_Update3DModelFromVector data) {
        //~ if(data.gcodes.size() > 0) {
            //~ updateGRBL();
            //~ gcReader.OpenVector(data.gcodes);
            //~ std::vector<Vec3>& vertices = gcReader.GetVertices();
            //~ std::vector<Vec3>& colours = gcReader.GetColours();
            //~ assert(vertices.size() == colours.size());
            //~ viewer.SetPath(vertices.size(), [&](size_t i) { 
                //~ return glm::vec3({ vertices[i].x, vertices[i].y, vertices[i].z }); 
            //~ }, [&](size_t i) { 
                //~ return glm::vec3({ colours[i].x, colours[i].y, colours[i].z }); 
            //~ });
        //~ } else {
            //~ viewer.Clear();
        //~ }
    //~ });
    
    
    
    //~ auto ScaleMouseData = [](Settings& settings, Viewer& viewer) {
        //~ // scale the cursor size
        //~ settings.p.sketch.cursor.Size_Scaled                = viewer.ScaleToPx(settings.p.sketch.cursor.Size);
        //~ // scale the cursor snap distance
        //~ float snapDistance                                  = viewer.ScaleToPx(settings.p.sketch.cursor.SnapDistance);
        //~ // make 0.01, 0.1, 1, 10 or 100
        //~ if(snapDistance <= 0.01f)      { snapDistance = 0.01f; }
        //~ else if(snapDistance <= 0.02f) { snapDistance = 0.02f; }
        //~ else if(snapDistance <= 0.05f) { snapDistance = 0.05f; }
        //~ else if(snapDistance <= 0.1f)  { snapDistance = 0.1f; } 
        //~ else if(snapDistance <= 0.2f)  { snapDistance = 0.2f; } 
        //~ else if(snapDistance <= 0.5f)  { snapDistance = 0.5f; } 
        //~ else if(snapDistance <= 1.0f)  { snapDistance = 1.0f; }
        //~ else if(snapDistance <= 2.0f)  { snapDistance = 2.0f; }
        //~ else if(snapDistance <= 5.0f)  { snapDistance = 5.0f; }
        //~ else if(snapDistance <= 10.0f) { snapDistance = 10.0f; }
        //~ else if(snapDistance <= 20.0f) { snapDistance = 20.0f; }
        //~ else if(snapDistance <= 50.0f) { snapDistance = 50.0f; }
        //~ else                           { snapDistance = 100.0f; }
        
        //~ settings.p.sketch.cursor.SnapDistance_Scaled = snapDistance;        
    //~ };
    
    //~ // scale cursor based on mouse scroll
    //~ Event<Event_MouseScroll>::RegisterHandler([&settings, &viewer, &ScaleMouseData](Event_MouseScroll data) {
        //~ (void)data;
        //~ ScaleMouseData(settings, viewer);
    //~ });
    
    //~ // initialise mouse scale data
    //~ ScaleMouseData(settings, viewer);
    
    
    //~ Event<Event_Set2DMode>::Dispatch( { true } );
    
        

    
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(glsys.GetWindow()))
    {    
        // Poll for and process events 
        GLCall(glfwPollEvents());
        // set background colour
		GLCall(glClearColor(0.8f, 0.8f, 0.8f, 1.0f));
       
       
        // Draw ImGui
        glsys.imgui_NewFrame();
		{
			
            // make updates for viewer
            // updater.HandleUpdateFlag();
            
            // update and render 3D Viewer
            viewer.Update(); 
            viewer.Render();
            
            // DrawImGuiFrames
            DrawFrames(viewer);
            
            // render imgui
            ///RenderImguiDrawList();
    
            // end dockspace todo: this should be with DrawFrames
            ImGui::End();
		}
		glsys.imgui_Render();
        // Swap front and back buffers 
        GLCall(glfwSwapBuffers(glsys.GetWindow()));
    }
    
    return 0;
}

//~ } // end namespace Viewer
