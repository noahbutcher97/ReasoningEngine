#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/REProcessor.h"
#include "Semantic/Data/RESemanticTypes.h"
#include "RECore.generated.h"

// Forward declarations
class UREFuzzy;
class URETokenizer;
class UREPatterns;
class UREKnowledge;
class UREInferences;
class URECache;

/**
 * Core semantic engine singleton
 * Central orchestrator for all semantic processing operations
 * 
 * IMPORTANT: Does not support hot reload due to singleton pattern.
 * Use Live Coding for iteration or restart editor for changes.
 */
UCLASS(BlueprintType, NotBlueprintable, meta=(DisplayName="Reasoning Engine"))
class REASONINGENGINE_API URECore : public UObject
{
    GENERATED_BODY()
    
private:
    /** Singleton instance */
    static URECore* Instance;
    
    /** Thread safety for singleton access */
    static FCriticalSection InstanceMutex;
    
    /** Prevent resurrection during shutdown */
    static bool bIsShuttingDown;
    
    // ========== CORE COMPONENTS ==========
    
    UPROPERTY()
    UREFuzzy* FuzzyMatcher;
    
    UPROPERTY()
    URETokenizer* Tokenizer;
    
    UPROPERTY()
    UREPatterns* PatternEngine;
    
    UPROPERTY()
    UREKnowledge* KnowledgeBase;
    
    UPROPERTY()
    UREInferences* InferenceEngine;
    
    UPROPERTY()
    URECache* CacheManager;
    
    // ========== PROCESSOR REGISTRY ==========
    
    UPROPERTY()
    TMap<FName, TScriptInterface<IREProcessor>> RegisteredProcessors;
    
    // ========== CONFIGURATION ==========
    
    UPROPERTY()
    class UREEngineConfiguration* Configuration;
    
    // ========== PERFORMANCE TRACKING ==========
    
    mutable FThreadSafeCounter TotalOperations;
    FDateTime InitializationTime;
    mutable TMap<FString, int32> OperationCounts;
    mutable FCriticalSection StatsMutex;
    
protected:
    /** Protected constructor for singleton */
    URECore();
    
    /** Initialize all core components */
    void InitializeCoreComponents();
    
    /** Cleanup all core components */
    void CleanupCoreComponents();
    
    /** Wire up component dependencies */
    void ConnectComponentDependencies();
    
public:
    // ========== SINGLETON MANAGEMENT ==========
    
    /**
     * Get the semantic engine singleton instance
     * Creates the instance on first call if needed
     * @return Semantic engine instance, or nullptr during shutdown
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Core", 
              meta=(DisplayName="Get Semantic Engine"))
    static URECore* Get();
    
    /**
     * Initialize the singleton (called by module)
     * Safe to call multiple times
     */
    static void InitializeSingleton();
    
    /**
     * Destroy the singleton (called by module)
     * Ensures proper cleanup on shutdown
     */
    static void DestroySingleton();
    
    /**
     * Check if the semantic engine is available and initialized
     * @return true if engine is ready for use
     */
    UFUNCTION(BlueprintPure, Category="MM|Semantic|Core",
              meta=(DisplayName="Is Engine Available"))
    static bool IsAvailable();
    
    // ========== CORE COMPONENTS ACCESS ==========
    
    /**
     * Get the fuzzy matching component
     * @return Fuzzy matcher for string operations
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Components",
              meta=(DisplayName="Get Fuzzy Matcher"))
    UREFuzzy* GetFuzzyMatcher();
    
    /**
     * Get the tokenizer component
     * @return Tokenizer for text processing
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Components",
              meta=(DisplayName="Get Tokenizer"))
    URETokenizer* GetTokenizer();
    
    /**
     * Get the pattern engine component
     * @return Pattern engine for pattern matching
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Components",
              meta=(DisplayName="Get Pattern Engine"))
    UREPatterns* GetPatternEngine();
    
    /**
     * Get the knowledge base component
     * @return Knowledge base for semantic knowledge
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Components",
              meta=(DisplayName="Get Knowledge Base"))
    UREKnowledge* GetKnowledgeBase();
    
    /**
     * Get the inference engine component
     * @return Inference engine for reasoning
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Components",
              meta=(DisplayName="Get Inference Engine"))
    UREInferences* GetInferenceEngine();
    
    /**
     * Get the cache manager component
     * @return Cache manager for performance optimization
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Components",
              meta=(DisplayName="Get Cache Manager"))
    URECache* GetCacheManager();
    
    // ========== PROCESSOR REGISTRATION ==========
    
    // ========== PROCESSOR REGISTRATION - ENHANCED ==========
    
    /**
     * Register a semantic processor for domain-specific processing
     * Single source of truth - no separate registry
     * @param ProcessorName - Unique name for the processor
     * @param Processor - The processor implementation
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Processors",
              meta=(DisplayName="Register Processor"))
    void RegisterProcessor(FName ProcessorName, TScriptInterface<IREProcessor> Processor);
    
    /**
     * Unregister a processor
     * @param ProcessorName - Name of processor to remove
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Processors",
              meta=(DisplayName="Unregister Processor"))
    void UnregisterProcessor(FName ProcessorName);
    
    /**
     * Get a registered processor by name
     * @param ProcessorName - Name of the processor to retrieve
     * @return The processor, or null if not found
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Processors",
              meta=(DisplayName="Get Processor"))
    TScriptInterface<IREProcessor> GetProcessor(FName ProcessorName);
    
    /**
     * Check if a processor is registered
     * @param ProcessorName - Name to check
     * @return true if processor exists
     */
    UFUNCTION(BlueprintPure, Category="MM|Semantic|Processors",
              meta=(DisplayName="Has Processor"))
    bool HasProcessor(FName ProcessorName) const;
    
    /**
     * Get list of all registered processors
     * @return Array of processor names
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Processors",
              meta=(DisplayName="Get All Processors"))
    TArray<FName> GetAllProcessors() const;
    
    /**
     * Get processors by category
     * @param Category - Category to filter by
     * @return Array of processor names in category
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Processors",
              meta=(DisplayName="Get Processors By Category"))
    TArray<FName> GetProcessorsByCategory(const FString& Category) const;
    
    /**
     * Get processor registration info
     * @param ProcessorName - Processor to query
     * @return Registration information
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Processors",
              meta=(DisplayName="Get Processor Info"))
    FProcessorRegistration GetProcessorInfo(FName ProcessorName) const;
    
    /**
     * Find best processor for input
     * @param Input - Input to evaluate
     * @param MinRelevance - Minimum relevance threshold
     * @return Best matching processor name or NAME_None
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Processors",
              meta=(DisplayName="Find Best Processor"))
    FName FindBestProcessor(const FString& Input, float MinRelevance = 0.5f) const;
    
    // ========== CONFIGURATION ==========
    
    /**
     * Load configuration from asset
     * @param Config - Configuration asset to load
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Config",
              meta=(DisplayName="Load Configuration"))
    void LoadConfiguration(class UREEngineConfiguration* Config);
    
    /**
     * Get current configuration
     * @return Active configuration or null
     */
    UFUNCTION(BlueprintPure, Category="MM|Semantic|Config",
              meta=(DisplayName="Get Configuration"))
    class UREEngineConfiguration* GetConfiguration() const { return Configuration; }
    
    /**
     * Apply runtime configuration overrides
     * @param MaxCacheSizeMB - Maximum cache size in MB
     * @param bEnableMultithreading - Enable parallel processing
     * @param ThreadPoolSize - Number of worker threads
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Config",
              meta=(DisplayName="Configure Runtime"))
    void ConfigureRuntime(int32 MaxCacheSizeMB = 64, 
                         bool bEnableMultithreading = true, 
                         int32 ThreadPoolSize = 4);
    
    // ========== PERFORMANCE & DIAGNOSTICS ==========
    
    /**
     * Get engine performance statistics
     * @return Formatted string with performance data
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Diagnostics",
              meta=(DisplayName="Get Performance Stats"))
    FString GetPerformanceStats() const;
    
    /**
     * Clear all caches in all components
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Diagnostics",
              meta=(DisplayName="Clear All Caches"))
    void ClearAllCaches();
    
    /**
     * Get total memory usage of the engine
     * @return Memory usage in bytes
     */
    UFUNCTION(BlueprintPure, Category="MM|Semantic|Diagnostics",
              meta=(DisplayName="Get Memory Usage"))
    int64 GetMemoryUsage() const;
    
    /**
     * Perform self-diagnostic check
     * @return true if all systems operational
     */
    UFUNCTION(BlueprintCallable, Category="MM|Semantic|Diagnostics",
              meta=(DisplayName="Perform Self Check"))
    bool PerformSelfCheck() const;
    
    /**
     * Track an operation for statistics
     */
    void TrackOperation(const FString& OperationType) const;
    
    // ========== CONTEXT DETECTION ==========
    
    /**
     * Check if running in editor context
     */
    UFUNCTION(BlueprintPure, Category="MM|Semantic|Context",
              meta=(DisplayName="Is In Editor"))
    bool IsInEditorContext() const;
    
    /**
     * Check if running in runtime/game context
     */
    UFUNCTION(BlueprintPure, Category="MM|Semantic|Context",
              meta=(DisplayName="Is In Runtime"))
    bool IsInRuntimeContext() const;
    
    /**
     * Check if running in PIE
     */
    UFUNCTION(BlueprintPure, Category="MM|Semantic|Context",
              meta=(DisplayName="Is In PIE"))
    bool IsInPIEContext() const;
    
    /**
     * Get current execution context as string
     */
    UFUNCTION(BlueprintPure, Category="MM|Semantic|Context",
              meta=(DisplayName="Get Context"))
    FString GetCurrentContext() const;
};
