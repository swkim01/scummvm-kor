/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This _program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This _program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this _program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 */

#include "backends/platform/3ds/osystem.h"
#include "backends/platform/3ds/shader_shbin.h"
#include "common/rect.h"

// Used to transfer the final rendered display to the framebuffer
#define DISPLAY_TRANSFER_FLAGS                                                 \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |                     \
	 GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |  \
	 GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |                            \
	 GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

void OSystem_3DS::initGraphics() {
	_pfGame = Graphics::PixelFormat::createFormatCLUT8();
	_pfGameTexture = Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0);

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	// Initialize the render targets
	_renderTargetTop =
	    C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetClear(_renderTargetTop, C3D_CLEAR_ALL, 0x0000000, 0);
	C3D_RenderTargetSetOutput(_renderTargetTop, GFX_TOP, GFX_LEFT,
	                          DISPLAY_TRANSFER_FLAGS);

	_renderTargetBottom =
	    C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetClear(_renderTargetBottom, C3D_CLEAR_ALL, 0x00000000, 0);
	C3D_RenderTargetSetOutput(_renderTargetBottom, GFX_BOTTOM, GFX_LEFT,
	                          DISPLAY_TRANSFER_FLAGS);

	// Load and bind simple default shader (shader.v.pica)
	_dvlb = DVLB_ParseFile((u32*)shader_shbin, shader_shbin_size);
	shaderProgramInit(&_program);
	shaderProgramSetVsh(&_program, &_dvlb->DVLE[0]);
	C3D_BindProgram(&_program);
	
	_projectionLocation = shaderInstanceGetUniformLocation(_program.vertexShader, "projection");
	_modelviewLocation = shaderInstanceGetUniformLocation(_program.vertexShader, "modelView");
	
	C3D_AttrInfo *attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord
	
	Mtx_OrthoTilt(&_projectionTop, 0.0, 400.0, 240.0, 0.0, 0.0, 1.0);
	Mtx_OrthoTilt(&_projectionBottom, 0.0, 320.0, 240.0, 0.0, 0.0, 1.0);
	
	C3D_TexEnv *env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, 0, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
	
	C3D_DepthTest(false, GPU_GEQUAL, GPU_WRITE_ALL);
	C3D_CullFace(GPU_CULL_NONE);
}

void OSystem_3DS::destroyGraphics() {
	_gameScreen.free();
	_gameTexture.free();
	_overlay.free();

	shaderProgramFree(&_program);
	DVLB_Free(_dvlb);

	C3D_RenderTargetDelete(_renderTargetTop);
	C3D_RenderTargetDelete(_renderTargetBottom);

	C3D_Fini();
}

bool OSystem_3DS::hasFeature(OSystem::Feature f) {
	return (f == OSystem::kFeatureFullscreenMode ||
	        f == OSystem::kFeatureCursorPalette ||
	        f == OSystem::kFeatureOverlaySupportsAlpha);
}

void OSystem_3DS::setFeatureState(OSystem::Feature f, bool enable) {
	switch (f) {
	case OSystem::kFeatureFullscreenMode:
		_isFullscreen = enable;
		break;
	case OSystem::kFeatureCursorPalette:
		_cursorPaletteEnabled = enable;
		flushCursor();
		break;
	default:
		break;
	}
}

bool OSystem_3DS::getFeatureState(OSystem::Feature f) {
	switch (f) {
	case OSystem::kFeatureFullscreenMode:
		return _isFullscreen;
	case OSystem::kFeatureCursorPalette:
		return _cursorPaletteEnabled;
	default:
		return false;
	}
}

const OSystem::GraphicsMode *
OSystem_3DS::getSupportedGraphicsModes() const {
	return s_graphicsModes;
}

int OSystem_3DS::getDefaultGraphicsMode() const {
	return GFX_LINEAR;
}

bool OSystem_3DS::setGraphicsMode(int mode) {
	return true;
}

void OSystem_3DS::resetGraphicsScale() {
	debug("resetGraphicsScale");
}

int OSystem_3DS::getGraphicsMode() const {
	return GFX_LINEAR;
}
void OSystem_3DS::initSize(uint width, uint height,
                                   const Graphics::PixelFormat *format) {
	debug("3ds initsize w:%d h:%d", width, height);
	_gameWidth = width;
	_gameHeight = height;
	_gameTexture.create(width, height, _pfGameTexture);
	_overlay.create(getOverlayWidth(), getOverlayHeight(), _pfGameTexture);
	
	if (format) {
		debug("pixelformat: %d %d %d %d %d", format->bytesPerPixel, format->rBits(), format->gBits(), format->bBits(), format->aBits());;
		_pfGame = *format;
	}

	_gameScreen.create(width, height, _pfGame);
	
	_focusDirty = true;
	_focusRect = Common::Rect(_gameWidth, _gameHeight);

	if (_isFullscreen) {
		_gameRatio = 320.f / 240.f;
		_gameX = _gameY = 0;
		_gameTexture.setScale(320.f / width, 240.f / height);
	} else {
		_gameRatio = static_cast<float>(width) / height;
		if (width > height) {
			_gameX = 0;
			_gameY = (240.f - 320.f / width * height) / 2.f;
		} else {
			_gameY = 0;
			_gameX = (320.f - 240.f / height * width) / 2.f;
		}
		_gameTexture.setScale((width > 320) ? 320.f / width : 1.f,
		                      (height > 240) ? 240.f / height : 1.f);
	}
	_gameTexture.setPosition(_gameX, _gameY);
	_cursorTexture.setScale(_gameTexture.getScaleX(), _gameTexture.getScaleY());
	
	float ratio = 400.f / _gameWidth;
	int y = (_gameHeight * ratio - 240.f) / 2;
	Mtx_Identity(&_focusMatrix);
	Mtx_Translate(&_focusMatrix, 0, -y, 0);
	Mtx_Scale(&_focusMatrix, ratio, ratio, 1.f);
}

Common::List<Graphics::PixelFormat> OSystem_3DS::getSupportedFormats() const {
	Common::List<Graphics::PixelFormat> list;
	list.push_back(Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0)); // GPU_RGBA8
	list.push_back(Graphics::PixelFormat(2, 5, 6, 5, 0, 11, 5, 0, 0)); // GPU_RGB565
// 		list.push_back(Graphics::PixelFormat(3, 0, 0, 0, 8, 0, 8, 16, 0)); // GPU_RGB8
	list.push_back(Graphics::PixelFormat(2, 5, 5, 5, 0, 10, 5, 0, 0)); // RGB555 (needed for FMTOWNS?)
	list.push_back(Graphics::PixelFormat(2, 5, 5, 5, 1, 11, 6, 1, 0)); // GPU_RGBA5551
	list.push_back(Graphics::PixelFormat::createFormatCLUT8());
	return list;
}

void OSystem_3DS::beginGFXTransaction() {
	//
}
OSystem::TransactionError OSystem_3DS::endGFXTransaction() {
	return OSystem::kTransactionSuccess;
}

void OSystem_3DS::setPalette(const byte *colors, uint start, uint num) {
	assert(start + num <= 256);
	memcpy(_palette + 3 * start, colors, 3 * num);
	
	// Manually update all color that were changed
	if (_gameScreen.format.bytesPerPixel == 1) {
		flushGameScreen();
	}
}
void OSystem_3DS::grabPalette(byte *colors, uint start, uint num) {
	assert(start + num <= 256);
	memcpy(colors, _palette + 3 * start, 3 * num);
}

void OSystem_3DS::copyRectToScreen(const void *buf, int pitch, int x,
                                           int y, int w, int h) {
	Common::Rect rect(x, y, x+w, y+h);
	_gameScreen.copyRectToSurface(buf, pitch, x, y, w, h);
	Graphics::Surface subSurface = _gameScreen.getSubArea(rect);
	
	Graphics::Surface *convertedSubSurface = subSurface.convertTo(_pfGameTexture, _palette);
	_gameTexture.copyRectToSurface(*convertedSubSurface, x, y, Common::Rect(w, h));
	
	convertedSubSurface->free();
	delete convertedSubSurface;
	_gameTexture.markDirty();
}

void OSystem_3DS::flushGameScreen() {
	Graphics::Surface *converted = _gameScreen.convertTo(_pfGameTexture, _palette);
	_gameTexture.copyRectToSurface(*converted, 0, 0, Common::Rect(converted->w, converted->h));
	_gameTexture.markDirty();
	converted->free();
	delete converted;
}

Graphics::Surface *OSystem_3DS::lockScreen() {
	return &_gameScreen;
}
void OSystem_3DS::unlockScreen() {
	flushGameScreen();
}

void OSystem_3DS::updateScreen() {

	if (sleeping || exiting)
		return;
	
// 	updateFocus();

	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		// Render top screen
		C3D_FrameDrawOn(_renderTargetTop);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, _projectionLocation, &_projectionTop);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, _modelviewLocation, &_focusMatrix);
		_gameTexture.render();
		_gameTexture.render();
		
		// Render bottom screen
		C3D_FrameDrawOn(_renderTargetBottom);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, _projectionLocation, &_projectionBottom);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, _modelviewLocation, _gameTexture.getMatrix());
		_gameTexture.render();
		if (_overlayVisible) {
			C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, _modelviewLocation, _overlay.getMatrix());
			_overlay.render();
		}
		if (_cursorVisible) {
			C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, _modelviewLocation, _cursorTexture.getMatrix());
			_cursorTexture.render();
		}
	C3D_FrameEnd(0);
}

void OSystem_3DS::setShakePos(int shakeOffset) {
	// TODO: implement this in overlay, top screen, and mouse too
	_screenShakeOffset = shakeOffset;
	_gameTexture.setPosition(_gameX, _gameY + shakeOffset);
}

void OSystem_3DS::setFocusRectangle(const Common::Rect &rect) {
	debug("setfocus: %d %d %d %d", rect.left, rect.top, rect.width(), rect.height());
	_focusRect = rect;
	_focusDirty = true;
	_focusClearTime = 0;
}

void OSystem_3DS::clearFocusRectangle() {
	_focusClearTime = getMillis();
}

void OSystem_3DS::updateFocus() {
	
	if (_focusClearTime && getMillis() - _focusClearTime > 5000) {
		_focusClearTime = 0;
		_focusDirty = true;
		_focusRect = Common::Rect(_gameWidth, _gameHeight);
	}

	if (_focusDirty) {
		float duration = 1.f / 20.f; // Focus animation in frame duration
		float w = 400.f;
		float h = 240.f;
		float ratio = _focusRect.width() / _focusRect.height();
		if (ratio > w/h) {
			_focusTargetScaleX = w / _focusRect.width();
			float newHeight = (float)_focusRect.width() / w/h;
			_focusTargetScaleY = h / newHeight;
			_focusTargetPosX = _focusTargetScaleX * _focusRect.left;
			_focusTargetPosY = _focusTargetScaleY * ((float)_focusRect.top - (newHeight - _focusRect.height())/2.f);
		} else {
			_focusTargetScaleY = h / _focusRect.height();
			float newWidth = (float)_focusRect.height() * w/h;
			_focusTargetScaleX = w / newWidth;
			_focusTargetPosY = _focusTargetScaleY * _focusRect.top;
			_focusTargetPosX = _focusTargetScaleX * ((float)_focusRect.left - (newWidth - _focusRect.width())/2.f);
		}
		if (_focusTargetPosX < 0 && _focusTargetScaleY != 240.f / _gameHeight)
			_focusTargetPosX = 0;
		if (_focusTargetPosY < 0 && _focusTargetScaleX != 400.f / _gameWidth)
			_focusTargetPosY = 0;
		_focusStepPosX = duration * (_focusTargetPosX - _focusPosX);
		_focusStepPosY = duration * (_focusTargetPosY - _focusPosY);
		_focusStepScaleX = duration * (_focusTargetScaleX - _focusScaleX);
		_focusStepScaleY = duration * (_focusTargetScaleY - _focusScaleY);
	}
	
	if (_focusDirty || _focusPosX != _focusTargetPosX || _focusPosY != _focusTargetPosY ||
			_focusScaleX != _focusTargetScaleX || _focusScaleY != _focusTargetScaleY) {
		_focusDirty = false;
	
		if ((_focusStepPosX > 0 && _focusPosX > _focusTargetPosX) || (_focusStepPosX < 0 && _focusPosX < _focusTargetPosX))
			_focusPosX = _focusTargetPosX;
		else if (_focusPosX != _focusTargetPosX)
			_focusPosX += _focusStepPosX;
		
		if ((_focusStepPosY > 0 && _focusPosY > _focusTargetPosY) || (_focusStepPosY < 0 && _focusPosY < _focusTargetPosY))
			_focusPosY = _focusTargetPosY;
		else if (_focusPosY != _focusTargetPosY)
			_focusPosY += _focusStepPosY;
		
		if ((_focusStepScaleX > 0 && _focusScaleX > _focusTargetScaleX) || (_focusStepScaleX < 0 && _focusScaleX < _focusTargetScaleX))
			_focusScaleX = _focusTargetScaleX;
		else if (_focusScaleX != _focusTargetScaleX)
			_focusScaleX += _focusStepScaleX;
		
		if ((_focusStepScaleY > 0 && _focusScaleY > _focusTargetScaleY) || (_focusStepScaleY < 0 && _focusScaleY < _focusTargetScaleY))
			_focusScaleY = _focusTargetScaleY;
		else if (_focusScaleY != _focusTargetScaleY)
			_focusScaleY += _focusStepScaleY;

		Mtx_Identity(&_focusMatrix);
		Mtx_Translate(&_focusMatrix, -_focusPosX, -_focusPosY, 0);
		Mtx_Scale(&_focusMatrix, _focusScaleX, _focusScaleY, 1.f);
	}
}

void OSystem_3DS::showOverlay() {
	_overlayVisible = true;
	_cursorTexture.setScale(1.f, 1.f);
	updateScreen();
}

void OSystem_3DS::hideOverlay() {
	_overlayVisible = false;
	_cursorTexture.setScale(_gameTexture.getScaleX(), _gameTexture.getScaleY());
	updateScreen();
}

Graphics::PixelFormat OSystem_3DS::getOverlayFormat() const {
	return _pfGameTexture;
}

void OSystem_3DS::clearOverlay() {
	_overlay.clear();
}

void OSystem_3DS::grabOverlay(void *buf, int pitch) {
	for (int y = 0; y < getOverlayHeight(); ++y) {
		memcpy(buf, _overlay.getBasePtr(0, y), pitch);
	}
}

void OSystem_3DS::copyRectToOverlay(const void *buf, int pitch, int x,
                                            int y, int w, int h) {
	_overlay.copyRectToSurface(buf, pitch, x, y, w, h);
	_overlay.markDirty();
}

int16 OSystem_3DS::getOverlayHeight() {
	return 240;
}

int16 OSystem_3DS::getOverlayWidth() {
	return 320;
}

bool OSystem_3DS::showMouse(bool visible) {
	_cursorVisible = visible;
	flushCursor();
	return !visible;
}

void OSystem_3DS::warpMouse(int x, int y) {
	_cursorX = x;
	_cursorY = y;
	// TODO: adjust for _cursorScalable ?
	_cursorTexture.setPosition(x - _cursorHotspotX + (_overlayVisible ? 0 : _gameX),
							   y - _cursorHotspotY + (_overlayVisible ? 0 : _gameY));
}

void OSystem_3DS::setMouseCursor(const void *buf, uint w, uint h,
                                         int hotspotX, int hotspotY,
                                         uint32 keycolor, bool dontScale,
                                         const Graphics::PixelFormat *format) {
	_cursorScalable = !dontScale;
	_cursorHotspotX = hotspotX;
	_cursorHotspotY = hotspotY;
	_cursorKeyColor = keycolor;
	_pfCursor = !format ? Graphics::PixelFormat::createFormatCLUT8() : *format;

	if (w != _cursor.w || h != _cursor.h || _cursor.format != _pfCursor) {
		_cursor.create(w, h, _pfCursor);
		_cursorTexture.create(w, h, _pfGameTexture);
	}
	
	_cursor.copyRectToSurface(buf, w, 0, 0, w, h);
	flushCursor();
	
	warpMouse(_cursorX, _cursorY);
}

void OSystem_3DS::setCursorPalette(const byte *colors, uint start, uint num) {
	assert(start + num <= 256);
	memcpy(_cursorPalette + 3 * start, colors, 3 * num);
	_cursorPaletteEnabled = true;
	flushCursor();
}

void OSystem_3DS::flushCursor() {
	if (_cursor.getPixels()) {
		Graphics::Surface *converted = _cursor.convertTo(_pfGameTexture, _cursorPaletteEnabled ? _cursorPalette : _palette);
		_cursorTexture.copyRectToSurface(*converted, 0, 0, Common::Rect(converted->w, converted->h));
		_cursorTexture.markDirty();
		converted->free();
		delete converted;
	
		if (_pfCursor.bytesPerPixel == 1) {
			uint* dest = (uint*) _cursorTexture.getPixels();
			byte* src = (byte*) _cursor.getPixels();
			for (int y = 0; y < _cursor.h; ++y) {
				for (int x = 0; x < _cursor.w; ++x) {
					if (*src++ == _cursorKeyColor)
						*dest++ = 0;
					else
						dest++;
				}
				dest += _cursorTexture.w - _cursorTexture.actualWidth;
			}
		}
	}
}
