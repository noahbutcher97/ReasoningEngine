#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Symbolic/Data/RESymbolicTypes.h"  // UPDATED: Was "Data/REPatternTypes.h"
#include "REPatterns.generated.h"

// Forward declarations
class URETokenizer;
class URECache;

/**
 * Advanced pattern matching engine
 * Supports multiple pattern types, state machines, and semantic matching
 */
UCLASS(BlueprintType)
class REASONINGENGINE_API UREPatterns : public UObject
{
    GENERATED_BODY()
    
private:
    // ========== DEPENDENCIES ==========
    
    UPROPERTY()
    URETokenizer* Tokenizer;
    
    UPROPERTY()
    URECache* CacheManager;
    
    // ========== PATTERN STORAGE ==========
    
    /** Registered pattern templates */
    UPROPERTY()
    TMap<FName, FREPatternTemplate> PatternTemplates;
    
    /** Pattern state machines for complex patterns */
    UPROPERTY()
    TMap<FName, FREPatternStateMachine> StateMachines;
    
    /** Regular expression patterns */
    TMap<FName, FString> RegexPatterns;
    
    /** Wildcard patterns */
    TMap<FName, FString> WildcardPatterns;
    
    // ========== CACHING ==========
    
    /** Pattern match cache */
    mutable TMap<uint32, TArray<FREPatternMatch>> MatchCache;
    mutable FCriticalSection MatchCacheMutex;
    
    // ========== STATISTICS ==========
    
    mutable FThreadSafeCounter TotalMatches;
    mutable FThreadSafeCounter SuccessfulMatches;
    
    // ========== HELPERS ==========
    
    /** Generate cache key */
    static uint32 GetCacheKey(const FString& Text, FName PatternID);
    
    /** Execute state machine */
    FREPatternMatch ExecuteStateMachine(const FREPatternStateMachine& Machine,
                                        const FTokenStream& Tokens) const;
    
    /** Match with regex */
    FREPatternMatch MatchRegex(const FString& Pattern,
                               const FString& Text) const;
    
    /** Match with wildcards */
    FREPatternMatch MatchWildcard(const FString& Pattern,
                                  const FString& Text) const;
    
public:
    // ========== LIFECYCLE ==========
    
    void Initialize();
    void Shutdown();
    bool IsOperational() const { return true; }
    
    // ========== DEPENDENCIES ==========
    
    void SetTokenizer(URETokenizer* InTokenizer);
    void SetCacheManager(URECache* InCacheManager);
    
    // ========== PATTERN REGISTRATION ==========
    
    /**
     * Register a pattern template
     * @param PatternID - Unique pattern identifier
     * @param Template - Pattern template
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Registration",
              meta=(DisplayName="Register Pattern"))
    void RegisterPattern(FName PatternID, const FREPatternTemplate& Template);
    
    /**
     * Register a state machine pattern
     * @param PatternID - Unique pattern identifier
     * @param StateMachine - State machine definition
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Registration",
              meta=(DisplayName="Register State Machine"))
    void RegisterStateMachine(FName PatternID, const FREPatternStateMachine& StateMachine);
    
    /**
     * Register a regex pattern
     * @param PatternID - Unique pattern identifier
     * @param RegexPattern - Regular expression
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Registration",
              meta=(DisplayName="Register Regex"))
    void RegisterRegex(FName PatternID, const FString& RegexPattern);
    
    /**
     * Register a wildcard pattern
     * @param PatternID - Unique pattern identifier
     * @param WildcardPattern - Wildcard pattern (* and ?)
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Registration",
              meta=(DisplayName="Register Wildcard"))
    void RegisterWildcard(FName PatternID, const FString& WildcardPattern);
    
    /**
     * Unregister a pattern
     * @param PatternID - Pattern to remove
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Registration",
              meta=(DisplayName="Unregister Pattern"))
    void UnregisterPattern(FName PatternID);
    
    /**
     * Check if pattern exists
     * @param PatternID - Pattern to check
     * @return true if pattern is registered
     */
    UFUNCTION(BlueprintPure, Category="MM|Pattern|Registration",
              meta=(DisplayName="Has Pattern"))
    bool HasPattern(FName PatternID) const;
    
    // ========== PATTERN MATCHING ==========
    
    /**
     * Match text against a specific pattern
     * @param Text - Text to match
     * @param PatternID - Pattern to use
     * @param Mode - Match mode
     * @return Pattern match result
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Matching",
              meta=(DisplayName="Match Pattern"))
    FREPatternMatch MatchPattern(const FString& Text,
                                 FName PatternID,
                                 EREPatternMatchMode Mode = EREPatternMatchMode::Fuzzy);
    
	/**
	 * Find all patterns in text
	 * @param Text - Text to search
	 * @param PatternIDs - Specific patterns to search for (empty = all)
	 * @return Array of pattern matches
	 */
	UFUNCTION(BlueprintCallable, Category="MM|Pattern|Matching",
			  meta=(DisplayName="Find Patterns"))
	TArray<FREPatternMatch> FindPatterns(const FString& Text,
										 const TArray<FName>& PatternIDs);  // Removed default parameter
	/**
 * Find all patterns in text (with defaults)
 * C++ only overload with default parameter
 */
	TArray<FREPatternMatch> FindPatterns(const FString& Text)
	{
		return FindPatterns(Text, TArray<FName>());
	}
    
    
    /**
     * Find best matching pattern
     * @param Text - Text to match
     * @param MinConfidence - Minimum confidence threshold
     * @return Best matching pattern
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Matching",
              meta=(DisplayName="Find Best Pattern"))
    FREPatternMatch FindBestPattern(const FString& Text,
                                    float MinConfidence = 0.5f);
    
    /**
     * Match token stream against patterns
     * @param TokenStream - Pre-tokenized stream
     * @param PatternID - Pattern to match
     * @return Pattern match result
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Matching",
              meta=(DisplayName="Match Token Stream"))
    FREPatternMatch MatchTokenStream(const FRETokenStream& TokenStream,
                                     FName PatternID);
    
    // ========== CAPTURE GROUPS ==========
    
    /**
     * Extract captured values from match
     * @param Match - Pattern match result
     * @param GroupName - Capture group name
     * @return Captured value or empty
     */
    UFUNCTION(BlueprintPure, Category="MM|Pattern|Capture",
              meta=(DisplayName="Get Captured Value"))
    FString GetCapturedValue(const FREPatternMatch& Match,
                             const FString& GroupName) const;
    
    /**
     * Extract all captured values
     * @param Match - Pattern match result
     * @return Map of capture group values
     */
    UFUNCTION(BlueprintPure, Category="MM|Pattern|Capture",
              meta=(DisplayName="Get All Captures"))
    TMap<FString, FString> GetAllCaptures(const FREPatternMatch& Match) const;
    
    // ========== PATTERN BUILDING ==========
    
    /**
     * Build pattern from examples
     * @param Examples - Example strings that should match
     * @param CounterExamples - Strings that should not match
     * @return Generated pattern template
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Building",
              meta=(DisplayName="Build From Examples"))
    FREPatternTemplate BuildPatternFromExamples(const TArray<FString>& Examples,
                                                const TArray<FString>& CounterExamples);
    
    /**
     * Validate pattern template
     * @param Template - Template to validate
     * @param OutErrors - Validation errors
     * @return true if valid
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Building",
              meta=(DisplayName="Validate Pattern"))
    bool ValidatePattern(const FREPatternTemplate& Template,
                        TArray<FString>& OutErrors);
    
    // ========== UTILITIES ==========
    
    /**
     * Clear pattern cache
     */
    void ClearCache();
    
    /**
     * Get memory usage
     * @return Memory in bytes
     */
    int64 GetMemoryUsage() const;
    
    /**
     * Get pattern statistics
     * @param OutTotalMatches - Total match attempts
     * @param OutSuccessful - Successful matches
     * @param OutSuccessRate - Success rate (0-1)
     */
    UFUNCTION(BlueprintCallable, Category="MM|Pattern|Stats",
              meta=(DisplayName="Get Pattern Stats"))
    void GetPatternStats(int32& OutTotalMatches,
                        int32& OutSuccessful,
                        float& OutSuccessRate) const;
    
    /**
     * Initialize default patterns for common use cases
     */
    void InitializeDefaultPatterns();
};
