#include "font.h"

#ifdef CHOWDREN_USE_FT2

#include <wctype.h>
#include <iostream>
#include "platform.h"
#include "assetfile.h"
#include "render.h"

// front-end font loader

bool load_fonts(FontList & fonts)
{
    AssetFile fp;
    fp.open();
    FileStream stream(fp);

    for (int i = 0; i < FONT_COUNT; i++) {
        fp.set_item(i, AssetFile::FONT_DATA);
        unsigned int count = stream.read_uint32();
        for (unsigned int i = 0; i < count; i++) {
            FTTextureFont * font = new FTTextureFont(stream);
            fonts.push_back(font);
        }
    }
    return fonts.size() > 0;
}

// unicode support

template <typename T>
class FTUnicodeStringItr
{
public:
    /**
     * Constructor.  Also reads the first character and stores it.
     *
     * @param string  The buffer to iterate.  No copy is made.
     */
    FTUnicodeStringItr(const T* string)
    : cur(string), next(string)
    {
        ++(*this);
    }

    /**
     * Pre-increment operator.  Reads the next unicode character and sets
     * the state appropriately.
     * Note - not protected against overruns.
     */
    FTUnicodeStringItr& operator++()
    {
        cur = next;
        // unicode handling
        switch (sizeof(T)) {
            case 1: // UTF-8
                // get this character
                readUTF8(); break;
            case 2: // UTF-16
                readUTF16(); break;
            case 4: // UTF-32
                // fall through
            default: // error condition really, but give it a shot anyway
                curChar = *next_32++;
        }
        return *this;
    }

    /**
     * Post-increment operator.  Reads the next character and sets
     * the state appropriately.
     * Note - not protected against overruns.
     */
    FTUnicodeStringItr operator++(int)
    {
        FTUnicodeStringItr temp = *this;
        ++*this;
        return temp;
    }

    /**
     * Equality operator.  Two FTUnicodeStringItrs are considered equal
     * if they have the same current buffer and buffer position.
     */
    bool operator==(const FTUnicodeStringItr& right) const
    {
        if (cur == right.getBufferFromHere())
            return true;
        return false;
    }

    /**
     * Dereference operator.
     *
     * @return  The unicode codepoint of the character currently pointed
     * to by the FTUnicodeStringItr.
     */
    unsigned int operator*() const
    {
        return curChar;
    }

    /**
     * Buffer-fetching getter.  You can use this to retreive the buffer
     * starting at the currently-iterated character for functions which
     * require a Unicode string as input.
     */
    const T* getBufferFromHere() const { return cur; }

private:
    /**
     * Helper function for reading a single UTF8 character from the string.
     * Updates internal state appropriately.
     */
    void readUTF8();

    /**
     * Helper function for reading a single UTF16 character from the string.
     * Updates internal state appropriately.
     */
    void readUTF16();

    /**
     * The buffer position of the first element in the current character.
     */
    union {
        const T * cur;
        const unsigned char * cur_8;
        const unsigned short * cur_16;
        const unsigned int * cur_32;
    };

    /**
     * The character stored at the current buffer position (prefetched on
     * increment, so there's no penalty for dereferencing more than once).
     */
    unsigned int curChar;

    /**
     * The buffer position of the first element in the next character.
     */
    union {
        const T * next;
        const unsigned char * next_8;
        const unsigned short * next_16;
        const unsigned int * next_32;
    };

    // unicode magic numbers
    static const unsigned char utf8bytes[256];
    static const unsigned long offsetsFromUTF8[6];
    static const unsigned long highSurrogateStart;
    static const unsigned long highSurrogateEnd;
    static const unsigned long lowSurrogateStart;
    static const unsigned long lowSurrogateEnd;
    static const unsigned long highSurrogateShift;
    static const unsigned long lowSurrogateBase;
};

/* The first character in a UTF8 sequence indicates how many bytes
 * to read (among other things) */
template <typename T>
const unsigned char FTUnicodeStringItr<T>::utf8bytes[256] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4,4,4,4,4,4,4,4,5,5,5,5,6,6,6,6
};

/* Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence. */
template <typename T>
const unsigned long FTUnicodeStringItr<T>::offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL };

// get a UTF8 character; leave the tracking pointer at the start of the
// next character
// not protected against invalid UTF8
template <typename T>
inline void FTUnicodeStringItr<T>::readUTF8()
{
    unsigned int ch = 0;
    unsigned int extraBytesToRead = utf8bytes[*next_8];
    // falls through
    switch (extraBytesToRead) {
          case 6: ch += *next_8++; ch <<= 6; /* remember, illegal UTF-8 */
          case 5: ch += *next_8++; ch <<= 6; /* remember, illegal UTF-8 */
          case 4: ch += *next_8++; ch <<= 6;
          case 3: ch += *next_8++; ch <<= 6;
          case 2: ch += *next_8++; ch <<= 6;
          case 1: ch += *next_8++;
    }
    ch -= offsetsFromUTF8[extraBytesToRead-1];
    curChar = ch;
}

// Magic numbers for UTF-16 conversions
template <typename T>
const unsigned long FTUnicodeStringItr<T>::highSurrogateStart = 0xD800;
template <typename T>
const unsigned long FTUnicodeStringItr<T>::highSurrogateEnd = 0xDBFF;
template <typename T>
const unsigned long FTUnicodeStringItr<T>::lowSurrogateStart = 0xDC00;
template <typename T>
const unsigned long FTUnicodeStringItr<T>::lowSurrogateEnd = 0xDFFF;
template <typename T>
const unsigned long FTUnicodeStringItr<T>::highSurrogateShift = 10;
template <typename T>
const unsigned long FTUnicodeStringItr<T>::lowSurrogateBase = 0x0010000UL;

template <typename T>
inline void FTUnicodeStringItr<T>::readUTF16()
{
    unsigned int ch = *next_16++;
    // if we have the first half of the surrogate pair
    if (ch >= highSurrogateStart && ch <= highSurrogateEnd)
    {
        unsigned int ch2 = *cur_16;
        // complete the surrogate pair
        if (ch2 >= lowSurrogateStart && ch2 <= lowSurrogateEnd)
        {
            ch = ((ch - highSurrogateStart) << highSurrogateShift)
                + (ch2 - lowSurrogateStart) + lowSurrogateBase;
            ++next_16;
        }
    }
    curChar = ch;
}


// FTFont


FTFont::FTFont(FileStream & stream)
{
    glyphList = new FTGlyphContainer(this);

    size = stream.read_int32();
    width = stream.read_float();
    height = stream.read_float();
    ascender = stream.read_float();
    descender = stream.read_float();
    numGlyphs = stream.read_int32();

    for (int i = 0; i < numGlyphs; i++) {
        FTGlyph * glyph = new FTGlyph(stream);
        glyphList->Add(glyph, glyph->charcode);
    }
}


FTFont::~FTFont()
{
    if (glyphList == NULL)
        return;
    delete glyphList;
}


FTPoint FTFont::KernAdvance(unsigned int index1, unsigned int index2)
{
    return FTPoint(0.0, 0.0);
    // if (!hasKerningTable || !index1 || !index2) {
    //     return FTPoint(0.0, 0.0);
    // }

    // FT_Vector kernAdvance;
    // kernAdvance.x = kernAdvance.y = 0;

    // err = FT_Get_Kerning(*ftFace, index1, index2, ft_kerning_unfitted,
    //                      &kernAdvance);
    // if (err) {
    //     return FTPoint(0.0f, 0.0f);
    // }

    // double x = float(kernAdvance.x) / 64.0f;
    // double y = float(kernAdvance.y) / 64.0f;
    // return FTPoint(x, y);
}


float FTFont::Ascender() const
{
    return ascender;
}


float FTFont::Descender() const
{
    return descender;
}


float FTFont::LineHeight() const
{
    return height;
}


template <typename T>
inline FTBBox FTFont::BBoxI(const T* string, const int len,
                                FTPoint position, FTPoint spacing)
{
    FTBBox totalBBox;

    /* Only compute the bounds if string is non-empty. */
    if(string && ('\0' != string[0]))
    {
        // for multibyte - we can't rely on sizeof(T) == character
        FTUnicodeStringItr<T> ustr(string);
        unsigned int thisChar = *ustr++;
        unsigned int nextChar = *ustr;

        if(CheckGlyph(thisChar))
        {
            totalBBox = glyphList->BBox(thisChar);
            totalBBox += position;

            position += FTPoint(glyphList->Advance(thisChar, nextChar), 0.0);
        }

        /* Expand totalBox by each glyph in string */
        for(int i = 1; (len < 0 && *ustr) || (len >= 0 && i < len); i++)
        {
            thisChar = *ustr++;
            nextChar = *ustr;

            if(CheckGlyph(thisChar))
            {
                position += spacing;

                FTBBox tempBBox = glyphList->BBox(thisChar);
                tempBBox += position;
                totalBBox |= tempBBox;

                position += FTPoint(glyphList->Advance(thisChar, nextChar),
                                    0.0);
            }
        }
    }

    return totalBBox;
}


FTBBox FTFont::BBox(const char *string, const int len,
                        FTPoint position, FTPoint spacing)
{
    /* The chars need to be unsigned because they are cast to int later */
    return BBoxI((const unsigned char *)string, len, position, spacing);
}


FTBBox FTFont::BBox(const wchar_t *string, const int len,
                        FTPoint position, FTPoint spacing)
{
    return BBoxI(string, len, position, spacing);
}


template <typename T>
inline float FTFont::AdvanceI(const T* string, const int len,
                                  FTPoint spacing)
{
    float advance = 0.0f;
    FTUnicodeStringItr<T> ustr(string);

    for (int i = 0; (len < 0 && *ustr) || (len >= 0 && i < len); i++) {
        unsigned int thisChar = *ustr++;
        unsigned int nextChar = *ustr;

        if (CheckGlyph(thisChar)) {
            advance += glyphList->Advance(thisChar, nextChar);
        }

        if (nextChar) {
            advance += spacing.Xf();
        }
    }

    return advance;
}


float FTFont::Advance(const char* string, const int len, FTPoint spacing)
{
    /* The chars need to be unsigned because they are cast to int later */
    const unsigned char *ustring = (const unsigned char *)string;
    return AdvanceI(ustring, len, spacing);
}


float FTFont::Advance(const wchar_t* string, const int len, FTPoint spacing)
{
    return AdvanceI(string, len, spacing);
}


template <typename T>
inline FTPoint FTFont::RenderI(const T* string, const int len,
                               FTPoint position, FTPoint spacing)
{
    // for multibyte - we can't rely on sizeof(T) == character
    FTUnicodeStringItr<T> ustr(string);

    for(int i = 0; (len < 0 && *ustr) || (len >= 0 && i < len); i++) {
        unsigned int thisChar = *ustr++;
        unsigned int nextChar = *ustr;

        if (CheckGlyph(thisChar)) {
            position += glyphList->Render(thisChar, nextChar,
                                          position);
        }

        if (nextChar) {
            position += spacing;
        }
    }

    return position;
}


FTPoint FTFont::Render(const char * string, const int len,
                           FTPoint position, FTPoint spacing)
{
    return RenderI((const unsigned char *)string,
                   len, position, spacing);
}


FTPoint FTFont::Render(const wchar_t * string, const int len,
                           FTPoint position, FTPoint spacing)
{
    return RenderI(string, len, position, spacing);
}


// FTTextureFont

Color FTTextureFont::color;

static inline GLuint ClampSize(GLuint in, GLuint maxTextureSize)
{
    // Find next power of two
    --in;
    in |= in >> 16;
    in |= in >> 8;
    in |= in >> 4;
    in |= in >> 2;
    in |= in >> 1;
    ++in;

    // Clamp to max texture size
    if (in < maxTextureSize)
        return in;
    return maxTextureSize;
}

//
// FTTextureFont
//

FTTextureFont::FTTextureFont(FileStream & stream)
: FTFont(stream), maximumGLTextureSize(0), textureWidth(0),
  textureHeight(0), xOffset(0), yOffset(0), padding(3)
{
    remGlyphs = numGlyphs;
    glyphHeight = std::max(1, int(height + 0.5f));
    glyphWidth = std::max(1, int(width + 0.5f));
}


FTTextureFont::~FTTextureFont()
{
    if (textureIDList.empty())
        return;
    glDeleteTextures((GLsizei)textureIDList.size(),
                     (const GLuint*)&textureIDList[0]);
}


void FTTextureFont::CalculateTextureSize()
{
    if (!maximumGLTextureSize) {
        maximumGLTextureSize = 1024;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&maximumGLTextureSize);
    }

    // Texture width required for numGlyphs glyphs. Will probably not be
    // large enough, but we try to fit as many glyphs in one line as possible
    textureWidth = ClampSize(glyphWidth * numGlyphs + padding * 2,
                             maximumGLTextureSize);

    // Number of lines required for that many glyphs in a line
    int tmp = (textureWidth - (padding * 2)) / glyphWidth;
    tmp = tmp > 0 ? tmp : 1;
    tmp = (numGlyphs + (tmp - 1)) / tmp; // round division up

    // Texture height required for tmp lines of glyphs
    textureHeight = ClampSize(glyphHeight * tmp + padding * 2,
                              maximumGLTextureSize);
}

bool FTTextureFont::CheckGlyph(const unsigned int characterCode)
{
    FTGlyph * glyph = glyphList->Glyph(characterCode);

    if (glyph == NULL) {
        return false;
    }

    if (glyph->loaded)
        return true;

    if (textureIDList.empty()) {
        textureIDList.push_back(CreateTexture());
        xOffset = yOffset = padding;
    }

    if (xOffset > (textureWidth - glyphWidth)) {
        xOffset = padding;
        yOffset += glyphHeight;

        if (yOffset > (textureHeight - glyphHeight)) {
            textureIDList.push_back(CreateTexture());
            yOffset = padding;
        }
    }

    unsigned int n = textureIDList.size() - 1;

    glyph->Load(textureIDList[n], xOffset, yOffset,
                textureWidth, textureHeight);
    xOffset += int(glyph->BBox().Upper().X() -
                   glyph->BBox().Lower().X() + padding + 0.5);

    --remGlyphs;
    return true;
}

GLuint FTTextureFont::CreateTexture()
{
    CalculateTextureSize();

    GLuint textID;
    glGenTextures(1, &textID);

    glBindTexture(GL_TEXTURE_2D, textID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, textureWidth, textureHeight,
                 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);

    return textID;
}


FTPoint FTTextureFont::Render(const char * string, const int len,
                              FTPoint position, FTPoint spacing)
{
    return RenderI(string, len, position, spacing);
}


FTPoint FTTextureFont::Render(const wchar_t * string, const int len,
                              FTPoint position, FTPoint spacing)
{
    return RenderI(string, len, position, spacing);
}

//
//  FTGlyph
//

GLint FTGlyph::activeTextureID = 0;

FTGlyph::FTGlyph(FileStream & stream)
: glTextureID(0), loaded(false)
{
    charcode = stream.read_uint32();
    float x1, y1, x2, y2;
    x1 = stream.read_float();
    y1 = stream.read_float();
    x2 = stream.read_float();
    y2 = stream.read_float();
    bBox = FTBBox(x1, y1, 0.0f, x2, y2, 0.0f);
    float advance_x, advance_y;
    advance_x = stream.read_float();
    advance_y = stream.read_float();
    advance = FTPoint(advance_x, advance_y);
    float corner_x, corner_y;
    corner_x = stream.read_float();
    corner_y = stream.read_float();
    corner = FTPoint(corner_x, corner_y, 0.0f);
    width = stream.read_int32();
    height = stream.read_int32();
    data = new char[width*height];
    stream.read(data, width*height);
}


void FTGlyph::Load(int id, int xOffset, int yOffset,
                   int tex_width, int tex_height)
{
    loaded = true;
    glTextureID = id;

    if (width && height) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, glTextureID);

        if (yOffset + height > tex_height) {
            // We'll only get here if we are soft-failing our asserts. In that
            // case, since the data we're trying to put into our texture is
            // too long, we'll only copy a portion of the image.
            height = tex_height - yOffset;
        }
        if (height >= 0) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset,
                            width, height, GL_ALPHA, GL_UNSIGNED_BYTE,
                            data);
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

//      0
//      +----+
//      |    |
//      |    |
//      |    |
//      +----+
//           1

    uv[0].X(float(xOffset) / float(tex_width));
    uv[0].Y(float(yOffset) / float(tex_height));
    uv[1].X(float(xOffset + width) / float(tex_width));
    uv[1].Y(float(yOffset + height) / float(tex_height));

    delete data;
    data = NULL;
}


FTGlyph::~FTGlyph()
{
    delete data;
}


float FTGlyph::Advance() const
{
    return advance.Xf();
}


const FTBBox& FTGlyph::BBox() const
{
    return bBox;
}


const FTPoint& FTGlyph::Render(const FTPoint& pen)
{
    float dx, dy;

    dx = floor(pen.Xf() + corner.Xf());
    dy = floor(pen.Yf() + corner.Yf());

    Render::draw_tex(dx, dy, dx + width, dy + height, FTTextureFont::color,
                     glTextureID,
                     uv[0].Xf(), uv[0].Yf(), uv[1].Xf(), uv[1].Yf());

    return advance;
}

// glyphcontainer

FTGlyphContainer::FTGlyphContainer(FTFont* f)
: font(f)
{
    glyphs.push_back((FTGlyph*)NULL);
}


FTGlyphContainer::~FTGlyphContainer()
{
    GlyphVector::iterator it;
    for (it = glyphs.begin(); it != glyphs.end(); ++it) {
        delete *it;
    }

    glyphs.clear();
}


unsigned int FTGlyphContainer::FontIndex(const unsigned int charCode)
{
    return charMap.find(charCode);
}


void FTGlyphContainer::Add(FTGlyph* tempGlyph, const unsigned int charCode)
{
    charMap.insert(charCode, glyphs.size());
    glyphs.push_back(tempGlyph);
}


FTGlyph* FTGlyphContainer::Glyph(const unsigned int charCode)
{
    unsigned int index = FontIndex(charCode);

    if (index < glyphs.size())
        return glyphs[index];
    return NULL;
}


FTBBox FTGlyphContainer::BBox(const unsigned int charCode)
{
    return Glyph(charCode)->BBox();
}


float FTGlyphContainer::Advance(const unsigned int charCode,
                                const unsigned int nextCharCode)
{
    unsigned int left = FontIndex(charCode);
    unsigned int right = FontIndex(nextCharCode);
    const FTGlyph *glyph = Glyph(charCode);

    if (!glyph)
      return 0.0f;

    return font->KernAdvance(left, right).Xf() + glyph->Advance();
}


FTPoint FTGlyphContainer::Render(const unsigned int charCode,
                                 const unsigned int nextCharCode,
                                 FTPoint penPosition)
{
    unsigned int left = FontIndex(charCode);
    unsigned int right = FontIndex(nextCharCode);

    FTPoint kernAdvance = font->KernAdvance(left, right);

    FTGlyph * glyph = Glyph(charCode);
    if (glyph != NULL)
        kernAdvance += glyph->Render(penPosition);

    return kernAdvance;
}


// FTSimpleLayout

FTSimpleLayout::FTSimpleLayout()
{
    currentFont = NULL;
    lineLength = 100.0f;
    alignment = ALIGN_LEFT;
    lineSpacing = 1.0f;
    tabSpacing = 1.0f / 0.6f;
}


template <typename T>
inline FTBBox FTSimpleLayout::BBoxI(const T* string, const int len,
                                        FTPoint position)
{
    FTBBox tmp;

    WrapText(string, len, position, &tmp);

    return tmp;
}


FTBBox FTSimpleLayout::BBox(const char *string, const int len,
                                FTPoint position)
{
    return BBoxI(string, len, position);
}


FTBBox FTSimpleLayout::BBox(const wchar_t *string, const int len,
                            FTPoint position)
{
    return BBoxI(string, len, position);
}


template <typename T>
inline void FTSimpleLayout::RenderI(const T *string, const int len,
                                    FTPoint position)
{
    pen = FTPoint(0.0f, 0.0f);
    WrapText(string, len, position, NULL);
}


void FTSimpleLayout::Render(const char *string, const int len,
                            FTPoint position)
{
    RenderI(string, len, position);
}


void FTSimpleLayout::Render(const wchar_t* string, const int len,
                                FTPoint position)
{
    RenderI(string, len, position);
}


bool is_linebreak(unsigned int v)
{
    switch (v) {
        case '\n':
        case '\x0B':
            return true;
    }
    return false;
}

bool is_line_extender(unsigned int v)
{
    switch (v) {
        case 12289: // japanese comma
        case 12290: // japanese dot
        case 65289: // japanese end paranthesis
        case 65281: // japanese exclamation mark
        case 65311: // japanese question mark
        case 0x2026: // triple quote
        case 0x300D: // japanese right corner bracket
        case 0x300F: // japanese white corner bracket
        case 0x3011: // japanese right black lenticular bracket
            return true;
    }
    return false;
}

bool is_break_start(unsigned int v)
{
    switch (v) {
        case 0xFF08:
        case 0x300C:
        case 0x300E:
        case 0x3010:
            return true;
    }
    return false;
}

template <typename T>
inline void FTSimpleLayout::WrapTextI(const T *buf, const int len,
                                      FTPoint position, FTBBox *bounds)
{
    FTUnicodeStringItr<T> breakItr(buf); // points to the last break character
    FTUnicodeStringItr<T> lineStart(buf); // points to the line start
    float nextStart = 0.0;     // total width of the current line
    float breakWidth = 0.0;    // width of the line up to the last word break
    float currentWidth = 0.0;  // width of all characters on the current line
    float prevWidth;           // width of all characters but the current glyph
    float wordLength = 0.0;    // length of the block since the last break char
    int charCount = 0;         // number of characters so far on the line
    int breakCharCount = 0;    // number of characters before the breakItr
    float glyphWidth, advance;
    FTBBox glyphBounds;

    // Reset the pen position
    pen.Y(0);

    // If we have bounds mark them invalid
    if (bounds)
        bounds->Invalidate();

    bool restore_break = false;

    // Scan the input for all characters that need output
    FTUnicodeStringItr<T> prevItr(buf);
    for (FTUnicodeStringItr<T> itr(buf); *itr; prevItr = itr++, charCount++) {
        // Find the width of the current glyph
        glyphBounds = currentFont->BBox(itr.getBufferFromHere(), 1);
        glyphWidth = glyphBounds.Upper().Xf() - glyphBounds.Lower().Xf();

        advance = currentFont->Advance(itr.getBufferFromHere(), 1);
        prevWidth = currentWidth;
        // Compute the width of all glyphs up to the end of buf[i]
        currentWidth = nextStart + glyphWidth;
        // Compute the position of the next glyph

        bool width_test = !is_line_extender(*itr) && currentWidth > lineLength;
        bool linebreak = is_linebreak(*itr);

        nextStart += advance;

        // See if the current character is a space, a break or a regular
        // character
        if (width_test || linebreak) {
            // A non whitespace character has exceeded the line length.  Or a
            // newline character has forced a line break.  Output the last
            // line and start a new line after the break character.
            // If we have not yet found a break, break on the last character
            if(breakItr == lineStart || is_linebreak(*itr)) {
                // Break on the previous character
                breakItr = prevItr;
                breakCharCount = charCount - 1;
                breakWidth = prevWidth;
                // None of the previous words will be carried to the next line
                wordLength = 0;
            }

            // If the current character is a newline discard its advance
            if (is_linebreak(*itr))
                advance = 0;

            float remainingWidth = lineLength - breakWidth;

            // Render the current substring
            FTUnicodeStringItr<T> breakChar = breakItr;
            // move past the break character and don't count it on the next line either
            ++breakChar; --charCount;
            // If the break character is a newline do not render it
            float currentSpacing;
            switch (*breakChar) {
                case '\n':
                    ++breakChar; --charCount;
                    currentSpacing = lineSpacing;
                    break;
                case '\x0B':
                    ++breakChar; --charCount;
                    currentSpacing = tabSpacing;
                    break;
                default:
                    currentSpacing = lineSpacing;
                    break;
            }

            OutputWrapped(lineStart.getBufferFromHere(), breakCharCount,
                          position, remainingWidth, bounds);

            // Store the start of the next line
            lineStart = breakChar;
            // TODO: Is Height() the right value here?
            pen += FTPoint(0, currentFont->LineHeight() * currentSpacing);
            // The current width is the width since the last break
            nextStart = wordLength + advance;
            wordLength += advance;
            currentWidth = wordLength + advance;
            // Reset the safe break for the next line
            breakItr = lineStart;
            charCount -= breakCharCount;
        } else if(iswspace(*itr)) {
            // This is the last word break position
            wordLength = 0;
            breakItr = itr;
            breakCharCount = charCount;

            // Check to see if this is the first whitespace character in a run
            if(buf == itr.getBufferFromHere() || !iswspace(*prevItr)) {
                // Record the width of the start of the block
                breakWidth = currentWidth;
            }
        } else {
            wordLength += advance;
        }

        if (restore_break)
            breakItr = lineStart;

        if (is_break_start(*itr)) {
            wordLength = 0;
            breakItr = prevItr;
            breakCharCount = charCount-1;
            wordLength += advance;
            restore_break = true;
        }
    }

    float remainingWidth = lineLength - currentWidth;
    // Render any remaining text on the last line
    // Disable justification for the last row
    if(alignment == ALIGN_JUSTIFY) {
        alignment = ALIGN_LEFT;
        OutputWrapped(lineStart.getBufferFromHere(), -1, position,
                      remainingWidth, bounds);
        alignment = ALIGN_JUSTIFY;
    }  else {
        OutputWrapped(lineStart.getBufferFromHere(), -1, position,
                      remainingWidth, bounds);
    }
}


void FTSimpleLayout::WrapText(const char *buf, const int len,
                                  FTPoint position, FTBBox *bounds)
{
    WrapTextI(buf, len, position, bounds);
}


void FTSimpleLayout::WrapText(const wchar_t* buf, const int len,
                                  FTPoint position, FTBBox *bounds)
{
    WrapTextI(buf, len, position, bounds);
}


template <typename T>
inline void FTSimpleLayout::OutputWrappedI(const T *buf, const int len,
                                               FTPoint position,
                                               const float remaining,
                                               FTBBox *bounds)
{
    float distributeWidth = 0.0;
    // Align the text according as specified by Alignment
    switch (alignment)
    {
        case ALIGN_LEFT:
            pen.X(0);
            break;
        case ALIGN_HCENTER:
            pen.X(remaining / 2);
            break;
        case ALIGN_RIGHT:
            pen.X(remaining);
            break;
        case ALIGN_JUSTIFY:
            pen.X(0);
            distributeWidth = remaining;
            break;
        default:
            break;
    }

    // If we have bounds expand them by the line's bounds, otherwise render
    // the line.
    if (bounds) {
        FTBBox temp = currentFont->BBox(buf, len);

        // Add the extra space to the upper x dimension
        temp = FTBBox(temp.Lower() + pen,
                      temp.Upper() + pen + FTPoint(distributeWidth, 0));

        // See if this is the first area to be added to the bounds
        if(bounds->IsValid())
        {
            *bounds |= temp;
        }
        else
        {
            *bounds = temp;
        }
    } else {
        RenderSpace(buf, len, position, distributeWidth);
    }
}


void FTSimpleLayout::OutputWrapped(const char *buf, const int len,
                                       FTPoint position,
                                       const float remaining, FTBBox *bounds)
{
    OutputWrappedI(buf, len, position, remaining, bounds);
}


void FTSimpleLayout::OutputWrapped(const wchar_t *buf, const int len,
                                       FTPoint position,
                                       const float remaining, FTBBox *bounds)
{
    OutputWrappedI(buf, len, position, remaining, bounds);
}


template <typename T>
inline void FTSimpleLayout::RenderSpaceI(const T *string, const int len,
                                             FTPoint position,
                                             const float extraSpace)
{
    (void)position;

    float space = 0.0;

    // If there is space to distribute, count the number of spaces
    if(extraSpace > 0.0)
    {
        int numSpaces = 0;

        // Count the number of space blocks in the input
        FTUnicodeStringItr<T> prevItr(string), itr(string);
        for(int i = 0; ((len < 0) && *itr) || ((len >= 0) && (i <= len));
            ++i, prevItr = itr++)
        {
            // If this is the end of a space block, increment the counter
            if((i > 0) && !iswspace(*itr) && iswspace(*prevItr))
            {
                numSpaces++;
            }
        }

        space = extraSpace/numSpaces;
    }

    // Output all characters of the string
    FTUnicodeStringItr<T> prevItr(string), itr(string);
    for(int i = 0; ((len < 0) && *itr) || ((len >= 0) && (i <= len));
        ++i, prevItr = itr++)
    {
        // If this is the end of a space block, distribute the extra space
        // inside it
        if((i > 0) && !iswspace(*itr) && iswspace(*prevItr))
        {
            pen += FTPoint(space, 0);
        }

        pen = currentFont->Render(itr.getBufferFromHere(), 1, pen, FTPoint());
    }
}


void FTSimpleLayout::RenderSpace(const char *string, const int len,
                                     FTPoint position,
                                     const float extraSpace)
{
    RenderSpaceI(string, len, position, extraSpace);
}


void FTSimpleLayout::RenderSpace(const wchar_t *string, const int len,
                                     FTPoint position,
                                     const float extraSpace)
{
    RenderSpaceI(string, len, position, extraSpace);
}

void FTSimpleLayout::SetFont(FTFont *fontInit)
{
    currentFont = fontInit;
}


FTFont *FTSimpleLayout::GetFont()
{
    return currentFont;
}


void FTSimpleLayout::SetLineLength(const float LineLength)
{
    lineLength = LineLength;
}


float FTSimpleLayout::GetLineLength() const
{
    return lineLength;
}


void FTSimpleLayout::SetAlignment(const TextAlignment Alignment)
{
    alignment = Alignment;
}

TextAlignment FTSimpleLayout::GetAlignment() const
{
    return alignment;
}


void FTSimpleLayout::SetLineSpacing(const float LineSpacing)
{
    lineSpacing = LineSpacing;
}


float FTSimpleLayout::GetLineSpacing() const
{
    return lineSpacing;
}

#endif // CHOWDREN_USE_FT2
