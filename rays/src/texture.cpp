#include "texture.h"


#include <assert.h>
#include "rays/exception.h"
#include "rays/bitmap.h"
#include "rays/debug.h"
#include "opengl.h"
#include "color_space.h"
#include "frame_buffer.h"


namespace Rays
{


	struct Texture::Data
	{

		Context context = NULL;

		GLuint id = 0;

		int width, height, width_pow2, height_pow2;

		ColorSpace color_space;

		bool modified;

		Data ()
		{
			clear();
		}

		~Data ()
		{
			clear();
		}

		void clear ()
		{
			delete_texture();

			width       =
			height      =
			width_pow2  =
			height_pow2 = 0;
			color_space = COLORSPACE_UNKNOWN;
			modified    = false;
		}

		void delete_texture ()
		{
			if (!has_id()) return;

			Context current_context = OpenGL_get_context();

			assert(context);
			OpenGL_set_context(context);

			glDeleteTextures(1, &id);

			OpenGL_set_context(current_context);

			context = NULL;
			id      = 0;
		}

		bool has_id () const
		{
			return context && id > 0;
		}

	};// Texture::Data


	static int
	min_pow2 (int num)
	{
		int n = 1;
		while (n < num) n *= 2;
		return n;
	}

	static void
	setup_texture (Texture::Data* self, const void* pixels = NULL)
	{
		assert(self && !self->has_id());

		if (self->context)
			invalid_state_error(__FILE__, __LINE__);

		self->context = OpenGL_get_context();
		if (!self->context)
			opengl_error(__FILE__, __LINE__);

		glGenTextures(1, &self->id);
		glBindTexture(GL_TEXTURE_2D, self->id);
		if (glIsTexture(self->id) == GL_FALSE)
			opengl_error(__FILE__, __LINE__, "failed to create texture.");

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_LINEAR);

		GLenum format, type;
		ColorSpace_get_gl_format_and_type(&format, &type, self->color_space);

		glTexImage2D(
			GL_TEXTURE_2D, 0, format, self->width_pow2, self->height_pow2, 0,
			format, type, pixels);

		check_error(__FILE__, __LINE__);
	}

	template <int BytesPerPixel>
	static inline void
	copy_pixel (uchar* dest, const uchar* src)
	{
		*dest = *src;
		copy_pixel<BytesPerPixel - 1>(dest + 1, src + 1);
	}

	template <>
	inline void
	copy_pixel<1> (uchar* dest, const uchar* src)
	{
		*dest = *src;
	}

	template <>
	inline void
	copy_pixel<2> (uchar* dest, const uchar* src)
	{
		assert(sizeof(ushort) == 2);
		*(ushort*) dest = *(ushort*) src;
	}

	template <>
	inline void
	copy_pixel<4> (uchar* dest, const uchar* src)
	{
		assert(sizeof(uint) == 4);
		*(uint*) dest = *(uint*) src;
	}

	template <int BytesPerPixel>
	static inline void
	copy_pixels (
		size_t width,
		uchar* dest, size_t dest_stride, const uchar* src, size_t src_stride)
	{
		if (!dest || !src || dest_stride <= 0 || src_stride <= 0)
			argument_error(__FILE__, __LINE__);

		while (width--)
		{
			copy_pixel<BytesPerPixel>(dest, src);
			dest += dest_stride;
			src  += src_stride;
		}
	}

	static inline void
	copy_pixels (
		size_t Bpp, size_t width,
		uchar* dest, size_t dest_stride, const uchar* src, size_t src_stride)
	{
		switch (Bpp)
		{
			case 1: copy_pixels<1>(width, dest, dest_stride, src, src_stride); break;
			case 2: copy_pixels<2>(width, dest, dest_stride, src, src_stride); break;
			case 3: copy_pixels<3>(width, dest, dest_stride, src, src_stride); break;
			case 4: copy_pixels<4>(width, dest, dest_stride, src, src_stride); break;
		}
	}

	static void
	copy_bitmap (Bitmap* dest, const Bitmap& src)
	{
		if (!dest || !src)
			argument_error(__FILE__, __LINE__);

		int width    = std::min(src.width(),  dest->width());
		int height   = std::min(src.height(), dest->height());
		int src_Bpp  = src.color_space().Bpp();
		int dest_Bpp = dest->color_space().Bpp();

		for (int y = 0; y < height; ++y)
		{
			copy_pixels(
				src_Bpp, width,
				dest->at<uchar>(0, y), dest_Bpp,
				src.  at<uchar>(0, y),  src_Bpp);
		}
	}


	Texture::Texture ()
	{
	}

	Texture::Texture (int width, int height, const ColorSpace& cs)
	{
		if (width <= 0 || height <= 0 || !cs)
			argument_error(__FILE__, __LINE__);

		self->width       = width;
		self->height      = height;
		self->width_pow2  = min_pow2(width);
		self->height_pow2 = min_pow2(height);
		self->color_space = cs;
		self->modified    = true;

		setup_texture(self.get());
	}

	Texture::Texture (const Bitmap& bitmap)
	{
		if (!bitmap)
			argument_error(__FILE__, __LINE__);

		self->width       = bitmap.width();
		self->height      = bitmap.height();
		self->width_pow2  = min_pow2(self->width);
		self->height_pow2 = min_pow2(self->height);
		self->color_space = bitmap.color_space();
		self->modified    = true;

		Bitmap bmp = bitmap;
		if (
			self->width_pow2  != self->width ||
			self->height_pow2 != self->height)
		{
			bmp = Bitmap(self->width_pow2, self->height_pow2, self->color_space);
			if (!bmp)
				rays_error(__FILE__, __LINE__);

			copy_bitmap(&bmp, bitmap);
		}

		setup_texture(self.get(), bmp.pixels());
	}

	Texture::~Texture ()
	{
	}

	int
	Texture::width () const
	{
		return self->width;
	}

	int
	Texture::reserved_width () const
	{
		return self->width_pow2;
	}

	int
	Texture::height () const
	{
		return self->height;
	}

	int
	Texture::reserved_height () const
	{
		return self->height_pow2;
	}

	const ColorSpace&
	Texture::color_space () const
	{
		return self->color_space;
	}

	Context
	Texture::context () const
	{
		return self->context;
	}

	GLuint
	Texture::id () const
	{
		return self->id;
	}

	void
	Texture::set_modified (bool modified)
	{
		self->modified = modified;
	}

	bool
	Texture::modified () const
	{
		return self->modified;
	}

	Texture::operator bool () const
	{
		return
			self->has_id()   &&
			self->width  > 0 &&
			self->height > 0 &&
			self->color_space;
	}

	bool
	Texture::operator ! () const
	{
		return !operator bool();
	}


}// Rays
