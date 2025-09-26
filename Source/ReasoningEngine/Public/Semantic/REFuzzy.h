#pragma once

#include "CoreMinimal.h"
#include "Semantic/Data/RESemanticTypes.h"
#include "REFuzzy.generated.h"

/**
 * Layer 0 Fuzzy String Matcher
 * String-only algorithms, no tokens or vectors required
 * Always available as the base fallback
 */
UCLASS(BlueprintType)
class REASONINGENGINE_API UREFuzzy : public UObject
{
    GENERATED_BODY()
    
private:
    // Keyboard layout for typo distance
    static TMap<TCHAR, FVector2D> KeyboardLayout;
    static bool bKeyboardInitialized;
    
    // Phonetic mappings
    static TMap<TCHAR, TCHAR> SoundexMap;
    static bool bPhoneticInitialized;
    
    // Initialize static data
    static void InitializeKeyboardLayout();
    static void InitializePhoneticMaps();
    
    // Helper to normalize strings before comparison (uses URENormalizer)
    static FString PrepareString(const FString& Input, bool bNormalize);
    
public:
    // ========== LIFECYCLE ==========
    
    /**
     * Initialize fuzzy matcher
     */
    static void Initialize();
    
    /**
     * Check if fuzzy matcher is ready
     */
    static bool IsReady() { return true; } // Always ready - Layer 0
    
    // ========== MAIN API ==========
    
    /**
     * Compare two strings with all algorithms
     * @param A - First string
     * @param B - Second string
     * @param bNormalize - Apply normalization before comparison
     * @return Complete match result
     */
    UFUNCTION(BlueprintCallable, Category="RE|Semantic|Fuzzy",
              meta=(DisplayName="Compare Strings Full"))
    static FREStringMatch CompareStrings(const FString& A, const FString& B, 
                                        bool bNormalize = true);


	
	/**
	 * Compare two strings and select a single algorithm’s score.
	 * Still computes all metrics internally, but highlights the chosen algorithm’s similarity in BestSimilarity.
	 *
	 * @param A - First string
	 * @param B - Second string
	 * @param Algorithm - The fuzzy algorithm to emphasize
	 * @param bNormalize - If true, strings are normalized (lowercased, accents removed, whitespace trimmed)
	 * @return FREStringMatch with BestSimilarity set to the chosen algorithm’s score
	 */
	UFUNCTION(BlueprintCallable, Category="Reasoning|Fuzzy")
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
    UFUNCTION(BlueprintCallable, Category="RE|Semantic|Fuzzy",
              meta=(DisplayName="Get Similarity"))
    static float GetSimilarity(const FString& A, const FString& B,
                               EREFuzzyAlgorithm Algorithm = EREFuzzyAlgorithm::Auto,
                               bool bNormalize = true);
    
    /**
     * Get edit distance between strings
     * @param A - First string
     * @param B - Second string
     * @param Algorithm - Distance algorithm
     * @return Edit distance
     */
    UFUNCTION(BlueprintCallable, Category="RE|Semantic|Fuzzy",
              meta=(DisplayName="Get Edit Distance"))
    static int32 GetEditDistance(const FString& A, const FString& B,
                                 EREFuzzyAlgorithm Algorithm = EREFuzzyAlgorithm::Levenshtein);
    
    // ========== EDIT DISTANCE ALGORITHMS ==========
    
    /**
     * Calculate Levenshtein distance
     * @param A - First string
     * @param B - Second string
     * @return Minimum edit operations
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Distance",
              meta=(DisplayName="Levenshtein Distance"))
    static int32 CalculateLevenshtein(const FString& A, const FString& B);
    
    /**
     * Calculate Damerau-Levenshtein distance (includes transpositions)
     * @param A - First string
     * @param B - Second string
     * @return Edit distance with transpositions
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Distance",
              meta=(DisplayName="Damerau-Levenshtein Distance"))
    static int32 CalculateDamerauLevenshtein(const FString& A, const FString& B);
    
    /**
     * Calculate Optimal Alignment distance
     * @param A - First string
     * @param B - Second string
     * @return Optimal alignment distance
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Distance",
              meta=(DisplayName="Optimal Alignment Distance"))
    static int32 CalculateOptimalAlignment(const FString& A, const FString& B);
    
    /**
     * Calculate Hamming distance (same length strings only)
     * @param A - First string
     * @param B - Second string
     * @return Hamming distance or -1 if different lengths
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Distance",
              meta=(DisplayName="Hamming Distance"))
    static int32 CalculateHamming(const FString& A, const FString& B);
    
    // ========== SIMILARITY ALGORITHMS ==========
    
    /**
     * Calculate Jaro similarity
     * @param A - First string
     * @param B - Second string
     * @return Similarity score (0-1)
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Similarity",
              meta=(DisplayName="Jaro Similarity"))
    static float CalculateJaro(const FString& A, const FString& B);
    
    /**
     * Calculate Jaro-Winkler similarity
     * @param A - First string
     * @param B - Second string
     * @param PrefixScale - Weight for common prefix (default 0.1)
     * @return Similarity score (0-1)
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Similarity",
              meta=(DisplayName="Jaro-Winkler Similarity"))
    static float CalculateJaroWinkler(const FString& A, const FString& B, 
                                      float PrefixScale = 0.1f);
    
    // ========== SUBSEQUENCE ALGORITHMS ==========
    
    /**
     * Calculate longest common subsequence
     * @param A - First string
     * @param B - Second string
     * @return Length of LCS
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Subsequence",
              meta=(DisplayName="Longest Common Subsequence"))
    static int32 CalculateLCS(const FString& A, const FString& B);
    
    /**
     * Calculate longest common substring
     * @param A - First string
     * @param B - Second string
     * @return Length of LCSS
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Subsequence",
              meta=(DisplayName="Longest Common Substring"))
    static int32 CalculateLCSS(const FString& A, const FString& B);
    
    // ========== N-GRAM ALGORITHMS ==========
    
    /**
     * Generate N-grams from string
     * @param Source - Source string
     * @param N - Gram size (default 2)
     * @return N-gram set
     */
    UFUNCTION(BlueprintCallable, Category="RE|Semantic|Fuzzy|NGram",
              meta=(DisplayName="Generate N-Grams"))
    static FRENGramSet GenerateNGrams(const FString& Source, int32 N = 2);
    
    /**
     * Calculate Dice coefficient
     * @param A - First string
     * @param B - Second string
     * @param N - Gram size
     * @return Dice coefficient (0-1)
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|NGram",
              meta=(DisplayName="Dice Coefficient"))
    static float CalculateDice(const FString& A, const FString& B, int32 N = 2);
    
    /**
     * Calculate Jaccard index
     * @param A - First string
     * @param B - Second string
     * @param N - Gram size
     * @return Jaccard index (0-1)
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|NGram",
              meta=(DisplayName="Jaccard Index"))
    static float CalculateJaccard(const FString& A, const FString& B, int32 N = 2);
    
    /**
     * Calculate Cosine similarity
     * @param A - First string
     * @param B - Second string
     * @param N - Gram size
     * @return Cosine similarity (0-1)
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|NGram",
              meta=(DisplayName="Cosine Similarity"))
    static float CalculateCosine(const FString& A, const FString& B, int32 N = 2);
    
    // ========== PHONETIC ALGORITHMS ==========
    
    /**
     * Generate Soundex code
     * @param Input - Input string
     * @return 4-character Soundex code
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Phonetic",
              meta=(DisplayName="Generate Soundex"))
    static FString GenerateSoundex(const FString& Input);
    
    /**
     * Generate Metaphone encoding
     * @param Input - Input string
     * @param bDouble - Use double metaphone
     * @return Metaphone encoding(s)
     */
    UFUNCTION(BlueprintCallable, Category="RE|Semantic|Fuzzy|Phonetic",
              meta=(DisplayName="Generate Metaphone"))
    static TArray<FString> GenerateMetaphone(const FString& Input, bool bDouble = true);
    
    /**
     * Check phonetic equality
     * @param A - First string
     * @param B - Second string
     * @return true if phonetically similar
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Phonetic",
              meta=(DisplayName="Are Phonetically Equal"))
    static bool ArePhoneticallyEqual(const FString& A, const FString& B);
    
    // ========== TYPO/VISUAL ALGORITHMS ==========
    
    /**
     * Calculate keyboard distance (typo probability)
     * @param A - First string
     * @param B - Second string
     * @return Normalized keyboard distance (0-1)
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Visual",
              meta=(DisplayName="Keyboard Distance"))
    static float CalculateKeyboardDistance(const FString& A, const FString& B);
    
    /**
     * Check if strings are visual confusables (l/1, O/0)
     * @param A - First string
     * @param B - Second string
     * @return true if visually similar
     */
    UFUNCTION(BlueprintPure, Category="RE|Semantic|Fuzzy|Visual",
              meta=(DisplayName="Are Visual Confusables"))
    static bool AreVisualConfusables(const FString& A, const FString& B);
    
    // ========== BATCH OPERATIONS ==========
    
    /**
     * Find best matches from candidates
     * @param Query - Query string
     * @param Candidates - List of candidates
     * @param MaxResults - Max results to return
     * @param MinSimilarity - Minimum similarity threshold
     * @param Algorithm - Algorithm to use
     * @return Best matching strings
     */
    UFUNCTION(BlueprintCallable, Category="RE|Semantic|Fuzzy",
              meta=(DisplayName="Find Best Matches"))
    static TArray<FString> FindBestMatches(const FString& Query,
                                           const TArray<FString>& Candidates,
                                           int32 MaxResults = 5,
                                           float MinSimilarity = 0.5f,
                                           EREFuzzyAlgorithm Algorithm = EREFuzzyAlgorithm::Auto);
};