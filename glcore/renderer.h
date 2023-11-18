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
	uint primitive 	= 0; // GL_POINTS, GL_TRIANGLES etc
	uint offset 	= 0;	
	uint drawCount 	= 0;
    };

    static void Clear();
    static void Draw(uint primitive, const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
    static void Draw(uint primitive, const VertexArray& va, const IndexBuffer& ib, const Shader& shader, uint offset, uint drawCount);	
    static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, std::vector<ElementData> data)
};
