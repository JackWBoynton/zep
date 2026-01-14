#include "zep/completion_window.h"
#include "zep/window.h"
#include "zep/buffer.h"
#include "zep/theme.h"
#include "zep/mode.h"

#include <sstream>
#include <algorithm>

namespace Zep
{

CompletionWindow::CompletionWindow(ZepWindow& window)
    : m_window(window)
    , m_isVisible(false)
    , m_selectedIndex(0)
{
}

CompletionWindow::~CompletionWindow()
{
    Hide();
}

void CompletionWindow::Show(const NVec2f& position, const std::vector<CompletionItem>& completions,
                           const GlyphIterator& triggerPos)
{
    if (completions.empty())
    {
        Hide();
        return;
    }

    m_completions = completions;
    m_triggerPos = triggerPos;
    m_selectedIndex = 0;
    m_isVisible = true;

    CreateCompletionMarker(position);
}

void CompletionWindow::Hide()
{
    if (m_completionMarker)
    {
        m_window.GetBuffer().ClearRangeMarker(m_completionMarker);
        m_completionMarker.reset();
    }
    m_isVisible = false;
    m_completions.clear();
    m_selectedIndex = 0;
}

bool CompletionWindow::HandleKeyPress(uint32_t key, uint32_t modifiers)
{
    if (!m_isVisible || m_completions.empty())
    {
        return false;
    }

    // Handle navigation keys
    switch (key)
    {
    case ExtKeys::UP:
        SelectPrevious();
        return true;

    case ExtKeys::DOWN:
        SelectNext();
        return true;

    case ExtKeys::TAB:
    case ExtKeys::RETURN:
        // Accept the current selection
        return true; // Caller should handle the actual insertion

    case ExtKeys::ESCAPE:
        Hide();
        return true;

    default:
        // Let other keys pass through to continue typing
        return false;
    }
}

const CompletionItem* CompletionWindow::GetSelectedItem() const
{
    if (!m_isVisible || m_completions.empty() || m_selectedIndex < 0 ||
        m_selectedIndex >= static_cast<int>(m_completions.size()))
    {
        return nullptr;
    }
    return &m_completions[m_selectedIndex];
}

void CompletionWindow::UpdateCompletions(const std::vector<CompletionItem>& completions)
{
    if (!m_isVisible)
    {
        return;
    }

    m_completions = completions;
    ClampSelection();

    if (m_completions.empty())
    {
        Hide();
        return;
    }

    // Update the visual representation
    if (m_completionMarker)
    {
        m_completionMarker->SetDescription(BuildCompletionText());
    }
}

void CompletionWindow::CreateCompletionMarker(const NVec2f& position)
{
    auto& buffer = m_window.GetBuffer();

    // Create a range marker for the completion popup
    m_completionMarker = std::make_shared<RangeMarker>(buffer);

    // Set the range to a single character at the trigger position
    ByteRange range(m_triggerPos.Index(), m_triggerPos.Index() + 1);
    m_completionMarker->SetRange(range);

    // Configure the marker to show as a tooltip
    m_completionMarker->displayType = RangeMarkerDisplayType::Tooltip;
    m_completionMarker->tipPos = ToolTipPos::BelowLine;

    // Set the content
    m_completionMarker->SetName("Completions");
    m_completionMarker->SetDescription(BuildCompletionText());

    // Set colors for the completion popup
    m_completionMarker->SetColors(ThemeColor::Background, ThemeColor::Text, ThemeColor::Info);

    // Add the marker to the buffer
    buffer.AddRangeMarker(m_completionMarker);
}

std::string CompletionWindow::BuildCompletionText() const
{
    std::ostringstream ss;

    for (int i = 0; i < static_cast<int>(m_completions.size()); ++i)
    {
        const auto& item = m_completions[i];

        // Highlight the selected item
        if (i == m_selectedIndex)
        {
            ss << "> " << item.display;
        }
        else
        {
            ss << "  " << item.display;
        }

        // Add description if available
        if (!item.description.empty())
        {
            ss << " - " << item.description;
        }

        // Add newline except for the last item
        if (i < static_cast<int>(m_completions.size()) - 1)
        {
            ss << "\n";
        }
    }

    return ss.str();
}

void CompletionWindow::SelectPrevious()
{
    if (m_completions.empty())
        return;

    m_selectedIndex--;
    ClampSelection();

    // Update the visual representation
    if (m_completionMarker)
    {
        m_completionMarker->SetDescription(BuildCompletionText());
    }
}

void CompletionWindow::SelectNext()
{
    if (m_completions.empty())
        return;

    m_selectedIndex++;
    ClampSelection();

    // Update the visual representation
    if (m_completionMarker)
    {
        m_completionMarker->SetDescription(BuildCompletionText());
    }
}

void CompletionWindow::ClampSelection()
{
    if (m_completions.empty())
    {
        m_selectedIndex = 0;
        return;
    }

    if (m_selectedIndex < 0)
        m_selectedIndex = static_cast<int>(m_completions.size()) - 1;
    else if (m_selectedIndex >= static_cast<int>(m_completions.size()))
        m_selectedIndex = 0;
}

} // namespace Zep