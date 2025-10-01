// Source/ReasoningEngine/Public/Infrastructure/RENormalizer.h
#pragma once

#include "CoreMinimal.h"
#include "Infrastructure/Data/REInfrastructureTypes.h"

/**
 * Static utility class for text normalization
 * First step in the processing pipeline - no dependencies
 * Pure text transformation functions
 * 
 * Design Philosophy:
 * - Completely stateless
 * - No UObject overhead
 * - Thread-safe by design
 * - Foundation for all text processing
 */
class REASONINGENGINE_API RENormalizer
{
public:
    // ========== PRIMARY NORMALIZATION ==========
    
    /**
     * Normalize text with default settings
     * Default: lowercase, trim, collapse whitespace, remove extra punctuation
     * @param Text - Input text to normalize
     * @return Normalized text
     */
    static FString NormalizeText(const FString& Text);
    
    /**
     * Normalize text with specific configuration
     * @param Text - Input text to normalize
     * @param Config - Normalization configuration
     * @return Normalized text
     */
    static FString NormalizeTextWithConfig(
        const FString& Text,
        const FRENormalizationConfig& Config
    );
    
    // ========== SPECIFIC NORMALIZATIONS ==========
    
    /**
     * Convert text to lowercase
     * @param Text - Input text
     * @return Lowercase text
     */
    static FString ToLowercase(const FString& Text);
    
    /**
     * Convert text to uppercase
     * @param Text - Input text
     * @return Uppercase text
     */
    static FString ToUppercase(const FString& Text);
    
    /**
     * Trim leading and trailing whitespace
     * @param Text - Input text
     * @return Trimmed text
     */
    static FString TrimWhitespace(const FString& Text);
    
    /**
     * Remove all punctuation from text
     * @param Text - Input text
     * @return Text without punctuation
     */
    static FString RemovePunctuation(const FString& Text);
    
    /**
     * Remove diacritical marks from text
     * @param Text - Input text
     * @return Text without accents
     */
    static FString RemoveAccents(const FString& Text);
    
    /**
     * Collapse multiple spaces to single space
     * @param Text - Input text
     * @return Text with collapsed whitespace
     */
    static FString CollapseWhitespace(const FString& Text);
    
    /**
     * Remove all numeric characters
     * @param Text - Input text
     * @return Text without numbers
     */
    static FString RemoveNumbers(const FString& Text);
    
    /**
     * Remove special characters (keep only alphanumeric and spaces)
     * @param Text - Input text
     * @return Text with only alphanumeric and spaces
     */
    static FString RemoveSpecialCharacters(const FString& Text);
    
    // ========== TEXT ANALYSIS ==========
    
    /**
     * Check if text contains only alphabetic characters
     * @param Text - Text to check
     * @return true if alphabetic only
     */
    static bool IsAlphabetic(const FString& Text);
    
    /**
     * Check if text contains only alphanumeric characters
     * @param Text - Text to check
     * @return true if alphanumeric only
     */
    static bool IsAlphanumeric(const FString& Text);
    
    /**
     * Check if text contains only numeric characters
     * @param Text - Text to check
     * @return true if numeric only
     */
    static bool IsNumeric(const FString& Text);
    
    /**
     * Check if text contains any punctuation
     * @param Text - Text to check
     * @return true if contains punctuation
     */
    static bool ContainsPunctuation(const FString& Text);
    
    /**
     * Check if text contains any whitespace
     * @param Text - Text to check
     * @return true if contains whitespace
     */
    static bool ContainsWhitespace(const FString& Text);
    
    // ========== UNICODE HANDLING ==========
    
    /**
     * Normalize Unicode representation (NFC, NFD, etc.)
     * @param Text - Input text
     * @param Form - Unicode normalization form
     * @return Normalized Unicode text
     */
    static FString NormalizeUnicode(
        const FString& Text,
        EREUnicodeNormalizationForm Form = EREUnicodeNormalizationForm::NFC
    );
    
    /**
     * Convert to ASCII by removing or replacing non-ASCII characters
     * @param Text - Input text
     * @param ReplacementChar - Character to use for non-ASCII (empty = remove)
     * @return ASCII-only text
     */
    static FString ToAscii(
        const FString& Text,
        const FString& ReplacementChar = TEXT("")
    );
    
    // ========== CASE CONVERSION ==========
    
    /**
     * Convert to title case (capitalize first letter of each word)
     * @param Text - Input text
     * @return Title case text
     */
    static FString ToTitleCase(const FString& Text);
    
    /**
     * Convert to sentence case (capitalize first letter of sentences)
     * @param Text - Input text
     * @return Sentence case text
     */
    static FString ToSentenceCase(const FString& Text);
    
    /**
     * Convert from camelCase/PascalCase to space-separated
     * @param Text - Input text
     * @return Space-separated text
     */
    static FString FromCamelCase(const FString& Text);
    
    /**
     * Convert from snake_case to space-separated
     * @param Text - Input text
     * @return Space-separated text
     */
    static FString FromSnakeCase(const FString& Text);
    
    /**
     * Convert from kebab-case to space-separated
     * @param Text - Input text
     * @return Space-separated text
     */
    static FString FromKebabCase(const FString& Text);
    
    // ========== UTILITY FUNCTIONS ==========
    
    /**
     * Get character type classification
     * @param Char - Character to classify
     * @return Character type
     */
    static ERECharacterType GetCharacterType(TCHAR Char);
    
    /**
     * Check if character is a vowel
     * @param Char - Character to check
     * @return true if vowel
     */
    static bool IsVowel(TCHAR Char);
    
    /**
     * Check if character is a consonant
     * @param Char - Character to check
     * @return true if consonant
     */
    static bool IsConsonant(TCHAR Char);
    
    /**
     * Get a normalized version suitable for comparison
     * Applies aggressive normalization for matching
     * @param Text - Input text
     * @return Aggressively normalized text
     */
    static FString GetComparisonForm(const FString& Text);
    
private:
    // ========== DELETED CONSTRUCTORS ==========
    
    RENormalizer() = delete;
    ~RENormalizer() = delete;
    RENormalizer(const RENormalizer&) = delete;
    RENormalizer& operator=(const RENormalizer&) = delete;
    RENormalizer(RENormalizer&&) = delete;
    RENormalizer& operator=(RENormalizer&&) = delete;
};