#include "zep/completion.h"
#include "zep/mcommon/string/stringutils.h"

#include <algorithm>

namespace Zep
{

SignalCompletionProvider::SignalCompletionProvider(char triggerChar)
    : m_triggerChar(triggerChar)
{
}

void SignalCompletionProvider::SetTriggerCharacter(char triggerChar)
{
    m_triggerChar = triggerChar;
}

void SignalCompletionProvider::SetSignalList(const std::vector<std::string>& signals)
{
    m_signals.clear();
    for (const auto& signal : signals)
    {
        m_signals.emplace_back(signal, "signal");
    }
}

void SignalCompletionProvider::AddSignal(const std::string& signal, const std::string& description)
{
    m_signals.emplace_back(signal, signal, description, "signal");
}

void SignalCompletionProvider::ClearSignals()
{
    m_signals.clear();
}

std::vector<CompletionItem> SignalCompletionProvider::GetCompletions(
    const std::string& prefix,
    const GlyphIterator& position,
    ZepBuffer& buffer)
{
    // Remove the trigger character from the prefix if it's there
    std::string searchPrefix = prefix;
    if (!searchPrefix.empty() && searchPrefix[0] == m_triggerChar)
    {
        searchPrefix = searchPrefix.substr(1);
    }

    return FilterByPrefix(searchPrefix);
}

bool SignalCompletionProvider::ShouldTrigger(char triggerChar, const GlyphIterator& position, ZepBuffer& buffer)
{
    return triggerChar == m_triggerChar;
}

std::vector<char> SignalCompletionProvider::GetTriggerCharacters() const
{
    return { m_triggerChar };
}

std::vector<CompletionItem> SignalCompletionProvider::FilterByPrefix(const std::string& prefix) const
{
    std::vector<CompletionItem> results;

    if (prefix.empty())
    {
        // Return all signals if no prefix
        return m_signals;
    }

    // Case-insensitive prefix matching
    std::string lowerPrefix = string_tolower(prefix);

    for (const auto& signal : m_signals)
    {
        std::string lowerSignal = string_tolower(signal.text);
        if (lowerSignal.find(lowerPrefix) == 0) // Starts with prefix
        {
            results.push_back(signal);
        }
    }

    // Sort results by length first (shorter matches first), then alphabetically
    std::sort(results.begin(), results.end(), [](const CompletionItem& a, const CompletionItem& b) {
        if (a.text.length() != b.text.length())
            return a.text.length() < b.text.length();
        return a.text < b.text;
    });

    return results;
}

} // namespace Zep