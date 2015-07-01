/*
    GWEN
    Copyright (c) 2011 Facepunch Studios
    See license in Gwen.h
*/

#ifndef GWEN_RENDERERS_CHOWDREN_H
#define GWEN_RENDERERS_CHOWDREN_H

#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"
#include "image.h"
#include "color.h"

namespace Gwen
{
    namespace Renderer
    {

        class Chowdren : public Gwen::Renderer::Base
        {
                ::Color    m_Color;

            public:

                Chowdren();
                ~Chowdren();

                virtual void Init();

                virtual void Begin();
                virtual void End();

                virtual void SetDrawColor( Gwen::Color color );
                virtual void DrawFilledRect( Gwen::Rect rect );

                virtual void StartClip();
                virtual void EndClip();

                virtual void DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect pTargetRect, float u1 = 0.0f, float v1 = 0.0f, float u2 = 1.0f, float v2 = 1.0f );
                virtual void LoadTexture( Gwen::Texture* pTexture );
                virtual void FreeTexture( Gwen::Texture* pTexture );
                virtual Gwen::Color PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color & col_default );

                virtual void LoadFont( Gwen::Font* pFont );
                virtual void FreeFont( Gwen::Font* pFont );
                virtual void RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString & text );
                virtual Gwen::Point MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString & text );

            public:

                //
                // Self Initialization
                //

                virtual bool InitializeContext( Gwen::WindowProvider* pWindow );
                virtual bool ShutdownContext( Gwen::WindowProvider* pWindow );
                virtual bool PresentContext( Gwen::WindowProvider* pWindow );
                virtual bool ResizedContext( Gwen::WindowProvider* pWindow, int w, int h );
                virtual bool BeginContext( Gwen::WindowProvider* pWindow );
                virtual bool EndContext( Gwen::WindowProvider* pWindow );

                void*    m_pContext;
        };

    }
}
#endif
