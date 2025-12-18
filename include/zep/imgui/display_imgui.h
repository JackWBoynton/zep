#pragma once
#include "../display.h"
#include "../syntax.h"
#include <imgui.h>
#include <string>

// Can't include this publicly
// #include "zep/mcommon/logger.h"

namespace Zep
{

inline NVec2f toNVec2f(const ImVec2& im)
{
    return NVec2f(im.x, im.y);
}

inline ImVec2 toImVec2(const NVec2f& im)
{
    return ImVec2(im.x, im.y);
}

inline NVec4f toNVec4f(const ImVec4& im)
{
    return NVec4f(im.x, im.y, im.z, im.w);
}

inline ImVec4 toImVec4(const NVec4f& im)
{
    return ImVec4(im.x, im.y, im.z, im.w);
}

static ImWchar greek_range[] = { 0x300, 0x52F, 0x1f00, 0x1fff, 0, 0 };

class ZepFont_ImGui : public ZepFont
{
public:
    ZepFont_ImGui(ZepDisplay& display)
        : ZepFont(display)
        , m_baseFontSize(ImGui::GetFontSize())
        , m_scale(1.0f)
    {
        m_pixelHeight = static_cast<int>(m_baseFontSize);
    }

    virtual void SetPixelHeight(int pixelHeight) override
    {
        m_scale = static_cast<float>(pixelHeight) / m_baseFontSize;
        m_pixelHeight = pixelHeight;
    }

    virtual NVec2f GetTextSize(const uint8_t* pBegin, const uint8_t* pEnd = nullptr) const override
    {
        ImGui::PushFontSize(m_baseFontSize * m_scale);
        ImVec2 text_size = ImGui::CalcTextSize((const char*)pBegin, (const char*)pEnd);
        if (text_size.x == 0.0)
        {
            const char chDefault = 'A';
            text_size = ImGui::CalcTextSize(&chDefault, &chDefault + 1);
        }
        ImGui::PopFontSize();
        return toNVec2f(text_size);
    }

    void PushSize() const { ImGui::PushFontSize(m_baseFontSize * m_scale); }
    void PopSize() const { ImGui::PopFontSize(); }

    void AdjustScale(float delta) { m_scale = std::max(0.5f, m_scale + delta); }

private:
    float m_baseFontSize;
    float m_scale;
};

class ZepDisplay_ImGui : public ZepDisplay
{
public:
    ZepDisplay_ImGui()
        : ZepDisplay()
    {
    }

    void DrawChars(ZepFont& font, const NVec2f& pos, const NVec4f& col, const uint8_t* text_begin, const uint8_t* text_end) const override
    {
        auto& zepFont = static_cast<ZepFont_ImGui&>(font);
        zepFont.PushSize();
        
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        if (text_end == nullptr)
        {
            text_end = text_begin + strlen((const char*)text_begin);
        }
        const auto modulatedColor = GetStyleModulatedColor(col);
        if (m_clipRect.Width() == 0)
        {
            drawList->AddText(toImVec2(pos), modulatedColor, (const char*)text_begin, (const char*)text_end);
        }
        else
        {
            drawList->PushClipRect(toImVec2(m_clipRect.topLeftPx), toImVec2(m_clipRect.bottomRightPx));
            drawList->AddText(toImVec2(pos), modulatedColor, (const char*)text_begin, (const char*)text_end);
            drawList->PopClipRect();
        }
        
        zepFont.PopSize();
    }

    void DrawLine(const NVec2f& start, const NVec2f& end, const NVec4f& color, float width) const override
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const auto modulatedColor = GetStyleModulatedColor(color);
        // Background rect for numbers
        if (m_clipRect.Width() == 0)
        {
            drawList->AddLine(toImVec2(start), toImVec2(end), modulatedColor, width);
        }
        else
        {
            drawList->PushClipRect(toImVec2(m_clipRect.topLeftPx), toImVec2(m_clipRect.bottomRightPx));
            drawList->AddLine(toImVec2(start), toImVec2(end), modulatedColor, width);
            drawList->PopClipRect();
        }
    }

    void DrawRectFilled(const NRectf& rc, const NVec4f& color) const override
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const auto modulatedColor = GetStyleModulatedColor(color);
        // Background rect for numbers
        if (m_clipRect.Width() == 0)
        {
            drawList->AddRectFilled(toImVec2(rc.topLeftPx), toImVec2(rc.bottomRightPx), modulatedColor);
        }
        else
        {
            drawList->PushClipRect(toImVec2(m_clipRect.topLeftPx), toImVec2(m_clipRect.bottomRightPx));
            drawList->AddRectFilled(toImVec2(rc.topLeftPx), toImVec2(rc.bottomRightPx), modulatedColor);
            drawList->PopClipRect();
        }
    }

    virtual void SetClipRect(const NRectf& rc) override
    {
        m_clipRect = rc;
    }

    virtual ZepFont& GetFont(ZepTextType type) override
    {
        if (!m_font)
        {
            m_font = std::make_unique<ZepFont_ImGui>(*this);
            m_cachedSize = ImGui::GetFontSize();
        }
        return *m_font;
    }

    void HandleFontSizeChange(float delta)
    {
        if (m_font)
        {
            static_cast<ZepFont_ImGui*>(m_font.get())->AdjustScale(delta);
        }
    }

private:
    static ImU32 GetStyleModulatedColor(const NVec4f& color)
    {
        return ToPackedABGR(NVec4f(color.x, color.y, color.z, color.w * ImGui::GetStyle().Alpha));
    }

    NRectf m_clipRect;
    mutable std::unique_ptr<ZepFont_ImGui> m_font;
    mutable float m_cachedSize = 0.0f;
}; // namespace Zep

} // namespace Zep
