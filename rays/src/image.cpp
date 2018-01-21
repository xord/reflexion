#include "rays/image.h"


#include <math.h>
#include <assert.h>
#include "rays/exception.h"
#include "rays/debug.h"
#include "opengl.h"
#include "bitmap.h"
#include "texture.h"


namespace Rays
{


	struct Image::Data
	{

		int width = 0, height = 0;

		ColorSpace color_space;

		float pixel_density = 1;

		mutable Bitmap bitmap;

		mutable Texture texture;

	};// Image::Data


	static void
	invalidate_texture (Image* image)
	{
		image->bitmap();// update bitmap
		image->self->texture = Texture();
	}

	static void
	invalidate_texture_for_another_context (Image* image)
	{
		assert(image);

		const Texture& tex = image->self->texture;
		if (!tex) return;

		Context tex_context = tex.context();
		if (!tex_context) return;

		Context current_context = OpenGL_get_context();
		if (tex_context == current_context)
			return;

		OpenGL_set_context(tex_context);
		invalidate_texture(image);
		OpenGL_set_context(current_context);
	}

	static Bitmap&
	get_bitmap (Image& image)
	{
		Image::Data* self = image.self.get();

		if (!image)
		{
			assert(!self->bitmap);
			return self->bitmap;
		}

		if (!self->bitmap)
		{
			if (self->texture)
				self->bitmap = Bitmap_from(self->texture);
			else
				self->bitmap = Bitmap(self->width, self->height, self->color_space);
		}
		else if (
			self->texture &&
			self->texture.modified() &&
			!Bitmap_get_modified(self->bitmap))
		{
			self->bitmap = Bitmap_from(self->texture);
		}

		if (self->bitmap)  Bitmap_set_modified(&self->bitmap, false);
		if (self->texture) self->texture.set_modified(false);

		return self->bitmap;
	}

	Texture&
	Image_get_texture (Image& image)
	{
		Image::Data* self = image.self.get();

		if (!image)
		{
			assert(!self->texture);
			return self->texture;
		}

		invalidate_texture_for_another_context(&image);

		if (!self->texture)
		{
			if (self->bitmap)
				self->texture = Texture(self->bitmap);
			else
			{
				self->texture = Texture(self->width, self->height, self->color_space);

				Painter p = image.painter();
				p.begin();
				p.clear();
				p.end();
			}
		}
		else if (
			self->bitmap &&
			Bitmap_get_modified(self->bitmap) &&
			!self->texture.modified())
		{
			self->texture = Texture(self->bitmap);
		}

		if (self->bitmap)  Bitmap_set_modified(&self->bitmap, false);
		if (self->texture) self->texture.set_modified(false);

		return self->texture;
	}

	const Texture&
	Image_get_texture (const Image& image)
	{
		return Image_get_texture(const_cast<Image&>(image));
	}

	void
	save_image (const Image& image, const char* path)
	{
		if (!image)
			argument_error(__FILE__, __LINE__);

		return Bitmap_save(image.bitmap(), path);
	}

	Image
	load_image (const char* path)
	{
		return Image(Bitmap_load(path));
	}


	Image::Image ()
	{
	}

	Image::Image (
		int width, int height, const ColorSpace& cs, float pixel_density)
	{
		if (pixel_density <= 0)
			argument_error(__FILE__, __LINE__, "invalid pixel_density.");

		coord w = width  * pixel_density;
		coord h = height * pixel_density;
		if ((w - (int) w) != 0 || (h - (int) h) != 0)
			argument_error(__FILE__, __LINE__, "invalid size for image.");

		self->width         = w;
		self->height        = h;
		self->color_space   = cs;
		self->pixel_density = pixel_density;
	}

	Image::Image (const Bitmap& bitmap, float pixel_density)
	{
		if (pixel_density <= 0)
			argument_error(__FILE__, __LINE__, "invalid pixel_density.");

		self->bitmap        = bitmap;
		self->width         = bitmap.width();
		self->height        = bitmap.height();
		self->color_space   = bitmap.color_space();
		self->pixel_density = pixel_density;
	}

	Image::~Image ()
	{
	}

	Image
	Image::copy () const
	{
		return Image(bitmap(), pixel_density());
	}

	coord
	Image::width () const
	{
		return self->width / self->pixel_density;
	}

	coord
	Image::height () const
	{
		return self->height / self->pixel_density;
	}

	const ColorSpace&
	Image::color_space () const
	{
		return self->color_space;
	}

	float
	Image::pixel_density () const
	{
		return self->pixel_density;
	}

	Painter
	Image::painter ()
	{
		Painter p;
		p.bind(*this);
		return p;
	}

	Bitmap&
	Image::bitmap ()
	{
		return get_bitmap(*this);
	}

	const Bitmap&
	Image::bitmap () const
	{
		return get_bitmap(const_cast<Image&>(*this));
	}

	Image::operator bool () const
	{
		return
			self->width         > 0 &&
			self->height        > 0 &&
			self->pixel_density > 0 &&
			self->color_space;
	}

	bool
	Image::operator ! () const
	{
		return !operator bool();
	}


}// Rays
