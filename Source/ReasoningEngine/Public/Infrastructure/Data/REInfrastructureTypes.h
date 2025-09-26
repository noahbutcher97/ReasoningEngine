#pragma once

#include "CoreMinimal.h"
#include "REInfrastructureTypes.generated.h"

/**
 * Unified infrastructure type definitions for ReasoningEngine.
 * Core types for preprocessing, tokenization, caching, and processor results.
 */

// =========================================================================
// Core Processing Types
// =========================================================================

/**
 * Text normalization modes
 */
UENUM(BlueprintType)
enum class ERENormalizationMode : uint8
{
    None            UMETA(DisplayName = "None"),
    Lowercase       UMETA(DisplayName = "Lowercase"),
    Uppercase       UMETA(DisplayName = "Uppercase"),
    TrimWhitespace  UMETA(DisplayName = "Trim Whitespace"),
    RemoveAccents   UMETA(DisplayName = "Remove Accents"),
    Full            UMETA(DisplayName = "Full Normalization")
};

/**
 * Processing mode for the reasoning engine
 */
UENUM(BlueprintType)
enum class EREProcessingMode : uint8
{
    Semantic        UMETA(DisplayName = "Semantic Only"),
    Symbolic        UMETA(DisplayName = "Symbolic Only"),
    Hybrid          UMETA(DisplayName = "Hybrid"),
    Auto            UMETA(DisplayName = "Auto-Select")
};

/**
 * Log level for engine logging
 */
UENUM(BlueprintType)
enum class ERELogLevel : uint8
{
    None            UMETA(DisplayName = "None"),
    Error           UMETA(DisplayName = "Error"),
    Warning         UMETA(DisplayName = "Warning"),
    Info            UMETA(DisplayName = "Info"),
    Verbose         UMETA(DisplayName = "Verbose"),
    VeryVerbose     UMETA(DisplayName = "Very Verbose")
};

/**
 * Text normalization configuration
 * Used by URENormalizer for text preprocessing
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRENormalizationConfig
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Normalization")
    bool bLowercase = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Normalization")
    bool bTrimWhitespace = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Normalization")
    bool bRemoveAccents = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Normalization")
    bool bCollapseWhitespace = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Normalization")
    bool bRemovePunctuation = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Normalization")
    bool bRemoveNumbers = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Normalization")
    bool bPreserveCase = false;
};

/**
 * Query context for reasoning operations
 * Shared across semantic and symbolic processing
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREQueryContext
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    FString Domain;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    TMap<FString, FString> Parameters;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    float ConfidenceThreshold = 0.7f;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    bool bUseCache = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    bool bNormalizeInput = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    EREProcessingMode ProcessingMode = EREProcessingMode::Auto;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    int32 MaxResults = 10;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    float TimeoutSeconds = 5.0f;
};

/**
 * Unified processing result
 * Returned by all processors and reasoning operations
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREProcessorResult
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    bool bSuccess = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    FString ProcessorName;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    FString Output;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    float Confidence = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    float SemanticScore = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    TArray<FName> SymbolicEntities;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    bool bUsedSemanticFallback = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    bool bUsedSymbolicFallback = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    FString Explanation;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    EREProcessingMode ProcessingMode = EREProcessingMode::Auto;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    TMap<FString, FString> Metadata;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    float ProcessingTimeMS = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    TArray<FString> Errors;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    TArray<FString> Warnings;
};

// =========================================================================
// Tokenization Types (formerly from RETokenTypes.h)
// =========================================================================

/**
 * Token type enumeration
 */
UENUM(BlueprintType)
enum class ERETokenType : uint8
{
    Unknown         UMETA(DisplayName = "Unknown"),
    Word            UMETA(DisplayName = "Word"),
    Number          UMETA(DisplayName = "Number"),
    Symbol          UMETA(DisplayName = "Symbol"),
    Delimiter       UMETA(DisplayName = "Delimiter"),
    Operator        UMETA(DisplayName = "Operator"),
    Keyword         UMETA(DisplayName = "Keyword"),
    Whitespace      UMETA(DisplayName = "Whitespace"),
    Punctuation     UMETA(DisplayName = "Punctuation"),
    CamelCase       UMETA(DisplayName = "CamelCase"),
    SnakeCase       UMETA(DisplayName = "SnakeCase"),
    Identifier      UMETA(DisplayName = "Identifier"),
    Literal         UMETA(DisplayName = "Literal")
};

/**
 * Naming convention detection
 */
UENUM(BlueprintType)
enum class ERENamingConvention : uint8
{
    Unknown         UMETA(DisplayName = "Unknown"),
    CamelCase       UMETA(DisplayName = "CamelCase"),
    PascalCase      UMETA(DisplayName = "PascalCase"),
    SnakeCase       UMETA(DisplayName = "snake_case"),
    KebabCase       UMETA(DisplayName = "kebab-case"),
    UpperCase       UMETA(DisplayName = "UPPER_CASE"),
    LowerCase       UMETA(DisplayName = "lowercase"),
    Mixed           UMETA(DisplayName = "Mixed"),
    Natural         UMETA(DisplayName = "Natural Language")
};

/**
 * Single token representation
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREToken
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Token")
    FString Text;
    
    UPROPERTY(BlueprintReadWrite, Category="Token")
    FString NormalizedText;
    
    UPROPERTY(BlueprintReadWrite, Category="Token")
    ERETokenType Type = ERETokenType::Unknown;
    
    UPROPERTY(BlueprintReadWrite, Category="Token")
    int32 StartIndex = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Token")
    int32 EndIndex = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Token")
    int32 LineNumber = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Token")
    int32 ColumnNumber = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Variants")
    TArray<FString> Variants;
    
    UPROPERTY(BlueprintReadWrite, Category="Metadata")
    TMap<FString, FString> Metadata;
    
    UPROPERTY(BlueprintReadWrite, Category="Scoring")
    float Weight = 1.0f;
    
    UPROPERTY(BlueprintReadWrite, Category="Scoring")
    float Confidence = 1.0f;
    
    int32 Length() const { return EndIndex - StartIndex; }
    bool IsEmpty() const { return Text.IsEmpty(); }
};

/**
 * Token stream - sequence of tokens with navigation
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRETokenStream
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Stream")
    TArray<FREToken> Tokens;
    
    UPROPERTY(BlueprintReadWrite, Category="Stream")
    FString OriginalText;
    
    UPROPERTY(BlueprintReadWrite, Category="Stream")
    int32 CurrentIndex = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Stream")
    ERENamingConvention DetectedConvention = ERENamingConvention::Unknown;
    
    UPROPERTY(BlueprintReadWrite, Category="Metadata")
    TMap<FString, FString> Metadata;
    
    // Navigation methods
    bool HasNext() const { return CurrentIndex < Tokens.Num() - 1; }
    bool HasPrevious() const { return CurrentIndex > 0; }
    
    FREToken* Next()
    {
        if (HasNext())
        {
            CurrentIndex++;
            return &Tokens[CurrentIndex];
        }
        return nullptr;
    }
    
    FREToken* Previous()
    {
        if (HasPrevious())
        {
            CurrentIndex--;
            return &Tokens[CurrentIndex];
        }
        return nullptr;
    }
    
    FREToken* Current()
    {
        return Tokens.IsValidIndex(CurrentIndex) ? &Tokens[CurrentIndex] : nullptr;
    }
    
    FREToken* Peek(int32 Offset = 1)
    {
        int32 Index = CurrentIndex + Offset;
        return Tokens.IsValidIndex(Index) ? &Tokens[Index] : nullptr;
    }
    
    void Reset() { CurrentIndex = 0; }
    int32 Num() const { return Tokens.Num(); }
    bool IsEmpty() const { return Tokens.Num() == 0; }
};

/**
 * Token group for similarity clustering
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRETokenGroup
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Group")
    FString GroupName;
    
    UPROPERTY(BlueprintReadWrite, Category="Group")
    TArray<FREToken> Tokens;
    
    UPROPERTY(BlueprintReadWrite, Category="Group")
    FString CanonicalForm;
    
    UPROPERTY(BlueprintReadWrite, Category="Group")
    float AverageSimilarity = 0.0f;
    
    UPROPERTY(BlueprintReadWrite, Category="Group")
    int32 Frequency = 0;
};

/**
 * Tokenizer configuration
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRETokenizerConfig
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    FString Delimiters = TEXT(" _-.,;:!?()[]{}");
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bPreserveDelimiters = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bDetectNamingConvention = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bGenerateVariants = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bNormalizeCase = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    int32 MinTokenLength = 1;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    int32 MaxTokenLength = 100;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bSplitNumbers = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bSplitCamelCase = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bMergeContractions = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bExpandAbbreviations = false;
};

/**
 * Vocabulary entry for domain-specific terms
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREVocabularyEntry
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Vocabulary")
    FString Term;
    
    UPROPERTY(BlueprintReadWrite, Category="Vocabulary")
    TArray<FString> Synonyms;
    
    UPROPERTY(BlueprintReadWrite, Category="Vocabulary")
    TArray<FString> Abbreviations;
    
    UPROPERTY(BlueprintReadWrite, Category="Vocabulary")
    FString Category;
    
    UPROPERTY(BlueprintReadWrite, Category="Vocabulary")
    float Weight = 1.0f;
    
    UPROPERTY(BlueprintReadWrite, Category="Vocabulary")
    TMap<FString, FString> Metadata;
};

// =========================================================================
// Cache Types
// =========================================================================

/**
 * Cache entry metadata
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRECacheEntry
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Cache")
    FString Key;
    
    UPROPERTY(BlueprintReadOnly, Category="Cache")
    int32 SizeBytes = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Cache")
    FDateTime LastAccessed;
    
    UPROPERTY(BlueprintReadOnly, Category="Cache")
    int32 AccessCount = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Cache")
    float Priority = 1.0f;
};

/**
 * Cache statistics
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRECacheStatistics
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int32 TotalEntries = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int64 TotalSizeBytes = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int32 HitCount = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int32 MissCount = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    float HitRate = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int32 EvictionCount = 0;
    
    float CalculateHitRate() const
    {
        int32 Total = HitCount + MissCount;
        return Total > 0 ? (float)HitCount / (float)Total : 0.0f;
    }
};

// =========================================================================
// Query Settings (additional for pipeline control)
// =========================================================================

/**
 * Query settings for fine-tuning reasoning operations
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREQuerySettings
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Settings")
    bool bEnableSemantic = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Settings")
    bool bEnableSymbolic = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Settings")
    bool bEnableCache = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Settings")
    bool bEnableEnrichment = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Settings")
    float SemanticWeight = 0.5f;
    
    UPROPERTY(BlueprintReadWrite, Category="Settings")
    float SymbolicWeight = 0.5f;
    
    UPROPERTY(BlueprintReadWrite, Category="Settings")
    FString PreferredStrategy;
};

// =========================================================================
// Symbolic Result (for Core layer use)
// =========================================================================

/**
 * Symbolic processing result
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRESymbolicResult
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    TArray<FName> Entities;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    TArray<FString> Patterns;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    float Confidence = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    bool bUsedFallback = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    FString Explanation;
};

// =========================================================================
// Compatibility Types for Tokenizer  
// =========================================================================

/**
 * Fuzzy token type (for backwards compatibility)
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREFuzzyToken
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Token")
    FString Text;
    
    UPROPERTY(BlueprintReadWrite, Category="Token")
    float Weight = 1.0f;
    
    // Conversion constructor from FREToken
    FREFuzzyToken() {}
    FREFuzzyToken(const FREToken& Token) : Text(Token.Text), Weight(Token.Weight) {}
};

/**
 * Token group type alias (for compatibility)
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FTokenGroup
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Group")
    FString Name;
    
    UPROPERTY(BlueprintReadWrite, Category="Group")
    TArray<FREFuzzyToken> Tokens;
    
    UPROPERTY(BlueprintReadWrite, Category="Group")
    float Similarity = 0.0f;
};

/**
 * Vocabulary collection wrapper
 * Wrapper needed because TMap cannot have TArray as value in UHT
 */
USTRUCT()
struct FREVocabularyCollection
{
    GENERATED_BODY()
    
    UPROPERTY()
    TArray<FREVocabularyEntry> Entries;
};

/**
 * Token stream type alias (for compatibility)
 */
using FTokenStream = FRETokenStream;

/**
 * Token type enum alias (for compatibility)
 */
using ETokenType = ERETokenType;

/**
 * Naming convention enum alias (for compatibility)  
 */
using ENamingConvention = ERENamingConvention;