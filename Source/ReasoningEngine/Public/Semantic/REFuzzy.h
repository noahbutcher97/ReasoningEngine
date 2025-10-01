// Source/ReasoningEngine/Public/Semantic/REFuzzy.h
#pragma once

#include "CoreMinimal.h"
#include "Semantic/Data/RESemanticTypes.h"

/**
 * Static utility class for fuzzy string matching algorithms
 * Replaces UREFuzzy with pure static methods (no UObject overhead)
 * 
 * Layer 0 - Always available as base fallback
 * No state management, pure algorithmic functions
 * Thread-safe by design
 * 
 * REFACTOR NOTES:
 * - All methods converted from UREFuzzy instance methods to static
 * - Removed UCLASS, UFUNCTION, GENERATED_BODY macros
 * - Preserved ALL algorithms from original implementation
 * - Blueprint access moved to separate UREFuzzyBlueprintLibrary
 */
class REASONINGENGINE_API REFuzzy
{
public:
    // ========== MAIN API ==========
    
    /**
     * Compare two strings with all algorithms
     * @param A - First string
     * @param B - Second string
     * @param bNormalize - Apply normalization before comparison
     * @return Complete match result with all metrics
     */
    static FREStringMatch CompareStrings(
        const FString& A, 
        const FString& B,
        bool bNormalize = true
    );
    
    /**
     * Compare two strings emphasizing a specific algorithm
     * @param A - First string
     * @param B - Second string
     * @param Algorithm - The fuzzy algorithm to emphasize
     * @param bNormalize - Apply normalization
     * @return Match result with BestSimilarity set to chosen algorithm's score
     */
    static FREStringMatch CompareStringsWithAlgo(
        const FString& A,
        const FString& B,
        EREFuzzyAlgorithm Algorithm,
        bool bNormalize = true
    );
    
    /**
     * Fast similarity score using best algorithm
     * @param A - First string
     * @param B - Second string
     * @param Algorithm - Algorithm to use (Auto selects best)
     * @param bNormalize - Apply normalization
     * @return Similarity score (0-1)
     */
    static float GetSimilarity(
        const FString& A,
        const FString& B,
        EREFuzzyAlgorithm Algorithm = EREFuzzyAlgorithm::Auto,
        bool bNormalize = true
    );
    
    /**
     * Get edit distance between strings
     * @param A - First string
     * @param B - Second string
     * @param Algorithm - Distance algorithm
     * @return Edit distance
     */
    static int32 GetEditDistance(
        const FString& A,
        const FString& B,
        EREFuzzyAlgorithm Algorithm = EREFuzzyAlgorithm::Levenshtein
    );
    
    // ========== EDIT DISTANCE ALGORITHMS ==========
    
    /**
     * Calculate Levenshtein distance
     * Insertions, deletions, substitutions
     * Complexity: O(m*n) time, O(min(m,n)) space optimized
     */
    static int32 CalculateLevenshtein(const FString& A, const FString& B);
    
    /**
     * Calculate Damerau-Levenshtein distance
     * Includes transpositions
     * Complexity: O(m*n) time, O(m*n) space
     */
    static int32 CalculateDamerauLevenshtein(const FString& A, const FString& B);
    
    /**
     * Calculate Optimal Alignment distance
     * Restricted transpositions
     * Complexity: O(m*n) time, O(m*n) space
     */
    static int32 CalculateOptimalAlignment(const FString& A, const FString& B);
    
    /**
     * Calculate Hamming distance
     * Substitutions only, requires equal length
     * Complexity: O(n) time, O(1) space
     * @return Distance or -1 if different lengths
     */
    static int32 CalculateHamming(const FString& A, const FString& B);
    
    // ========== SIMILARITY ALGORITHMS ==========
    
    /**
     * Calculate Jaro similarity
     * Based on matching characters and transpositions
     * Complexity: O(m*n) time, O(m+n) space
     */
    static float CalculateJaro(const FString& A, const FString& B);
    
    /**
     * Calculate Jaro-Winkler similarity
     * Boosts strings with common prefix
     * Complexity: O(m*n) time, O(m+n) space
     * @param PrefixScale - Weight for common prefix (default 0.1)
     */
    static float CalculateJaroWinkler(
        const FString& A,
        const FString& B,
        float PrefixScale = 0.1f
    );
    
    // ========== SUBSEQUENCE ALGORITHMS ==========
    
    /**
     * Calculate Longest Common Subsequence length
     * Complexity: O(m*n) time, O(m*n) space
     */
    static int32 CalculateLCS(const FString& A, const FString& B);
    
    /**
     * Calculate Longest Common Substring length
     * Complexity: O(m*n) time, O(m*n) space
     */
    static int32 CalculateLCSS(const FString& A, const FString& B);
    
    // ========== N-GRAM ALGORITHMS ==========
    
    /**
     * Generate N-grams from string
     * @param Source - Input string
     * @param N - Size of grams (default 2 for bigrams)
     * @return Set of N-grams with frequencies
     */
    static FRENGramSet GenerateNGrams(const FString& Source, int32 N = 2);
    
    /**
     * Calculate Dice coefficient
     * Based on N-gram overlap
     * @param N - Gram size (default 2)
     * @return Dice coefficient (0-1)
     */
    static float CalculateDice(const FString& A, const FString& B, int32 N = 2);
    
    /**
     * Calculate Jaccard index
     * Based on N-gram set intersection/union
     * @param N - Gram size (default 2)
     * @return Jaccard index (0-1)
     */
    static float CalculateJaccard(const FString& A, const FString& B, int32 N = 2);
    
    /**
     * Calculate Cosine similarity
     * Based on N-gram vectors
     * @param N - Gram size (default 2)
     * @return Cosine similarity (0-1)
     */
    static float CalculateCosine(const FString& A, const FString& B, int32 N = 2);
    
    // ========== PHONETIC ALGORITHMS ==========
    
    /**
     * Generate Soundex code
     * Standard phonetic algorithm
     * @return 4-character Soundex code
     */
    static FString GenerateSoundex(const FString& Input);
    
    /**
     * Generate Metaphone encoding
     * Advanced phonetic algorithm
     * @param bDouble - Use double metaphone for alternate pronunciations
     * @return Metaphone encoding(s)
     */
    static TArray<FString> GenerateMetaphone(const FString& Input, bool bDouble = true);
    
    /**
     * Check phonetic equality
     * Uses both Soundex and Metaphone
     * @return true if phonetically similar
     */
    static bool ArePhoneticallyEqual(const FString& A, const FString& B);
    
    // ========== TYPO/VISUAL ALGORITHMS ==========
    
    /**
     * Calculate keyboard distance
     * For typo probability assessment
     * @return Normalized keyboard distance (0-1, higher = more similar)
     */
    static float CalculateKeyboardDistance(const FString& A, const FString& B);
    
    /**
     * Check if strings are visual confusables
     * Detects similar-looking characters (l/1, O/0, etc.)
     * @return true if visually similar
     */
    static bool AreVisualConfusables(const FString& A, const FString& B);
    
    // ========== BATCH OPERATIONS ==========
    
    /**
     * Find best matches from candidates
     * @param Query - Query string
     * @param Candidates - List of candidates to search
     * @param MaxResults - Maximum results to return
     * @param MinSimilarity - Minimum similarity threshold
     * @param Algorithm - Algorithm to use for matching
     * @return Best matching strings sorted by score
     */
    static TArray<FString> FindBestMatches(
        const FString& Query,
        const TArray<FString>& Candidates,
        int32 MaxResults = 5,
        float MinSimilarity = 0.5f,
        EREFuzzyAlgorithm Algorithm = EREFuzzyAlgorithm::Auto
    );
    
    /**
     * Batch compare a query against multiple candidates
     * @param Query - Query string
     * @param Candidates - List of candidates
     * @param Algorithm - Algorithm to use
     * @param bNormalize - Apply normalization
     * @return Array of match results
     */
    static TArray<FREStringMatch> BatchCompare(
        const FString& Query,
        const TArray<FString>& Candidates,
        EREFuzzyAlgorithm Algorithm = EREFuzzyAlgorithm::Auto,
        bool bNormalize = true
    );
    
    // ========== INITIALIZATION ==========
    
    /**
     * Initialize static data (keyboard layout, phonetic maps)
     * Called automatically on first use
     */
    static void Initialize();
    
    /**
     * Check if fuzzy matcher is ready
     * @return Always true - Layer 0 always available
     */
    static bool IsReady() { return true; }
    
    // ========== UTILITY FUNCTIONS ==========
    
    /**
     * Select best algorithm for given string pair
     * Based on string length and characteristics
     */
    static EREFuzzyAlgorithm SelectBestAlgorithm(const FString& A, const FString& B);
    
    /**
     * Normalize string for comparison
     * Delegates to RENormalizer for consistency
     */
    static FString PrepareString(const FString& Input, bool bNormalize);
    
private:
    // ========== STATIC DATA ==========
    
    /** Keyboard layout for typo distance calculations */
    static TMap<TCHAR, FVector2D> KeyboardLayout;
    static bool bKeyboardInitialized;
    
    /** Phonetic mappings for Soundex */
    static TMap<TCHAR, TCHAR> SoundexMap;
    static bool bPhoneticInitialized;
    
    /** Visual confusables mapping */
    static TMap<TCHAR, TArray<TCHAR>> VisualConfusables;
    static bool bVisualConfusablesInitialized;
    
    // ========== HELPER METHODS ==========
    
    /** Initialize keyboard layout (QWERTY) */
    static void InitializeKeyboardLayout();
    
    /** Initialize phonetic mappings */
    static void InitializePhoneticMaps();
    
    /** Initialize visual confusables */
    static void InitializeVisualConfusables();
    
    /** Three-way minimum for dynamic programming */
    static FORCEINLINE int32 Min3(int32 A, int32 B, int32 C)
    {
        return FMath::Min(A, FMath::Min(B, C));
    }
    
    /** Four-way minimum for dynamic programming */
    static FORCEINLINE int32 Min4(int32 A, int32 B, int32 C, int32 D)
    {
        return FMath::Min(FMath::Min(A, B), FMath::Min(C, D));
    }
    
    /** Get common prefix length */
    static int32 GetCommonPrefixLength(
        const FString& A,
        const FString& B,
        int32 MaxLength = 4
    );
    
    /** Check if character is vowel */
    static FORCEINLINE bool IsVowel(TCHAR Ch)
    {
        Ch = FChar::ToUpper(Ch);
        return Ch == 'A' || Ch == 'E' || Ch == 'I' || Ch == 'O' || Ch == 'U' || Ch == 'Y';
    }
    
    // ========== DELETED CONSTRUCTORS ==========
    
    REFuzzy() = delete;
    ~REFuzzy() = delete;
    REFuzzy(const REFuzzy&) = delete;
    REFuzzy& operator=(const REFuzzy&) = delete;
    REFuzzy(REFuzzy&&) = delete;
    REFuzzy& operator=(REFuzzy&&) = delete;
};