#include "beam_config.h"

#include "../lib/litelibs/litegfx.h"
#include "../lib/stb/stretchy_buffer.h"
#include "color.h"
#ifdef USE_DEFAULT_FONT
#include "default_font.h"
#endif
#include "font.h"
#include "screen.h"
#include "texture.h"
#include <string.h>

typedef struct {
    char name[STRING_SIZE];
    float height;
    Font* font;
} LoadedFont;

static LoadedFont* _loaded_fonts = NULL;
static Font* _active_font = NULL;
#ifdef USE_DEFAULT_FONT
static Font* _default_font = NULL;
#endif

EXPORT void CALL bmSetup2D() {
#ifndef PLATFORM_NULL
    lgfx_setup2d(bmGetScreenWidth(), bmGetScreenHeight());
#else
    lgfx_setup2d(0, 0);
#endif
}

EXPORT void CALL bmSetViewport(int x, int y, int w, int h) {
    lgfx_setviewport(x, y, w, h);
}

EXPORT void CALL bmSetResolution(int w, int h) {
    lgfx_setresolution(w, h);
}

EXPORT void CALL bmSetDrawColor(int color) {
    lgfx_setcolor(
        bmGetRed(color) / 255.0f,
        bmGetGreen(color) / 255.0f,
        bmGetBlue(color) / 255.0f,
        bmGetAlpha(color) / 255.0f);
}

EXPORT void CALL bmSetDrawFont(const char* filename, float height) {
    Font* font = NULL;
    int i;

    /* search for already loaded font */
    for (i = 0; i < sb_count(_loaded_fonts); ++i) {
        if (strcmp(_loaded_fonts[i].name, filename) == 0 && _loaded_fonts[i].height == height) {
            _active_font = _loaded_fonts[i].font;
            return;
        }
    }

    /* load font */
    font = LoadFont(filename, height);
    if (font) {
        LoadedFont data;

        strncpy(data.name, filename, STRING_SIZE);
        data.name[STRING_SIZE-1] = 0;
        data.height = height;
        data.font = font;
        sb_push(_loaded_fonts, data);
        _active_font = font;
    }
}

EXPORT void CALL bmClearScreen(int color) {
    lgfx_clearcolorbuffer(
        bmGetRed(color) / 255.0f,
        bmGetGreen(color) / 255.0f,
        bmGetBlue(color) / 255.0f
    );
}

EXPORT void CALL bmSetDefaultFont() {
#ifdef USE_DEFAULT_FONT
#ifdef USE_RETINA
    float font_height = 28;
#else
    float font_height = 14;
#endif
    _default_font = _LoadBase64Font(DEFAULT_FONT, DEFAULT_FONT_BLOCKSIZE, font_height);
    _active_font = _default_font;
#else
    _active_font = NULL;
#endif
}

EXPORT void CALL bmDrawPoint(float x, float y) {
    lgfx_drawpoint(x, y);
}

EXPORT void CALL bmDrawLine(float x1, float y1, float x2, float y2) {
    lgfx_drawline(x1, y1, x2, y2);
}

EXPORT void CALL bmDrawEllipse(float x, float y, float width, float height) {
    lgfx_drawoval(x, y, width, height);
}

EXPORT void CALL bmDrawRect(float x, float y, float width, float height) {
    lgfx_drawrect(x, y, width, height);
}

EXPORT void CALL bmDrawTexture(const Texture* tex, float x, float y, float width, float height) {
    const ltex_t* ltex = (const ltex_t*)_GetTexturePtr(tex);
    ltex_drawrotsized(ltex, x, y, 0, 0, 0, width != 0 ? width : ltex->width, height != 0 ? height : ltex->height, 0, 0, 1, 1);
}

EXPORT void CALL bmDrawText(const char* text, float x, float y) {
    if (_active_font) {
        DrawFont(_active_font, text, x, y);
    }
}

EXPORT float CALL bmGetTextWidth(const char* text) {
    if (_active_font) {
        return GetFontTextWidth(_active_font, text);
    } else {
        return 0;
    }
}

EXPORT float CALL bmGetTextHeight(const char* text) {
    if (_active_font) {
        return GetFontTextHeight(_active_font, text);
    } else {
        return 0;
    }
}

EXPORT void CALL _UnloadFonts() {
    int i;

    for (i = 0; i < sb_count(_loaded_fonts); ++i)
        ReleaseFont(_loaded_fonts[i].font);
    sb_free(_loaded_fonts);
    _loaded_fonts = NULL;
#ifdef USE_DEFAULT_FONT
    if (_default_font) ReleaseFont(_default_font);
    _default_font = NULL;
#endif
}
