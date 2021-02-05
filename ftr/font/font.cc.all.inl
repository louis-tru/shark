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

namespace ftr {

/**
 * @get font {Font*}
 */
inline Font* FontGlyph::font() {
	return _container->font;
}

void Font::Inl::initialize(
	FontPool* pool,
	FontFamily* family,
	String font_name,
	TextStyleEnum style,
	uint num_glyphs,
	uint face_index,
	int height,       /* text height in 26.6 frac. pixels       */
	int max_advance,  /* max horizontal advance, in 26.6 pixels */
	int ascender,     /* ascender in 26.6 frac. pixels          */
	int descender,    /* descender in 26.6 frac. pixels         */
	int underline_position,
	int underline_thickness,
	FT_Library lib
) {
	_pool = pool;
	_font_family = family;
	_font_name = font_name;
	_style = style;
	_num_glyphs = num_glyphs;
	_ft_glyph = nullptr;
	_face_index = face_index;
	_ft_lib = lib;
	_ft_face = nullptr;
	_descender = 0;
	_height = 0;
	_max_advance = 0;
	_ascender = 0;
	_containers = nullptr;
	_flags = nullptr;
	_height = height;
	_max_advance = max_advance;
	_ascender = ascender;
	_descender = descender;
	_underline_position = underline_position;
	_underline_thickness = underline_thickness;
}

/**
 * @func move_to # 鏂扮殑澶氳竟褰㈠紑濮�
 */
int Font::Inl::move_to(const FT_Vector* to, void* user) {
	DecomposeData* data = static_cast<DecomposeData*>(user);
	if (data->length) { // 娣诲姞涓€涓杈瑰舰
		tessAddContour(data->tess, 2, *data->vertex, sizeof(Vec2), data->length);
		data->length = 0;
	}
	//    LOG("move_to:%d,%d", to->x, to->y);
	
	Vec2 vertex = Vec2(to->x, -to->y);
	*data->push_vertex(1) = vertex;
	data->p0 = vertex;
	return FT_Err_Ok;
}

/**
 * @func line_to # 涓€娆¤礉濉炲皵鐐癸紙鐩寸嚎锛�
 */
int Font::Inl::line_to(const FT_Vector* to, void* user) {
	DecomposeData* data = static_cast<DecomposeData*>(user);
	Vec2 vertex = Vec2(to->x, -to->y);
	//  LOG("line_to:%d,%d", to->x, to->y);
	*data->push_vertex(1) = vertex;
	data->p0 = vertex;
	return FT_Err_Ok;
}

/**
 * @func line_to # 浜屾璐濆灏旀洸绾跨偣,杞崲鍒颁竴娆″灏旂偣
 */
int Font::Inl::conic_to(const FT_Vector* control, const FT_Vector* to, void* user) {
	DecomposeData* data = static_cast<DecomposeData*>(user);
	Vec2 p2 = Vec2(to->x, -to->y);
	//  LOG("conic_to:%d,%d|%d,%d", control->x, control->y, to->x, to->y);
	QuadraticBezier bezier(data->p0, Vec2(control->x, -control->y), p2);
	// 浣跨敤10鐐归噰鏍�,閲囨牱瓒婂瓒婅兘杩樺師鏇茬嚎,浣嗛渶瑕佹洿澶氭湁瀛樺偍绌洪棿
	bezier.sample_curve_points(data->sample, (float*)(data->push_vertex(data->sample - 1) - 1));
	data->p0 = p2;
	return FT_Err_Ok;
}

/**
 * @func line_to # 涓夋璐濆灏旀洸绾跨偣,杞崲鍒颁竴娆″灏旂偣
 */
int Font::Inl::cubic_to(const FT_Vector* control1,
												const FT_Vector* control2,
												const FT_Vector* to, void* user) 
{
	DecomposeData* data = static_cast<DecomposeData*>(user);
	Vec2 p3 = Vec2(to->x, -to->y);
	//  LOG("cubic_to:%d,%d|%d,%d|%d,%d",
	//        control1->x, control1->y, control2->x, control2->y, to->x, to->y);
	CubicBezier bezier(data->p0,
										 Vec2(control1->x, -control1->y),
										 Vec2(control2->x, -control2->y), p3);
	bezier.sample_curve_points(data->sample, (float*)(data->push_vertex(data->sample - 1) - 1));
	data->p0 = p3;
	return FT_Err_Ok;
}

/**
 * @func del_glyph_data
 */
void Font::Inl::del_glyph_data(GlyphContainer* container) {
	
	if ( container ) {
		FontGlyph* glyph = container->glyphs;
		auto ctx = _pool->_draw_ctx;
		
		for ( int i = 0; i < 128; i++, glyph++ ) {
			if ( glyph->_vertex_value ) {
				ctx->del_buffer( glyph->_vertex_value );
				glyph->_vertex_value = 0;
				glyph->_vertex_count = 0;
			}
			
			for ( int i = 1; i < 12; i++ ) {
				if ( glyph->_textures[i] ) { // 鍒犻櫎绾圭悊
					ctx->del_texture( glyph->_textures[i] );
				}
			}
			memset(glyph->_textures, 0, sizeof(uint) * 13);
		}
		_pool->_total_data_size -= container->data_size;
		container->use_count = 0;
		container->data_size = 0;
	}
}

/**
 * @func get_glyph
 */
FontGlyph* Font::Inl::get_glyph(uint16_t unicode, uint region,
																uint index, FGTexureLevel level, bool vector) 
{
	ASSERT(region < 512);
	ASSERT(index < 128);
	
	load(); ASSERT(_ft_face);
	
	GlyphContainerFlag* flags = _flags[region];
	
	if ( !flags ) {
		flags = new GlyphContainerFlag();
		_flags[region] = flags;
		memset(flags, 0, sizeof(GlyphContainerFlag));
	}
	
	FontGlyph* glyph = nullptr;
	
	switch ( flags->flags[index] ) {
		default:
		{
			return nullptr;
		}
		case CF_NO_READY:
		{
			uint16_t glyph_index = FT_Get_Char_Index( (FT_Face)_ft_face, unicode );
			
			if (! glyph_index ) goto cf_none;
			
			GlyphContainer* container = _containers[region];
			if ( !container ) {
				_containers[region] = container = new GlyphContainer();
				memset(container, 0, sizeof(GlyphContainer));
				container->font = this;
			}
			
			FT_Error error = FT_Set_Char_Size( (FT_Face)_ft_face, 0, 64 * 64, 72, 72);
			if (error) {
				FX_WARN("%s", "parse font glyph vbo data error"); goto cf_none;
			}
			
			error = FT_Load_Glyph( (FT_Face)_ft_face, glyph_index, FT_LOAD_NO_HINTING);
			if (error) {
				FX_WARN("%s", "parse font glyph vbo data error"); goto cf_none;
			}
		
			FT_GlyphSlot ft_glyph = (FT_GlyphSlot)_ft_glyph;
			
			glyph = container->glyphs + index;
			glyph->_container = container;
			glyph->_unicode = unicode;
			glyph->_glyph_index = glyph_index;
			glyph->_hori_bearing_x = ft_glyph->metrics.horiBearingX;
			glyph->_hori_bearing_y = ft_glyph->metrics.horiBearingY;
			glyph->_hori_advance = ft_glyph->metrics.horiAdvance;
			glyph->_have_outline = ft_glyph->outline.points;
			
			if (vector) {
				if ( ! set_vertex_data(glyph) ) {
					goto cf_none;
				}
			} else if ( FontGlyph::LEVEL_NONE != level ) {
				if ( ! set_texture_data(glyph, level) ) {
					goto cf_none;
				}
			}
			
			flags->flags[index] = CF_READY;
		}
		case CF_READY:
		{
			glyph = _containers[region]->glyphs + index;
			
			if (vector) {
				if ( ! glyph->vertex_data() ) {
					if ( ! set_vertex_data(glyph) ) {
						goto cf_none;
					}
				}
			} else if ( FontGlyph::LEVEL_NONE != level ) {
				if ( ! glyph->has_texure_level(level) ) {
					if ( ! set_texture_data(glyph, level) ) {
						goto cf_none;
					}
				}
			}
			break;
		}
		// switch end
	}
	
	return glyph;
	
 cf_none:
	flags->flags[index] = CF_NONE;
	return nullptr;
}

/**
 * @func clear
 * 鍦ㄨ皟鐢ㄨ繖涓柟娉曞悗姝ゅ墠閫氳繃浠讳綍閫斿緞鑾峰彇鍒扮殑瀛楀瀷鏁版嵁灏嗕笉鑳藉啀浣跨敤
 * 鍖呮嫭浠嶧ontGlyphTable鑾峰彇鍒扮殑瀛楀瀷,璋冪敤FontGlyphTable::Inl.clear_table()鍙竻鐞嗗紩鐢�
 */
void Font::Inl::clear(bool full) {
	if ( _ft_face ) {
		if ( full ) { // 瀹屽叏娓呯悊
			
			for (int i = 0; i < 512; i++) {
				del_glyph_data( _containers[i] );
				delete _containers[i]; _containers[i] = nullptr;
				delete _flags[i]; _flags[i] = nullptr;
			}
			delete _containers; _containers = nullptr;
			delete _flags; _flags = nullptr;
			
			FT_Done_Face((FT_Face)_ft_face);
			_ft_face = nullptr;
			_ft_glyph = nullptr;
			
		} else {
			struct Container {
				GlyphContainer* container;
				int region;
				uint64 use_count;
			};
			uint64 total_data_size = 0;
			List<Container> containers_sort;
			// 鎸変娇鐢ㄤ娇鐢ㄦ鏁版帓搴�
			for (int regioni = 0; regioni < 512; regioni++) {
				GlyphContainer* con = _containers[regioni];
				if ( con ) {
					auto it = containers_sort.end();
					uint64 use_count = con->use_count;
					
					for ( auto& j : containers_sort ) {
						if ( use_count <= j.value().use_count ) {
							it = j; break;
						}
					}
					if ( it.is_null() ) {
						containers_sort.push({ con, regioni, use_count });
					} else {
						containers_sort.before(it, { con, regioni, use_count });
					}
					total_data_size += con->data_size;
					con->use_count /= 2;
				} else { // 瀹瑰櫒涓嶅瓨鍦�,鏍囧織涔熶笉闇€瑕佸瓨鍦�
					delete _flags[regioni]; _flags[regioni] = nullptr;
				}
			}
			
			if ( containers_sort.length() ) {
				uint64 total_data_size_1_3 = total_data_size / 3;
				uint64 del_data_size = 0;
				// 浠庢帓搴忓垪琛ㄩ《閮ㄥ紑濮嬪垹闄ゆ€诲閲忕殑1/3,骞剁疆闆跺鍣ㄤ娇鐢ㄦ鏁�
				auto last = --containers_sort.end();
				
				for ( auto it = containers_sort.begin(); it != last; it++ ) {
					if ( del_data_size < total_data_size_1_3 ) {
						int region = it.value().region;
						del_data_size += it.value().container->data_size;
						del_glyph_data( it.value().container );
						delete _containers[region]; _containers[region] = nullptr;
						delete _flags[region]; _flags[region] = nullptr;
					}
				}
				// 濡傛灉鍒犻櫎鍒版渶鍚庝竴涓鍣ㄨ繕涓嶈冻鎬诲閲忕殑1/3,骞朵笖鏈€鍚庝竴涓鍣ㄦ€诲閲忚秴杩�512kb涔熶竴骞跺垹闄�
				if ( del_data_size < total_data_size_1_3 ) {
					if ( last.value().container->data_size > 512 * 1024 ) {
						int region = last.value().region;
						del_glyph_data( last.value().container );
						delete _containers[region]; _containers[region] = nullptr;
						delete _flags[region]; _flags[region] = nullptr;
					}
				}
				
				FX_DEBUG("Font memory clear, %ld", del_data_size);
			}
			// not full clear end
		}
	}
}

/**
 * @destructor
 */
Font::~Font() {
	_inl_font(this)->clear(true);
}

cString& Font::name() const {
	return _font_name;
}

Font* Font::font(TextStyleEnum style) {
	return this;
}

/**
 * @func load
 */
bool Font::load() {
	if ( !_ft_face ) {
		install();
		
		if ( !_ft_face ) {
			FX_ERR("Unable to install font");
			return false;
		}
		
		_ft_glyph = ((FT_Face)_ft_face)->glyph;
		
		if ( ! _containers ) { // 鍒涘缓鍧楀鍣�
			_containers = new GlyphContainer*[512];
			_flags = new GlyphContainerFlag*[512];
			memset( _containers, 0, sizeof(GlyphContainer*) * 512);
			memset( _flags, 0, sizeof(GlyphContainerFlag*) * 512);
		}
	}
	return true;
}

/**
 * @func unload
 */
void Font::unload() {
	if ( _ft_face ) {
		for (int i = 0; i < 512; i++) {
			_inl_font(this)->del_glyph_data( _containers[i] );
		}
		FT_Done_Face((FT_Face)_ft_face);
		_ft_face = nullptr;
		_ft_glyph = nullptr;
	}
}

/**
 * @class FontFromData
 */
class FontFromData: public Font {
 public:
	
	class Data: public Reference {
	 public:
		Data(Buffer& buff)
		: value((FT_Byte*)*buff)
		, length(buff.length()), storage(buff) {}
		FT_Byte* value;
		uint  length;
		Buffer storage;
	};
	
	Data*  _data;
	
	FontFromData(Data* data) : _data(data) {
		_data->retain();
	}
	
	virtual ~FontFromData() {
		_data->release();
	}
	
	/**
	 * @overwrite
	 */
	void install() {
		ASSERT(!_ft_face);
		FT_New_Memory_Face((FT_Library)_ft_lib,
											 _data->value,
											 _data->length,
											 _face_index,
											 (FT_Face*)&_ft_face);
	}
};

/**
 * @class FontFromFile
 */
class FontFromFile: public Font {
 public:
	String  _font_path;    // 瀛椾綋鏂囦欢璺緞
	
	FontFromFile(cString& path) : _font_path(path) { }
	
	/**
	 * @overwrite
	 */
	void install() {
		ASSERT(!_ft_face);
		FT_New_Face((FT_Library)_ft_lib,
								Path::fallback_c(_font_path),
								_face_index,
								(FT_Face*)&_ft_face);
	}
};

// ------------------------ font glyph ------------------------

class FontFamilysID::Inl: public FontFamilysID {
 public:
	#define _inl_ff_id(self) static_cast<FontFamilysID::Inl*>(self)
	
	void initialize(const Array<String>& names) {
		_names = names;
		_name = names.join(',');
		_code = _name.hash_code();
	}
};

class FontGlyphTable::Inl: public FontGlyphTable {
 public:
	#define _inl_table(self) static_cast<FontGlyphTable::Inl*>(self)
	
	void clear_table() {
		for ( int i = 0; i < 512; i++ ) {
			delete _blocks[i]; _blocks[i] = nullptr;
		}
	}
	
	void initialize(cFFID ffid, TextStyleEnum style, FontPool* pool) {
		_ffid = ffid;
		_style = style;
		_pool = pool;
		make();
	}
	
	void make() {
		clear_table();
		_fonts.clear();
		
		Map<String, bool> fonts_name;
		
		for ( auto& i : _ffid->names() ) {
			Font* font = _pool->get_font(i.value(), _style);
			if ( font && !fonts_name.has(font->name()) ) {
				if ( font->load() ) { // 杞藉叆瀛椾綋
					_fonts.push(font);
					fonts_name.set(font->name(), true);
				}
			}
		}
		
		/* 濡傛灉浣跨敤icon鍥炬爣瀛椾綋鏃讹紝鍔犲叆榛樿瀛椾綋浼氬鑷翠笉鍚岀郴缁焋ascender`涓巂descender`鏈夋墍宸紓,
		 * 浠庤€屽鑷翠笉鍚岀郴缁熺殑瀛椾綋鍩虹嚎鐨勪綅缃笉鐩稿悓锛屾棤娉曚娇鐢╜icon`绮惧噯鎺掔増锛�
		 * 涓轰簡浣跨敤杩欎釜`icon`杩涜绮惧噯鎺掔増鐜板湪鏆傛椂鍋氫緥澶栧鐞嗭紝鍦ㄤ娇鐢╜icon`瀛椾綋鍚嶇О鏃朵笉鍔犲叆榛樿瀛椾綋
		 */
		
		if (_ffid->code() != 2090550926) {
			for ( auto& i : _pool->_default_fonts ) {
				Font* font = i.value()->font(_style);
				if ( ! fonts_name.has(font->name()) ) { // 閬垮厤閲嶅鐨勫悕绉�
					if ( font->load() ) { // 杞藉叆瀛椾綋
						_fonts.push(font);
						fonts_name.set(font->name(), true);
					}
				}
			}
		} else {
			ASSERT(_ffid->name() == "icon");
		}
		
		// 鏌ユ壘鏈€澶ч珮搴︿笌琛岄珮搴�
		_height = 0;
		_ascender = 0;
		_descender = 0;
		
		for ( uint i = 0; i < _fonts.length(); i++ ) {
			Font* font = _fonts[i];
			_height = FX_MAX(_height, font->height());
			_ascender = FX_MAX(_ascender, font->ascender());
			_descender = FX_MAX(_descender, font->descender());
		}
	}
	
	void set_glyph(uint region, uint index, FontGlyph* glyph) {
		ASSERT( glyph );
		if ( !_blocks[region] ) {
			GlyphsBlock* block = new GlyphsBlock();
			memset(block, 0, sizeof(GlyphsBlock));
			_blocks[region] = block;
		}
		_blocks[region]->glyphs[index] = glyph;
	}
	
	FontGlyph* get_glyph(uint16_t unicode) {
		uint region = unicode / 128;
		uint index = unicode % 128;
		GlyphsBlock* con = _blocks[region];
		if ( con ) {
			FontGlyph* glyph = con->glyphs[index];
			if ( glyph ) {
				return glyph;
			}
		}
		return nullptr;
	}
	
	/**
	 * @func find_glyph
	 */
	FontGlyph* find_glyph(uint16_t unicode, FGTexureLevel level, bool vector) {
		
		FontGlyph* glyph = nullptr;
		
		uint region = unicode / 128;
		uint index = unicode % 128;
		
		if ( _fonts.length() ) {
			
			Font** begin = &_fonts[0];
			Font** end = begin + _fonts.length();
			
			do {
				glyph = _inl_font(*begin)->get_glyph(unicode, region, index, level, vector);
				if ( glyph ) {
					set_glyph(region, index, glyph); return glyph;
				}
				begin++;
			} while (begin != end);
		}
		
		/* TODO 浣跨敤涓€涓粯璁ゅ瓧褰�  锟� 65533 */
		Font* font = _pool->_spare_family->font(_style);
		
		glyph = _inl_font(font)->get_glyph(65533, 65533 / 128, 65533 % 128, level, vector);
		
		set_glyph(region, index, glyph);
		
		return glyph;
	}
	
};

FontGlyphTable::~FontGlyphTable() {
	_inl_table(this)->clear_table();
}

/**
 * @func glyph
 */
FontGlyph* FontGlyphTable::glyph(uint16_t unicode) {
	FontGlyph* glyph = _inl_table(this)->get_glyph(unicode);
	if ( glyph ) {
		return glyph;
	}
	return _inl_table(this)->find_glyph(unicode, FontGlyph::LEVEL_NONE, false);
}

/**
 * @func use_texture_glyph 浣跨敤绾圭悊瀛楀瀷
 */
FontGlyph* FontGlyphTable::use_texture_glyph(uint16_t unicode, FGTexureLevel level) {
	ASSERT(level < FontGlyph::LEVEL_NONE);
	
	FontGlyph* glyph = _inl_table(this)->get_glyph(unicode);
	
	if ( glyph ) {
		if ( glyph->has_texure_level(level) ) {
			glyph->_container->use_count++; return glyph;
		}
		else { // 鍏堝皾璇曚娇鐢ㄦ渶蹇殑鏂规硶
			if ( _inl_font(glyph->font())->set_texture_data(glyph, level) ) { //
				glyph->_container->use_count++; return glyph;
			}
		}
	}
	glyph = _inl_table(this)->find_glyph(unicode, level, false);
	
	glyph->_container->use_count++; return glyph;
}

/**
 * @func use_vector_glyph # 浣跨敤瀛楀瀷,骞朵笖杞藉叆vbo鐭㈤噺椤剁偣鏁版嵁
 */
FontGlyph* FontGlyphTable::use_vector_glyph(uint16_t unicode) {
	FontGlyph* glyph = _inl_table(this)->get_glyph(unicode);
	
	if ( glyph ) {
		if ( glyph->vertex_data() ) {
			glyph->_container->use_count++; return glyph;
		}
		else { // 鍏堝皾璇曚娇鐢ㄦ渶蹇殑鏂规硶
			if ( _inl_font(glyph->font())->set_vertex_data(glyph) ) { //
				glyph->_container->use_count++; return glyph;
			}
		}
	}
	glyph = _inl_table(this)->find_glyph(unicode, FontGlyph::LEVEL_NONE, true);
	
	glyph->_container->use_count++; return glyph;
}

// ------------------------FontFamily::Inl------------------------

class FontFamily::Inl: public FontFamily {
 public:
	#define _inl_family(self) static_cast<FontFamily::Inl*>(self)
	
	int get_font_style_index(TextStyleEnum style) {
		int index;
		switch(style) {
			case TextStyleEnum::THIN: index = 0; break;
			case TextStyleEnum::ULTRALIGHT: index = 1; break;
			case TextStyleEnum::LIGHT: index = 2; break;
			case TextStyleEnum::REGULAR: index = 3; break;
			case TextStyleEnum::MEDIUM: index = 4; break;
			case TextStyleEnum::SEMIBOLD: index = 5; break;
			case TextStyleEnum::BOLD: index = 6; break;
			case TextStyleEnum::HEAVY: index = 7; break;
			case TextStyleEnum::BLACK: index = 8; break;
			case TextStyleEnum::BLACK_ITALIC: index = 9; break;
			case TextStyleEnum::HEAVY_ITALIC: index = 10; break;
			case TextStyleEnum::BOLD_ITALIC: index = 11; break;
			case TextStyleEnum::SEMIBOLD_ITALIC: index = 12; break;
			case TextStyleEnum::MEDIUM_ITALIC: index = 13; break;
			case TextStyleEnum::ITALIC: index = 14; break;
			case TextStyleEnum::LIGHT_ITALIC: index = 15; break;
			case TextStyleEnum::ULTRALIGHT_ITALIC: index = 16; break;
			case TextStyleEnum::THIN_ITALIC: index = 17; break;
			default: index = 18; break;
		}
		return index;
	}
	
	/**
	 * @func add_font
	 */
	void add_font(Font* font) {
		int index = get_font_style_index(font->style());
		if ( !_fonts[index] || _fonts[index]->num_glyphs() < font->num_glyphs() ) {
			_fonts[index] = font;
		}
		_all_fonts.push(font);
	}
};

/**
 * @constructor
 */
FontFamily::FontFamily(cString& family_name)
: _family_name(family_name)
, _fonts()
{
	memset(_fonts, 0, sizeof(_fonts));
}

/**
 * @func font_names
 */
Array<String> FontFamily::font_names() const {
	Array<String> rev;
	
	for (auto i = _all_fonts.begin(),
						e = _all_fonts.end(); i != e; i++) {
		rev.push(i.value()->font_name());
	}
	return rev;
}

/**
 * @overwrite
 */
cString& FontFamily::name() const {
	return _family_name;
}

Font* FontFamily::font(TextStyleEnum style) {
	int index = _inl_family(this)->get_font_style_index(style);
	Font* font = _fonts[index];
	
	if ( font ) {
		return font;
	}
	
	int big = index + 1;
	int small = index - 1;
	
	// 鏌ユ壘鐩搁偦瀛楅噸
	while (big < 19 || small >= 0) {
		if ( small >= 0 ) {
			font = _fonts[small];
			if ( font ) break;
			small--;
		}
		if ( big < 19 ) {
			font = _fonts[big];
			if ( font ) break;
			big++;
		}
	}
	return font;
}

}
