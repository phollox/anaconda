#ifndef CHOWDREN_FONT_H
#define CHOWDREN_FONT_H

#define CHOWDREN_USE_FT2

typedef enum
{
    ALIGN_LEFT = 1 << 0,
    ALIGN_HCENTER = 1 << 1,
    ALIGN_RIGHT = 1 << 2,
    ALIGN_JUSTIFY = 1 << 3,
    ALIGN_TOP = 1 << 4,
    ALIGN_BOTTOM = 1 << 5,
    ALIGN_VCENTER = 1 << 6
} TextAlignment;

#ifdef CHOWDREN_USE_FT2

#include <math.h>
#include "include_gl.h"
#include "types.h"
#include "datastream.h"
#include <string>
#include "color.h"


class FTPoint
{
    public:
        inline FTPoint()
        {
            values[0] = 0;
            values[1] = 0;
            values[2] = 0;
        }

        inline FTPoint(const double x, const double y,
                       const double z = 0)
        {
            values[0] = x;
            values[1] = y;
            values[2] = z;
        }

        inline FTPoint& operator += (const FTPoint& point)
        {
            values[0] += point.values[0];
            values[1] += point.values[1];
            values[2] += point.values[2];

            return *this;
        }

        inline FTPoint operator + (const FTPoint& point) const
        {
            FTPoint temp;
            temp.values[0] = values[0] + point.values[0];
            temp.values[1] = values[1] + point.values[1];
            temp.values[2] = values[2] + point.values[2];

            return temp;
        }

        inline FTPoint& operator -= (const FTPoint& point)
        {
            values[0] -= point.values[0];
            values[1] -= point.values[1];
            values[2] -= point.values[2];

            return *this;
        }

        inline FTPoint operator - (const FTPoint& point) const
        {
            FTPoint temp;
            temp.values[0] = values[0] - point.values[0];
            temp.values[1] = values[1] - point.values[1];
            temp.values[2] = values[2] - point.values[2];

            return temp;
        }

        inline FTPoint operator * (double multiplier) const
        {
            FTPoint temp;
            temp.values[0] = values[0] * multiplier;
            temp.values[1] = values[1] * multiplier;
            temp.values[2] = values[2] * multiplier;

            return temp;
        }


        inline friend FTPoint operator * (double multiplier, FTPoint& point)
        {
            return point * multiplier;
        }

        inline friend double operator * (FTPoint &a, FTPoint& b)
        {
            return a.values[0] * b.values[0]
                 + a.values[1] * b.values[1]
                 + a.values[2] * b.values[2];
        }

        friend bool operator == (const FTPoint &a, const FTPoint &b);
        friend bool operator != (const FTPoint &a, const FTPoint &b);

        inline operator const double*() const
        {
            return values;
        }

        inline void X(double x) { values[0] = x; };
        inline void Y(double y) { values[1] = y; };
        inline void Z(double z) { values[2] = z; };

        inline double X() const { return values[0]; };
        inline double Y() const { return values[1]; };
        inline double Z() const { return values[2]; };
        inline float Xf() const { return float(values[0]); };
        inline float Yf() const { return float(values[1]); };
        inline float Zf() const { return float(values[2]); };

    private:
        double values[3];
};

class FTBBox
{
    public:
        FTBBox()
        :   lower(0.0f, 0.0f, 0.0f),
            upper(0.0f, 0.0f, 0.0f)
        {}

        FTBBox(float lx, float ly, float lz, float ux, float uy, float uz)
        :   lower(lx, ly, lz),
            upper(ux, uy, uz)
        {}

        FTBBox(FTPoint l, FTPoint u)
        :   lower(l),
            upper(u)
        {}

        ~FTBBox()
        {}

        void Invalidate()
        {
            lower = FTPoint(1.0f, 1.0f, 1.0f);
            upper = FTPoint(-1.0f, -1.0f, -1.0f);
        }

        bool IsValid()
        {
            return lower.X() <= upper.X()
                && lower.Y() <= upper.Y()
                && lower.Z() <= upper.Z();
        }


        FTBBox& operator += (const FTPoint & vector)
        {
            lower += vector;
            upper += vector;

            return *this;
        }

        FTBBox& operator |= (const FTBBox& bbox)
        {
            if(bbox.lower.X() < lower.X()) lower.X(bbox.lower.X());
            if(bbox.lower.Y() < lower.Y()) lower.Y(bbox.lower.Y());
            if(bbox.lower.Z() < lower.Z()) lower.Z(bbox.lower.Z());
            if(bbox.upper.X() > upper.X()) upper.X(bbox.upper.X());
            if(bbox.upper.Y() > upper.Y()) upper.Y(bbox.upper.Y());
            if(bbox.upper.Z() > upper.Z()) upper.Z(bbox.upper.Z());

            return *this;
        }

        inline FTPoint const Upper() const
        {
            return upper;
        }

        inline FTPoint const Lower() const
        {
            return lower;
        }

    private:
        FTPoint lower, upper;
};


class FTCharToGlyphIndexMap
{
    public:
        typedef unsigned long CharacterCode;
        typedef signed long GlyphIndex;

        // XXX: always ensure that 1 << (3 * BucketIdxBits) >= UnicodeValLimit
        static const int BucketIdxBits = 7;
        static const int BucketIdxSize = 1 << BucketIdxBits;
        static const int BucketIdxMask = BucketIdxSize - 1;

        static const CharacterCode UnicodeValLimit = 0x110000;
        static const int IndexNotFound = -1;

        FTCharToGlyphIndexMap()
        {
            Indices = 0;
        }

        virtual ~FTCharToGlyphIndexMap()
        {
            // Free all buckets
            clear();
        }

        inline void clear()
        {
            for(int j = 0; Indices && j < BucketIdxSize; j++)
            {
                for(int i = 0; Indices[j] && i < BucketIdxSize; i++)
                {
                    delete[] Indices[j][i];
                    Indices[j][i] = 0;
                }
                delete[] Indices[j];
                Indices[j] = 0;
            }
            delete[] Indices;
            Indices = 0;
        }

        GlyphIndex find(CharacterCode c)
        {
            int OuterIdx = (c >> (BucketIdxBits * 2)) & BucketIdxMask;
            int InnerIdx = (c >> BucketIdxBits) & BucketIdxMask;
            int Offset = c & BucketIdxMask;

            if (c >= UnicodeValLimit || !Indices
                 || !Indices[OuterIdx] || !Indices[OuterIdx][InnerIdx])
                return 0;

            GlyphIndex g = Indices[OuterIdx][InnerIdx][Offset];

            return (g != IndexNotFound) ? g : 0;
        }

        void insert(CharacterCode c, GlyphIndex g)
        {
            int OuterIdx = (c >> (BucketIdxBits * 2)) & BucketIdxMask;
            int InnerIdx = (c >> BucketIdxBits) & BucketIdxMask;
            int Offset = c & BucketIdxMask;

            if (c >= UnicodeValLimit)
                return;

            if (!Indices)
            {
                Indices = new GlyphIndex** [BucketIdxSize];
                for(int i = 0; i < BucketIdxSize; i++)
                    Indices[i] = 0;
            }

            if (!Indices[OuterIdx])
            {
                Indices[OuterIdx] = new GlyphIndex* [BucketIdxSize];
                for(int i = 0; i < BucketIdxSize; i++)
                    Indices[OuterIdx][i] = 0;
            }

            if (!Indices[OuterIdx][InnerIdx])
            {
                Indices[OuterIdx][InnerIdx] = new GlyphIndex [BucketIdxSize];
                for(int i = 0; i < BucketIdxSize; i++)
                    Indices[OuterIdx][InnerIdx][i] = IndexNotFound;
            }

            Indices[OuterIdx][InnerIdx][Offset] = g;
        }

    private:
        GlyphIndex*** Indices;
};

class FTGlyph;
class FTFont;

typedef vector<FTGlyph*> GlyphVector;

class FTGlyphContainer
{
public:
    FTGlyphContainer(FTFont* font);
    ~FTGlyphContainer();
    unsigned int FontIndex(const unsigned int characterCode);
    void Add(FTGlyph* glyph, const unsigned int characterCode);
    FTGlyph* Glyph(const unsigned int characterCode);
    FTBBox BBox(const unsigned int characterCode);
    float Advance(const unsigned int characterCode,
                  const unsigned int nextCharacterCode);
    FTPoint Render(const unsigned int characterCode,
                   const unsigned int nextCharacterCode,
                   FTPoint penPosition);

private:
    FTFont* font;
    FTCharToGlyphIndexMap charMap;
    GlyphVector glyphs;
};

class FTFont
{
public:
    int size;
    float width, height;
    float ascender, descender;

    bool hasKerningTable;
    int numGlyphs;

    FTFont(FileStream & fp);

    FTPoint KernAdvance(unsigned int index1, unsigned int index2);

    virtual ~FTFont();
    virtual float Ascender() const;
    virtual float Descender() const;
    virtual float LineHeight() const;
    virtual FTBBox BBox(const char *s, const int len = -1,
                        FTPoint position = FTPoint(),
                        FTPoint spacing = FTPoint());
    virtual FTBBox BBox(const wchar_t *s, const int len = -1,
                        FTPoint position = FTPoint(),
                        FTPoint spacing = FTPoint());
    virtual float Advance(const char *s, const int len = -1,
                          FTPoint spacing = FTPoint());
    virtual float Advance(const wchar_t *s, const int len = -1,
                          FTPoint spacing = FTPoint());
    virtual FTPoint Render(const char *s, const int len,
                           FTPoint, FTPoint);
    virtual FTPoint Render(const wchar_t *s, const int len,
                           FTPoint, FTPoint);
    virtual bool CheckGlyph(const unsigned int chr) = 0;
    FTGlyphContainer* glyphList;
    FTPoint pen;

    template <typename T>
    inline FTBBox BBoxI(const T *s, const int len,
                        FTPoint position, FTPoint spacing);

    template <typename T>
    inline float AdvanceI(const T *s, const int len, FTPoint spacing);

    template <typename T>
    inline FTPoint RenderI(const T *s, const int len,
                           FTPoint position, FTPoint spacing);
};

class FTGlyph
{
public:
    unsigned int charcode;
    bool loaded;
    FTPoint advance;
    FTBBox bBox;
    char * data;

    FTGlyph(FileStream & stream);
    void Load(int id, int xOffset, int yOffset, int tex_width, int tex_height);
    ~FTGlyph();
    const FTPoint& Render(const FTPoint& pen);
    static void ResetActiveTexture() { activeTextureID = 0; }
    float Advance() const;
    const FTBBox& BBox() const;

private:
    int width;
    int height;
    FTPoint corner;
    FTPoint uv[2];
    int glTextureID;
    static GLint activeTextureID;
};

class FTTextureFont : public FTFont
{
public:
    GLsizei maximumGLTextureSize;
    GLsizei textureWidth;
    GLsizei textureHeight;
    vector<GLuint> textureIDList;
    int glyphHeight;
    int glyphWidth;
    unsigned int padding;
    unsigned int remGlyphs;
    int xOffset;
    int yOffset;
    static Color color;

    FTTextureFont(FileStream & stream);
    ~FTTextureFont();
    void CalculateTextureSize();
    GLuint CreateTexture();

    template <typename T>
    inline FTPoint RenderI(const T* string, const int len,
                           FTPoint position, FTPoint spacing)
    {
        FTGlyph::ResetActiveTexture();
        FTPoint tmp = FTFont::Render(string, len,
                                     position, spacing);
        return tmp;
    }

    FTPoint Render(const char * string, const int len,
                   FTPoint position, FTPoint spacing);
    FTPoint Render(const wchar_t * string, const int len,
                   FTPoint position, FTPoint spacing);
    bool CheckGlyph(const unsigned int chr);
};


class FTSimpleLayout
{
    public:
        FTSimpleLayout();
        FTBBox BBox(const char* string, const int len = -1,
                    FTPoint position = FTPoint());
        FTBBox BBox(const wchar_t* string, const int len = -1,
                    FTPoint position = FTPoint());
        void Render(const char *string, const int len = -1,
                    FTPoint position = FTPoint());
        void Render(const wchar_t *string, const int len = -1,
                            FTPoint position = FTPoint());
        void SetFont(FTFont *fontInit);
        FTFont *GetFont();
        void SetLineLength(const float LineLength);
        float GetLineLength() const;
        void SetAlignment(const TextAlignment Alignment);
        TextAlignment GetAlignment() const;
        void SetLineSpacing(const float LineSpacing);
        float GetLineSpacing() const;
        void RenderSpace(const char *string, const int len,
                                 FTPoint position, const float extraSpace);
        void RenderSpace(const wchar_t *string, const int len,
                                 FTPoint position, const float extraSpace);
        FTPoint pen;
    private:
        FTFont *currentFont;
        float lineLength;
        TextAlignment alignment;
        float lineSpacing;
        float tabSpacing;

        void WrapText(const char *buf, const int len, FTPoint position,
                      FTBBox *bounds);
        void WrapText(const wchar_t *buf, const int len, FTPoint position,
                      FTBBox *bounds);
        void OutputWrapped(const char *buf, const int len, FTPoint position,
                           const float RemainingWidth, FTBBox *bounds);
        void OutputWrapped(const wchar_t *buf, const int len, FTPoint position,
                           const float RemainingWidth, FTBBox *bounds);

        template <typename T>
        inline FTBBox BBoxI(const T* string, const int len, FTPoint position);

        template <typename T>
        inline void RenderI(const T* string, const int len,
                            FTPoint position);

        template <typename T>
        inline void RenderSpaceI(const T* string, const int len,
                                 FTPoint position, const float extraSpace);
        template <typename T>
        void WrapTextI(const T* buf, const int len, FTPoint position,
                       FTBBox *bounds);

        template <typename T>
        void OutputWrappedI(const T* buf, const int len, FTPoint position,
                            const float RemainingWidth, FTBBox *bounds);


};

typedef vector<FTTextureFont*> FontList;

bool load_fonts(FontList & fonts);

#endif // CHOWDREN_USE_FT2

#endif // CHOWDREN_FONT_H
