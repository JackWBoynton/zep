#pragma once

#include <memory>
#include <vector>

#include "zep/completion.h"
#include "zep/glyph_iterator.h"
#include "zep/range_markers.h"

namespace Zep
{

class ZepWindow;
class ZepBuffer;

// Manages the completion popup window and user interaction
class CompletionWindow
{
public:
    CompletionWindow(ZepWindow& window);
    virtual ~CompletionWindow();

    // Show completion list at the given position
    void Show(const NVec2f& position, const std::vector<CompletionItem>& completions,
              const GlyphIterator& triggerPos);

    // Hide the completion window
    void Hide();

    // Check if completion window is currently visible
    bool IsVisible() const { return m_isVisible; }

    // Handle navigation and selection within the completion list
    bool HandleKeyPress(uint32_t key, uint32_t modifiers);

    // Get the currently selected completion item
    const CompletionItem* GetSelectedItem() const;

    // Update the completion list (e.g., when user types more characters)
    void UpdateCompletions(const std::vector<CompletionItem>& completions);

    // Get the position where completion was triggered
    GlyphIterator GetTriggerPosition() const { return m_triggerPos; }

private:
    ZepWindow& m_window;
    bool m_isVisible;
    std::vector<CompletionItem> m_completions;
    int m_selectedIndex;
    GlyphIterator m_triggerPos;
    std::shared_ptr<RangeMarker> m_completionMarker;

    // Create the visual representation of the completion list
    void CreateCompletionMarker(const NVec2f& position);

    // Build the display text for the completion popup
    std::string BuildCompletionText() const;

    // Navigate selection up/down
    void SelectPrevious();
    void SelectNext();

    // Ensure selected index is valid
    void ClampSelection();
};

} // namespace Zep