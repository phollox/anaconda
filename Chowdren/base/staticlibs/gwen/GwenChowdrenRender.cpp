
#include "Gwen/Renderers/Chowdren.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"
#include "Gwen/WindowProvider.h"

#include <math.h>

#include "common.h"
#include "render.h"
#include "font.h"

namespace Gwen
{
	namespace Renderer
	{
		Chowdren::Chowdren()
		{
		}

		Chowdren::~Chowdren()
		{
		}

		void Chowdren::Init()
		{
		}

		void Chowdren::Begin()
		{
		}

		void Chowdren::End()
		{
		}

		void Chowdren::DrawFilledRect( Gwen::Rect rect )
		{
            Translate(rect);
            Render::draw_quad(rect.x, rect.y, rect.x + rect.w,
                rect.y + rect.h, m_Color);
		}

		void Chowdren::SetDrawColor( Gwen::Color color )
		{
            m_Color = ::Color(color.r, color.g, color.b, color.a);
		}

		void Chowdren::StartClip()
		{
			Gwen::Rect rect = ClipRegion();
			Render::enable_scissor(rect.x * Scale(), rect.y * Scale(),
                rect.w * Scale(), rect.h * Scale());
		};

		void Chowdren::EndClip()
		{
			Render::disable_scissor();
		};

		void Chowdren::DrawTexturedRect( Gwen::Texture* pTexture,
            Gwen::Rect rect, float u1, float v1, float u2, float v2 )
		{
            Translate(rect);

            if (pTexture->data) {
                ::Texture tex = ((FileImage*)pTexture->data)->tex;
                if (tex) {
                    Render::draw_tex(rect.x, rect.y, rect.x + rect.w,
                        rect.y + rect.h, m_Color, tex, u1, v1, u2, v2);
                    return;
                }
            }

            DrawMissingImage(rect);
		}

		void Chowdren::LoadTexture( Gwen::Texture* pTexture )
		{
            pTexture->failed = true;
            return;

            if (pTexture->data)
                FreeTexture(pTexture);

            pTexture->data = (void*)new FileImage(pTexture->name.c_str(), 0, 0,
                0, 0, TransparentColor());

            if (((FileImage*)pTexture->data)->tex == 0) {
                pTexture->failed = true;
                FreeTexture(pTexture);
            }
		}

		void Chowdren::FreeTexture( Gwen::Texture* pTexture )
		{
            if (pTexture->data) {
                delete (FileImage*)pTexture->data;
                pTexture->data = NULL;
            }
		}

		Gwen::Color Chowdren::PixelColour( Gwen::Texture* pTexture,
            unsigned int x, unsigned int y, const Gwen::Color & col_default )
		{
            // For texture-based skins, ignored
			return Gwen::Color(0, 0, 0, 0);
		}

        void Chowdren::LoadFont( Gwen::Font* pFont )
        {
            // Never called...
        }

        void Chowdren::FreeFont( Gwen::Font* pFont )
        {
            // Never called...
        }

        void Chowdren::RenderText( Gwen::Font* pFont, Gwen::Point pos,
            const Gwen::UnicodeString & text )
        {
            if (!init_font())
                return;

            FTTextureFont* font = get_font(pFont->size);
            if (font) {
			    Translate(pos.x, pos.y);
                FTTextureFont::color = ::Color(0, 0, 0, 255);
                FTSimpleLayout layout;
                layout.SetLineLength(10000);
                layout.SetFont(font);
                layout.Render(text.c_str(), -1, FTPoint(pos.x, pos.y + font->Ascender()));
            }
        }

        Gwen::Point Chowdren::MeasureText( Gwen::Font* pFont,
            const Gwen::UnicodeString & text )
        {
            FTSimpleLayout layout;
            layout.SetLineLength(10000);
            layout.SetFont(get_font(pFont->size));
            FTBBox bbox = layout.BBoxL(text.c_str());
            FTPoint size = bbox.Upper() - bbox.Lower();
            return Gwen::Point((int)ceil(size.X())+1, (int)ceil(size.Y()));
        }

		bool Chowdren::InitializeContext( Gwen::WindowProvider* pWindow )
		{
            return true;
		}

		bool Chowdren::ShutdownContext( Gwen::WindowProvider* pWindow )
		{
            return true;
		}

		bool Chowdren::PresentContext( Gwen::WindowProvider* pWindow )
		{
            return true;
		}

		bool Chowdren::ResizedContext( Gwen::WindowProvider* pWindow, int w,
            int h )
		{
            return true;
		}

		bool Chowdren::BeginContext( Gwen::WindowProvider* pWindow )
		{
            return true;
		}

		bool Chowdren::EndContext( Gwen::WindowProvider* pWindow )
		{
			return true;
		}

	}
}