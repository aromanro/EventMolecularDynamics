#pragma once


#include "Program.h"
#include "Texture.h"
#include "VertexBufferRenderable.h"

#include "SkyBase.h"

#include <glm.hpp>

namespace OpenGL {

	class SkySphereProgram : public SkyBase
	{
	public:
		SkySphereProgram();
		~SkySphereProgram() override;

		bool LoadTexture(const char* name = "Textures\\skysphere.jpg");

		void Draw(const glm::mat4& mat) override;
		bool SetShaders() override;

	protected:
		class SkySphere : public VertexBufferRenderable
		{
		public:
			SkySphere(float r = 1., unsigned int lats = 32, unsigned int longs = 32);

			void Draw() override;

		private:
			unsigned int m_lats;
			unsigned int m_longs;
		};

		bool LoadTextureData(const CString& filename);

	private:
		Texture* sphereTexture;
		SkySphere* skySphere;

		GLint textureLoc;
	};

}

