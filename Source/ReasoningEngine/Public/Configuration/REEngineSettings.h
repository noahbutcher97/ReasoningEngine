#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "REEngineSettings.generated.h"

/**
 * Project-wide settings for MM Semantic Engine
 * Accessible via Project Settings -> Plugins -> MM Semantic Engine
 */
UCLASS(config=Engine, defaultconfig, meta=(DisplayName="MM Semantic Engine"))
class REASONINGENGINE_API UREEngineSettings : public UDeveloperSettings
{
    GENERATED_BODY()
    
public:
    UREEngineSettings();
    
    // ========== DEFAULT CONFIGURATION ==========
	UPROPERTY(EditAnywhere, Config, Category="General")
	bool bAutoInitialize = true;  // ADD THIS
	
    /**
     * Default configuration asset to use
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Configuration",
              meta=(AllowedClasses="MMEngineConfiguration"))
    FSoftObjectPath DefaultConfiguration;
    
    /**
     * Configuration overrides for specific contexts
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Configuration")
    TMap<FName, FSoftObjectPath> ContextConfigurations;
    
    // ========== ENGINE BEHAVIOR ==========
    
    /**
     * Initialize semantic engine on module startup
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Behavior")
    bool bAutoInitializeEngine = true;
    
    /**
     * Create singleton in editor context
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Behavior")
    bool bEnableInEditor = true;
    
    /**
     * Create singleton in runtime/game context
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Behavior")
    bool bEnableInGame = true;
    
    /**
     * Enable engine in shipping builds
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Behavior")
    bool bEnableInShipping = true;
    
    // ========== RESOURCE PATHS ==========
    
    /**
     * Paths to search for vocabulary assets
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Resources")
    TArray<FDirectoryPath> VocabularySearchPaths;
    
    /**
     * Paths to search for pattern assets
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Resources")
    TArray<FDirectoryPath> PatternSearchPaths;
    
    /**
     * Paths to search for knowledge assets
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Resources")
    TArray<FDirectoryPath> KnowledgeSearchPaths;
    
    /**
     * Auto-load assets from search paths
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Resources")
    bool bAutoLoadAssets = false;
    
    // ========== PERFORMANCE ==========
    
    /**
     * Maximum memory allocation for semantic engine (MB)
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Performance",
              meta=(ClampMin=32, ClampMax=4096))
    int32 MaxMemoryAllocationMB = 256;
    
    /**
     * Enable multithreading globally
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Performance")
    bool bEnableMultithreading = true;
    
    /**
     * Number of worker threads
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Performance",
              meta=(ClampMin=1, ClampMax=16))
    int32 WorkerThreadCount = 4;
    
    /**
     * Enable performance profiling
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Performance")
    bool bEnableProfiling = false;
    
    // ========== LOGGING ==========
    
    /**
     * Enable verbose logging
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Logging")
    bool bEnableVerboseLogging = false;
    
    /**
     * Log performance statistics
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Logging")
    bool bLogPerformanceStats = false;
    
    /**
     * Performance logging interval (seconds)
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Logging",
              meta=(ClampMin=1.0f, ClampMax=60.0f))
    float PerformanceLogInterval = 10.0f;
    
    /**
     * Log cache statistics
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Logging")
    bool bLogCacheStats = false;
    
    // ========== DEVELOPER OPTIONS ==========
    
    /**
     * Enable debug visualizations
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Developer")
    bool bEnableDebugVisualization = false;
    
    /**
     * Enable console commands
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Developer")
    bool bEnableConsoleCommands = true;
    
    /**
     * Enable stats commands
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Developer")
    bool bEnableStatsCommands = true;
    
    /**
     * Validate data on load
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category="Developer")
    bool bValidateDataOnLoad = true;
    
    // ========== FUNCTIONS ==========
    
    /**
     * Get the settings instance
     */
    UFUNCTION(BlueprintCallable, Category="MM|Settings",
              meta=(DisplayName="Get Semantic Engine Settings"))
    static UREEngineSettings* Get();
    
    /**
     * Load default configuration asset
     */
    UFUNCTION(BlueprintCallable, Category="MM|Settings",
              meta=(DisplayName="Load Default Configuration"))
    class UREEngineConfiguration* LoadDefaultConfiguration() const;
    
    /**
     * Get configuration for context
     */
    UFUNCTION(BlueprintCallable, Category="MM|Settings",
              meta=(DisplayName="Get Context Configuration"))
    class UREEngineConfiguration* GetContextConfiguration(FName Context) const;
    
    /**
     * Apply settings to engine
     */
    UFUNCTION(BlueprintCallable, Category="MM|Settings",
              meta=(DisplayName="Apply Settings"))
    void ApplySettings(class URECore* Engine) const;
    
    // ========== UOBJECT INTERFACE ==========
    
#if WITH_EDITOR
    virtual FText GetSectionText() const override;
    virtual FText GetSectionDescription() const override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
    
    virtual FName GetCategoryName() const override;
};
