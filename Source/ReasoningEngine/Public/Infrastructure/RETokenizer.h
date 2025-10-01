// Source/ReasoningEngine/Public/Infrastructure/RETokenizer.h
#pragma once

#include "CoreMinimal.h"
#include "Infrastructure/Data/REInfrastructureTypes.h"

// Forward declarations
class FREKnowledgeBase;  // From Symbolic layer

/**
 * Static utility class for text tokenization
 * Second step in processing pipeline (after normalization)
 * Pure tokenization functions - no state management
 * 
 * Design Philosophy:
 * - Stateless tokenization algorithms
 * - Reads vocabularies/stopwords from Symbolic Knowledge
 * - Thread-safe by design
 * - No UObject overhead
 * 
 * Dependencies:
 * - RENormalizer for text preparation
 * - REFuzzy for variant generation (typos)
 * - Symbolic Knowledge for vocabularies/stopwords
 */
class REASONINGENGINE_API RETokenizer
{
public:
    // ========== PRIMARY TOKENIZATION ==========
    
    /**
     * Tokenize text with default configuration
     * Uses standard delimiters and rules
     * @param Text - Input text (should be normalized)
     * @return Token stream
     */
    static FRETokenStream Tokenize(const FString& Text);
    
    /**
     * Tokenize text with specific configuration
     * @param Text - Input text
     * @param Config - Tokenization configuration
     * @return Token stream
     */
    static FRETokenStream TokenizeWithConfig(
        const FString& Text,
        const FRETokenizerConfig& Config
    );
    
    /**
     * Tokenize text using knowledge base for vocabularies
     * @param Text - Input text
     * @param Config - Tokenization configuration
     * @param Knowledge - Knowledge base for vocabularies/stopwords
     * @return Enriched token stream
     */
    static FRETokenStream TokenizeWithKnowledge(
        const FString& Text,
        const FRETokenizerConfig& Config,
        const FREKnowledgeBase& Knowledge
    );
    
    // ========== TEXT SPLITTING ==========
    
    /**
     * Split text by delimiters
     * @param Text - Input text
     * @param Delimiters - Characters to split on
     * @param bKeepDelimiters - Include delimiters as tokens
     * @return Array of split strings
     */
    static TArray<FString> SplitByDelimiters(
        const FString& Text,
        const FString& Delimiters = TEXT(" \t\n\r.,;:!?()[]{}"),
        bool bKeepDelimiters = false
    );
    
    /**
     * Split camelCase/PascalCase text
     * @param Text - CamelCase text
     * @return Array of component words
     */
    static TArray<FString> SplitCamelCase(const FString& Text);
    
    /**
     * Split snake_case text
     * @param Text - Snake case text
     * @return Array of component words
     */
    static TArray<FString> SplitSnakeCase(const FString& Text);
    
    /**
     * Split kebab-case text
     * @param Text - Kebab case text
     * @return Array of component words
     */
    static TArray<FString> SplitKebabCase(const FString& Text);
    
    /**
     * Split text with numbers
     * @param Text - Text with mixed alphanumeric
     * @return Array with separated numbers and words
     */
    static TArray<FString> SplitAlphanumeric(const FString& Text);
    
    // ========== NAMING CONVENTION ==========
    
    /**
     * Detect naming convention of text
     * @param Text - Input text
     * @return Detected convention
     */
    static ERENamingConvention DetectNamingConvention(const FString& Text);
    
    /**
     * Convert between naming conventions
     * @param Text - Input text
     * @param FromConvention - Source convention
     * @param ToConvention - Target convention
     * @return Converted text
     */
    static FString ConvertNamingConvention(
        const FString& Text,
        ERENamingConvention FromConvention,
        ERENamingConvention ToConvention
    );
    
    /**
     * Intelligently parse animation names
     * Handles prefixes, suffixes, numbers, etc.
     * @param AnimationName - e.g., "MM_Walk_Forward_01"
     * @return Token stream with proper segmentation
     */
    static FRETokenStream TokenizeAnimationName(const FString& AnimationName);
    
    // ========== TOKEN CREATION ==========
    
    /**
     * Create token from text with metadata
     * @param Text - Token text
     * @param StartIndex - Start position in original text
     * @param EndIndex - End position in original text
     * @param Type - Token type
     * @return Configured token
     */
    static FREToken CreateToken(
        const FString& Text,
        int32 StartIndex,
        int32 EndIndex,
        ERETokenType Type = ERETokenType::Word
    );
    
    /**
     * Classify token type
     * @param Text - Token text to classify
     * @return Token type
     */
    static ERETokenType ClassifyTokenType(const FString& Text);
    
    // ========== VARIANT GENERATION ==========
    
    /**
     * Generate text variants (typos, abbreviations, expansions)
     * @param Token - Original token
     * @param bIncludeTypos - Generate common typos
     * @param bIncludeAbbreviations - Generate abbreviations
     * @param bIncludeExpansions - Generate expansions
     * @return Array of variants
     */
    static TArray<FString> GenerateVariants(
        const FString& Token,
        bool bIncludeTypos = true,
        bool bIncludeAbbreviations = true,
        bool bIncludeExpansions = true
    );
    
    /**
     * Generate common typos for a word
     * Uses keyboard distance and common mistakes
     * @param Word - Original word
     * @param MaxDistance - Maximum edit distance
     * @return Array of typo variants
     */
    static TArray<FString> GenerateTypos(
        const FString& Word,
        int32 MaxDistance = 1
    );
    
    /**
     * Generate abbreviations
     * @param Word - Original word
     * @return Array of abbreviations
     */
    static TArray<FString> GenerateAbbreviations(const FString& Word);
    
    /**
     * Generate expansions (opposite of abbreviations)
     * @param Abbreviation - Abbreviated form
     * @return Array of possible expansions
     */
    static TArray<FString> GenerateExpansions(const FString& Abbreviation);
    
    // ========== TOKEN ANALYSIS ==========
    
    /**
     * Group tokens by similarity
     * @param Tokens - Tokens to group
     * @param SimilarityThreshold - Minimum similarity for grouping
     * @return Array of token groups
     */
    static TArray<FRETokenGroup> GroupTokensBySimilarity(
        const TArray<FREToken>& Tokens,
        float SimilarityThreshold = 0.7f
    );
    
    /**
     * Find compound words in token stream
     * @param Stream - Token stream
     * @param Vocabularies - Known compound words
     * @return Array of identified compounds
     */
    static TArray<FString> FindCompoundWords(
        const FRETokenStream& Stream,
        const TArray<FREVocabularyEntry>& Vocabularies
    );
    
    /**
     * Calculate token weights (TF-IDF style)
     * @param Stream - Token stream
     * @param DocumentFrequencies - Optional document frequencies for IDF
     * @return Stream with updated weights
     */
    static FRETokenStream CalculateTokenWeights(
        const FRETokenStream& Stream,
        const TMap<FString, float>* DocumentFrequencies = nullptr
    );
    
    // ========== STOP WORDS ==========
    
    /**
     * Filter stop words from token stream
     * @param Stream - Token stream
     * @param StopWords - Set of stop words to filter
     * @return Filtered stream
     */
    static FRETokenStream FilterStopWords(
        const FRETokenStream& Stream,
        const TSet<FString>& StopWords
    );
    
    /**
     * Get default English stop words
     * @return Set of common stop words
     */
    static TSet<FString> GetDefaultStopWords();
    
    /**
     * Check if word is a common stop word
     * @param Word - Word to check
     * @return true if stop word
     */
    static bool IsDefaultStopWord(const FString& Word);
    
    // ========== N-GRAMS ==========
    
    /**
     * Generate character n-grams from text
     * @param Text - Input text
     * @param N - Size of grams
     * @return Array of n-grams
     */
    static TArray<FString> GenerateCharacterNGrams(
        const FString& Text,
        int32 N = 2
    );
    
    /**
     * Generate word n-grams from token stream
     * @param Tokens - Input tokens
     * @param N - Size of grams
     * @return Array of n-gram combinations
     */
    static TArray<FString> GenerateWordNGrams(
        const TArray<FREToken>& Tokens,
        int32 N = 2
    );
    
    // ========== UTILITY FUNCTIONS ==========
    
    /**
     * Merge adjacent tokens of same type
     * @param Stream - Token stream
     * @return Stream with merged tokens
     */
    static FRETokenStream MergeAdjacentTokens(const FRETokenStream& Stream);
    
    /**
     * Split tokens that are too long
     * @param Stream - Token stream
     * @param MaxLength - Maximum token length
     * @return Stream with split tokens
     */
    static FRETokenStream SplitLongTokens(
        const FRETokenStream& Stream,
        int32 MaxLength = 50
    );
    
    /**
     * Remove tokens below minimum length
     * @param Stream - Token stream
     * @param MinLength - Minimum token length
     * @return Filtered stream
     */
    static FRETokenStream RemoveShortTokens(
        const FRETokenStream& Stream,
        int32 MinLength = 1
    );
    
    /**
     * Convert token stream to string representation
     * @param Stream - Token stream
     * @param Separator - Separator between tokens
     * @return String representation
     */
    static FString TokenStreamToString(
        const FRETokenStream& Stream,
        const FString& Separator = TEXT(" ")
    );
    
    /**
     * Create token stream from string array
     * @param Strings - Array of strings
     * @return Token stream
     */
    static FRETokenStream CreateTokenStream(const TArray<FString>& Strings);
    
private:
    // ========== INTERNAL HELPERS ==========
    
    /**
     * Check if character is a delimiter
     */
    static FORCEINLINE bool IsDelimiter(TCHAR Char, const FString& Delimiters)
    {
        return Delimiters.Contains(FString::Chr(Char));
    }
    
    /**
     * Check if transition indicates camelCase boundary
     */
    static bool IsCamelCaseBoundary(TCHAR Prev, TCHAR Current, TCHAR Next);
    
    /**
     * Common abbreviation patterns
     */
    static bool IsCommonAbbreviation(const FString& Text);
    
    /**
     * Get default delimiter set
     */
    static FString GetDefaultDelimiters()
    {
        return TEXT(" \t\n\r.,;:!?()[]{}'\"-/\\|");
    }
    
    // ========== DELETED CONSTRUCTORS ==========
    
    RETokenizer() = delete;
    ~RETokenizer() = delete;
    RETokenizer(const RETokenizer&) = delete;
    RETokenizer& operator=(const RETokenizer&) = delete;
    RETokenizer(RETokenizer&&) = delete;
    RETokenizer& operator=(RETokenizer&&) = delete;
};