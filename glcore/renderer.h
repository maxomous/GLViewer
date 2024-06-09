#pragma once

#include <stdlib.h>

//forward declaration
class VertexArray;
class IndexBuffer;
class Shader;
   
class Renderer
{
public:
    struct ElementData 
    {
	ElementData(uint primi, uint offs, uint count) 
	    : primitive(primi), offset(offs), drawCount(count) 
	{}
	uint primitive 	= 0; // GL_POINTS, GL_TRIANGLES etc
	uint offset 	= 0; // Where to render from
	uint drawCount 	= 0; // number of elements to render
    };

    static void Clear();
    static void Draw(uint primitive, const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
    static void Draw(uint primitive, const VertexArray& va, const IndexBuffer& ib, const Shader& shader, uint offset, uint drawCount);	
    static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, std::vector<ElementData>& data);
};
