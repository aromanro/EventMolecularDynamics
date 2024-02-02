#pragma once

#include "VertexBufferRenderable.h"

namespace OpenGL {

	class Sphere : public VertexBufferRenderable
	{
	public:
		Sphere(float r = 1., unsigned int lats = 64, unsigned int longs = 64, bool useTexture = true);

		void Draw() override;
		void DrawInstanced(unsigned int count);

	private:
		unsigned int m_lats;
		unsigned int m_longs;
	};

}
