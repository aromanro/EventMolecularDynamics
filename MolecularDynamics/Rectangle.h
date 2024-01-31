#pragma once


#include <gl/glew.h>
#include <gl/GL.h>      // OpenGL Libraries
#include <gl/GLU.h>     // GLU OpenGL Libraries

#include "VertexBufferRenderable.h"

namespace OpenGL {

	class Rectangle : public VertexBufferRenderable
	{
	public:
		Rectangle(float aspectRatio = 1.0f);
		~Rectangle() override;

		void Draw() override;
	};

}

