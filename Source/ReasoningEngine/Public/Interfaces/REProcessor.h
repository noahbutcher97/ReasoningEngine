#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Semantic/Data/RESemanticTypes.h"
#include "REProcessor.generated.h"

// Forward declarations
class URECore;

/**
 * Processor capabilities flags
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FProcessorCapabilities
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Capabilities")
    bool bSupportsAsync = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Capabilities")
    bool bSupportsBatch = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Capabilities")
    bool bSupportsStreaming = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Capabilities")
    bool bRequiresKnowledge = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Capabilities")
    bool bRequiresTokenization = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Capabilities")
    bool bModifiesKnowledge = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Capabilities")
    TArray<FString> RequiredComponents;
};

/**
 * Processor registration information
 * Moved from deleted MMProcessorRegistry.h
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FProcessorRegistration
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Registration")
    FName ProcessorName;
    
    UPROPERTY(BlueprintReadOnly, Category="Registration")
    FString DisplayName;
    
    UPROPERTY(BlueprintReadOnly, Category="Registration")
    FString Description;
    
    UPROPERTY(BlueprintReadOnly, Category="Registration")
    FString Category;
    
    UPROPERTY(BlueprintReadOnly, Category="Registration")
    int32 Priority = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Registration")
    bool bEnabled = true;
    
    UPROPERTY(BlueprintReadOnly, Category="Registration")
    FProcessorCapabilities Capabilities;
};

/**
 * Processor statistics
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FProcessorStats
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int32 TotalProcessed = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int32 SuccessfulProcessed = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    float AverageProcessTimeMS = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    float LastProcessTimeMS = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int64 TotalMemoryUsed = 0;
};

/**
 * Base interface for semantic processors
 * Single interface - no separate registry
 */
UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UREProcessor : public UInterface
{
    GENERATED_BODY()
};

class REASONINGENGINE_API IREProcessor
{
    GENERATED_BODY()
    
public:
    // ========== CORE INTERFACE ==========
    
    /**
     * Get processor name
     * @return Unique processor identifier
     */
    virtual FName GetProcessorName() const = 0;
    
    /**
     * Get processor display name
     * @return Human-readable name
     */
    virtual FString GetDisplayName() const { return GetProcessorName().ToString(); }
    
    /**
     * Get processor description
     * @return Detailed description
     */
    virtual FString GetDescription() const { return TEXT("Semantic Processor"); }
    
    /**
     * Get processor category
     * @return Category for organization
     */
    virtual FString GetCategory() const { return TEXT("General"); }
    
    /**
     * Get processor priority
     * @return Priority for ordering (higher = first)
     */
    virtual int32 GetPriority() const { return 0; }
    
    /**
     * Calculate relevance for input
     * @param Input - Input to evaluate
     * @return Relevance score (0-1)
     */
    virtual float CalculateRelevance(const FString& Input) const = 0;
    
    /**
     * Process input string
     * @param Input - Input to process
     * @param Context - Processing context
     * @return Processing result
     */
    virtual FREProcessorResult ProcessInput(const FString& Input, const FREQueryContext& Context) = 0;
    
    /**
     * Initialize processor with engine reference
     * @param Engine - Semantic engine core
     */
    virtual void Initialize(URECore* Engine) = 0;
    
    // ========== REGISTRATION INFO ==========
    
    /**
     * Get complete registration info for this processor
     * Single source of truth for processor metadata
     * @return Registration metadata
     */
    virtual FProcessorRegistration GetRegistrationInfo() const
    {
        FProcessorRegistration Info;
        Info.ProcessorName = GetProcessorName();
        Info.DisplayName = GetDisplayName();
        Info.Description = GetDescription();
        Info.Category = GetCategory();
        Info.Priority = GetPriority();
        Info.bEnabled = true;
        Info.Capabilities = GetCapabilities();
        return Info;
    }
    
    // ========== EXTENDED INTERFACE ==========
    
    /**
     * Shutdown processor
     */
    virtual void Shutdown() {}
    
    /**
     * Get processor capabilities
     * @return Capability flags
     */
    virtual FProcessorCapabilities GetCapabilities() const 
    { 
        return FProcessorCapabilities(); 
    }
    
    /**
     * Check if processor is ready
     * @return true if operational
     */
    virtual bool IsReady() const { return true; }
    
    /**
     * Get processor statistics
     * @return Current statistics
     */
    virtual FProcessorStats GetStatistics() const 
    { 
        return FProcessorStats(); 
    }
    
    /**
     * Reset processor state
     */
    virtual void Reset() {}
    
    // ========== ASYNC OPERATIONS ==========
    
    /**
     * Process input asynchronously
     * @param Input - Input to process
     * @param Context - Processing context
     * @param OnComplete - Completion callback
     */
    virtual void ProcessInputAsync(const FString& Input, 
                                   const FREQueryContext& Context,
                                   TFunction<void(const FREProcessorResult&)> OnComplete)
    {
        // Default implementation: run synchronously
        FREProcessorResult Result = ProcessInput(Input, Context);
        OnComplete(Result);
    }
    
    // ========== BATCH OPERATIONS ==========
    
    /**
     * Process multiple inputs
     * @param Inputs - Array of inputs
     * @param Context - Shared context
     * @return Array of results
     */
    virtual TArray<FREProcessorResult> ProcessBatch(const TArray<FString>& Inputs,
                                                    const FREQueryContext& Context)
    {
        TArray<FREProcessorResult> Results;
        Results.Reserve(Inputs.Num());
        
        for (const FString& Input : Inputs)
        {
            Results.Add(ProcessInput(Input, Context));
        }
        
        return Results;
    }
    
    // ========== CONFIGURATION ==========
    
    /**
     * Configure processor
     * @param Configuration - Configuration data
     */
    virtual void Configure(const TMap<FString, FString>& Configuration) {}
    
    /**
     * Get current configuration
     * @return Configuration map
     */
    virtual TMap<FString, FString> GetConfiguration() const 
    { 
        return TMap<FString, FString>(); 
    }
    
    // ========== SERIALIZATION ==========
    
    /**
     * Save processor state
     * @return Serialized state
     */
    virtual FString SaveState() const { return TEXT(""); }
    
    /**
     * Load processor state
     * @param State - Serialized state
     * @return true if loaded successfully
     */
    virtual bool LoadState(const FString& State) { return true; }
};
