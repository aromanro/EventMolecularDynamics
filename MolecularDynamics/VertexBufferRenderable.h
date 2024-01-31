#pragma once
#include "Renderable.h"

namespace OpenGL {

	class VertexBufferRenderable :
		public Renderable
	{
	public:
		VertexBufferRenderable();
		~VertexBufferRenderable() override;

		void Bind() override;
		void UnBind() override;

		void setData(void *data, unsigned int len, GLenum type = GL_STATIC_DRAW);

	private:
		VertexBufferObject vbo;
	};

}
