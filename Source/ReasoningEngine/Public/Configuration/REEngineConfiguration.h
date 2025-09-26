#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Semantic/Data/RESemanticTypes.h"
#include "REEngineConfiguration.generated.h"

/**
 * Fuzzy matcher configuration
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FFuzzyMatcherConfig
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Algorithms")
    bool bEnableLevenshtein = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Algorithms")
    bool bEnableDamerauLevenshtein = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Algorithms")
    bool bEnableJaroWinkler = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Algorithms")
    bool bEnableNGram = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Algorithms")
    bool bEnablePhonetic = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Algorithms", meta=(ClampMin=0.0, ClampMax=1.0))
    float JaroWinklerPrefixWeight = 0.1f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Algorithms", meta=(ClampMin=2, ClampMax=5))
    int32 DefaultNGramSize = 3;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Performance")
    bool bUseMultithreading = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Performance", meta=(ClampMin=1, ClampMax=16))
    int32 ThreadPoolSize = 4;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caching")
    bool bEnableCaching = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Caching", meta=(ClampMin=100, ClampMax=100000))
    int32 MaxCacheEntries = 10000;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character")
    bool bEnableKeyboardDistance = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character")
    bool bEnableVisualSimilarity = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character")
    bool bEnablePhoneticSimilarity = true;
};

/**
 * Tokenizer configuration
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FTokenizerConfigAsset
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic")
    FString DefaultDelimiters = TEXT(" _-.,;:!?()[]{}");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic")
    bool bPreserveDelimiters = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic", meta=(ClampMin=1, ClampMax=100))
    int32 MinTokenLength = 1;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic", meta=(ClampMin=1, ClampMax=500))
    int32 MaxTokenLength = 100;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Processing")
    bool bDetectNamingConvention = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Processing")
    bool bSplitCamelCase = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Processing")
    bool bSplitNumbers = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Processing")
    bool bNormalizeCase = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    bool bGenerateVariants = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    bool bIncludeTypos = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    bool bIncludeAbbreviations = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants", meta=(ClampMin=1, ClampMax=3))
    int32 MaxTypoDistance = 1;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="StopWords")
    TArray<FString> DefaultStopWords = {
        TEXT("a"), TEXT("an"), TEXT("the"), TEXT("is"), TEXT("are"), 
        TEXT("was"), TEXT("were"), TEXT("be"), TEXT("been"), TEXT("being"),
        TEXT("have"), TEXT("has"), TEXT("had"), TEXT("do"), TEXT("does"),
        TEXT("did"), TEXT("will"), TEXT("would"), TEXT("could"), TEXT("should")
    };
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="StopWords")
    bool bFilterStopWords = false;
};

/**
 * Pattern engine configuration
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FPatternEngineConfig
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Matching")
    float DefaultMinConfidence = 0.7f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Matching")
    bool bAllowPartialMatches = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Matching")
    bool bEnableFuzzyMatching = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Matching", meta=(ClampMin=1, ClampMax=10))
    int32 MaxPatternDepth = 5;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Performance")
    bool bCachePatternResults = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Performance", meta=(ClampMin=100, ClampMax=10000))
    int32 MaxCachedPatterns = 1000;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Regex")
    bool bEnableRegexPatterns = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Regex", meta=(ClampMin=100, ClampMax=10000))
    int32 RegexTimeoutMS = 1000;
};

/**
 * Knowledge base configuration
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FKnowledgeBaseConfig
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Storage", meta=(ClampMin=1000, ClampMax=1000000))
    int32 MaxFacts = 100000;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Storage", meta=(ClampMin=100, ClampMax=100000))
    int32 MaxConcepts = 10000;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Storage", meta=(ClampMin=1000, ClampMax=1000000))
    int32 MaxRelations = 50000;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
    bool bEnableGraphIndexing = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph", meta=(ClampMin=1, ClampMax=10))
    int32 DefaultMaxTraversalDepth = 5;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph", meta=(ClampMin=0.0, ClampMax=1.0))
    float ActivationDecayRate = 0.3f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Query")
    bool bEnableParallelQueries = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Query", meta=(ClampMin=10, ClampMax=1000))
    int32 DefaultMaxQueryResults = 100;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Persistence")
    bool bAutoSave = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Persistence", meta=(ClampMin=60, ClampMax=3600))
    float AutoSaveIntervalSeconds = 300.0f;
};

/**
 * Inference engine configuration
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FInferenceEngineConfig
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rules", meta=(ClampMin=10, ClampMax=10000))
    int32 MaxRules = 1000;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rules", meta=(ClampMin=0.0, ClampMax=1.0))
    float DefaultMinConfidence = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inference", meta=(ClampMin=1, ClampMax=20))
    int32 MaxInferenceDepth = 5;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inference", meta=(ClampMin=100, ClampMax=10000))
    int32 MaxInferencesPerCycle = 1000;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inference")
    bool bAllowAssumptions = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inference")
    bool bTrackInferenceHistory = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Methods")
    bool bEnableForwardChaining = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Methods")
    bool bEnableBackwardChaining = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Methods")
    bool bEnableFuzzyLogic = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Methods")
    bool bEnableProbabilistic = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConflictResolution")
    bool bAutoResolveConflicts = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConflictResolution", meta=(ClampMin=0.0, ClampMax=1.0))
    float ConflictConfidenceThreshold = 0.7f;
};

/**
 * Cache manager configuration
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FCacheManagerConfig
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Memory", meta=(ClampMin=16, ClampMax=2048))
    int32 MaxMemoryMB = 128;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Memory", meta=(ClampMin=0.5, ClampMax=0.95))
    float MemoryPressureThreshold = 0.8f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eviction")
    bool bUseLRU = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eviction")
    bool bUseLFU = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eviction")
    bool bUsePriorityEviction = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Statistics")
    bool bTrackStatistics = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Statistics")
    bool bLogCachePerformance = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Persistence")
    bool bPersistCaches = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Persistence")
    FString CachePersistencePath = TEXT("SemanticEngine/Cache");
};

/**
 * Processor auto-registration entry
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FProcessorAutoRegister
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Processor")
    FName ProcessorName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Processor", 
              meta=(MustImplement="MMSemanticProcessor"))
    TSubclassOf<UObject> ProcessorClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Processor")
    bool bEnabled = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Processor")
    TMap<FString, FString> InitialConfiguration;
};

/**
 * Main semantic engine configuration asset
 * Centralizes all component configurations
 */
UCLASS(BlueprintType)
class REASONINGENGINE_API UREEngineConfiguration : public UDataAsset
{
    GENERATED_BODY()
    
public:
    // ========== COMPONENT CONFIGURATIONS ==========
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|FuzzyMatcher")
    FFuzzyMatcherConfig FuzzyMatcherConfig;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|Tokenizer")
    FTokenizerConfigAsset TokenizerConfig;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|PatternEngine")
    FPatternEngineConfig PatternEngineConfig;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|KnowledgeBase")
    FKnowledgeBaseConfig KnowledgeBaseConfig;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|InferenceEngine")
    FInferenceEngineConfig InferenceEngineConfig;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|CacheManager")
    FCacheManagerConfig CacheManagerConfig;
    
    // ========== PROCESSOR AUTO-REGISTRATION ==========
    // Single source of truth for processor configuration
    
    /**
     * Processors to automatically create and register on engine initialization
     * This replaces the need for a separate registry
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Processors",
              meta=(TitleProperty="ProcessorName"))
    TArray<FProcessorAutoRegister> AutoRegisterProcessors;
    
    /**
     * Default processor to use when none specified
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Processors")
    FName DefaultProcessor;
    
    // ========== GLOBAL SETTINGS ==========
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Global", meta=(ClampMin=16, ClampMax=2048))
    int32 MaxTotalMemoryMB = 256;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Global")
    bool bEnableGlobalMultithreading = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Global", meta=(ClampMin=2, ClampMax=16))
    int32 GlobalThreadPoolSize = 4;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Global")
    bool bEnablePerformanceTracking = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Global")
    bool bVerboseLogging = false;
    
    // ========== STARTUP BEHAVIOR ==========
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Startup")
    bool bAutoInitializeOnLoad = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Startup")
    bool bPreloadDefaultVocabularies = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Startup")
    bool bPreloadDefaultPatterns = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Startup")
    bool bPreloadDefaultRules = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Startup")
    TArray<TSoftObjectPtr<UDataAsset>> PreloadAssets;
    
    // ========== VALIDATION ==========
    
    /**
     * Validate configuration consistency
     * @param OutErrors - Validation errors found
     * @return true if configuration is valid
     */
    UFUNCTION(BlueprintCallable, Category="MM|Config",
              meta=(DisplayName="Validate Configuration"))
    bool ValidateConfiguration(TArray<FString>& OutErrors) const;
    
    /**
     * Apply configuration to engine
     * @param Engine - Engine to configure
     */
    UFUNCTION(BlueprintCallable, Category="MM|Config",
              meta=(DisplayName="Apply To Engine"))
    void ApplyToEngine(class URECore* Engine) const;
    
    /**
     * Get estimated memory usage
     * @return Estimated memory in MB
     */
    UFUNCTION(BlueprintPure, Category="MM|Config",
              meta=(DisplayName="Get Estimated Memory"))
    int32 GetEstimatedMemoryUsage() const;
    
    /**
     * Create and register all configured processors
     * @param Engine - Engine to register with
     */
    void RegisterConfiguredProcessors(class URECore* Engine) const;
    
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
