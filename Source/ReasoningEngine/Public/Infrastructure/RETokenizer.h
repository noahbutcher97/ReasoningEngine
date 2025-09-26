#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/REInfrastructureTypes.h"
#include "RETokenizer.generated.h"

// Forward declarations
class UREFuzzy;
class URECache;



/**
 * Advanced text tokenization component
 * Handles multiple naming conventions, variant generation, and vocabulary management
 */
UCLASS(BlueprintType)
class REASONINGENGINE_API URETokenizer : public UObject
{
    GENERATED_BODY()
    
private:
    // ========== DEPENDENCIES ==========
    
    UPROPERTY()
    UREFuzzy* FuzzyMatcher;
    
    UPROPERTY()
    URECache* CacheManager;
    
    // ========== CONFIGURATION ==========
    
    UPROPERTY()
    FRETokenizerConfig DefaultConfig;
    
    // ========== VOCABULARIES ==========
    
    /** Domain-specific vocabularies by category */
	/** Domain-specific vocabularies by category */
	UPROPERTY()
	TMap<FString, FREVocabularyCollection> Vocabularies;  // Changed from TArray to wrapper
    
    /** Canonical form mappings */
    TMap<FString, FString> CanonicalForms;
    
    /** Stop words to filter */
    TSet<FString> StopWords;
    
    // ========== CACHING ==========
    
    /** Token stream cache */
    mutable TMap<uint32, FTokenStream> TokenStreamCache;
    mutable FCriticalSection TokenStreamCacheMutex;
    
    /** Variant cache */
    mutable TMap<FString, TArray<FString>> VariantCache;
    mutable FCriticalSection VariantCacheMutex;
    
    // ========== STATISTICS ==========
    
    mutable FThreadSafeCounter TotalTokenizations;
    mutable FThreadSafeCounter TotalVariantsGenerated;
    
public:
    // ========== LIFECYCLE ==========
    
    void Initialize();
    void Shutdown();
    bool IsOperational() const { return true; }
    
    // ========== DEPENDENCIES ==========
    
    void SetFuzzyMatcher(UREFuzzy* InFuzzyMatcher);
    void SetCacheManager(URECache* InCacheManager);
    void ApplyConfiguration(class UTokenizerConfig* Config);
    
    // ========== TOKENIZATION ==========
    
    /**
     * Tokenize text with default configuration
     * @param Text - Input text to tokenize
     * @return Token stream
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer",
              meta=(DisplayName="Tokenize"))
    FRETokenStream Tokenize(const FString& Text);
    
    /**
     * Tokenize with custom configuration
     * @param Text - Input text
     * @param Config - Custom tokenization settings
     * @return Token stream
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer",
              meta=(DisplayName="Tokenize With Config"))
    FRETokenStream TokenizeWithConfig(const FString& Text, const FRETokenizerConfig& Config);
    
    /**
     * Tokenize animation name intelligently
     * @param AnimationName - Animation name (e.g., "MM_Walk_Forward_01")
     * @return Token stream with proper segmentation
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer",
              meta=(DisplayName="Tokenize Animation Name"))
    FRETokenStream TokenizeAnimationName(const FString& AnimationName);
    
    /**
     * Split text by delimiters
     * @param Text - Input text
     * @param Delimiters - Delimiter characters
     * @param bKeepDelimiters - Include delimiters as tokens
     * @return Array of token strings
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer",
              meta=(DisplayName="Split By Delimiters"))
    TArray<FString> SplitByDelimiters(const FString& Text, 
                                      const FString& Delimiters,
                                      bool bKeepDelimiters = false);
    
    // ========== NAMING CONVENTION ==========
    
    /**
     * Detect naming convention of text
     * @param Text - Input text
     * @return Detected naming convention
     */
    UFUNCTION(BlueprintPure, Category="MM|Tokenizer",
              meta=(DisplayName="Detect Naming Convention"))
    ENamingConvention DetectNamingConvention(const FString& Text) const;
    
    /**
     * Split camelCase/PascalCase text
     * @param Text - CamelCase text
     * @return Array of components
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer",
              meta=(DisplayName="Split Camel Case"))
    TArray<FString> SplitCamelCase(const FString& Text) const;
    
    /**
     * Convert between naming conventions
     * @param Text - Input text
     * @param FromConvention - Source convention
     * @param ToConvention - Target convention
     * @return Converted text
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer",
              meta=(DisplayName="Convert Naming Convention"))
    FString ConvertNamingConvention(const FString& Text,
                                    ENamingConvention FromConvention,
                                    ENamingConvention ToConvention);
    
    // ========== VARIANT GENERATION ==========
    
    /**
     * Generate variants of a token
     * @param Token - Original token
     * @param bIncludeTypos - Include common typos
     * @param bIncludeAbbreviations - Include abbreviations
     * @param bIncludeExpansions - Include expanded forms
     * @return Array of variants
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|Variants",
              meta=(DisplayName="Generate Variants"))
    TArray<FString> GenerateVariants(const FString& Token,
                                     bool bIncludeTypos = true,
                                     bool bIncludeAbbreviations = true,
                                     bool bIncludeExpansions = true);
    
    /**
     * Generate common typos
     * @param Word - Original word
     * @param MaxDistance - Maximum edit distance
     * @return Array of typo variants
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|Variants",
              meta=(DisplayName="Generate Typos"))
    TArray<FString> GenerateTypos(const FString& Word, int32 MaxDistance = 1);
    
    /**
     * Generate abbreviations
     * @param Word - Original word
     * @return Array of abbreviations
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|Variants",
              meta=(DisplayName="Generate Abbreviations"))
    TArray<FString> GenerateAbbreviations(const FString& Word);
    
    // ========== VOCABULARY MANAGEMENT ==========
    
    /**
     * Add vocabulary entry
     * @param Category - Vocabulary category
     * @param Entry - Vocabulary entry to add
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|Vocabulary",
              meta=(DisplayName="Add Vocabulary"))
    void AddVocabulary(const FString& Category, const FREVocabularyEntry& Entry);
    
    /**
     * Load vocabulary from asset
     * @param Category - Category name
     * @param VocabularyAsset - Vocabulary data asset
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|Vocabulary",
              meta=(DisplayName="Load Vocabulary Asset"))
    void LoadVocabularyAsset(const FString& Category, class UDataAsset* VocabularyAsset);
    
    /**
     * Check if word is in vocabulary
     * @param Word - Word to check
     * @param OutCategory - Category containing the word
     * @return true if word exists in vocabulary
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|Vocabulary",
              meta=(DisplayName="Is In Vocabulary"))
    bool IsInVocabulary(const FString& Word, FString& OutCategory) const;
    
    /**
     * Get canonical form of word
     * @param Word - Word to normalize
     * @return Canonical form
     */
    UFUNCTION(BlueprintPure, Category="MM|Tokenizer|Vocabulary",
              meta=(DisplayName="Get Canonical Form"))
    FString GetCanonicalForm(const FString& Word) const;
    
    /**
     * Get synonyms for word
     * @param Word - Word to look up
     * @return Array of synonyms
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|Vocabulary",
              meta=(DisplayName="Get Synonyms"))
    TArray<FString> GetSynonyms(const FString& Word) const;
    
    // ========== ANALYSIS ==========
    
    /**
     * Group tokens by similarity
     * @param Tokens - Tokens to group
     * @param SimilarityThreshold - Minimum similarity for grouping
     * @return Array of token groups
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|Analysis",
              meta=(DisplayName="Group Tokens"))
    TArray<FTokenGroup> GroupTokensBySimilarity(const TArray<FREFuzzyToken>& Tokens,
                                                  float SimilarityThreshold = 0.7f);
    
    /**
     * Find compound words in token stream
     * @param Stream - Token stream to analyze
     * @return Array of compound words found
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|Analysis",
              meta=(DisplayName="Find Compound Words"))
    TArray<FString> FindCompoundWords(const FRETokenStream& Stream) const;
    
    /**
     * Calculate token importance weights
     * @param Stream - Token stream
     * @return Updated stream with weights
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|Analysis",
              meta=(DisplayName="Calculate Token Weights"))
    FRETokenStream CalculateTokenWeights(const FRETokenStream& Stream);
    
    // ========== STOP WORDS ==========
    
    /**
     * Add stop words to filter
     * @param Words - Words to add as stop words
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|StopWords",
              meta=(DisplayName="Add Stop Words"))
    void AddStopWords(const TArray<FString>& Words);
    
    /**
     * Check if word is a stop word
     * @param Word - Word to check
     * @return true if stop word
     */
    UFUNCTION(BlueprintPure, Category="MM|Tokenizer|StopWords",
              meta=(DisplayName="Is Stop Word"))
    bool IsStopWord(const FString& Word) const;
    
    /**
     * Filter stop words from tokens
     * @param Tokens - Tokens to filter
     * @return Filtered tokens
     */
    UFUNCTION(BlueprintCallable, Category="MM|Tokenizer|StopWords",
              meta=(DisplayName="Filter Stop Words"))
    TArray<FREFuzzyToken> FilterStopWords(const TArray<FREFuzzyToken>& Tokens) const;
    
    // ========== UTILITIES ==========
    
    /**
     * Clear all caches
     */
    void ClearCache();
    
    /**
     * Get memory usage
     * @return Memory in bytes
     */
    int64 GetMemoryUsage() const;
    
    /**
     * Initialize default vocabularies
     */
    void InitializeDefaultVocabularies();
};
