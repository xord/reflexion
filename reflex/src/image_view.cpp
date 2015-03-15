#include "reflex/image_view.h"


#include <assert.h>
#include "reflex/exception.h"


namespace Reflex
{


	struct ImageView::Data
	{

		Image image;

	};// ImageView::Data


	ImageView::ImageView (const char* name)
	:	Super(name)
	{
	}

	ImageView::~ImageView ()
	{
	}

	void
	ImageView::set_image (Image image)
	{
		self->image = image;
		redraw();
	}

	Image
	ImageView::get_image () const
	{
		return self->image;
	}

	Point
	ImageView::content_size () const
	{
		if (self->image)
			return Point(self->image.width(), self->image.height());
		else
			return Super::content_size();
	}

	void
	ImageView::on_draw (DrawEvent* e)
	{
		if (!self->image) return;

		assert(e && e->painter);
		Painter* p = e->painter;

		Color f = p->fill(), s = p->stroke();
		p->set_fill(1);
		p->no_stroke();

		p->image(self->image);

		p->set_fill(f);
		p->set_stroke(s);
	}


}// Reflex
