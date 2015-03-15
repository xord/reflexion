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
					if (create)
						p = reset(new Wrapper());
					else
						invalid_state_error(__FILE__, __LINE__);
				}
				return p->get();
			}

			const Value& value (bool create = false) const
			{
				return const_cast<This*>(this)->value(create);
			}

			void set_inherited (bool state = true)
			{
				if (pointer()) pwrapper = Xot::set_pointer_flag(pwrapper, state);
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

			      Wrapper* pointer ()       {return Xot::set_pointer_flag(pwrapper, false);}

			const Wrapper* pointer () const {return const_cast<StyleValue*>(this)->pointer();}

	};// StyleValue


	struct Style::Data : public Xot::RefCountable<>
	{

		typedef StyleValue<bool>         Bool;

		typedef StyleValue<int>          Int;

		typedef StyleValue<double>       Float;

		typedef StyleValue<StyleLength>  Length;

		typedef StyleValue<StyleLength2> Length2;

		typedef StyleValue<StyleLength4> Length4;

		typedef StyleValue<Color>        Color;

		typedef StyleValue<Image>        Image;

		View* owner;

		Selector selector;

		Int flow_main, flow_sub;

		Length2 size;

		Length4 position, offset, margin, padding;

		Color background_color;

		Image background_image;

		Data () : owner(NULL) {}

	};// Style::Data


	namespace Zero
	{

		static const StyleLength  length;

		static const StyleLength2 length2;

		static const StyleLength4 length4;

		static const Color        color(0);

		static const Image        image;

	}// Zero


	static Style::Data*
	get_data (Style* this_, bool create = false)
	{
		if (!this_)
			argument_error(__FILE__, __LINE__);

		if (create && !this_->ref)
			this_->ref.reset(new Style::Data());

		return this_->ref.get();
	}

	static const Style::Data*
	get_data (const Style* this_)
	{
		return get_data(const_cast<Style*>(this_));
	}


	Style::Style (const char* name)
	{
		if (name) set_name(name);
	}

	Style::Style (const This& obj)
	:	ref(obj.ref)
	{
	}

	Style&
	Style::operator = (const This& obj)
	{
		if (&obj != this) ref = obj.ref;
		return *this;
	}

	Style::~Style ()
	{
	}

	void
	Style::set_name (const char* name)
	{
		get_data(this, true)->selector.set_name(name);
	}

	const char*
	Style::name () const
	{
		const Data* data = get_data(this);
		return data ? data->selector.name() : NULL;
	}

	void
	Style::add_tag (const char* tag)
	{
		get_data(this, true)->selector.add_tag(tag);
	}

	void
	Style::remove_tag (const char* tag)
	{
		Data* data = get_data(this);
		if (!data) return;

		data->selector.remove_tag(tag);
	}

	static Selector::TagSet empty_tags;

	Selector::iterator
	Style::tag_begin ()
	{
		Data* data = get_data(this);
		return data ? data->selector.begin() : empty_tags.begin();
	}

	Selector::const_iterator
	Style::tag_begin () const
	{
		const Data* data = get_data(this);
		return data ? data->selector.begin() : empty_tags.begin();
	}

	Selector::iterator
	Style::tag_end ()
	{
		Data* data = get_data(this);
		return data ? data->selector.end() : empty_tags.end();
	}

	Selector::const_iterator
	Style::tag_end () const
	{
		const Data* data = get_data(this);
		return data ? data->selector.end() : empty_tags.end();
	}

	void
	Style::set_selector (const Selector& selector)
	{
		get_data(this, true)->selector = selector;
	}

	Selector&
	Style::selector ()
	{
		return get_data(this, true)->selector;
	}

	const Selector&
	Style::selector () const
	{
		static const Selector EMPTY;

		const Data* data = get_data(this);
		return data ? data->selector : EMPTY;
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
		}
		return FLOW_INVALID;
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

		Data* data = get_data(this, true);
		data->flow_main = main;
		data->flow_sub  = sub;
	}

	void
	Style::get_flow (Flow* main, Flow* sub) const
	{
		if (!main && !sub)
			argument_error(__FILE__, __LINE__);

		const Data* data = get_data(this);
		if (!data || (!data->flow_main && !data->flow_sub))
		{
			if (main) *main = FLOW_DOWN;
			if (sub)  *sub  = FLOW_NONE;
		}
		else if (data->flow_main && data->flow_sub)
		{
			if (main) *main = (Flow) data->flow_main.value();
			if (sub)  *sub  = (Flow) data->flow_sub.value();
		}
		else
			invalid_state_error(__FILE__, __LINE__);
	}

	void
	Style::set_size (const StyleLength2& size)
	{
		get_data(this, true)->size = size;
	}

	void
	Style::set_width (const StyleLength& width)
	{
		get_data(this, true)->size.value(true).set_width(width);
	}

	void
	Style::set_height (const StyleLength& height)
	{
		get_data(this, true)->size.value(true).set_height(height);
	}

	const StyleLength2&
	Style::size () const
	{
		const Data* data = get_data(this);
		return data && data->size ? data->size.value() : Zero::length2;
	}

	void
	Style::set_position (const StyleLength4& position)
	{
		get_data(this, true)->position = position;
	}

	void
	Style::set_left (const StyleLength& left)
	{
		get_data(this, true)->position.value(true).set_left(left);
	}

	void
	Style::set_top (const StyleLength& top)
	{
		get_data(this, true)->position.value(true).set_top(top);
	}

	void
	Style::set_right (const StyleLength& right)
	{
		get_data(this, true)->position.value(true).set_right(right);
	}

	void
	Style::set_bottom (const StyleLength& bottom)
	{
		get_data(this, true)->position.value(true).set_bottom(bottom);
	}

	const StyleLength4&
	Style::position () const
	{
		const Data* data = get_data(this);
		return data && data->position ? data->position.value() : Zero::length4;
	}

	void
	Style::set_offset (const StyleLength4& offset)
	{
		get_data(this, true)->offset = offset;
	}

	void
	Style::set_offset_left (const StyleLength& left)
	{
		get_data(this, true)->offset.value(true).set_left(left);
	}

	void
	Style::set_offset_top (const StyleLength& top)
	{
		get_data(this, true)->offset.value(true).set_top(top);
	}

	void
	Style::set_offset_right (const StyleLength& right)
	{
		get_data(this, true)->offset.value(true).set_right(right);
	}

	void
	Style::set_offset_bottom (const StyleLength& bottom)
	{
		get_data(this, true)->offset.value(true).set_bottom(bottom);
	}

	const StyleLength4&
	Style::offset () const
	{
		const Data* data = get_data(this);
		return data && data->offset ? data->offset.value() : Zero::length4;
	}

	void
	Style::set_margin (const StyleLength4& margin)
	{
		get_data(this, true)->margin = margin;
	}

	void
	Style::set_margin_left (const StyleLength& left)
	{
		get_data(this, true)->margin.value(true).set_left(left);
	}

	void
	Style::set_margin_top (const StyleLength& top)
	{
		get_data(this, true)->margin.value(true).set_top(top);
	}

	void
	Style::set_margin_right (const StyleLength& right)
	{
		get_data(this, true)->margin.value(true).set_right(right);
	}

	void
	Style::set_margin_bottom (const StyleLength& bottom)
	{
		get_data(this, true)->margin.value(true).set_bottom(bottom);
	}

	const StyleLength4&
	Style::margin () const
	{
		const Data* data = get_data(this);
		return data && data->margin ? data->margin.value() : Zero::length4;
	}

	void
	Style::set_padding (const StyleLength4& padding)
	{
		get_data(this, true)->padding = padding;
	}

	void
	Style::set_padding_left (const StyleLength& left)
	{
		get_data(this, true)->padding.value(true).set_left(left);
	}

	void
	Style::set_padding_top (const StyleLength& top)
	{
		get_data(this, true)->padding.value(true).set_top(top);
	}

	void
	Style::set_padding_right (const StyleLength& right)
	{
		get_data(this, true)->padding.value(true).set_right(right);
	}

	void
	Style::set_padding_bottom (const StyleLength& bottom)
	{
		get_data(this, true)->padding.value(true).set_bottom(bottom);
	}

	const StyleLength4&
	Style::padding () const
	{
		const Data* data = get_data(this);
		return data && data->padding ? data->padding.value() : Zero::length4;
	}

	void
	Style::set_background_color (const Color& value)
	{
		get_data(this, true)->background_color = value;
	}

	const Color&
	Style::background_color () const
	{
		const Data* data = get_data(this);
		return data && data->background_color ? data->background_color.value() : Zero::color;
	}

	void
	Style::set_background_image (const Image& value)
	{
		get_data(this, true)->background_image = value;
	}

	const Image&
	Style::background_image () const
	{
		const Data* data = get_data(this);
		return data && data->background_image ? data->background_image.value() : Zero::image;
	}

	bool
	operator == (const Style& lhs, const Style& rhs)
	{
		return lhs.ref == rhs.ref;
	}

	bool
	operator != (const Style& lhs, const Style& rhs)
	{
		return !operator==(lhs, rhs);
	}


}// Reflex
