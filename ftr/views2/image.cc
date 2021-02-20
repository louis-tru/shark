/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, xuewen.chu
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of xuewen.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL xuewen.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ***** END LICENSE BLOCK ***** */

#include "image.h"
#include "../texture.h"
#include "../draw.h"
#include "../display-port.h"
#include "../errno.h"

namespace ftr {

void _box_inl__solve_horizontal_size_with_full_width(Box* box, float parent);
void _box_inl__solve_horizontal_size_with_explicit_width(Box* box, float parent);
void _box_inl__solve_vertical_size_with_full_height(Box* box, float parent);
void _box_inl__solve_vertical_size_with_explicit_height(Box* box, float parent);

class Image::Inl : public Image {
public:
#define _inl(self) static_cast<Image::Inl*>(self)
	
	/**
	 * @func solve_explicit_size
	 */
	void solve_explicit_size() {
		
		Vec2 raw_limit = _limit;
		_final_width = _final_height = 0;
		
		if ( _width.type == ValueType::AUTO ) {
			_texture->load();
			if ( _height.type == ValueType::AUTO ) {
				solve_explicit_horizontal_size(1);
				solve_explicit_vertical_size(1);
			} else {
				solve_explicit_vertical_size(1);
				solve_explicit_horizontal_size(
					_texture->height() == 0 ? 1 : _final_height / _texture->height()
																			 );
			}
		} else if ( _height.type == ValueType::AUTO ) {
			_texture->load();
			solve_explicit_horizontal_size(1);
			solve_explicit_vertical_size(
				_texture->width() == 0 ? 1 : _final_width / _texture->width()
																	 );
		} else {
			solve_explicit_horizontal_size(1);
			solve_explicit_vertical_size(1);
		}
		
		// 造成子盒子变化有（包括尺寸变化与偏移变化）:
		// 1.最终宽度变化可能对子盒子造成影响
		// 2.明确与不明确的变化可能对子盒子造成影响
		// 3.宽度限制的变化可能会影响子盒子偏移变化
		//  前两项的变化都会造成第三项的变化..
		
		bool h = raw_limit.width() != _limit.width();
		bool v = raw_limit.height() != _limit.height();
		uint32_t child_mark = M_NONE;
		
		if ( h ) {
			if ( _content_align == ContentAlign::RIGHT ) {
				child_mark = M_MATRIX; // 右对齐宽度变化一定会影响到子盒子的最终位置偏移
			}
		}
		if ( v ) {
			if ( _content_align == ContentAlign::BOTTOM ) {
				child_mark = M_MATRIX; // 底部对齐高度变化一定会影响到子盒子的最终位置偏移
			}
		}
		solve_explicit_size_after(h, v, child_mark);
	}
	
	/**
	 * @func solve_explicit_horizontal_size # 设置水平尺寸
	 */
	void solve_explicit_horizontal_size(float height_ratio) {
		
		Box* parent = View::parent()->as_box();
		
		// 布局视图加入到普通视图内或父视图没有明确宽度时,属性vFULL/vPERCENT/vMINUS会失效.
		if ( parent && parent->_explicit_width ) {
			
			float parent_width = parent->_final_width;
			
			switch ( _width.type ) {
				case ValueType::AUTO: // AUTO (width=auto)
					_final_width = (_texture->width() ? _texture->width(): _final_height) * height_ratio;
					_limit.width(_final_width);
					_box_inl__solve_horizontal_size_with_explicit_width(this, parent_width);
					break;
				case ValueType::FULL: // 吸附父视图 (width=full)
					_box_inl__solve_horizontal_size_with_full_width(this, parent_width);
					break;
				case ValueType::PIXEL:  // 像素值  (width=50)
					_final_width = _width.value;
					_limit.width(_final_width);
					_box_inl__solve_horizontal_size_with_explicit_width(this, parent_width);
					break;
				case ValueType::PERCENT: // 百分比 (width=50%)
					_final_width = _width.value * parent_width;
					_limit.width(_final_width);
					_box_inl__solve_horizontal_size_with_explicit_width(this, parent_width);
					break;
				default: // 减法值 (width=50not)
					_final_width = FX_MAX(parent_width - _width.value, 0);
					_limit.width(_final_width);
					_box_inl__solve_horizontal_size_with_explicit_width(this, parent_width);
					break;
			}
			
		} else {
			
			if (_margin_left.type == ValueType::PIXEL) {
				_final_margin_left = _margin_left.value;
			} else {
				_final_margin_left = 0;
			}
			
			if (_margin_right.type == ValueType::PIXEL) {
				_final_margin_right = _margin_right.value;
			} else {
				_final_margin_right = 0;
			}
			
			_raw_client_width =  _border_left_width + _border_right_width +
														_final_margin_left + _final_margin_right;
			
			_explicit_width = true;  // 明确的宽度,不受内部挤压
			
			if (_width.type == ValueType::PIXEL) {
				_final_width = _width.value;
				_limit.width(_final_width);
			} else {
				_final_width = (_texture->width() ? _texture->width() : _final_height) * height_ratio;
				_limit.width(_final_width);
				
				// 如果父盒子为水平布局同时没有明确的宽度,FULL宽度会导致三次布局
				if ( parent && _width.type == ValueType::FULL ) { // 使用父视图的限制
					_limit.width(parent->_limit.width() - _raw_client_width);
					_explicit_width = false; // 不是明确的宽度,受内部挤压
				}
			}

			_raw_client_width += _final_width;
		}
	}
	
	/**
	 * @func solve_explicit_vertical_size # 设置垂直尺寸
	 */
	void solve_explicit_vertical_size(float width_ratio) {
		
		Box* parent = View::parent()->as_box();
		
		// 布局视图加入到普通视图内或父视图没有明确高度时,属性vAUTO/vFULL/vPERCENT/vMINUS会失效.
		if (parent && parent->_explicit_height) {
			
			float parent_height = parent->_final_height;
			
			switch(_height.type) {
				case ValueType::AUTO: // AUTO
					_final_height = (_texture->height() ? _texture->height(): _final_width) * width_ratio;
					_limit.height(_final_height);
					_box_inl__solve_vertical_size_with_explicit_height(this, parent_height);
					break;
				case ValueType::FULL: // FULL
					_box_inl__solve_vertical_size_with_full_height(this, parent_height);
					break;
				case ValueType::PIXEL: // 像素值
					_final_height = _height.value;
					_limit.height(_final_height);
					_box_inl__solve_vertical_size_with_explicit_height(this, parent_height);
					break;
				case ValueType::PERCENT: // 百分比
					_final_height = _height.value * parent_height;
					_limit.height(_final_height);
					_box_inl__solve_vertical_size_with_explicit_height(this, parent_height);
					break;
				default: // 减法值
					_final_height = FX_MAX(parent_height - _height.value, 0);
					_limit.height(_final_height);
					_box_inl__solve_vertical_size_with_explicit_height(this, parent_height);
					break;
			}
			
		} else {
			
			if (_margin_top.type == ValueType::PIXEL) {
				_final_margin_top = _margin_top.value;
			} else {
				_final_margin_top = 0;
			}
			
			if (_margin_bottom.type == ValueType::PIXEL) {
				_final_margin_bottom = _margin_bottom.value;
			} else {
				_final_margin_bottom = 0;
			}
			
			_raw_client_height = _border_top_width + _border_bottom_width +
														_final_margin_top + _final_margin_bottom;
			
			_explicit_height = true;  // 明确的宽度,不受内部挤压
			
			if (_height.type == ValueType::PIXEL) {
				_final_height = _height.value;
				_limit.height(_final_height);
			} else {
				_final_height = (_texture->height() ? _texture->height(): _final_width) * width_ratio;
				_limit.height(_final_height);
				
				// 如果父盒子为水平布局同时没有明确的宽度,FULL宽度会导致三次布局
				if ( parent && _height.type == ValueType::FULL ) { // 使用父视图的限制
					_limit.height(parent->_limit.height() - _raw_client_height);
					_explicit_width = false; // 不是明确的宽度,受内部挤压
				}
			}
			
			_raw_client_height += _final_height;
		}
	}
	
	/**
	 * @func texture_change_handle()
	 */
	void texture_change_handle(Event<int, Texture>& evt) { // 收到图像变化通知
		GUILock lock;
		int status = *evt.data();
		
		if (status & TEXTURE_CHANGE_OK) {
			mark_pre(M_LAYOUT | M_SIZE_HORIZONTAL | M_SIZE_VERTICAL | M_TEXTURE); // 标记
		}
		if (status & TEXTURE_CHANGE_ERROR) {
			Error err(ERR_IMAGE_LOAD_ERROR, "Image load error, %s", *evt.sender()->id());
			Handle<GUIEvent> evt = New<GUIEvent>(this, err);
			trigger(GUI_EVENT_ERROR, **evt);
		} else if (status & TEXTURE_COMPLETE) {
			display_port()->next_frame(Cb([this](CbData& e) {
				trigger(GUI_EVENT_LOAD);
			}, this));
		}
	}

};

/**
 * @constructor
 */
Image::Image()
: _tex_level(Texture::LEVEL_0)
, _texture(draw_ctx()->empty_texture())
{
	_texture->retain(); // 保持纹理
}

Image* Image::create(cString& src) {
	Image* img = New<Image>();
	img->set_src(src);
	return img;
}

/**
 * @destructor
 */
Image::~Image() {
	_texture->FX_Off(change, &Inl::texture_change_handle, _inl(this));
	_texture->release(); // 释放纹理
}

/**
 * @overwrite
 */
void Image::draw(Draw* draw) {
	if ( _visible ) {
		if ( mark_value ) {
			solve();
		}
		draw->draw(this);
		mark_value = M_NONE;
	}
}

/**
 * @func src # 图像路径
 * @ret {String}
 * @const
 */
String Image::src() const {
	return source();
}

/**
 * @func set_src(value) src路径,设置一个路径相当设置了一个对应路径的纹理对像
 * @arg value {cString}
 */
void Image::set_src(cString& value) {
	set_source(value);
}

/**
 * @func source
 */
String Image::source() const {
	return _texture->id();
}

/**
 * @func source
 */
void Image::set_source(cString& value) {
	if ( value.is_empty() ) {
		set_texture(draw_ctx()->empty_texture());
	} else {
		set_texture(tex_pool()->get_texture(value));
	}
}

/**
 * @func source_width
 */
uint32_t Image::source_width() const {
	return _texture->width();
}

/**
 * @func source_width
 */
uint32_t Image::source_height() const {
	return _texture->height();
}

/**
 * @func set_texture
 */
void Image::set_texture(Texture* value) {
	ASSERT(value);
	if (value == _texture) return;
	_texture->FX_Off(change, &Image::Inl::texture_change_handle, _inl(this));
	_texture->release(); // 释放
	_texture = value;
	_texture->retain(); // 保持
	_texture->FX_On(change, &Image::Inl::texture_change_handle, _inl(this));
	mark_pre(M_LAYOUT | M_SIZE_HORIZONTAL | M_SIZE_VERTICAL | M_TEXTURE);
}

/**
 * @overwrite
 */
void Image::set_layout_explicit_size() {
	
	if ( _final_visible ) {
		// 只需要解决 explicit size
		
		if ( mark_value & (M_SIZE_HORIZONTAL | M_SIZE_VERTICAL) ) {
			_inl(this)->solve_explicit_size();
		} else {
			return;
		}
	} else {
		//if ( ! (mark_value & (M_SIZE_HORIZONTAL | M_SIZE_VERTICAL)) ) {
		if ( ! (mark_value & M_VISIBLE) ) {
			return;
		}
	}
	// M_SIZE_HORIZONTAL | M_SIZE_VERTICAL
	// 布局尺寸改变可能会影响到所有的兄弟视图的偏移值(准确的说应该只可能会影响当前视图后面的兄弟视图)
	// 所以标记父视图M_CONTENT_OFFSET
	
	Layout* layout = parent()->as_layout();
	if ( layout ) {
		layout->mark_pre(M_CONTENT_OFFSET);
	} else {
		set_default_offset_value(); // 父视图只是个普通视图,默认将偏移设置为0
	}
	
}

/**
 * @overwrite
 */
void Image::set_layout_content_offset() {
	if (_final_visible) {
		Vec2 squeeze;
		
		Vec2 limit_min(_texture->width(), _texture->height());
		
		if ( limit_min.width() > _limit.width() ) {
			limit_min.width(_limit.width());
		}
		if ( limit_min.height() > _limit.height() ) {
			limit_min.height(_limit.height());
		}
		
		if ( set_div_content_offset(squeeze, limit_min) ) {
			// 高度或宽度被挤压即形状发生变化
			mark(M_SHAPE);
			
			// 被挤压会影响到所有的兄弟视图的偏移值
			// 所以标记父视图M_CONTENT_OFFSET
			Layout* layout = parent()->as_layout();
			if (layout) {
				layout->mark_pre(M_CONTENT_OFFSET);
			} else { // 父视图只是个普通视图,默认将偏移设置为0
				set_default_offset_value();
			}
		}
	}
}

void Image::set_draw_visible() {
	Div::set_draw_visible();
	_tex_level = _texture->get_texture_level_from_convex_quadrilateral(_final_vertex);
}

}
