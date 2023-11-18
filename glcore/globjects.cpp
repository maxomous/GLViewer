
#include <iostream>
#include <string>

#include "glcore.h"
#include "globjects.h"

// static
VertexBuffer::VertexBuffer(BufferType type, uint size, const void* data) 
    : m_BufferType(type)
{
	// generate a vertex buffer
    GLCall(glGenBuffers(1, &m_RendererID));
    // set size of buffer
    Resize(size, data);
}
// dynamic
VertexBuffer::VertexBuffer(BufferType type, uint size)
    : m_BufferType(type)
{
    // generate a vertex buffer
    GLCall(glGenBuffers(1, &m_RendererID));
    // set size of buffer
    Resize(size);
}
VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}
void VertexBuffer::Resize(uint size, const void* data) 
{
    // bind the buffer
    Bind();
    // state how data should be read
    if(m_BufferType == BufferType::Dynamic) {
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW)); // use for dynamic for constantly changing
    } 
    else if(m_BufferType == BufferType::Stream) {
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STREAM_DRAW)); // use for changing every frame
    } 
    else if(m_BufferType == BufferType::Static) {
	assert((data != nullptr) && "Data is invalid");
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW)); // use for data which doesnt change often
    } else {
	assert("VertexBuffer Type Unknown");
    }
}
void VertexBuffer::Bind() const
{
    // bind data to buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}
void VertexBuffer::Unbind() const
{
    // bind data to buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::DynamicUpdate(GLintptr offset, GLsizeiptr size, const void* data) const
{
    assert(((m_BufferType == BufferType::Dynamic) || (m_BufferType == BufferType::Stream)) && "VertexBuffer must be Dynamic or Stream");
    Bind();
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}



IndexBuffer::IndexBuffer(uint count, const uint* data)
    : m_Count(count)
{
    ASSERT(sizeof(uint) == sizeof(GLuint));
    // generate an index buffer object
    GLCall(glGenBuffers(1, &m_RendererID));
    Resize(count, data);
}
IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}
void IndexBuffer::Resize(uint count, const uint* data) 
{
    // bind the buffer
    Bind();
    // state how data should be read
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, GL_STATIC_DRAW));
}
void IndexBuffer::Bind() const
{
    // bind data to buffer
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}
void IndexBuffer::Unbind() const
{
    // bind data to buffer
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}



uint VertexBufferElement::GetSizeOfType(uint type) 
{
    switch (type)
    {
	    case GL_FLOAT: 		return 4;
	    case GL_UNSIGNED_INT: 	return 4;
	    case GL_UNSIGNED_BYTE: 	return 1;
    }
    ASSERT(false);
    return 0;
}


VertexArray::VertexArray()
{
    GLCall(glGenVertexArrays(1, &m_RendererID));
	
} 
VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::Bind() const
{
    GLCall(glBindVertexArray(m_RendererID));
}
 
void VertexArray::Unbind() const
{
    GLCall(glBindVertexArray(0));
}
	
void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElements();
    uint offset = 0;
    for (uint i = 0; i < elements.size(); i++)
    {
	const auto& element = elements[i];
	
	GLCall(glEnableVertexAttribArray(element.id));
	GLCall(glVertexAttribPointer(element.id, element.count, element.type, element.normalised, layout.GetStride(), (const void*)offset));
	/* glsl (3.3+)
	GLCall(glEnableVertexAttribArray(i));
	GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.GetStride(), (const void*)offset));
	*/
	offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}
