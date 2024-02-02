#pragma once
#include "OpenGLObject.h"

namespace OpenGL {

	class Texture :
		public OpenGLObject
	{
	public:
		Texture();
		~Texture() override;

		void Bind() override;
		void Bind(int nr);

		void UnBind() override;
		GLuint getType() const override;

		virtual void setData(const void *data, int width, int height, int nr = 0, int nrBytes = 3);
		void GenerateMipmaps();
	};


	class PixelBuffer;

	class TextureWithPixelBuffer : public Texture
	{
	public:
		~TextureWithPixelBuffer() override;

		void setData(const void* data, int width, int height, int nr = 0, int nrBytes = 3) override;
		void Draw();

	private:
		PixelBuffer* pixelBuffer = nullptr;

		int m_width = 0;
		int m_height = 0;
		int m_nrBytes = 0;
	};

}
