#pragma once

#include <memory>
#include <string>
#include <vector>

#include "zep/buffer.h"
#include "zep/glyph_iterator.h"

namespace Zep
{

// Represents a single completion item that can be suggested to the user
struct CompletionItem
{
    std::string text;        // The actual text to insert (e.g., "clk", "rst")
    std::string display;     // Text shown in the completion list (can include extra info)
    std::string description; // Optional detailed description
    std::string kind;        // Type of completion: "signal", "variable", "function", etc.

    CompletionItem(const std::string& text, const std::string& kind = "signal")
        : text(text), display(text), kind(kind)
    {
    }

    CompletionItem(const std::string& text, const std::string& display,
                  const std::string& description, const std::string& kind = "signal")
        : text(text), display(display), description(description), kind(kind)
    {
    }
};

// Interface for providing completions based on context
class ICompletionProvider
{
public:
    virtual ~ICompletionProvider() = default;

    // Get completions for the given prefix at the specified position
    virtual std::vector<CompletionItem> GetCompletions(
        const std::string& prefix,
        const GlyphIterator& position,
        ZepBuffer& buffer) = 0;

    // Check if this provider should be activated for the given trigger character
    virtual bool ShouldTrigger(char triggerChar, const GlyphIterator& position, ZepBuffer& buffer) = 0;

    // Get the trigger characters this provider responds to
    virtual std::vector<char> GetTriggerCharacters() const = 0;
};

// Configurable signal name completion provider
class SignalCompletionProvider : public ICompletionProvider
{
public:
    SignalCompletionProvider(char triggerChar = '$');
    virtual ~SignalCompletionProvider() = default;

    // Configuration
    void SetTriggerCharacter(char triggerChar);
    void SetSignalList(const std::vector<std::string>& signals);
    void AddSignal(const std::string& signal, const std::string& description = "");
    void ClearSignals();

    // ICompletionProvider implementation
    std::vector<CompletionItem> GetCompletions(
        const std::string& prefix,
        const GlyphIterator& position,
        ZepBuffer& buffer) override;

    bool ShouldTrigger(char triggerChar, const GlyphIterator& position, ZepBuffer& buffer) override;
    std::vector<char> GetTriggerCharacters() const override;

private:
    char m_triggerChar;
    std::vector<CompletionItem> m_signals;

    // Filter signals based on prefix
    std::vector<CompletionItem> FilterByPrefix(const std::string& prefix) const;
};

} // namespace Zep