#include "painter.h"


#include <math.h>
#include <assert.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "rays/exception.h"
#include "rays/point.h"
#include "rays/bounds.h"
#include "rays/color.h"
#include "rays/debug.h"
#include "opengl.h"
#include "matrix.h"
#include "polygon.h"
#include "bitmap.h"
#include "texture.h"
#include "image.h"
#include "font.h"
#include "frame_buffer.h"
#include "shader.h"
#include "shader_program.h"
#include "shader_source.h"


namespace Rays
{


	enum ColorType
	{

		FILL = 0,
		STROKE,

		COLOR_TYPE_END,
		COLOR_TYPE_BEGIN = 0

	};// ColorType


	struct State
	{

		Color background, colors[2];

		coord stroke_width;

		CapType stroke_cap;

		JoinType stroke_join;

		coord miter_limit;

		uint nsegment;

		BlendMode blend_mode;

		Bounds clip;

		Font font;

		Shader shader;

		void init ()
		{
			background     .reset(0, 0);
			colors[FILL]   .reset(1, 1);
			colors[STROKE] .reset(1, 0);
			stroke_width   = 0;
			stroke_cap     = CAP_DEFAULT;
			stroke_join    = JOIN_DEFAULT;
			miter_limit    = JOIN_DEFAULT_MITER_LIMIT;
			nsegment       = 0;
			blend_mode     = BLEND_NORMAL;
			clip           .reset(-1);
			font           = default_font();
			shader         = Shader();
		}

		bool has_color ()
		{
			return colors[FILL] || colors[STROKE];
		}

	};// State


	struct TextureInfo
	{

		const Texture& texture;

		Point texcoord_min, texcoord_max;

		TextureInfo (
			const Texture& texture,
			coord x_min, coord y_min,
			coord x_max, coord y_max)
		:	texture(texture)
		{
			texcoord_min.reset(x_min, y_min);
			texcoord_max.reset(x_max, y_max);
		}

		operator bool () const
		{
			return
				texture &&
				texcoord_min.x < texcoord_max.x &&
				texcoord_min.y < texcoord_max.y;
		}

		bool operator ! () const
		{
			return !operator bool();
		}

	};// TextureInfo


	struct OpenGLState
	{

		GLint viewport[4];

		GLclampf color_clear[4];

		GLboolean depth_test;
		GLint depth_func;

		GLboolean scissor_test;
		GLint scissor_box[4];

		GLboolean blend;
		GLint blend_equation_rgb, blend_equation_alpha;
		GLint blend_src_rgb, blend_src_alpha, blend_dst_rgb, blend_dst_alpha;

		GLint framebuffer_binding;

		void push ()
		{
			glGetIntegerv(GL_VIEWPORT, viewport);

			glGetFloatv(GL_COLOR_CLEAR_VALUE, color_clear);

			glGetBooleanv(GL_DEPTH_TEST, &depth_test);
			glGetIntegerv(GL_DEPTH_FUNC, &depth_func);

			glGetBooleanv(GL_SCISSOR_TEST, &scissor_test);
			glGetIntegerv(GL_SCISSOR_BOX, scissor_box);

			glGetBooleanv(GL_BLEND, &blend);
			glGetIntegerv(GL_BLEND_EQUATION_RGB,   &blend_equation_rgb);
			glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &blend_equation_alpha);
			glGetIntegerv(GL_BLEND_SRC_RGB,   &blend_src_rgb);
			glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_src_alpha);
			glGetIntegerv(GL_BLEND_DST_RGB,   &blend_dst_rgb);
			glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_dst_alpha);

			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer_binding);
		}

		void pop ()
		{
			glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

			glClearColor(
				color_clear[0], color_clear[1], color_clear[2], color_clear[3]);

			enable(GL_DEPTH_TEST, depth_test);
			glDepthFunc(depth_func);

			enable(GL_SCISSOR_TEST, scissor_test);
			glScissor(scissor_box[0], scissor_box[1], scissor_box[2], scissor_box[3]);

			enable(GL_BLEND, blend);
			glBlendEquationSeparate(blend_equation_rgb, blend_equation_alpha);
			glBlendFuncSeparate(
				blend_src_rgb, blend_dst_rgb, blend_src_alpha, blend_dst_alpha);

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_binding);
		}

		private:

			void enable(GLenum type, GLboolean value)
			{
				if (value)
					glEnable(type);
				else
					glDisable(type);
			}

	};// OpenGLState


	class DefaultIndices
	{

		public:

			void resize (size_t size)
			{
				indices.reserve(size);
				while (indices.size() < size)
					indices.emplace_back(indices.size());
			}

			void clear ()
			{
				decltype(indices)().swap(indices);
			}

			const uint* get () const
			{
				return &indices[0];
			}

		private:

			std::vector<uint> indices;

	};// DefaultIndices


	template <typename COORD>
	static GLenum get_gl_type ();

	template <>
	GLenum
	get_gl_type<float> ()
	{
		return GL_FLOAT;
	}


	struct Painter::Data
	{

		bool painting = false;

		float pixel_density = 1;

		Bounds viewport;

		State              state;

		std::vector<State> state_stack;

		Matrix              position_matrix;

		std::vector<Matrix> position_matrix_stack;

		FrameBuffer frame_buffer;

		Image text_image;

		OpenGLState opengl_state;

		DefaultIndices default_indices;

		Data ()
		{
			state.init();
		}

		void set_pixel_density (float density)
		{
			if (density <= 0)
				argument_error(__FILE__, __LINE__, "invalid pixel_density.");

			this->pixel_density = density;
			text_image = Image();
		}

		void update_clip ()
		{
			const Bounds& clip = state.clip;
			if (clip)
			{
				coord y = frame_buffer ? clip.y : viewport.h - (clip.y + clip.h);
				glEnable(GL_SCISSOR_TEST);
				glScissor(
					pixel_density * clip.x,
					pixel_density * y,
					pixel_density * clip.width,
					pixel_density * clip.height);
			}
			else
			{
				glDisable(GL_SCISSOR_TEST);
			}

			OpenGL_check_error(__FILE__, __LINE__);
		}

		bool get_color (Color* color, ColorType type)
		{
			const Color& c = state.colors[type];
			if (!c) return false;

			*color = c;
			return true;
		}

		void draw_polygon (
			GLenum mode, const Color& color,
			const Coord3* points,           size_t npoints,
			const uint*   indices   = NULL, size_t nindices = 0,
			const Coord3* texcoords = NULL,
			const Shader& default_shader = Shader_get_default_shader_for_shape(),
			const TextureInfo* texinfo = NULL)
		{
			if (!points || npoints <= 0)
				argument_error(__FILE__, __LINE__);

			if (!painting)
				invalid_state_error(__FILE__, __LINE__, "'painting' should be true.");

			if (!indices || nindices <= 0)
			{
				default_indices.resize(npoints);
				indices  = default_indices.get();
				nindices = npoints;
			}

			if (!texcoords)
				texcoords = points;

			const Shader& shader = state.shader ? state.shader : default_shader;
			const ShaderProgram* program = Shader_get_program(shader);
			if (!program || !*program) return;

			ShaderProgram_activate(*program);

			const auto& names = Shader_get_builtin_variable_names(shader);
			apply_builtin_uniforms(*program, names, texinfo);
			apply_attributes(*program, names, points, texcoords, color, [&]() {
				//activate_texture(texture);

				glDrawElements(mode, (GLsizei) nindices, GL_UNSIGNED_INT, indices);
				OpenGL_check_error(__FILE__, __LINE__);

				//deactivate_texture(texture);
			});

			ShaderProgram_deactivate();
		}

		private:

			typedef std::vector<GLint> LocationList;

			void apply_attributes (
				const ShaderProgram& program, const ShaderBuiltinVariableNames& names,
				const Coord3* points, const Coord3* texcoords, const Color& color,
				std::function<void()> draw)
			{
				assert(points && texcoords);

				LocationList locations;
				for (const auto& name : names.attribute_position_names)
				{
					apply_attribute(&locations, program, name, [&](GLint loc) {
						glEnableVertexAttribArray(loc);
					OpenGL_check_error(__FILE__, __LINE__);
						glVertexAttribPointer(
							loc, Coord3::SIZE, get_gl_type<coord>(),
							GL_FALSE, sizeof(Coord3), points);
					});
				}
				for (const auto& name : names.attribute_texcoord_names)
				{
					apply_attribute(&locations, program, name, [&](GLint loc) {
						glEnableVertexAttribArray(loc);
					OpenGL_check_error(__FILE__, __LINE__);
						glVertexAttribPointer(
							loc, Coord3::SIZE, get_gl_type<coord>(),
							GL_FALSE, sizeof(Coord3), texcoords);
					});
				}
				for (const auto& name : names.attribute_color_names)
				{
					apply_attribute(NULL, program, name, [&](GLint loc) {
						glVertexAttrib4fv(loc, color.array);
					});
				}

				draw();

				for (auto loc : locations)
				{
					glDisableVertexAttribArray(loc);
					OpenGL_check_error(__FILE__, __LINE__);
				}
			}

			void apply_builtin_uniforms (
				const ShaderProgram& program, const ShaderBuiltinVariableNames& names,
				const TextureInfo* texinfo)
			{
				const Texture* texture = texinfo ? &texinfo->texture : NULL;

				Matrix texcoord_matrix(1);
				if (texture && *texture)
				{
					texcoord_matrix.scale(
						1.0 / texture->reserved_width(),
						1.0 / texture->reserved_height());
				}

				for (const auto& name : names.uniform_position_matrix_names)
				{
					apply_uniform(program, name, [&](GLint loc) {
						glUniformMatrix4fv(loc, 1, GL_FALSE, position_matrix.array);
					});
				}
				for (const auto& name : names.uniform_texcoord_matrix_names)
				{
					apply_uniform(program, name, [&](GLint loc) {
						glUniformMatrix4fv(loc, 1, GL_FALSE, texcoord_matrix.array);
					});
				}

				if (!texinfo || !texture || !*texture) return;

				for (const auto& name : names.uniform_texcoord_min_names)
				{
					apply_uniform(program, name, [&](GLint loc) {
						Point min = texcoord_matrix * texinfo->texcoord_min;
						glUniform3fv(loc, 1, min.array);
					});
				}
				for (const auto& name : names.uniform_texcoord_max_names)
				{
					apply_uniform(program, name, [&](GLint loc) {
						Point max = texcoord_matrix * texinfo->texcoord_max;
						glUniform3fv(loc, 1, max.array);
					});
				}
				for (const auto& name : names.uniform_texcoord_offset_names)
				{
					apply_uniform(program, name, [&](GLint loc) {
						Point offset(1.0 / texture->width(), 1.0 / texture->height());
						glUniform3fv(loc, 1, offset.array);
					});
				}
				for (const auto& name : names.uniform_texture_names)
				{
					apply_uniform(program, name, [&](GLint loc) {
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, texture->id());
						glUniform1i(loc, 0);
					});
				}
			}

			void apply_attribute (
				LocationList* locations, const ShaderProgram& program, const char* name,
				std::function<void(GLint)> apply)
			{
				GLint loc = glGetAttribLocation(program.id(), name);
				if (loc < 0) return;

				apply(loc);
				OpenGL_check_error(__FILE__, __LINE__);

				if (locations) locations->push_back(loc);
			}

			void apply_uniform (
				const ShaderProgram& program, const char* name,
				std::function<void(GLint)> apply)
			{
				GLint loc = glGetUniformLocation(program.id(), name);
				if (loc < 0) return;

				apply(loc);
				OpenGL_check_error(__FILE__, __LINE__);
			}

			void activate_texture (const Texture* texture)
			{
				if (!texture)
				{
					glDisable(GL_TEXTURE_2D);
					return;
				}

				if (!*texture)
					argument_error(__FILE__, __LINE__, "invalid texture.");

				GLuint id = texture->id();
				if (id != get_current_texture_id())
					glBindTexture(GL_TEXTURE_2D, id);

				glEnable(GL_TEXTURE_2D);
			}

			GLuint get_current_texture_id ()
			{
				GLint id = 0;
				glGetIntegerv(GL_TEXTURE_BINDING_2D, &id);
				return (GLuint) id;
			}

			void deactivate_texture (const Texture* texture)
			{
				if (!texture) return;

				glDisable(GL_TEXTURE_2D);
			}

	};// Painter::Data


	static void
	draw_polygon (
		Painter* painter,
		const GLenum* modes,
		coord offset_x, coord offset_y,
		bool nofill, bool nostroke,
		const Coord3* points,           size_t npoints,
		const uint*   indices   = NULL, size_t nindices = 0,
		const Coord3* texcoords = NULL,
		const Shader& default_shader = Shader_get_default_shader_for_shape(),
		const TextureInfo* texinfo = NULL)
	{
		assert(painter);

		bool offset = offset_x != 0 || offset_y != 0;
		if (offset)
		{
			painter->push_matrix();
			painter->translate(offset_x, offset_y);
		}

		Color color;
		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if ((nofill && type == FILL) || (nostroke && type == STROKE))
				continue;

			if (!painter->self->get_color(&color, (ColorType) type))
				continue;

			painter->self->draw_polygon(
				modes[type], color, points, npoints, indices, nindices, texcoords,
				default_shader, texinfo);
		}

		if (offset)
			painter->pop_matrix();
	}

	void
	Painter_draw_polygon (
		Painter* painter, GLenum mode, const Color& color,
		const Coord3* points,  size_t npoints,
		const uint*   indices, size_t nindices)
	{
		painter->self->draw_polygon(mode, color, points, npoints, indices, nindices);
	}


	Painter::Painter ()
	{
	}

	Painter::~Painter ()
	{
	}

	void
	Painter::canvas (
		coord x, coord y, coord width, coord height, float pixel_density)
	{
		canvas(x, y, 0, width, height, 0, pixel_density);
	}

	void
	Painter::canvas (
		coord x, coord y, coord z, coord width, coord height, coord depth,
		float pixel_density)
	{
		canvas(Bounds(x, y, z, width, height, depth), pixel_density);
	}

	void
	Painter::canvas (const Bounds& viewport, float pixel_density)
	{
		if (!viewport)
			argument_error(__FILE__, __LINE__);

		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be false.");

		self->viewport = viewport;
		self->set_pixel_density(pixel_density);
	}

	void
	Painter::bind (const Image& image)
	{
		if (!image)
			argument_error(__FILE__, __LINE__, "invalid image.");

		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be false.");

		FrameBuffer fb(Image_get_texture(image));
		if (!fb)
			rays_error(__FILE__, __LINE__, "invalid frame buffer.");

		unbind();

		self->frame_buffer = fb;
		canvas(0, 0, image.width(), image.height(), image.pixel_density());
	}

	void
	Painter::unbind ()
	{
		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		self->frame_buffer = FrameBuffer();
	}

	const Bounds&
	Painter::bounds () const
	{
		return self->viewport;
	}

	float
	Painter::pixel_density () const
	{
		return self->pixel_density;
	}

	void
	Painter::begin ()
	{
		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be false.");

		self->opengl_state.push();

		//glEnable(GL_CULL_FACE);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		OpenGL_check_error(__FILE__, __LINE__);

		glEnable(GL_BLEND);
		set_blend_mode(self->state.blend_mode);

		FrameBuffer& fb = self->frame_buffer;
		if (fb) FrameBuffer_bind(fb.id());

		const Bounds& vp = self->viewport;
		float density    = self->pixel_density;
		glViewport(
			(int) (vp.x      * density), (int) (vp.y      * density),
			(int) (vp.width  * density), (int) (vp.height * density));
		OpenGL_check_error(__FILE__, __LINE__);

		coord x1 = vp.x, x2 = vp.x + vp.width;
		coord y1 = vp.y, y2 = vp.y + vp.height;
		coord z1 = vp.z, z2 = vp.z + vp.depth;
		if (z1 == 0 && z2 == 0) {z1 = -1000; z2 = 1000;}
		if (!fb) std::swap(y1, y2);

		self->position_matrix.reset(1);
		self->position_matrix *= to_rays(glm::ortho(x1, x2, y1, y2));

		// map z to 0.0-1.0
		self->position_matrix.scale(1, 1, 1.0 / (z2 - z1));
		self->position_matrix.translate(0, 0, -z2);

		//self->position_matrix.translate(0.375f, 0.375f);

		self->update_clip();

		self->painting = true;

		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void
	Painter::end ()
	{
		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state_stack.empty())
			invalid_state_error(__FILE__, __LINE__, "state stack is not empty.");

		if (!self->position_matrix_stack.empty())
			invalid_state_error(__FILE__, __LINE__, "position matrix stack is not empty.");

		self->painting = false;
		self->opengl_state.pop();
		self->default_indices.clear();

		glFinish();

		if (self->frame_buffer)
		{
			FrameBuffer_unbind();

			Texture& tex = self->frame_buffer.texture();
			if (tex) tex.set_modified();
		}
	}

	void
	Painter::clear ()
	{
		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		const Color& c = self->state.background;
		glClearColor(c.red, c.green, c.blue, c.alpha);
		glClear(GL_COLOR_BUFFER_BIT);
		OpenGL_check_error(__FILE__, __LINE__);
	}

	static inline void
	debug_draw_triangulation (
		Painter* painter, const Polygon& polygon, const Color& color)
	{
#ifdef _DEBUG
		assert(painter);

		Color invert_color(
			1.f - color.red,
			1.f - color.green,
			1.f - color.blue);

		Polygon::TrianglePointList triangles;
		if (Polygon_triangulate(&triangles, polygon))
		{
			for (size_t i = 0; i < triangles.size(); i += 3)
			{
				painter->self->draw_polygon(
					GL_LINE_LOOP, invert_color, &triangles[i], 3);
			}
		}
#endif
	}

	void
	Painter::polygon (const Polygon& polygon)
	{
		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state.has_color())
			return;

		Color color;

		if (self->get_color(&color, FILL))
		{
			Polygon_fill(polygon, this, color);
			debug_draw_triangulation(this, polygon, color);
		}

		if (self->get_color(&color, STROKE))
			Polygon_stroke(polygon, this, color);
	}

	void
	Painter::line (coord x1, coord y1, coord x2, coord y2)
	{
		polygon(create_line(x1, y1, x2, y2));
	}

	void
	Painter::line (const Point& p1, const Point& p2)
	{
		polygon(create_line(p1, p2));
	}

	void
	Painter::line (const Point* points, size_t size, bool loop)
	{
		polygon(create_line(points, size, loop));
	}

	void
	Painter::line (const Polyline& polyline)
	{
		polygon(create_line(polyline));
	}

	void
	Painter::rect (coord x, coord y, coord width, coord height, coord round)
	{
		polygon(create_rect(x, y, width, height, round, nsegment()));
	}

	void
	Painter::rect (
		coord x, coord y, coord width, coord height,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom)
	{
		polygon(create_rect(
			x, y, width, height,
			round_left_top,    round_right_top,
			round_left_bottom, round_right_bottom,
			nsegment()));
	}

	void
	Painter::rect (const Bounds& bounds, coord round)
	{
		polygon(create_rect(bounds, round, nsegment()));
	}

	void
	Painter::rect (
		const Bounds& bounds,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom)
	{
		polygon(create_rect(
			bounds,
			round_left_top,    round_right_top,
			round_left_bottom, round_right_bottom,
			nsegment()));
	}

	void
	Painter::ellipse (
		coord x, coord y, coord width, coord height,
		const Point& hole_size,
		float angle_from, float angle_to)
	{
		polygon(create_ellipse(
			x, y, width, height, hole_size, angle_from, angle_to, nsegment()));
	}

	void
	Painter::ellipse (
		const Bounds& bounds,
		const Point& hole_size,
		float angle_from, float angle_to)
	{
		polygon(create_ellipse(
			bounds, hole_size, angle_from, angle_to, nsegment()));
	}

	void
	Painter::ellipse (
		const Point& center, const Point& radius, const Point& hole_radius,
		float angle_from, float angle_to)
	{
		polygon(create_ellipse(
			center, radius, hole_radius, angle_from, angle_to, nsegment()));
	}

	void
	Painter::curve (
		coord x1, coord y1, coord x2, coord y2,
		coord x3, coord y3, coord x4, coord y4,
		bool loop)
	{
		polygon(create_curve(x1, y1, x2, y2, x3, y3, x4, y4, loop));
	}

	void
	Painter::curve (
		const Point& p1, const Point& p2, const Point& p3, const Point& p4,
		bool loop)
	{
		polygon(create_curve(p1, p2, p3, p4, loop));
	}

	void
	Painter::curve (const Point* points, size_t size, bool loop)
	{
		polygon(create_curve(points, size, loop));
	}

	void
	Painter::bezier (
		coord x1, coord y1, coord x2, coord y2,
		coord x3, coord y3, coord x4, coord y4,
		bool loop)
	{
		polygon(create_bezier(x1, y1, x2, y2, x3, y3, x4, y4, loop));
	}

	void
	Painter::bezier (
		const Point& p1, const Point& p2, const Point& p3, const Point& p4,
		bool loop)
	{
		polygon(create_bezier(p1, p2, p3, p4, loop));
	}

	void
	Painter::bezier (const Point* points, size_t size, bool loop)
	{
		polygon(create_bezier(points, size, loop));
	}

	static void
	draw_image (
		Painter* painter, const Image& image,
		coord src_x, coord src_y, coord src_w, coord src_h,
		coord dst_x, coord dst_y, coord dst_w, coord dst_h,
		bool nostroke = false, const Shader* shader = NULL)
	{
		static const GLenum MODES[] = {GL_TRIANGLE_FAN, GL_LINE_LOOP};

		assert(painter && image);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state.has_color())
			return;

		const Texture& texture = Image_get_texture(image);
		if (!texture)
			invalid_state_error(__FILE__, __LINE__);

		float density = image.pixel_density();
		src_x *= density;
		src_y *= density;
		src_w *= density;
		src_h *= density;

		Point points[4], texcoords[4];
		points[0]   .reset(dst_x,         dst_y);
		points[1]   .reset(dst_x,         dst_y + dst_h);
		points[2]   .reset(dst_x + dst_w, dst_y + dst_h);
		points[3]   .reset(dst_x + dst_w, dst_y);
		texcoords[0].reset(src_x,         src_y);
		texcoords[1].reset(src_x,         src_y + src_h);
		texcoords[2].reset(src_x + src_w, src_y + src_h);
		texcoords[3].reset(src_x + src_w, src_y);

		TextureInfo texinfo(texture, src_x, src_y, src_x + src_w, src_y + src_h);

		if (!shader)
			shader = &Shader_get_default_shader_for_texture();

		draw_polygon(
			painter, MODES, 0, 0, false, nostroke, points, 4, NULL, 0, texcoords,
			*shader, &texinfo);
	}

	void
	Painter::image (const Image& image_, coord x, coord y)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		draw_image(
			this, image_,
			0, 0, image_.width(), image_.height(),
			x, y, image_.width(), image_.height());
	}

	void
	Painter::image (const Image& image_, const Point& position)
	{
		image(image_, position.x, position.y);
	}

	void
	Painter::image (
		const Image& image_, coord x, coord y, coord width, coord height)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		draw_image(
			this, image_,
			0, 0, image_.width(), image_.height(),
			x, y, width,          height);
	}

	void
	Painter::image (
		const Image& image_, const Bounds& bounds)
	{
		image(image_, bounds.x, bounds.y, bounds.width, bounds.height);
	}

	void
	Painter::image (
		const Image& image_,
		coord src_x, coord src_y, coord src_width, coord src_height,
		coord dst_x, coord dst_y)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		draw_image(
			this, image_,
			src_x, src_y, src_width,      src_height,
			dst_x, dst_y, image_.width(), image_.height());
	}

	void
	Painter::image (
		const Image& image_, const Bounds& src_bounds, const Point& dst_position)
	{
		image(
			image_,
			src_bounds.x, src_bounds.y, src_bounds.width, src_bounds.height,
			dst_position.x, dst_position.y);
	}

	void
	Painter::image (
		const Image& image_,
		coord src_x, coord src_y, coord src_width, coord src_height,
		coord dst_x, coord dst_y, coord dst_width, coord dst_height)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		draw_image(
			this, image_,
			src_x, src_y, src_width, src_height,
			dst_x, dst_y, dst_width, dst_height);
	}

	void
	Painter::image (
		const Image& image_, const Bounds& src_bounds, const Bounds& dst_bounds)
	{
		image(
			image_,
			src_bounds.x, src_bounds.y, src_bounds.width, src_bounds.height,
			dst_bounds.x, dst_bounds.y, dst_bounds.width, dst_bounds.height);
	}

	static inline void
	debug_draw_text (
		Painter* painter, const Font& font,
		coord x, coord y, coord str_width, coord str_height)
	{
#if 0
		save_image(painter->self->text_image, "/tmp/font.png");

		painter->push_state();
		{
			coord asc, desc, lead;
			font.get_height(&asc, &desc, &lead);
			//printf("%f %f %f %f \n", str_height, asc, desc, lead);

			painter->set_stroke(0.5, 0.5, 1);
			painter->no_fill();
			painter->rect(x - 1, y - 1, str_width + 2, str_height + 2);

			coord yy = y;
			painter->set_stroke(1, 0.5, 0.5, 0.4);
			painter->rect(x, yy, str_width, asc);//str_height);

			yy += asc;
			painter->set_stroke(1, 1, 0.5, 0.4);
			painter->rect(x, yy, str_width, desc);

			yy += desc;
			painter->set_stroke(1, 0.5, 1, 0.4);
			painter->rect(x, yy, str_width, lead);
		}
		painter->pop_state();
#endif
	}

	static void
	draw_text (
		Painter* painter, const Font& font,
		const char* str, coord x, coord y, coord width = 0, coord height = 0)
	{
		assert(painter && font && str && *str != '\0');

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state.has_color())
			return;

		float density = self->pixel_density;
		coord str_w = Font_get_width(font, density, str);
		coord str_h = Font_get_height(font, density);
		int tex_w   = ceil(str_w);
		int tex_h   = ceil(str_h);
		const Texture& texture = Image_get_texture(self->text_image);
		if (
			texture.width()  < tex_w ||
			texture.height() < tex_h ||
			self->text_image.pixel_density() != density)
		{
			int bmp_w = std::max(texture.width(),  tex_w);
			int bmp_h = std::max(texture.height(), tex_h);
			self->text_image = Image(Bitmap(bmp_w, bmp_h), density);
		}

		if (!self->text_image)
			invalid_state_error(__FILE__, __LINE__);

		assert(self->text_image.pixel_density() == density);

		Bitmap_draw_string(
			&self->text_image.bitmap(), Font_get_raw(font, density), str, 0, 0);

		str_w /= density;
		str_h /= density;
		if (width  == 0) width  = str_w;
		if (height == 0) height = str_h;

		draw_image(
			painter, self->text_image,
			0, 0, str_w, str_h,
			x, y, str_w, str_h,
			true, &Shader_get_default_shader_for_text());

		debug_draw_text(painter, font, x, y, str_w / density, str_h / density);
	}

	void
	Painter::text (const char* str, coord x, coord y)
	{
		if (!str)
			argument_error(__FILE__, __LINE__);

		if (*str == '\0') return;

		const Font& font = self->state.font;
		if (!font)
			invalid_state_error(__FILE__, __LINE__);

		draw_text(this, font, str, x, y);
	}

	void
	Painter::text (const char* str, const Point& position)
	{
		text(str, position.x, position.y);
	}

	void
	Painter::text (const char* str, coord x, coord y, coord width, coord height)
	{
		if (!str)
			argument_error(__FILE__, __LINE__);

		if (*str == '\0' || width == 0 || height == 0) return;

		const Font& font = self->state.font;
		if (!font)
			invalid_state_error(__FILE__, __LINE__);

		draw_text(this, font, str, x, y, width, height);
	}

	void
	Painter::text (const char* str, const Bounds& bounds)
	{
		text(str, bounds.x, bounds.y, bounds.width, bounds.height);
	}

	void
	Painter::set_background (
		float red, float green, float blue, float alpha, bool clear)
	{
		set_background(Color(red, green, blue, alpha), clear);
	}

	void
	Painter::set_background (const Color& color, bool clear)
	{
		self->state.background = color;

		if (self->painting && clear) this->clear();
	}

	void
	Painter::no_background (bool clear)
	{
		Color c = background();
		c.alpha = 0;
		set_background(c, clear);
	}

	const Color&
	Painter::background () const
	{
		return self->state.background;
	}

	void
	Painter::set_fill (float red, float green, float blue, float alpha)
	{
		set_fill(Color(red, green, blue, alpha));
	}

	void
	Painter::set_fill (const Color& color)
	{
		self->state.colors[FILL] = color;
	}

	void
	Painter::no_fill ()
	{
		self->state.colors[FILL].alpha = 0;
	}

	const Color&
	Painter::fill () const
	{
		return self->state.colors[FILL];
	}

	void
	Painter::set_stroke (float red, float green, float blue, float alpha)
	{
		set_stroke(Color(red, green, blue, alpha));
	}

	void
	Painter::set_stroke (const Color& color)
	{
		self->state.colors[STROKE] = color;
	}

	void
	Painter::no_stroke ()
	{
		self->state.colors[STROKE].alpha = 0;
	}

	const Color&
	Painter::stroke () const
	{
		return self->state.colors[STROKE];
	}

	void
	Painter::set_stroke_width (coord width)
	{
		self->state.stroke_width = width;
	}

	coord
	Painter::stroke_width () const
	{
		return self->state.stroke_width;
	}

	void
	Painter::set_stroke_cap (CapType cap)
	{
		self->state.stroke_cap = cap;
	}

	CapType
	Painter::stroke_cap () const
	{
		return self->state.stroke_cap;
	}

	void
	Painter::set_stroke_join (JoinType join)
	{
		self->state.stroke_join = join;
	}

	JoinType
	Painter::stroke_join () const
	{
		return self->state.stroke_join;
	}

	void
	Painter::set_miter_limit (coord limit)
	{
		self->state.miter_limit = limit;
	}

	coord
	Painter::miter_limit () const
	{
		return self->state.miter_limit;
	}

	void
	Painter::set_nsegment (int nsegment)
	{
		if (nsegment < 0) nsegment = 0;
		self->state.nsegment = nsegment;
	}

	uint
	Painter::nsegment () const
	{
		return self->state.nsegment;
	}

	void
	Painter::set_blend_mode (BlendMode mode)
	{
		self->state.blend_mode = mode;
		switch (mode)
		{
			case BLEND_NORMAL:
				glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				glBlendFuncSeparate(
					GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
				break;

			case BLEND_ADD:
				glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
				break;

			case BLEND_SUBTRACT:
				glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
				break;

			case BLEND_LIGHTEST:
				glBlendEquationSeparate(GL_MAX, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
				break;

			case BLEND_DARKEST:
				glBlendEquationSeparate(GL_MIN, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
				break;

			case BLEND_EXCLUSION:
				glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				glBlendFuncSeparate(
					GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_ONE, GL_ONE);
				break;

			case BLEND_MULTIPLY:
				glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ONE, GL_ONE);
				break;

			case BLEND_SCREEN:
				glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_ONE_MINUS_DST_COLOR, GL_ONE, GL_ONE, GL_ONE);
				break;

			case BLEND_REPLACE:
				glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
				break;

			default:
				argument_error(__FILE__, __LINE__, "unknown blend mode");
				break;
		}
		OpenGL_check_error(__FILE__, __LINE__);
	}

	BlendMode
	Painter::blend_mode () const
	{
		return self->state.blend_mode;
	}

	void
	Painter::set_clip (coord x, coord y, coord width, coord height)
	{
		set_clip(Bounds(x, y, width, height));
	}

	void
	Painter::set_clip (const Bounds& bounds)
	{
		self->state.clip = bounds;
		self->update_clip();
	}

	void
	Painter::no_clip ()
	{
		set_clip(0, 0, -1, -1);
	}

	const Bounds&
	Painter::clip () const
	{
		return self->state.clip;
	}

	static bool
	has_same_font (const Font& font, const char* name, coord size)
	{
		return
			font.size() == size &&
			font.name() == (name ? name : default_font().name().c_str());
	}

	void
	Painter::set_font (const char* name, coord size)
	{
		if (has_same_font(self->state.font, name, size)) return;

		set_font(Font(name, size));
	}

	void
	Painter::set_font (const Font& font)
	{
		self->state.font = font;
	}

	const Font&
	Painter::font () const
	{
		return self->state.font;
	}

	void
	Painter::set_shader (const Shader& shader)
	{
		self->state.shader = shader;
	}

	void
	Painter::no_shader ()
	{
		self->state.shader = Shader();
	}

	const Shader&
	Painter::shader () const
	{
		return self->state.shader;
	}

	void
	Painter::push_state ()
	{
		self->state_stack.emplace_back(self->state);
	}

	void
	Painter::pop_state ()
	{
		if (self->state_stack.empty())
			invalid_state_error(__FILE__, __LINE__, "state stack underflow.");

		self->state = self->state_stack.back();
		self->state_stack.pop_back();
		self->update_clip();
	}

	void
	Painter::translate (coord x, coord y, coord z)
	{
		self->position_matrix.translate(x, y, z);
	}

	void
	Painter::translate (const Point& value)
	{
		self->position_matrix.translate(value);
	}

	void
	Painter::scale (coord x, coord y, coord z)
	{
		self->position_matrix.scale(x, y, z);
	}

	void
	Painter::scale (const Point& value)
	{
		self->position_matrix.scale(value);
	}

	void
	Painter::rotate (float degree, coord x, coord y, coord z)
	{
		self->position_matrix.rotate(degree, x, y, z);
	}

	void
	Painter::rotate (float angle, const Point& normalized_axis)
	{
		self->position_matrix.rotate(angle, normalized_axis);
	}

	void
	Painter::set_matrix (float value)
	{
		self->position_matrix.reset(value);
	}

	void
	Painter::set_matrix (
		float a1, float a2, float a3, float a4,
		float b1, float b2, float b3, float b4,
		float c1, float c2, float c3, float c4,
		float d1, float d2, float d3, float d4)
	{
		self->position_matrix.reset(
			a1, a2, a3, a4,
			b1, b2, b3, b4,
			c1, c2, c3, c4,
			d1, d2, d3, d4);
	}

	void
	Painter::set_matrix (const coord* elements, size_t size)
	{
		self->position_matrix.reset(elements, size);
	}

	void
	Painter::set_matrix (const Matrix& matrix)
	{
		self->position_matrix = matrix;
	}

	const Matrix&
	Painter::matrix () const
	{
		return self->position_matrix;
	}

	void
	Painter::push_matrix ()
	{
		self->position_matrix_stack.emplace_back(self->position_matrix);
	}

	void
	Painter::pop_matrix ()
	{
		if (self->position_matrix_stack.empty())
			invalid_state_error(__FILE__, __LINE__, "matrix stack underflow.");

		self->position_matrix = self->position_matrix_stack.back();
		self->position_matrix_stack.pop_back();
	}

	Painter::operator bool () const
	{
		return self->viewport;
	}

	bool
	Painter::operator ! () const
	{
		return !operator bool();
	}


}// Rays
