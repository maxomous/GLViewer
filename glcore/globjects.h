#pragma once

#include <vector>

enum class BufferType {
	None,
	Static, 	// For data that is not expected to change frequently. Data must be provided when constructing
	Dynamic, 	// For data that may change frequently, such as animations or real-time updates. Do not supply data when constructing, instead call DynamicUpdate when required
	Stream 		// For data that changes every frame. Do not supply data when constructing, instead call DynamicUpdate every frame
};

class VertexBuffer
{
public:
	// static
	VertexBuffer(BufferType type, uint size, const void* data);
	// dynamic
	VertexBuffer(BufferType type, uint size);
	~VertexBuffer();
    
	void Bind() const;
	void Unbind() const;
	void DynamicUpdate(GLintptr offset, GLsizeiptr size, const void* data) const;
	
private:
	uint m_RendererID;
    BufferType m_BufferType;
    void Resize(uint size, const void* data = nullptr) ;
};


class IndexBuffer
{
public:
	IndexBuffer(uint count, const uint* data);
	~IndexBuffer();
	
	void Bind() const;
	void Unbind() const;
	
	inline uint GetCount() const { return m_Count; };
	
private:
	uint m_RendererID;
	uint m_Count;
    void Resize(uint count, const uint* data);
};

struct VertexBufferElement
{
	uint type;
	uint count;
	unsigned char normalised;
	// this should be removed for glsl(3.3+)
	uint id;
	
	static uint GetSizeOfType(uint type);
};

template<typename T>
struct identity { typedef T type; };

class VertexBufferLayout
{
public:
	VertexBufferLayout()
		: m_Stride(0) {}
		
	template <typename T>
	void Push(uint id, uint count)
	{
		Push(id, count, identity<T>());
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; };
	inline uint GetStride() const { return m_Stride; };
	
private:
	std::vector<VertexBufferElement> m_Elements;
	uint m_Stride;
	
	
	template <typename T>
	void Push(uint id, uint count, identity<T>)
	{
        std::cerr << "Error: Unknown push type" << std::endl;
		exit(1);
	}
	void Push(uint id, uint count, identity<float>)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE, id });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}
	void Push(uint id, uint count, identity<uint>)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE, id });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}
	void Push(uint id, uint count, identity<unsigned char>)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE, id });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}
};


class VertexArray
{
public:
	VertexArray();
	~VertexArray();
	
	void Bind() const;
	void Unbind() const;
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	
private:
	uint m_RendererID;
};
