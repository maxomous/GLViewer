
/*
 * GLViewer is a simple interface for opengl by handling the GLCore library 
 * 
 */


#pragma once
#include <MaxLib.h>
#include "glcore/glcore.h"
#include "shapes.h"

  
  
namespace GLViewer {

struct Shaders
{
    // basic shaders for just position and colour. No lighting. No normals, No texture coords
    inline static const std::string BasicVertex = R"(
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

    inline static std::string BasicFragment = R"(
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
};



struct Colour
{
    inline static const glm::vec3 Black     = { 0.0f, 0.0f, 0.0f };
    inline static const glm::vec3 White     = { 1.0f, 1.0f, 1.0f };
    inline static const glm::vec3 Red       = { 1.0f, 0.0f, 0.0f };
    inline static const glm::vec3 Green     = { 0.0f, 1.0f, 0.0f };
    inline static const glm::vec3 Blue      = { 0.0f, 0.0f, 1.0f };
    inline static const glm::vec3 Yellow    = { 1.0f, 1.0f, 0.0f };
    inline static const glm::vec3 Cyan      = { 0.0f, 1.0f, 1.0f };
    inline static const glm::vec3 Magenta   = { 1.0f, 0.0f, 1.0f };
    inline static const glm::vec3 Orange    = { 1.0f, 0.5f, 0.0f };
};

struct Vertex {
    Vertex() {}
    Vertex(const glm::vec3& pos, const glm::vec3& col) : position(pos), colour(col) {}
    glm::vec3 position;
    glm::vec3 colour;
};

// contains a list of points/lines + a colour
struct Vertices {
    Vertices() {}
    Vertices(const glm::vec3& col) : colour(col) {}
    Vertices(std::vector<glm::vec3>&& pos, const glm::vec3& col) : position(std::move(pos)), colour(col) {}
    std::vector<glm::vec3> position; 
    glm::vec3 colour = Colour::White; 
};

// A transformation matrix combines translate, scale and rotate matrices. Prevents unnessesary computation when tranforming many vertices.
class Transform
{
public:
    Transform(const glm::vec3& translate = { 0.0f, 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f }, const glm::vec2& rotate = { 0.0f, 0.0f }) // { RX, RZ }
        : t(translate), s(scale), r(rotate)
    { 
        isUnity = (translate == glm::vec3(0.0f, 0.0f, 0.0f)) && (scale == glm::vec3(1.0f, 1.0f, 1.0f)) && (rotate == glm::vec2(0.0f, 0.0f )); 
        transform = (isUnity) ? glm::mat4(1.0f) : glm::TransformMatrix(translate, scale, rotate);
    }
    // Returns transformed vertex
    glm::vec3 TransformVertex(const glm::vec3& vertex) const {
        return (isUnity) ? vertex : glm::vec3(transform * glm::vec4(vertex, 1.0f));
    }
private:
    glm::vec3 t;
    glm::vec3 s;
    glm::vec2 r;
    
    bool isUnity;
    glm::mat4 transform;
};





/*
        
TODO: 

    Viewer Dynamic GeometryBuffer:
        The raw value Viewer needs is a Vertex (glm::vec3 position; glm::vec3 colour; )
        currently we are using a Vertices (std::vector<glm::vec3> positions; glm::vec3 colour; )
        We want to tell the dynamic buffer, this is where the data is.. e.g:
        - allow a custom vector as a parameter like imgui does
        - AddVertexPtr() Callback function to show where the point is to render and how to render it. emplace_back(glm::vec3 pos, glm::vec3 colour)
        - maybe a template so you can pass anything?


// TODO: Callback to create vertices directly in place, also needs colour  

*/ 

// Forward declare
class Viewer;


struct GeometryBufferID {
    GeometryBufferID(uint ID) : id(ID) {}
    uint id;
};
struct TextBufferID {
    TextBufferID(uint ID) : id(ID) {}
    uint id;
};
struct ImageBufferID {
    ImageBufferID(uint ID) : id(ID) {}
    uint id;
};

     



class GeometryBuffer
{
public:
    
    void Reserve(size_t nVertices);
    // Adds a '+' style Cursor of size 1, use transform to set size, position & rotation
    void AddCursor(const glm::vec3& colour, const Transform& transform = {});
    // Adds a grid with size and spacing as paramters, use transform to set position & rotate if required 
    void AddGrid(const glm::vec2& size, float spacing, const glm::vec3& colour, const Transform& transform = {});
    // Adds an axis of length 1, use transform to set size and position 
    void AddAxes(const Transform& transform = {});
    // Adds a shape
    void AddShape(const Shape& shape, const glm::vec3& colour, const Transform& transform = {});
    // Adds vertices of any type primitive (GL_POINTS, GL_LINES etc), one colour
    void AddVertices(uint primitive, const Vertices& vertices, const Transform& transform = {});
    // Adds a list of vertices of any type primitive (GL_POINTS, GL_LINES etc), each has their own colour.
    void AddVerticesList(uint primitive, const std::vector<Vertices>& verticesList, const Transform& transform = {});
    
    // TODO AddVerticesDirectly(cb_AddVertex, colour) - cb function to create the vertex

    float pointSize = 1.0f;
    float lineWidth = 1.0f;
    GLenum depthFunction = GL_LEQUAL; // GL_ALWAYS
    
    // Don't construct directly, create with factory
    GeometryBuffer(BufferType type);
protected:
    
    BufferType m_Type;
    GeometryBufferID m_ID;
    
    bool m_Initialised = false;
    uint m_MaxVertices = 1024;
    std::vector<Vertex> m_Vertices;
    std::vector<uint> m_Indices;
    //std::vector<glm::vec3> m_Normals; // TODO: Define these
    //std::vector<glm::vec2> m_TextureCoords;
    std::vector<Renderer::ElementData> m_ElementData;
    
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    
    
    virtual void Update() = 0;
  
    void Render(const glm::mat4& proj, const glm::mat4& view);
    
    void DynamicUpdate();
    // Dont call directly, must called as part of cb in AddData 
    void AddVertex(const glm::vec3& position, const glm::vec3& colour, const Transform& transform); 
    

    // Wrapper for creating a elementdata class when adding vertices
    // Draws number of vertices equal to drawcount, unless drawCount <= -1, in which case all vertices will be drawn
    void AddData(uint primitive, std::function<void(GeometryBuffer&)> cb_AddVertices, int drawCount = -1);
    void ClearAll();
    
    // TODO: this function should be replaced when indices are pre specified, 
    // Creates a list of ascending numbers between 1 and size
    std::vector<uint> CreateBasicIndicesToMatchVertices(uint size);
    // Updates m_MaxVertices to ensure the buffer is at least as big as the number of vertices 
    // Having a value higher allows us to add more data later without constantly reallocating the memory
    bool SetMaxVertices();
    // The size of the buffers is linked to m_MaxVertices rather than m_Vertices.size().
    // This allows us to continue adding data upto m_MaxVertices without needing to reallocate the memory
    void Resize();
    
    
    friend class Buffers;
};

// A Static GeometryBuffer (See AllBuffers Class)
class StaticBuffer : public GeometryBuffer
{
public:
    // Don't construct directly, create with factory
    StaticBuffer(std::function<void(StaticBuffer&)> cb_AddData);
    
private:

    void Update() override {}
    
    void SetVertices(std::function<void(StaticBuffer&)> cb_AddData)
    {   // Clear old data
        ClearAll();
        // Add vertices via callback
        cb_AddData(*this);     
        // Resize if required
        Resize();     
        // Send buffer data
        Update();             
    }
    friend class Buffers;
}; 

// A Dynamic GeometryBuffer
class DynamicBuffer : public GeometryBuffer
{
public:
    // Don't construct directly, create with factory
    DynamicBuffer();
    
private:
    
    void Update() override { DynamicUpdate(); }
    
    void SetVertices(std::function<void(DynamicBuffer&)> cb_AddData)
    {   // Clear old data
        ClearAll();
        // Add vertices via callback
        cb_AddData(*this);     
        // Resize if required
        Resize();     
        // Send buffer data
        Update();             
    }
    friend class Buffers;
}; 

// A Stream GeometryBuffer
class StreamBuffer : public GeometryBuffer
{
public:
    // Don't construct directly, create with factory
    StreamBuffer();
    
private:
    
    void Update() override { DynamicUpdate(); }
    
    void SetVertices(std::function<void(StreamBuffer&)> cb_AddData)
    {   // Clear old data
        ClearAll();
        // Add vertices via callback
        cb_AddData(*this);     
        // Resize if required
        Resize();     
        // Send buffer data
        Update();             
    }
    friend class Buffers;
}; 


   
class RenderableImage
{
public:
    // Don't construct directly, create with factory
	RenderableImage(Viewer* viewer, ImageTexture texture, const glm::vec2& position2d, const glm::vec2& size);
	RenderableImage(Viewer* viewer, ImageTexture texture, const glm::vec3& position3d, const glm::vec2& size);
    
private:
    
	ImageTexture m_Texture;
	glm::vec2 m_Position; 
	glm::vec2 m_Size; 
    bool m_Valid = false;
    
    void Render();
    
    friend class ImageBuffer;
};



class ImageBuffer
{
public:
    
    void Add2DImage(ImageTexture texture, const glm::vec2& position2d, const glm::vec2& size);
    void Add3DImage(ImageTexture texture, const glm::vec3& position3d, const glm::vec2& size);
	
    // Don't construct directly, create with factory
	ImageBuffer(Viewer* viewer);
    
private:
    
    void ClearAll();
    
    void SetData(std::function<void(ImageBuffer&)> cb_AddData);
    
	void Render();
    
    Viewer* m_Parent;
    ImageBufferID m_ID;
	std::vector<RenderableImage> m_Images;
    
    friend class Buffers;
};






class RenderableText
{
public:
    // Don't construct directly, create with factory
    // Text at 3D (World) Position, text is aligned centre by default (0, 0 if top left)
    RenderableText(Viewer* viewer, const std::string& text, const glm::vec3& position3d, const glm::vec2& textAlign = { 0.5f, 0.5f });
    // Text at 2D (Window) Position, text is aligned centre by default
	RenderableText(Viewer* viewer, const std::string& text, const glm::vec2& position2d, const glm::vec2& textAlign = { 0.5f, 0.5f });
    
private:
	std::string m_Label;
	glm::vec2 m_Position;  
    bool m_Valid = false;
    
    void Render(); 
    
    friend class TextBuffer;  
};



class TextBuffer
{
public:
	
    void Add2DText(const std::string& label, const glm::vec2& position);

    void Add3DText(const std::string& label, const glm::vec3& position);

    void Add3DAxisLabels(const Transform& transform = {});

    // Don't construct directly, create with factory
	TextBuffer(Viewer* viewer);
    
private:
    
    Viewer* m_Parent;
    TextBufferID m_ID;
	std::vector<RenderableText> m_Texts;

    void ClearAll();
    
    void SetData(std::function<void(TextBuffer&)> cb_AddData);

	void Render();

    friend class Buffers;
};







// A factory for creating, managing & storing all Geometry AllBuffers (static, dynamic & stream)
    // Example: GeometryBufferID buffer = buffers.CreateBuffer<StaticBuffer>([](StaticBuffer& buffer){ buffer.Addxxx() });
    // Example: GeometryBufferID buffer = buffers.CreateBuffer<DynamicBuffer>();
    // Example: GeometryBufferID buffer = buffers.CreateBuffer<StreamBuffer>();
    // Example: GeometryBufferID buffer = buffers.CreateBuffer<ImageBuffer>();
    // Example: GeometryBufferID buffer = buffers.CreateBuffer<TextBuffer>();
class Buffers
{
    
public:    
    // Create a new static buffer
    GeometryBufferID CreateStaticBuffer(std::function<void(StaticBuffer&)> cb_AddData = {});
    // Create a new dynamic buffer
    GeometryBufferID CreateDynamicBuffer();
    // Create a new Stream buffer
    GeometryBufferID CreateStreamBuffer();
    // Create a new Text buffer
    TextBufferID CreateTextBuffer();
    // Create a new Image buffer
    ImageBufferID CreateImageBuffer();
    
    // Deletes a buffer, do not try to access this buffer after deletion
    void DeleteGeometryBuffer(GeometryBufferID bufferID);
    void DeleteTextBuffer(TextBufferID bufferID);
    void DeleteImageBuffer(ImageBufferID bufferID);
    // Deletes all geometry, text & image buffers
    void DeleteAllBuffers();

    void SetBufferData(TextBufferID bufferID, std::function<void(TextBuffer&)> cb_AddData) {
		// Get buffer from id & Replace data using callback
		GetTextBufferByID(bufferID.id).SetData(cb_AddData);
	}
    void SetBufferData(ImageBufferID bufferID, std::function<void(ImageBuffer&)> cb_AddData) {
		// Get buffer from id & Replace data using callback
		GetImageBufferByID(bufferID.id).SetData(cb_AddData);
	}
    void SetBufferData(GeometryBufferID bufferID, std::function<void(DynamicBuffer&)> cb_AddData) {
		// Get buffer from id & replace data with new vertices in callback
        GetGeometryBufferByID<DynamicBuffer>(bufferID.id).SetVertices(cb_AddData);
	}
    void SetBufferData(GeometryBufferID bufferID, std::function<void(StreamBuffer&)> cb_AddData) {
		// Get buffer from id & replace data with new vertices in callback
		GetGeometryBufferByID<StreamBuffer>(bufferID.id).SetVertices(cb_AddData);
	}

private:
    // Constructor
    Buffers(Viewer* viewer) : m_Parent(viewer) {}
    
    Viewer* m_Parent;
    
    // A array of buffers (static, dynamic or stream)
    MaxLib::Vector::Vector_Ptrs<GeometryBuffer> m_GeometryBuffers;
    MaxLib::Vector::Vector_Ptrs<TextBuffer>     m_TextBuffers;
    MaxLib::Vector::Vector_Ptrs<ImageBuffer>    m_ImageBuffers;

    // Renders each of the buffers per frame
    void RenderBuffers(const glm::mat4& proj, const glm::mat4& view);

	TextBuffer& GetTextBufferByID(TextBufferID bufferID) {
        // Go through each buffer
        for(size_t i = 0; i < m_TextBuffers.Size(); i++) {
            // Check if id's match, then return
			if(m_TextBuffers[i].m_ID.id == bufferID.id) { return m_TextBuffers[i]; }
		}
		assert(0 && "TextBuffer ID does not exist");
	}   
	ImageBuffer& GetImageBufferByID(ImageBufferID bufferID) {
        // Go through each buffer
        for(size_t i = 0; i < m_ImageBuffers.Size(); i++) {
            // Check if id's match, then return
			if(m_ImageBuffers[i].m_ID.id == bufferID.id) { return m_ImageBuffers[i]; }
		}
		assert(0 && "ImageBuffer ID does not exist");
	}   
    
    // T: StaticBuffer, DynamicBuffer, StreamBuffer
    template<typename T>
	T& GetGeometryBufferByID(GeometryBufferID bufferID) {
        // Go through each buffer
        for(size_t i = 0; i < m_GeometryBuffers.Size(); i++) {
            // Check if id's match
			if(m_GeometryBuffers[i].m_ID.id == bufferID.id) {
                // Convert to inherited class type and return
                return *(m_GeometryBuffers.CastItem<T>(i));
                //~ if(StaticBuffer* b = m_GeometryBuffers.CastItem<StaticBuffer>(i))          { return *b; } 
                //~ else if(DynamicBuffer* b = m_GeometryBuffers.CastItem<DynamicBuffer>(i))   { return *b; } 
                //~ else if(StreamBuffer* b = m_GeometryBuffers.CastItem<StreamBuffer>(i))     { return *b; } 
                //~ else { assert(0 && "Cannot case buffer type"); }
			}
		}
		assert(0 && "GeometryBuffer ID does not exist");
	}   
    
    friend class Viewer;
};








class Viewer
{
public:
    Viewer();

    Camera_CentreObject     camera;
    Buffers                 buffers;

    float ScaleToPx(float size) { return size * (camera.GetZoom() / Window::GetHeight()); } 
    
    glm::vec3 ScreenToWorldCoords(const glm::vec2& p, bool invertY = true) 
    {
        glm::vec2 screenCoords = invertY ? Window::InvertYCoord(p) : p;// + glm::vec2(0.0f, 1.0f); // TODO DO WE NEED TO OFFSET ???
        return camera.GetWorldPosition(screenCoords);      
    };

    std::optional<glm::vec2> WorldToScreenCoords(const glm::vec3& p, bool invertY = true) {
        
        auto [success, position] = camera.GetScreenCoords(glm::vec3({ p.x, p.y, 0.0f }));
        
        if (!success) { return std::nullopt; }
        
        return std::optional<glm::vec2>{ invertY ? Window::InvertYCoord(position) : position };
    };

    void Render();
    void RenderLoop(GLSystem& glsys, std::function<void()> cb_Configure_GL, std::function<void()> cb_Draw_ImGui, std::function<void()> cb_Set_DynamicBuffers);
    
private:
    
    std::unique_ptr<EventHandler<Event_WindowResize>> event_WindowResize;
    std::unique_ptr<EventHandler<Event_MouseScroll>> event_MouseScroll;
    std::unique_ptr<EventHandler<Event_MouseMove>> event_MouseDrag;
    
    // Projection / View matrices
    glm::mat4 m_Proj;
    glm::mat4 m_View;
    
    // Create material picker and set initial value
    //~ MaterialPicker m_MaterialPicker;
    //~ Material m_Material = m_MaterialPicker.GetCurrentMaterial();
    

};
} // End namespace GLViewer
