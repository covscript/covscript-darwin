/*
* Covariant Script Darwin Extension
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2017-2021 Michael Lee(李登淳)
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#define DARWIN_DISABLE_LOG
#define DARWIN_FORCE_BUILTIN

#include <darwin/darwin.hpp>
#include <covscript/dll.hpp>
#include <covscript/cni.hpp>

static cs::namespace_t darwin_ui_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t darwin_core_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t darwin_drawable_ext=cs::make_shared_namespace<cs::name_space>();
namespace cs_impl {
	template<>
	cs::namespace_t &get_ext<darwin::pixel>()
	{
		return darwin_core_ext;
	}

	template<>
	cs::namespace_t &get_ext<std::shared_ptr<darwin::drawable>>()
	{
		return darwin_drawable_ext;
	}

	template<>
	constexpr const char *get_name_of_type<darwin::colors>()
	{
		return "cs::darwin::colors";
	}

	template<>
	constexpr const char *get_name_of_type<darwin::pixel>()
	{
		return "cs::darwin::pixel";
	}

	template<>
	constexpr const char *get_name_of_type<std::shared_ptr<darwin::drawable>>()
	{
		return "cs::darwin::drawable";
	}
}
namespace darwin_cs_ext {
	using namespace cs;
	using drawable_t=std::shared_ptr<darwin::drawable>;
	darwin::sync_clock clock(30);

// Type Constructor
	var pixel(char ch, darwin::colors fc, darwin::colors bc)
	{
		return var::make_protect<darwin::pixel>(ch, true, false, fc, bc);
	}

	var picture(number w, number h)
	{
		return var::make_protect<drawable_t>(new darwin::picture(w, h));
	}

// Darwin Main Function
	void load()
	{
		darwin::runtime.load("./darwin.module");
	}

	void exit()
	{
		darwin::runtime.exit();
	}

	bool is_kb_hit()
	{
		return darwin::runtime.is_kb_hit();
	}

	char get_kb_hit()
	{
		return darwin::runtime.get_kb_hit();
	}

	void fit_drawable()
	{
		darwin::runtime.fit_drawable();
	}

	var get_drawable()
	{
		return var::make<drawable_t>(darwin::runtime.get_drawable(), [](darwin::drawable *) {});
	}

	void update_drawable()
	{
		darwin::runtime.update_drawable();
		clock.sync();
		clock.reset();
	}

	void set_frame_limit(number freq)
	{
		clock.set_freq(freq);
	}

	void set_draw_line_precision(number prec)
	{
		darwin::drawable::draw_line_precision = prec;
	}

// Darwin UI Function
	var parse_value(const std::string &str)
	{
		if (str == "true")
			return true;
		if (str == "false")
			return false;
		try {
			return parse_number(str);
		}
		catch (...) {
			return str;
		}
		return str;
	}

	void message_box(const string &title, const string &message, const string &button)
	{
		std::size_t width = std::max(title.size(), std::max(message.size(), button.size())) + 4;
		darwin::sync_clock c(10);
		while (true) {
			c.reset();
			if (darwin::runtime.is_kb_hit()) {
				darwin::runtime.get_kb_hit();
				break;
			}
			darwin::runtime.fit_drawable();
			std::size_t x(0.5 * (darwin::runtime.get_drawable()->get_width() - width)), y(
			    0.5 * (darwin::runtime.get_drawable()->get_height() - 6));
			darwin::runtime.get_drawable()->clear();
			darwin::runtime.get_drawable()->fill_rect(x, y, width, 6,
			        darwin::pixel(' ', true, false, darwin::colors::white,
			                      darwin::colors::white));
			darwin::runtime.get_drawable()->fill_rect(x, y, width, 1,
			        darwin::pixel(' ', true, false, darwin::colors::white,
			                      darwin::colors::cyan));
			darwin::runtime.get_drawable()->fill_rect(x + 0.5 * (width - button.size() - 2), y + 4, button.size() + 2,
			        1, darwin::pixel(' ', true, false, darwin::colors::white,
			                         darwin::colors::cyan));
			darwin::runtime.get_drawable()->draw_string(x + 0.5 * (width - title.size()), y, title,
			        darwin::pixel(' ', true, false, darwin::colors::black,
			                      darwin::colors::cyan));
			darwin::runtime.get_drawable()->draw_string(x + 0.5 * (width - message.size()), y + 2, message,
			        darwin::pixel(' ', true, false, darwin::colors::black,
			                      darwin::colors::white));
			darwin::runtime.get_drawable()->draw_string(x + 0.5 * (width - button.size()), y + 4, button,
			        darwin::pixel(' ', true, false, darwin::colors::white,
			                      darwin::colors::cyan));
			darwin::runtime.update_drawable();
			c.sync();
		}
	}

	var input_box(const string &title, const string &message, string buff, bool format)
	{
		darwin::sync_clock c(10);
		while (true) {
			c.reset();
			if (darwin::runtime.is_kb_hit()) {
				char ch = darwin::runtime.get_kb_hit();
				if (ch == '\n' || ch == '\r')
					break;
				else if (ch == '\b' || ch == 127) {
					if (!buff.empty())
						buff.pop_back();
				}
				else
					buff += ch;
			}
			darwin::runtime.fit_drawable();
			std::size_t width = std::max(title.size(), std::max(message.size(), buff.size())) + 4;
			std::size_t x(0.5 * (darwin::runtime.get_drawable()->get_width() - width)), y(
			    0.5 * (darwin::runtime.get_drawable()->get_height() - 6));
			darwin::runtime.get_drawable()->clear();
			darwin::runtime.get_drawable()->fill_rect(x, y, width, 6,
			        darwin::pixel(' ', true, false, darwin::colors::white,
			                      darwin::colors::white));
			darwin::runtime.get_drawable()->fill_rect(x, y, width, 1,
			        darwin::pixel(' ', true, false, darwin::colors::white,
			                      darwin::colors::cyan));
			darwin::runtime.get_drawable()->fill_rect(x + 2, y + 4, width - 4, 1,
			        darwin::pixel(' ', true, false, darwin::colors::white,
			                      darwin::colors::black));
			darwin::runtime.get_drawable()->draw_string(x + 0.5 * (width - title.size()), y, title,
			        darwin::pixel(' ', true, false, darwin::colors::black,
			                      darwin::colors::cyan));
			darwin::runtime.get_drawable()->draw_string(x + 0.5 * (width - message.size()), y + 2, message,
			        darwin::pixel(' ', true, false, darwin::colors::black,
			                      darwin::colors::white));
			darwin::runtime.get_drawable()->draw_string(x + 2, y + 4, buff,
			        darwin::pixel(' ', true, false, darwin::colors::white,
			                      darwin::colors::black));
			darwin::runtime.update_drawable();
			c.sync();
		}
		if (format)
			return parse_value(buff);
		else
			return buff;
	}

// Core Function
	char get_char(const darwin::pixel &pix)
	{
		return pix.get_char();
	}

	void set_char(darwin::pixel &pix, char ch)
	{
		pix.set_char(ch);
	}

	darwin::colors get_front_color(const darwin::pixel &pix)
	{
		return pix.get_front_color();
	}

	void set_front_color(darwin::pixel &pix, darwin::colors c)
	{
		pix.set_front_color(c);
	}

	darwin::colors get_back_color(const darwin::pixel &pix)
	{
		return pix.get_back_color();
	}

	void set_back_color(darwin::pixel &pix, darwin::colors c)
	{
		pix.set_back_color(c);
	}

// Drawable Function
	void load_from_file(const drawable_t &pic, const string &path)
	{
		std::ifstream infs(path);
		if (!infs.is_open())
			throw lang_error("File is not exist.");
		std::deque<char> buff;
		while (!infs.eof())
			buff.push_back(infs.get());
		darwin::unserial_picture(pic.get(), buff);
	}

	void save_to_file(const drawable_t &pic, const string &path)
	{
		std::ofstream outfs(path);
		if (!outfs.is_open())
			throw lang_error("Write file failed.");
		std::deque<char> buff;
		darwin::serial_picture(pic.get(), buff);
		for (auto &ch:buff)
			outfs.put(ch);
		outfs.flush();
	}

	void clear(const drawable_t &pic)
	{
		pic->clear();
	}

	void fill(const drawable_t &pic, const darwin::pixel &pix)
	{
		pic->fill(pix);
	}

	void resize(const drawable_t &pic, number w, number h)
	{
		pic->resize(w, h);
	}

	number get_height(const drawable_t &pic)
	{
		return pic->get_height();
	}

	number get_width(const drawable_t &pic)
	{
		return pic->get_width();
	}

	darwin::pixel get_pixel(const drawable_t &pic, number x, number y)
	{
		return pic->get_pixel(x, y);
	}

	void draw_pixel(const drawable_t &pic, number x, number y, const darwin::pixel &pix)
	{
		pic->draw_pixel(x, y, pix);
	}

	void draw_line(const drawable_t &pic, number x1, number y1, number x2, number y2, const darwin::pixel &pix)
	{
		pic->draw_line(x1, y1, x2, y2, pix);
	}

	void draw_rect(const drawable_t &pic, number x, number y, number w, number h, const darwin::pixel &pix)
	{
		pic->draw_rect(x, y, w, h, pix);
	}

	void fill_rect(const drawable_t &pic, number x, number y, number w, number h, const darwin::pixel &pix)
	{
		pic->fill_rect(x, y, w, h, pix);
	}

	void draw_triangle(const drawable_t &pic, number x1, number y1, number x2, number y2, number x3, number y3,
	                   const darwin::pixel &pix)
	{
		pic->draw_triangle(x1, y1, x2, y2, x3, y3, pix);
	}

	void fill_triangle(const drawable_t &pic, number x1, number y1, number x2, number y2, number x3, number y3,
	                   const darwin::pixel &pix)
	{
		pic->fill_triangle(x1, y1, x2, y2, x3, y3, pix);
	}

	void draw_string(const drawable_t &pic, number x, number y, const string &str, const darwin::pixel &pix)
	{
		pic->draw_string(x, y, str, pix);
	}

	void draw_picture(const drawable_t &pic, number x, number y, const drawable_t &p)
	{
		pic->draw_picture(x, y, *p);
	}

	void init(name_space* darwin_ext)
	{
		(*darwin_ext)
		// Namespaces
		.add_var("ui", make_namespace(darwin_ui_ext))
		.add_var("core", make_namespace(darwin_core_ext))
		.add_var("drawable", make_namespace(darwin_drawable_ext))
		// Colors
		.add_var("black", var::make_constant<darwin::colors>(darwin::colors::black))
		.add_var("white", var::make_constant<darwin::colors>(darwin::colors::white))
		.add_var("red", var::make_constant<darwin::colors>(darwin::colors::red))
		.add_var("green", var::make_constant<darwin::colors>(darwin::colors::green))
		.add_var("blue", var::make_constant<darwin::colors>(darwin::colors::blue))
		.add_var("pink", var::make_constant<darwin::colors>(darwin::colors::pink))
		.add_var("yellow", var::make_constant<darwin::colors>(darwin::colors::yellow))
		.add_var("cyan", var::make_constant<darwin::colors>(darwin::colors::cyan))
		// Type Constructor
		.add_var("pixel", make_cni(pixel, true))
		.add_var("picture", make_cni(picture, true))
		// Darwin Main Function
		.add_var("load", make_cni(load))
		.add_var("exit", make_cni(exit))
		.add_var("is_kb_hit", make_cni(is_kb_hit))
		.add_var("get_kb_hit", make_cni(get_kb_hit))
		.add_var("fit_drawable", make_cni(fit_drawable))
		.add_var("get_drawable", make_cni(get_drawable))
		.add_var("update_drawable", make_cni(update_drawable))
		.add_var("set_frame_limit", make_cni(set_frame_limit))
		.add_var("set_draw_line_precision", make_cni(set_draw_line_precision));
		// Darwin UI Function
		(*darwin_ui_ext)
		.add_var("message_box", make_cni(message_box))
		.add_var("input_box", make_cni(input_box));
		// Darwin Core Function
		(*darwin_core_ext)
		.add_var("get_char", make_cni(get_char, true))
		.add_var("set_char", make_cni(set_char))
		.add_var("get_front_color", make_cni(get_front_color, true))
		.add_var("set_front_color", make_cni(set_front_color))
		.add_var("get_back_color", make_cni(get_back_color, true))
		.add_var("set_back_color", make_cni(set_back_color));
		// Drawable Function
		(*darwin_drawable_ext)
		.add_var("load_from_file", make_cni(load_from_file))
		.add_var("save_to_file", make_cni(save_to_file))
		.add_var("clear", make_cni(clear))
		.add_var("fill", make_cni(fill))
		.add_var("resize", make_cni(resize))
		.add_var("get_height", make_cni(get_height))
		.add_var("get_width", make_cni(get_width))
		.add_var("get_pixel", make_cni(get_pixel))
		.add_var("draw_pixel", make_cni(draw_pixel))
		.add_var("draw_line", make_cni(draw_line))
		.add_var("draw_rect", make_cni(draw_rect))
		.add_var("fill_rect", make_cni(fill_rect))
		.add_var("draw_triangle", make_cni(draw_triangle))
		.add_var("fill_triangle", make_cni(fill_triangle))
		.add_var("draw_string", make_cni(draw_string))
		.add_var("draw_picture", make_cni(draw_picture));
	}
}

void cs_extension_main(cs::name_space* ns)
{
	darwin_cs_ext::init(ns);
}
