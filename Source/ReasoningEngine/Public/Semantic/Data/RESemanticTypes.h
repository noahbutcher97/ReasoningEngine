#pragma once

#include "CoreMinimal.h"
#include "RESemanticTypes.generated.h"

/**
 * Unified semantic type definitions for ReasoningEngine.
 * Consolidates fuzzy matching types, similarity metrics, and vector representations.
 */

// =========================================================================
// Fuzzy Matching Types (formerly from REStringTypes.h)
// =========================================================================

/**
 * Fuzzy matching algorithm type
 */
UENUM(BlueprintType)
enum class EREFuzzyAlgorithm : uint8
{
    Levenshtein         UMETA(DisplayName = "Levenshtein"),
    DamerauLevenshtein  UMETA(DisplayName = "Damerau-Levenshtein"),
    OptimalAlignment    UMETA(DisplayName = "Optimal Alignment"),
    Hamming            UMETA(DisplayName = "Hamming"),
    Jaro               UMETA(DisplayName = "Jaro"),
    JaroWinkler        UMETA(DisplayName = "Jaro-Winkler"),
    LCS                UMETA(DisplayName = "Longest Common Subsequence"),
    LCSS               UMETA(DisplayName = "Longest Common Substring"),
    Jaccard            UMETA(DisplayName = "Jaccard"),
    Dice               UMETA(DisplayName = "Dice Coefficient"),
    Cosine             UMETA(DisplayName = "Cosine Similarity"),
    Soundex            UMETA(DisplayName = "Soundex"),
    Metaphone          UMETA(DisplayName = "Metaphone"),
    KeyboardDistance   UMETA(DisplayName = "Keyboard Distance"),
    Auto               UMETA(DisplayName = "Auto-Select Best")
};

/**
 * Character-level comparison result
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRECharacterMatch
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Character")
    FString CharacterA;
    
    UPROPERTY(BlueprintReadOnly, Category="Character")
    FString CharacterB;
    
    UPROPERTY(BlueprintReadOnly, Category="Similarity")
    float ExactSimilarity = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Similarity")
    float KeyboardSimilarity = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Similarity")
    float VisualSimilarity = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Similarity")
    float PhoneticSimilarity = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Similarity")
    float CategorySimilarity = 0.0f;
    
    void SetCharacters(TCHAR A, TCHAR B)
    {
        CharacterA = FString::Chr(A);
        CharacterB = FString::Chr(B);
    }
    
    float GetWeightedSimilarity() const
    {
        return (ExactSimilarity * 0.4f + 
                KeyboardSimilarity * 0.2f + 
                VisualSimilarity * 0.15f + 
                PhoneticSimilarity * 0.15f + 
                CategorySimilarity * 0.1f);
    }
};

/**
 * String match result - contains all similarity metrics
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREStringMatch
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Strings")
    FString StringA;
    
    UPROPERTY(BlueprintReadOnly, Category="Strings")
    FString StringB;
    
    UPROPERTY(BlueprintReadOnly, Category="Distance")
    int32 LevenshteinDistance = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Distance")
    float NormalizedLevenshtein = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Distance")
    int32 DamerauLevenshteinDistance = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Distance")
    int32 HammingDistance = -1;
    
    UPROPERTY(BlueprintReadOnly, Category="Distance")
    int32 OptimalAlignmentDistance = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Similarity")
    float JaroSimilarity = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Similarity")
    float JaroWinklerSimilarity = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Subsequence")
    int32 LongestCommonSubsequence = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Subsequence")
    int32 LongestCommonSubstring = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="NGram")
    float DiceCoefficient = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="NGram")
    float JaccardIndex = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="NGram")
    float CosineSimilarity = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Phonetic")
    bool bSoundexMatch = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Phonetic")
    bool bMetaphoneMatch = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Phonetic")
    FString SoundexA;
    
    UPROPERTY(BlueprintReadOnly, Category="Phonetic")
    FString SoundexB;
    
    UPROPERTY(BlueprintReadOnly, Category="Visual")
    float KeyboardDistance = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Visual")
    bool bVisuallyConfusable = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Performance")
    float ComputationTimeMS = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Performance")
    bool bWasCached = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Algorithm")
    float BestSimilarity = 0.0f;
    
    float GetBestSimilarity() const
    {
        float Best = NormalizedLevenshtein;
        Best = FMath::Max(Best, JaroWinklerSimilarity);
        Best = FMath::Max(Best, DiceCoefficient);
        Best = FMath::Max(Best, JaccardIndex);
        Best = FMath::Max(Best, CosineSimilarity);
        return Best;
    }
};

/**
 * N-gram set for similarity calculations
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRENGramSet
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="NGram")
    int32 N = 2;
    
    UPROPERTY(BlueprintReadOnly, Category="NGram")
    FString SourceString;
    
    UPROPERTY(BlueprintReadOnly, Category="NGram")
    TMap<FString, int32> Grams;
    
    UPROPERTY(BlueprintReadOnly, Category="NGram")
    int32 TotalGrams = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="NGram")
    bool bCaseSensitive = false;
    
    float DiceSimilarity(const FRENGramSet& Other) const;
    float CosineSimilarity(const FRENGramSet& Other) const;
    float JaccardSimilarity(const FRENGramSet& Other) const;
};

// =========================================================================
// Vector & Embedding Types (formerly from RESemanticTypes.h)
// =========================================================================

/**
 * Vector embedding type
 */
UENUM(BlueprintType)
enum class EREVectorType : uint8
{
    Hash            UMETA(DisplayName = "Hash-based"),
    TfIdf           UMETA(DisplayName = "TF-IDF"),
    Word2Vec        UMETA(DisplayName = "Word2Vec"),
    GloVe           UMETA(DisplayName = "GloVe"),
    BERT            UMETA(DisplayName = "BERT"),
    Custom          UMETA(DisplayName = "Custom"),
    LibTorch        UMETA(DisplayName = "LibTorch Model")
};

/**
 * Similarity metric type
 */
UENUM(BlueprintType)
enum class ERESimilarityMetric : uint8
{
    Cosine          UMETA(DisplayName = "Cosine"),
    Euclidean       UMETA(DisplayName = "Euclidean"),
    Manhattan       UMETA(DisplayName = "Manhattan"),
    DotProduct      UMETA(DisplayName = "Dot Product"),
    Jaccard         UMETA(DisplayName = "Jaccard"),
    Fuzzy           UMETA(DisplayName = "Fuzzy String"),
    Hybrid          UMETA(DisplayName = "Hybrid"),
    Auto            UMETA(DisplayName = "Auto-Select")
};

/**
 * Vector representation with metadata
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREVector
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Vector")
    TArray<float> Values;
    
    UPROPERTY(BlueprintReadWrite, Category="Vector")
    int32 Dimensions = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Vector")
    FString SourceText;
    
    UPROPERTY(BlueprintReadWrite, Category="Vector")
    EREVectorType VectorType = EREVectorType::Hash;
    
    UPROPERTY(BlueprintReadWrite, Category="Vector")
    bool bNormalized = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Vector")
    float Magnitude = 0.0f;
    
    UPROPERTY(BlueprintReadWrite, Category="Metadata")
    TMap<FString, FString> Metadata;
    
    // Methods
    float CalculateMagnitude() const
    {
        float Sum = 0.0f;
        for (float Val : Values)
        {
            Sum += Val * Val;
        }
        return FMath::Sqrt(Sum);
    }
    
    void Normalize()
    {
        float Mag = CalculateMagnitude();
        if (Mag > 0.0f)
        {
            for (float& Val : Values)
            {
                Val /= Mag;
            }
            bNormalized = true;
            Magnitude = 1.0f;
        }
    }
    
    float DotProduct(const FREVector& Other) const
    {
        if (Dimensions != Other.Dimensions)
            return 0.0f;
        
        float Result = 0.0f;
        for (int32 i = 0; i < Dimensions; i++)
        {
            Result += Values[i] * Other.Values[i];
        }
        return Result;
    }
    
    float CosineSimilarity(const FREVector& Other) const
    {
        if (Dimensions != Other.Dimensions)
            return 0.0f;
        
        float Dot = DotProduct(Other);
        float MagA = bNormalized ? 1.0f : CalculateMagnitude();
        float MagB = Other.bNormalized ? 1.0f : Other.CalculateMagnitude();
        
        if (MagA == 0.0f || MagB == 0.0f)
            return 0.0f;
        
        return Dot / (MagA * MagB);
    }
};

/**
 * Similarity comparison result
 * Unified result from semantic similarity operations
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRESimilarityResult
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Input")
    FString InputA;
    
    UPROPERTY(BlueprintReadOnly, Category="Input")
    FString InputB;
    
    UPROPERTY(BlueprintReadOnly, Category="Score")
    float OverallScore = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Score")
    float FuzzyScore = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Score")
    float VectorScore = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Score")
    float TokenScore = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Score")
    ERESimilarityMetric UsedMetric = ERESimilarityMetric::Auto;
    
    UPROPERTY(BlueprintReadOnly, Category="Details")
    bool bUsedVectors = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Details")
    bool bUsedTokens = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Details")
    bool bUsedFuzzy = true;
    
    UPROPERTY(BlueprintReadOnly, Category="Performance")
    float ComputationTimeMS = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Performance")
    bool bWasCached = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Metadata")
    TMap<FString, FString> Metadata;
};

/**
 * Vector collection with indexing
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREVectorCollection
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Collection")
    TArray<FREVector> Vectors;
    
    UPROPERTY(BlueprintReadWrite, Category="Collection")
    TMap<FString, int32> TextToIndex;
    
    UPROPERTY(BlueprintReadWrite, Category="Collection")
    int32 Dimensions = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Collection")
    EREVectorType VectorType = EREVectorType::Hash;
    
    UPROPERTY(BlueprintReadWrite, Category="Collection")
    bool bNormalized = false;
    
    void AddVector(const FString& Text, const FREVector& Vector)
    {
        int32 Index = Vectors.Add(Vector);
        TextToIndex.Add(Text, Index);
        
        if (Vectors.Num() == 1)
        {
            Dimensions = Vector.Dimensions;
            VectorType = Vector.VectorType;
        }
    }
    
    FREVector* GetVector(const FString& Text)
    {
        if (int32* Index = TextToIndex.Find(Text))
        {
            return Vectors.IsValidIndex(*Index) ? &Vectors[*Index] : nullptr;
        }
        return nullptr;
    }
    
    void Clear()
    {
        Vectors.Empty();
        TextToIndex.Empty();
        Dimensions = 0;
    }
    
    int32 Num() const { return Vectors.Num(); }
};

/**
 * Vectorizer configuration
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREVectorizerConfig
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    EREVectorType VectorType = EREVectorType::Hash;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    int32 Dimensions = 128;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bNormalizeVectors = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bUseCache = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bUseTfIdf = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    float HashSeed = 42.0f;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    FString ModelPath;
};

/**
 * Similarity configuration
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRESimilarityConfig
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    ERESimilarityMetric PreferredMetric = ERESimilarityMetric::Auto;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bUseFuzzyFallback = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bUseVectorsIfAvailable = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bUseTokensIfAvailable = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    float FuzzyWeight = 0.3f;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    float VectorWeight = 0.5f;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    float TokenWeight = 0.2f;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    float MinSimilarityThreshold = 0.0f;
};

// =========================================================================
// Fuzzy Result Types (additional for compatibility)
// =========================================================================

/**
 * Fuzzy matching result
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREFuzzyResult
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    float Score = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    EREFuzzyAlgorithm Algorithm = EREFuzzyAlgorithm::Auto;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    bool bUsedFallback = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    FString Explanation;
};

/**
 * Vector result
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREVectorResult
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    FREVector Vector;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    bool bSuccess = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    bool bUsedFallback = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Result")
    FString Explanation;
    
    static FREVectorResult MakeEmptyFallback()
    {
        FREVectorResult Result;
        Result.bSuccess = false;
        Result.bUsedFallback = true;
        Result.Explanation = TEXT("Vector generation failed - using empty fallback");
        return Result;
    }
};