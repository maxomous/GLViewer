#include <iostream>
#include <string>
#include <functional>
#include <MaxLib.h>
#include "viewer.h"

using namespace std;
using namespace MaxLib;
using namespace MaxLib::Geom;
using namespace GLViewer;

ImFont* font_medium;
ImageTexture img_Add;


    
#define GUI_WINDOW_NAME     "GLViewer"
#define GUI_WINDOW_W        1280
#define GUI_WINDOW_H        720

#define GUI_CONFIG_FILE     "uiconfig.ini"
#define GUI_IMG_ICON        "/img/icon.png"


int main()
{
    // GLFW Config General
    auto cb_GLFW_Config = [](GLFWwindow* window) {
        // set minimum window size
        glfwSetWindowSizeLimits(window, 200, 200, GLFW_DONT_CARE, GLFW_DONT_CARE); 
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
        style.ScrollbarRounding         = 3.0f;             // scroll bars
        style.FrameRounding             = 2.0f;             // frames i.e. buttons, textboxes etc.
        
        // Load icon
        static string iconLocation = File::ThisDir(GUI_IMG_ICON);
        if(!LoadIconFromFile(window, iconLocation.c_str()))
            Log::Error(string("Could not find icon: ") + iconLocation);
    };
    

    // Fonts & Images
    auto cb_imgui_Assets = []() { 
	// Get IO
	ImGuiIO& io = ImGui::GetIO();
	   
	// Load Fonts (primary first)
	font_medium = io.Fonts->AddFontFromMemoryCompressedTTF(geomanist_compressed_data, geomanist_compressed_size, 17.0f);
	if(!font_medium)
		cout << "Error: Could not find font: Geomanist 17" << endl;
	   
	// Images
	img_Add.Init(File::ThisDir("img/img_add.png").c_str());
    };



    // glsl version  / glfw version major & minor
    GLSystem glsys(GUI_WINDOW_W, GUI_WINDOW_H, GUI_WINDOW_NAME, "#version 300 es", 3, 0, cb_GLFW_Config, cb_imgui_Config, cb_imgui_Assets);  
    // Create Viewer instance
    Viewer viewer;
    


    // Events (Mouse & Keyboard etc)
    auto event_Keyboard = make_unique<EventHandler<Event_KeyInput>>([&](Event_KeyInput data) {
        //bool ctrl = data.Modifier & GLFW_MOD_CONTROL;
        if(data.Action == GLFW_PRESS || data.Action == GLFW_REPEAT)  // GLFW_RELEASE / GLFW_REPEAT
        {
            switch (data.Key)
            {
                case GLFW_KEY_T:
		    viewer.camera.SetViewMode((viewer.camera.GetViewMode() == Camera::ViewMode::View2D) ? Camera::ViewMode::View3D : Camera::ViewMode::View2D);
                    break;
            }
        } 
    });
    
    
    
    
    // Create Buffers
    Buffers& buffers = viewer.buffers;
    
    glm::vec3 axisSize = {50,50,50};
	
    // Add Static Buffer
    buffers.CreateStaticBuffer([&](StaticBuffer& buffer) {
	// Draw axis
	buffer.AddAxes(Transform({0,0,0}, axisSize));
	// Add Grid
	buffer.AddGrid({ 600.0f, 1200.0f }, 100, Colour::Yellow, Transform({0, 0, 0}));
    });
	
    // Create dynamc buffers
    TextBufferID textBuffer = buffers.CreateTextBuffer();
    //ImageBufferID imageBuffer = buffers.CreateImageBuffer();
    GeometryBufferID dynamicBuffer = buffers.CreateDynamicBuffer();
    
    
    
     
    
    
    
    
    
    
    auto cb_Configure_GL = []() {
	// set background colour
	GLCall(glClearColor(0.8f, 0.8f, 0.8f, 1.0f));
    };
    
    auto cb_Draw_ImGui = []() {
	// Draw ImGui windows
	ImGuiModules::Dockspace::Begin();
	// show ImGui Demo 
	ImGui::ShowDemoWindow(NULL);
	
	static ImGuiModules::ImGuiWindow window_1("Test");
	if(window_1.Begin()) {
	    ImGui::Text("Test");
	    window_1.End();
	}
	// End dockspace
	ImGuiModules::Dockspace::End();
    };
    
    float angle = 0;
    // Set Dynamic / Stream / Text / Image buffers
    // Static buffers should only be set once, before the render loop
    auto cb_Set_DynamicBuffers = [&]() {
	// Set Text Buffer
	buffers.SetBufferData(textBuffer, [&](TextBuffer& buffer) {
	    buffer.Add3DAxisLabels(Transform({0,0,0}, axisSize));
	});
	 
	// Set Dynamic Buffer
	buffers.SetBufferData(dynamicBuffer, [&](DynamicBuffer& buffer) {
	    glm::vec3 MPos = {100,0,0};
	    glm::vec3 scaleTool = {100.0f, 100.0f, 100.0f};
	    
	    buffer.AddShape(Shapes::Body_Cube(),      Colour::Green, Transform(MPos, scaleTool, {45.0f, angle }));
	    buffer.AddShape(Shapes::Wireframe_Cube(), Colour::White, Transform(MPos, scaleTool, {45.0f, angle++ }));
	});
    };
    
    // Run the render loop
    viewer.RenderLoop(glsys, cb_Configure_GL, cb_Draw_ImGui, cb_Set_DynamicBuffers);
    
    
    return 0;
}

//~ } // end namespace Viewer
