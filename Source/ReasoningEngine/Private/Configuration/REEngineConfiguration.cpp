#include "Configuration/REEngineConfiguration.h"
#include "Core/RECore.h"
#include "Semantic/REFuzzy.h"
#include "Semantic/RETokenizer.h"
#include "Symbolic/REPatterns.h"
#include "Symbolic/REKnowledge.h"
#include "Symbolic/REInferences.h"
#include "Core/RECache.h"
#include "Interfaces/REProcessor.h"
#include "ReasoningEngine.h"

bool UREEngineConfiguration::ValidateConfiguration(TArray<FString>& OutErrors) const
{
    OutErrors.Empty();
    bool bValid = true;
    
    // Validate memory allocation
    int32 TotalComponentMemory = CacheManagerConfig.MaxMemoryMB + 
                                 (KnowledgeBaseConfig.MaxFacts * 100 / 1024 / 1024) +
                                 (PatternEngineConfig.MaxCachedPatterns * 10 / 1024);
    
    if (TotalComponentMemory > MaxTotalMemoryMB)
    {
        OutErrors.Add(FString::Printf(TEXT("Component memory (%dMB) exceeds total limit (%dMB)"),
            TotalComponentMemory, MaxTotalMemoryMB));
        bValid = false;
    }
    
    // Validate thread pool size
    if (bEnableGlobalMultithreading && GlobalThreadPoolSize < 1)
    {
        OutErrors.Add(TEXT("Thread pool size must be at least 1 when multithreading is enabled"));
        bValid = false;
    }
    
    // Validate processor configuration
    TSet<FName> ProcessorNames;
    for (const FProcessorAutoRegister& ProcessorConfig : AutoRegisterProcessors)
    {
        if (ProcessorConfig.ProcessorName.IsNone())
        {
            OutErrors.Add(TEXT("Processor registration contains empty name"));
            bValid = false;
        }
        
        if (ProcessorNames.Contains(ProcessorConfig.ProcessorName))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate processor name: %s"),
                *ProcessorConfig.ProcessorName.ToString()));
            bValid = false;
        }
        ProcessorNames.Add(ProcessorConfig.ProcessorName);
        
        if (!ProcessorConfig.ProcessorClass)
        {
            OutErrors.Add(FString::Printf(TEXT("Processor %s has no class specified"),
                *ProcessorConfig.ProcessorName.ToString()));
            bValid = false;
        }
        else if (!ProcessorConfig.ProcessorClass->ImplementsInterface(UREProcessor::StaticClass()))
        {
            OutErrors.Add(FString::Printf(TEXT("Processor class %s does not implement IREProcessor"),
                *ProcessorConfig.ProcessorClass->GetName()));
            bValid = false;
        }
    }
    
    // Validate default processor
    if (!DefaultProcessor.IsNone() && !ProcessorNames.Contains(DefaultProcessor))
    {
        OutErrors.Add(FString::Printf(TEXT("Default processor %s is not in registered processors"),
            *DefaultProcessor.ToString()));
        bValid = false;
    }
    
    return bValid;
}

void UREEngineConfiguration::ApplyToEngine(URECore* Engine) const
{
    if (!Engine)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Cannot apply configuration to null engine"));
        return;
    }
    
    // Apply component configurations
    if (REFuzzy* FuzzyMatcher = Engine->GetFuzzyMatcher())
    {
        // Apply fuzzy matcher config
        FuzzyMatcher->SetMultithreading(FuzzyMatcherConfig.bUseMultithreading, 
                                       FuzzyMatcherConfig.ThreadPoolSize);
    }
    
    if (URETokenizer* Tokenizer = Engine->GetTokenizer())
    {
        // Apply tokenizer config
        // Tokenizer->ApplyConfiguration(TokenizerConfig);
    }
    
    if (URECache* CacheManager = Engine->GetCacheManager())
    {
        CacheManager->SetMaxSizeMB(CacheManagerConfig.MaxMemoryMB);
    }
    
    // Register configured processors
    RegisterConfiguredProcessors(Engine);
    
    // Apply global settings
    Engine->ConfigureRuntime(MaxTotalMemoryMB, bEnableGlobalMultithreading, GlobalThreadPoolSize);
    
    UE_LOG(LogReasoningEngine, Log, TEXT("Applied configuration %s to engine"), *GetName());
}

void UREEngineConfiguration::RegisterConfiguredProcessors(URECore* Engine) const
{
    if (!Engine)
    {
        return;
    }
    
    for (const FProcessorAutoRegister& ProcessorConfig : AutoRegisterProcessors)
    {
        if (!ProcessorConfig.bEnabled)
        {
            continue;
        }
        
        if (!ProcessorConfig.ProcessorClass)
        {
            UE_LOG(LogReasoningEngine, Warning, TEXT("Cannot register processor %s: No class specified"),
                *ProcessorConfig.ProcessorName.ToString());
            continue;
        }
        
        // Create processor instance
        UObject* ProcessorObj = NewObject<UObject>(
            Engine,  // Outer is the engine for proper lifecycle
            ProcessorConfig.ProcessorClass,
            ProcessorConfig.ProcessorName
        );
        
        if (!ProcessorObj)
        {
            UE_LOG(LogReasoningEngine, Error, TEXT("Failed to create processor %s of class %s"),
                *ProcessorConfig.ProcessorName.ToString(),
                *ProcessorConfig.ProcessorClass->GetName());
            continue;
        }
        
        // Verify it implements the interface
        if (!ProcessorObj->GetClass()->ImplementsInterface(UREProcessor::StaticClass()))
        {
            UE_LOG(LogReasoningEngine, Error, TEXT("Processor class %s does not implement IREProcessor"),
                *ProcessorConfig.ProcessorClass->GetName());
            ProcessorObj->ConditionalBeginDestroy();
            continue;
        }
        
        // Configure if needed
        if (ProcessorConfig.InitialConfiguration.Num() > 0)
        {
            if (IREProcessor* Processor = Cast<IREProcessor>(ProcessorObj))
            {
                Processor->Configure(ProcessorConfig.InitialConfiguration);
            }
        }
        
        // Register with engine
        Engine->RegisterProcessor(ProcessorConfig.ProcessorName, ProcessorObj);
        
        UE_LOG(LogReasoningEngine, Log, TEXT("Registered processor %s from configuration"),
            *ProcessorConfig.ProcessorName.ToString());
    }
}

int32 UREEngineConfiguration::GetEstimatedMemoryUsage() const
{
    int32 EstimatedMB = 0;
    
    // Cache memory
    EstimatedMB += CacheManagerConfig.MaxMemoryMB;
    
    // Knowledge base estimate (rough: 100 bytes per fact, 1KB per concept)
    EstimatedMB += (KnowledgeBaseConfig.MaxFacts * 100) / (1024 * 1024);
    EstimatedMB += (KnowledgeBaseConfig.MaxConcepts * 1024) / (1024 * 1024);
    
    // Pattern cache estimate
    EstimatedMB += (PatternEngineConfig.MaxCachedPatterns * 10 * 1024) / (1024 * 1024);
    
    // Add 20% overhead for other structures
    EstimatedMB = FMath::CeilToInt(EstimatedMB * 1.2f);
    
    return FMath::Min(EstimatedMB, MaxTotalMemoryMB);
}

#if WITH_EDITOR
void UREEngineConfiguration::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    
    // Auto-validate on changes
    TArray<FString> Errors;
    if (!ValidateConfiguration(Errors))
    {
        for (const FString& Error : Errors)
        {
            UE_LOG(LogReasoningEngine, Warning, TEXT("Configuration validation: %s"), *Error);
        }
    }
}
#endif
