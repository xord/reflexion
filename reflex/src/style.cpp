#include "reflex/style.h"


#include <boost/noncopyable.hpp>
#include "reflex/view.h"
#include "reflex/exception.h"


namespace Reflex
{


	template <typename T>
	class StyleValue
	{

		typedef StyleValue<T> This;

		typedef T Value;

		class Wrapper : public boost::noncopyable, public Xot::RefCountable<>
		{

			public:

				Wrapper () {}

				Wrapper (const Value& value) : value(value) {}

				void reset (const Value& value_) {value = value_;}

				      Value& get ()       {return value;}

				const Value& get () const {return value;}

			private:

				Value value;

		};// Wrapper

		public:

			StyleValue ()
			:	pwrapper(NULL)
			{
			}

			StyleValue (const Value& value)
			:	pwrapper(NULL)
			{
				operator=(value);
			}

			StyleValue (const This& obj)
			:	pwrapper(NULL)
			{
				operator=(obj);
			}

			This& operator = (const Value& value)
			{
				reset(new Wrapper(value));
				return *this;
			}

			This& operator = (const This& obj)
			{
				if (&obj != this) reset(obj.pwrapper);
				return *this;
			}

			~StyleValue ()
			{
				clear();
			}

			void clear ()
			{
				reset();
			}

			Value& value (bool create = false)
			{
				Wrapper* p = pointer();
				if (!p)
				{
					if (!create)
						invalid_state_error(__FILE__, __LINE__);

					p = reset(new Wrapper());
				}
				return p->get();
			}

			const Value& get (const Value& defval) const
			{
				Wrapper* p = pointer();
				return p ? p->get() : defval;
			}

			bool set (const Value& val)
			{
				Value& current = value(true);
				if (current == val)
					return false;

				current = val;
				return true;
			}

			void override (const This& value)
			{
				if (!value || (*this && !is_inherited()))
					return;

				reset(value.pointer());
				set_inherited();
			}

			bool is_inherited () const
			{
				return get_pointer_flag(pwrapper);
			}

			operator bool () const {return pointer() != NULL;}

			bool operator ! () const {return !operator bool();}

		private:

			Wrapper* pwrapper;

			Wrapper* reset (Wrapper* ptr = NULL)
			{
				Wrapper* p = pointer();
				if (ptr != p)
				{
					if (p) p->release();
					pwrapper = p = ptr;
					if (p) p->retain();
				}
				return p;
			}

			void set_inherited (bool state = true)
			{
				if (pwrapper)
					pwrapper = Xot::set_pointer_flag(pwrapper, state);
			}

			Wrapper* pointer () const {return Xot::set_pointer_flag(pwrapper, false);}

	};// StyleValue


	struct StyleLength::Data
	{

		Value value;

		Unit unit;

		Data ()
		:	value(0), unit(NONE)
		{
		}

		bool is_variable () const
		{
			return unit == PERCENT;
		}

		friend bool operator == (const Data& lhs, const Data& rhs)
		{
			return lhs.value == rhs.value && lhs.unit == rhs.unit;
		}

	};// StyleLength::Data


	StyleLength::StyleLength ()
	{
	}

	StyleLength::StyleLength (Value value, Unit unit)
	{
		reset(value, unit);
	}

	StyleLength::StyleLength (const char* str)
	{
		reset(str);
	}

	StyleLength
	StyleLength::copy () const
	{
		return StyleLength(value(), unit());
	}

	void
	StyleLength::reset (Value value, Unit unit)
	{
		if (unit < NONE || UNIT_LAST <= unit)
			argument_error(__FILE__, __LINE__);

		self->value = value;
		self->unit  = unit;
	}

	static StyleLength::Unit
	str2unit (const char* s)
	{
		     if (strcasecmp(s, "px") == 0) return StyleLength::PIXEL;
		else if (strcasecmp(s, "%")  == 0) return StyleLength::PERCENT;
		else                               return StyleLength::NONE;
	}

	static const char*
	unit2str (StyleLength::Unit unit)
	{
		switch (unit)
		{
			case StyleLength::PIXEL:   return "px";
			case StyleLength::PERCENT: return "%";
			default:                   return NULL;
		}
	}

	void
	StyleLength::reset (const char* str)
	{
		Value num;
		char suffix[256];
		int count = sscanf(str, "%f%s", &num, suffix);
		if (count != 2)
			argument_error(__FILE__, __LINE__);

		reset(num, str2unit(suffix));
	}

	StyleLength::Value
	StyleLength::value () const
	{
		return self->value;
	}

	StyleLength::Unit
	StyleLength::unit () const
	{
		return self->unit;
	}

	String
	StyleLength::to_s () const
	{
		if (!*this)
			return "";

		String num;
		if (fmod(self->value, 1) == 0)
			num = Xot::stringf("%d", (long) self->value);
		else
			num = Xot::stringf("%g", self->value);

		const char* suffix = unit2str(self->unit);;
		if (!suffix)
			invalid_state_error(__FILE__, __LINE__);

		return num + suffix;
	}

	StyleLength::operator bool () const
	{
		return NONE < self->unit && self->unit < UNIT_LAST;
	}

	bool
	StyleLength::operator ! () const
	{
		return !operator bool();
	}

	bool
	operator == (const StyleLength& lhs, const StyleLength& rhs)
	{
		return (!lhs && !rhs) || *lhs.self == *rhs.self;
	}

	bool
	operator != (const StyleLength& lhs, const StyleLength& rhs)
	{
		return !operator ==(lhs, rhs);
	}


	void
	get_default_flow (Style::Flow* main, Style::Flow* sub)
	{
		assert(main || sub);

		if (main) *main = Style::FLOW_RIGHT;
		if (sub)  *sub  = Style::FLOW_DOWN;
	}


	struct Style::Data
	{

		typedef StyleValue<bool>        Bool;

		typedef StyleValue<int>         Int;

		typedef StyleValue<double>      Float;

		typedef StyleValue<Color>       Color;

		typedef StyleValue<Image>       Image;

		typedef StyleValue<StyleLength> Length;

		View* owner;

		Selector selector;

		Int flow;

		Length width, height;

		Length         left,         top,         right,         bottom;

		Length  offset_left,  offset_top,  offset_right,  offset_bottom;

		Length  margin_left,  margin_top,  margin_right,  margin_bottom;

		Length padding_left, padding_top, padding_right, padding_bottom;

		Color fill, stroke;

		Image image;

		Data ()
		:	owner(NULL)
		{
		}

		enum FlowOffset {FLOW_MASK = 0xffff, FLOW_SHIFT = 16};

		bool set_flow (Flow main, Flow sub)
		{
			return flow.set((main & FLOW_MASK) | ((sub & FLOW_MASK) << FLOW_SHIFT));
		}

		Flow flow_main () const
		{
			Flow defval = FLOW_NONE;
			get_default_flow(&defval, NULL);
			return (Flow) (flow.get(defval) & FLOW_MASK);
		}

		Flow flow_sub () const
		{
			Flow defval = FLOW_NONE;
			get_default_flow(NULL, &defval);
			return (Flow) ((flow.get(defval << FLOW_SHIFT) >> FLOW_SHIFT) & FLOW_MASK);
		}

	};// Data


	namespace Zero
	{

		static const Color       color;

		static const Image       image;

		static const StyleLength length;

	}// Zero


	bool
	set_style_owner (Style* style, View* owner)
	{
		assert(style);

		if (style->self->owner)
			return false;

		style->self->owner = owner;
		return true;
	}

	static void
	update_owner (const Style& style)
	{
		View* owner = style.self->owner;
		if (!owner) return;

		void update_styles_for_selector (View*, const Selector&);
		update_styles_for_selector(owner, style.self->selector);
	}

	template <typename T>
	static void
	clear_inherited_value (StyleValue<T>* value)
	{
		assert(value);

		if (value->is_inherited())
			value->clear();
	}

	void
	clear_inherited_values (Style* style)
	{
		assert(style);

		Style::Data* self = style->self.get();

		clear_inherited_value(&self->flow);
		clear_inherited_value(&self->width);
		clear_inherited_value(&self->height);
		clear_inherited_value(&self->left);
		clear_inherited_value(&self->top);
		clear_inherited_value(&self->right);
		clear_inherited_value(&self->bottom);
		clear_inherited_value(&self->offset_left);
		clear_inherited_value(&self->offset_top);
		clear_inherited_value(&self->offset_right);
		clear_inherited_value(&self->offset_bottom);
		clear_inherited_value(&self->margin_left);
		clear_inherited_value(&self->margin_top);
		clear_inherited_value(&self->margin_right);
		clear_inherited_value(&self->margin_bottom);
		clear_inherited_value(&self->padding_left);
		clear_inherited_value(&self->padding_top);
		clear_inherited_value(&self->padding_right);
		clear_inherited_value(&self->padding_bottom);
		clear_inherited_value(&self->fill);
		clear_inherited_value(&self->stroke);
		clear_inherited_value(&self->image);
	}

	void
	override_style (Style* overridden, const Style& overrides)
	{
		assert(overridden);

		Style::Data* from  = overrides.self.get();
		Style::Data* to    = overridden->self.get();
		if (!from || !to) return;

		to->flow          .override(from->flow);
		to->width         .override(from->width);
		to->height        .override(from->height);
		to->left          .override(from->left);
		to->top           .override(from->top);
		to->right         .override(from->right);
		to->bottom        .override(from->bottom);
		to->offset_left   .override(from->offset_left);
		to->offset_top    .override(from->offset_top);
		to->offset_right  .override(from->offset_right);
		to->offset_bottom .override(from->offset_bottom);
		to->margin_left   .override(from->margin_left);
		to->margin_top    .override(from->margin_top);
		to->margin_right  .override(from->margin_right);
		to->margin_bottom .override(from->margin_bottom);
		to->padding_left  .override(from->padding_left);
		to->padding_top   .override(from->padding_top);
		to->padding_right .override(from->padding_right);
		to->padding_bottom.override(from->padding_bottom);
		to->fill          .override(from->fill);
		to->stroke        .override(from->stroke);
		to->image         .override(from->image);
	}


	Style::Style (const char* name)
	{
		if (name) set_name(name);
	}

	Style::~Style ()
	{
	}

	void
	Style::set_name (const char* name)
	{
		update_owner(*this);

		self->selector.set_name(name);

		update_owner(*this);
	}

	const char*
	Style::name () const
	{
		return self->selector.name();
	}

	void
	Style::add_tag (const char* tag)
	{
		update_owner(*this);

		self->selector.add_tag(tag);

		update_owner(*this);
	}

	void
	Style::remove_tag (const char* tag)
	{
		update_owner(*this);

		self->selector.remove_tag(tag);

		update_owner(*this);
	}

	Selector::iterator
	Style::tag_begin ()
	{
		return self->selector.begin();
	}

	Selector::const_iterator
	Style::tag_begin () const
	{
		return self->selector.begin();
	}

	Selector::iterator
	Style::tag_end ()
	{
		return self->selector.end();
	}

	Selector::const_iterator
	Style::tag_end () const
	{
		return self->selector.end();
	}

	void
	Style::set_selector (const Selector& selector)
	{
		update_owner(*this);

		self->selector = selector;

		update_owner(*this);
	}

	Selector&
	Style::selector ()
	{
		return self->selector;
	}

	const Selector&
	Style::selector () const
	{
		return self->selector;
	}

	enum FlowDir {FLOW_INVALID = 0, FLOW_H, FLOW_V};

	static FlowDir
	get_flow_dir (Style::Flow flow)
	{
		switch (flow)
		{
			case Style::FLOW_LEFT:
			case Style::FLOW_RIGHT: return FLOW_H;
			case Style::FLOW_UP:
			case Style::FLOW_DOWN:  return FLOW_V;
			default:                return FLOW_INVALID;
		}
	}

	void
	Style::set_flow (Flow main, Flow sub)
	{
		if (
			main <  FLOW_NONE || FLOW_LAST <= main ||
			sub  <  FLOW_NONE || FLOW_LAST <= sub  ||
			(main != FLOW_NONE && (get_flow_dir(main) == get_flow_dir(sub))) ||
			(main == FLOW_NONE && sub != FLOW_NONE))
		{
			argument_error(__FILE__, __LINE__);
		}

		self->set_flow(main, sub);

		update_owner(*this);
	}

	void
	Style::get_flow (Flow* main, Flow* sub) const
	{
		if (!main && !sub)
			argument_error(__FILE__, __LINE__);

		if (main) *main = self->flow_main();
		if (sub)  *sub  = self->flow_sub();
	}

	void
	Style::set_width (const StyleLength& width)
	{
		if (self->width.set(width))
			update_owner(*this);
	}

	void
	Style::set_height (const StyleLength& height)
	{
		if (self->height.set(height))
			update_owner(*this);
	}

	const StyleLength&
	Style::width () const
	{
		return self->width.get(Zero::length);
	}

	const StyleLength&
	Style::height () const
	{
		return self->height.get(Zero::length);
	}

	void
	Style::set_left (const StyleLength& left)
	{
		if (self->left.set(left))
			update_owner(*this);
	}

	void
	Style::set_top (const StyleLength& top)
	{
		if (self->top.set(top))
			update_owner(*this);
	}

	void
	Style::set_right (const StyleLength& right)
	{
		if (self->right.set(right))
			update_owner(*this);
	}

	void
	Style::set_bottom (const StyleLength& bottom)
	{
		if (self->bottom.set(bottom))
			update_owner(*this);
	}

	const StyleLength&
	Style::left () const
	{
		return self->left.get(Zero::length);
	}

	const StyleLength&
	Style::top () const
	{
		return self->top.get(Zero::length);
	}

	const StyleLength&
	Style::right () const
	{
		return self->right.get(Zero::length);
	}

	const StyleLength&
	Style::bottom () const
	{
		return self->bottom.get(Zero::length);
	}

	void
	Style::set_offset_left (const StyleLength& left)
	{
		if (self->offset_left.set(left))
			update_owner(*this);
	}

	void
	Style::set_offset_top (const StyleLength& top)
	{
		if (self->offset_top.set(top))
			update_owner(*this);
	}

	void
	Style::set_offset_right (const StyleLength& right)
	{
		if (self->offset_right.set(right))
			update_owner(*this);
	}

	void
	Style::set_offset_bottom (const StyleLength& bottom)
	{
		if (self->offset_bottom.set(bottom))
			update_owner(*this);
	}

	const StyleLength&
	Style::offset_left () const
	{
		return self->offset_left.get(Zero::length);
	}

	const StyleLength&
	Style::offset_top () const
	{
		return self->offset_top.get(Zero::length);
	}

	const StyleLength&
	Style::offset_right () const
	{
		return self->offset_right.get(Zero::length);
	}

	const StyleLength&
	Style::offset_bottom () const
	{
		return self->offset_bottom.get(Zero::length);
	}

	void
	Style::set_margin_left (const StyleLength& left)
	{
		if (self->margin_left.set(left))
			update_owner(*this);
	}

	void
	Style::set_margin_top (const StyleLength& top)
	{
		if (self->margin_top.set(top))
			update_owner(*this);
	}

	void
	Style::set_margin_right (const StyleLength& right)
	{
		if (self->margin_right.set(right))
			update_owner(*this);
	}

	void
	Style::set_margin_bottom (const StyleLength& bottom)
	{
		if (self->margin_bottom.set(bottom))
			update_owner(*this);
	}

	const StyleLength&
	Style::margin_left () const
	{
		return self->margin_left.get(Zero::length);
	}

	const StyleLength&
	Style::margin_top () const
	{
		return self->margin_top.get(Zero::length);
	}

	const StyleLength&
	Style::margin_right () const
	{
		return self->margin_right.get(Zero::length);
	}

	const StyleLength&
	Style::margin_bottom () const
	{
		return self->margin_bottom.get(Zero::length);
	}

	void
	Style::set_padding_left (const StyleLength& left)
	{
		if (self->padding_left.set(left))
			update_owner(*this);
	}

	void
	Style::set_padding_top (const StyleLength& top)
	{
		if (self->padding_top.set(top))
			update_owner(*this);
	}

	void
	Style::set_padding_right (const StyleLength& right)
	{
		if (self->padding_right.set(right))
			update_owner(*this);
	}

	void
	Style::set_padding_bottom (const StyleLength& bottom)
	{
		if (self->padding_bottom.set(bottom))
			update_owner(*this);
	}

	const StyleLength&
	Style::padding_left () const
	{
		return self->padding_left.get(Zero::length);
	}

	const StyleLength&
	Style::padding_top () const
	{
		return self->padding_top.get(Zero::length);
	}

	const StyleLength&
	Style::padding_right () const
	{
		return self->padding_right.get(Zero::length);
	}

	const StyleLength&
	Style::padding_bottom () const
	{
		return self->padding_bottom.get(Zero::length);
	}

	void
	Style::set_fill (const Color& fill)
	{
		if (self->fill.set(fill))
			update_owner(*this);
	}

	const Color&
	Style::fill () const
	{
		return self->fill.get(Zero::color);
	}

	void
	Style::set_stroke (const Color& stroke)
	{
		if (self->stroke.set(stroke))
			update_owner(*this);
	}

	const Color&
	Style::stroke () const
	{
		return self->stroke.get(Zero::color);
	}

	void
	Style::set_image (const Image& image)
	{
		if (self->image.set(image))
			update_owner(*this);
	}

	const Image&
	Style::image () const
	{
		return self->image.get(Zero::image);
	}

	bool
	operator == (const Style& lhs, const Style& rhs)
	{
		return lhs.self.get() == rhs.self.get();
	}

	bool
	operator != (const Style& lhs, const Style& rhs)
	{
		return !operator==(lhs, rhs);
	}


	static bool
	is_variable (const StyleValue<StyleLength>& length)
	{
		return length && length.get(Zero::length).self->is_variable();
	}

	bool
	has_variable_lengths (const Style& style)
	{
		Style::Data* s = style.self.get();
		if (!s) return false;

		return
			is_variable(s->width)  ||
			is_variable(s->height) ||

			is_variable(s->left)   ||
			is_variable(s->top)    ||
			is_variable(s->right)  ||
			is_variable(s->bottom) ||

			is_variable(s->offset_left)   ||
			is_variable(s->offset_top)    ||
			is_variable(s->offset_right)  ||
			is_variable(s->offset_bottom) ||

			is_variable(s->margin_left)   ||
			is_variable(s->margin_top)    ||
			is_variable(s->margin_right)  ||
			is_variable(s->margin_bottom) ||

			is_variable(s->padding_left)  ||
			is_variable(s->padding_top)   ||
			is_variable(s->padding_right) ||
			is_variable(s->padding_bottom);
	}

	bool
	get_pixel_length (
		coord* pixel_length,
		const StyleLength& style_length, const coord* parent_size)
	{
		assert(pixel_length);

		if (!style_length)
			return false;

		coord length = 0;
		StyleLength::Value value = style_length.value();
		switch (style_length.unit())
		{
			case StyleLength::PIXEL:
				length = value;
				break;

			case StyleLength::PERCENT:
			{
				if (!parent_size)
					argument_error(__FILE__, __LINE__);

				length = (value == 100) ?
					*parent_size : floor(*parent_size * value / 100);
				break;
			}

			default:
				invalid_state_error(__FILE__, __LINE__);
		}

		if (length == *pixel_length)
			return false;

		*pixel_length = length;
		return true;
	}

	static void
	update_frame (View* view, const Style& style)
	{
		assert(view);
		Style::Data* s = style.self.get();

		Bounds frame         = view->frame();
		View* parent_view    = view->parent();
		const Bounds* parent = parent_view ? &parent_view->frame() : NULL;
		bool update          = false;

		if (s->width)
		{
			update |= get_pixel_length(
				&frame.width, s->width.value(), parent ? &parent->width  : NULL);
		}

		if (s->height) {
			update |= get_pixel_length(
				&frame.height, s->height.value(), parent ? &parent->height : NULL);
		}

		if (update)
			view->set_frame(frame);
	}

	void
	apply_style (View* view, const Style& style)
	{
		if (!view)
			argument_error(__FILE__, __LINE__);

		//update_margin(view, values);
		//update_padding(view, values);
		update_frame(view, style);
		//update_background(view, values);
	}


}// Reflex
