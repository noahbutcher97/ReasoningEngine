#include "Core/RECore.h"
#include "Semantic/REFuzzy.h"
#include "Semantic/RETokenizer.h"
#include "Symbolic/REPatterns.h"
#include "Symbolic/REKnowledge.h"
#include "Symbolic/REInferences.h"
#include "Core/RECache.h"
#include "Configuration/REEngineConfiguration.h"
#include "ReasoningEngine.h"

// Static member initialization
URECore* URECore::Instance = nullptr;
FCriticalSection URECore::InstanceMutex;
bool URECore::bIsShuttingDown = false;

URECore::URECore()
{
    InitializationTime = FDateTime::Now();
}

URECore* URECore::Get()
{
    // Early out if shutting down
    if (bIsShuttingDown)
    {
        return nullptr;
    }
    
    FScopeLock Lock(&InstanceMutex);
    
    if (!Instance && !bIsShuttingDown)
    {
        UE_LOG(LogReasoningEngine, Warning, 
            TEXT("Get() called before InitializeSingleton(). Auto-initializing..."));
        InitializeSingleton();
    }
    
    return Instance;
}

void URECore::InitializeSingleton()
{
    FScopeLock Lock(&InstanceMutex);
    
    if (bIsShuttingDown)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Cannot initialize during shutdown"));
        return;
    }
    
    if (Instance)
    {
        UE_LOG(LogReasoningEngine, Verbose, TEXT("Singleton already initialized"));
        return;
    }
    
    // Create the singleton instance with proper flags
    Instance = NewObject<URECore>(
        GetTransientPackage(),
        TEXT("ReasoningEngineCore"),
        RF_Standalone | RF_MarkAsRootSet
    );
    
    // Prevent garbage collection
    Instance->AddToRoot();
    
    // Initialize core components
    Instance->InitializeCoreComponents();
    
    // Connect dependencies between components
    Instance->ConnectComponentDependencies();
    
    UE_LOG(LogReasoningEngine, Log, TEXT("Semantic Engine Core initialized successfully"));
}

void URECore::DestroySingleton()
{
    FScopeLock Lock(&InstanceMutex);
    
    UE_LOG(LogReasoningEngine, Log, TEXT("Beginning singleton shutdown"));
    
    bIsShuttingDown = true;
    
    if (Instance)
    {
        // Log final stats
        UE_LOG(LogReasoningEngine, Log, TEXT("%s"), *Instance->GetPerformanceStats());
        
        // Cleanup components
        Instance->CleanupCoreComponents();
        
        // Remove from root and allow GC
        Instance->RemoveFromRoot();
        
        // Force immediate cleanup
        Instance->ConditionalBeginDestroy();
        
        Instance = nullptr;
    }
    
    bIsShuttingDown = false;
    
    UE_LOG(LogReasoningEngine, Log, TEXT("Singleton shutdown complete"));
}

bool URECore::IsAvailable()
{
    FScopeLock Lock(&InstanceMutex);
    return Instance != nullptr && !bIsShuttingDown;
}

void URECore::InitializeCoreComponents()
{
    UE_LOG(LogReasoningEngine, Log, TEXT("Initializing core components"));
    
    // Create all components with proper outer
    FuzzyMatcher = NewObject<UREFuzzy>(this, TEXT("FuzzyMatcher"));
    Tokenizer = NewObject<URETokenizer>(this, TEXT("Tokenizer"));
    PatternEngine = NewObject<UREPatterns>(this, TEXT("PatternEngine"));
    KnowledgeBase = NewObject<UREKnowledge>(this, TEXT("KnowledgeBase"));
    InferenceEngine = NewObject<UREInferences>(this, TEXT("InferenceEngine"));
    CacheManager = NewObject<URECache>(this, TEXT("CacheManager"));
    
    // Initialize each component
    if (FuzzyMatcher) FuzzyMatcher->Initialize();
    if (Tokenizer) Tokenizer->Initialize();
    if (PatternEngine) PatternEngine->Initialize();
    if (KnowledgeBase) KnowledgeBase->Initialize();
    if (InferenceEngine) InferenceEngine->Initialize();
    if (CacheManager) CacheManager->Initialize();
    
    UE_LOG(LogReasoningEngine, Log, TEXT("Core components initialized"));
}

void URECore::CleanupCoreComponents()
{
    UE_LOG(LogReasoningEngine, Log, TEXT("Cleaning up core components"));
    
    // Shutdown in reverse order
    if (CacheManager) CacheManager->Shutdown();
    if (InferenceEngine) InferenceEngine->Shutdown();
    if (KnowledgeBase) KnowledgeBase->Shutdown();
    if (PatternEngine) PatternEngine->Shutdown();
    if (Tokenizer) Tokenizer->Shutdown();
    if (FuzzyMatcher) FuzzyMatcher->Shutdown();
    
    // Clear processor registry
    RegisteredProcessors.Empty();
    
    // Null out pointers (will be GC'd)
    CacheManager = nullptr;
    InferenceEngine = nullptr;
    KnowledgeBase = nullptr;
    PatternEngine = nullptr;
    Tokenizer = nullptr;
    FuzzyMatcher = nullptr;
    
    UE_LOG(LogReasoningEngine, Log, TEXT("Core components cleaned up"));
}

void URECore::ConnectComponentDependencies()
{
    UE_LOG(LogReasoningEngine, Log, TEXT("Connecting component dependencies"));
    
    // Tokenizer needs FuzzyMatcher for similarity calculations
    if (Tokenizer && FuzzyMatcher)
    {
        Tokenizer->SetFuzzyMatcher(FuzzyMatcher);
    }
    
    // PatternEngine needs Tokenizer for token streams
    if (PatternEngine && Tokenizer)
    {
        PatternEngine->SetTokenizer(Tokenizer);
    }
    
    // InferenceEngine needs KnowledgeBase for facts
    if (InferenceEngine && KnowledgeBase)
    {
        InferenceEngine->SetKnowledgeBase(KnowledgeBase);
    }
    
    // All components can use CacheManager
    if (CacheManager)
    {
        if (FuzzyMatcher) FuzzyMatcher->SetCacheManager(CacheManager);
        if (Tokenizer) Tokenizer->SetCacheManager(CacheManager);
        if (PatternEngine) PatternEngine->SetCacheManager(CacheManager);
    }
    
    UE_LOG(LogReasoningEngine, Log, TEXT("Component dependencies connected"));
}

// ========== COMPONENT ACCESS ==========

UREFuzzy* URECore::GetFuzzyMatcher()
{
    if (!FuzzyMatcher)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Fuzzy matcher not initialized"));
    }
    TrackOperation(TEXT("GetFuzzyMatcher"));
    return FuzzyMatcher;
}

URETokenizer* URECore::GetTokenizer()
{
    if (!Tokenizer)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Tokenizer not initialized"));
    }
    TrackOperation(TEXT("GetTokenizer"));
    return Tokenizer;
}

UREPatterns* URECore::GetPatternEngine()
{
    if (!PatternEngine)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Pattern engine not initialized"));
    }
    TrackOperation(TEXT("GetPatternEngine"));
    return PatternEngine;
}

UREKnowledge* URECore::GetKnowledgeBase()
{
    if (!KnowledgeBase)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Knowledge base not initialized"));
    }
    TrackOperation(TEXT("GetKnowledgeBase"));
    return KnowledgeBase;
}

UREInferences* URECore::GetInferenceEngine()
{
    if (!InferenceEngine)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Inference engine not initialized"));
    }
    TrackOperation(TEXT("GetInferenceEngine"));
    return InferenceEngine;
}

URECache* URECore::GetCacheManager()
{
    if (!CacheManager)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Cache manager not initialized"));
    }
    TrackOperation(TEXT("GetCacheManager"));
    return CacheManager;
}

// ========== PROCESSOR REGISTRATION ==========

void URECore::RegisterProcessor(FName ProcessorName, TScriptInterface<IREProcessor> Processor)
{
    if (ProcessorName.IsNone())
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Cannot register processor with empty name"));
        return;
    }
    
    if (!Processor)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Cannot register null processor"));
        return;
    }
    
    // Check for duplicate
    if (RegisteredProcessors.Contains(ProcessorName))
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Processor %s already registered, replacing"),
            *ProcessorName.ToString());
    }
    
    // Initialize processor with engine reference
    if (Processor.GetInterface())
    {
        Processor.GetInterface()->Initialize(this);
    }
    
    RegisteredProcessors.Add(ProcessorName, Processor);
    
    // Get and log registration info
    if (IREProcessor* Interface = Processor.GetInterface())
    {
        FProcessorRegistration Info = Interface->GetRegistrationInfo();
        UE_LOG(LogReasoningEngine, Log, TEXT("Registered processor: %s (%s) - %s"),
            *Info.ProcessorName.ToString(),
            *Info.Category,
            *Info.Description);
    }
    
    TrackOperation(TEXT("RegisterProcessor"));
}

void URECore::UnregisterProcessor(FName ProcessorName)
{
    if (RegisteredProcessors.Remove(ProcessorName) > 0)
    {
        UE_LOG(LogReasoningEngine, Log, TEXT("Unregistered processor: %s"), *ProcessorName.ToString());
    }
    TrackOperation(TEXT("UnregisterProcessor"));
}

TScriptInterface<IREProcessor> URECore::GetProcessor(FName ProcessorName)
{
    TrackOperation(TEXT("GetProcessor"));
    return RegisteredProcessors.FindRef(ProcessorName);
}

bool URECore::HasProcessor(FName ProcessorName) const
{
    return RegisteredProcessors.Contains(ProcessorName);
}

TArray<FName> URECore::GetAllProcessors() const
{
    TArray<FName> ProcessorNames;
    RegisteredProcessors.GetKeys(ProcessorNames);
    return ProcessorNames;
}

// ========== CONFIGURATION - UPDATED ==========

void URECore::LoadConfiguration(UREEngineConfiguration* Config)
{
    if (!Config)
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Cannot load null configuration"));
        return;
    }
    
    Configuration = Config;
    
    // Apply component configurations - fixed to use address-of operator
    if (FuzzyMatcher)
    {
        FuzzyMatcher->ApplyConfiguration(&Config->FuzzyMatcherConfig);  // Pass address
    }
    
    if (Tokenizer)
    {
        // Tokenizer->ApplyConfiguration(&Config->TokenizerConfig);  // When implemented
    }
    
    if (CacheManager)
    {
        CacheManager->SetMaxSizeMB(Config->CacheManagerConfig.MaxMemoryMB);
    }
    
    // Register configured processors
    Config->RegisterConfiguredProcessors(this);
    
    UE_LOG(LogReasoningEngine, Log, TEXT("Loaded configuration: %s with %d processors"),
        *Config->GetName(), Config->AutoRegisterProcessors.Num());
    
    TrackOperation(TEXT("LoadConfiguration"));
}

// NEW: Get processors by category
TArray<FName> URECore::GetProcessorsByCategory(const FString& Category) const
{
    TArray<FName> Result;
    
    for (const auto& Pair : RegisteredProcessors)
    {
        if (IREProcessor* Processor = Pair.Value.GetInterface())
        {
            if (Processor->GetCategory() == Category)
            {
                Result.Add(Pair.Key);
            }
        }
    }
    
    return Result;
}

// NEW: Get processor info
FProcessorRegistration URECore::GetProcessorInfo(FName ProcessorName) const
{
    if (const TScriptInterface<IREProcessor>* Processor = RegisteredProcessors.Find(ProcessorName))
    {
        if (const IREProcessor* Interface = Processor->GetInterface())
        {
            return Interface->GetRegistrationInfo();
        }
    }
    
    return FProcessorRegistration();
}

FName URECore::FindBestProcessor(const FString& Input, float MinRelevance) const
{
    return "None";  // Placeholder
}

void URECore::ConfigureRuntime(int32 MaxCacheSizeMB, bool bEnableMultithreading, int32 ThreadPoolSize)
{
    if (CacheManager)
    {
        CacheManager->SetMaxSizeMB(MaxCacheSizeMB);
    }
    
    if (FuzzyMatcher)
    {
        FuzzyMatcher->SetMultithreading(bEnableMultithreading, ThreadPoolSize);
    }
    
    UE_LOG(LogReasoningEngine, Log, TEXT("Runtime configured - Cache: %dMB, Threading: %s, Threads: %d"),
        MaxCacheSizeMB, bEnableMultithreading ? TEXT("Yes") : TEXT("No"), ThreadPoolSize);
    
    TrackOperation(TEXT("ConfigureRuntime"));
}

// ========== PERFORMANCE & DIAGNOSTICS ==========

FString URECore::GetPerformanceStats() const
{
    FScopeLock Lock(&StatsMutex);
    
    FString Stats;
    Stats += TEXT("=== MM Semantic Engine Statistics ===\n");
    Stats += FString::Printf(TEXT("Status: %s\n"), IsAvailable() ? TEXT("Active") : TEXT("Inactive"));
    
    if (Instance)
    {
        FTimespan Uptime = FDateTime::Now() - InitializationTime;
        Stats += FString::Printf(TEXT("Uptime: %s\n"), *Uptime.ToString());
        Stats += FString::Printf(TEXT("Total Operations: %d\n"), TotalOperations.GetValue());
        
        // Operation breakdown
        Stats += TEXT("\n--- Operation Counts ---\n");
        for (const auto& Op : OperationCounts)
        {
            Stats += FString::Printf(TEXT("%s: %d\n"), *Op.Key, Op.Value);
        }
        
        // Component status
        Stats += TEXT("\n--- Component Status ---\n");
        Stats += FString::Printf(TEXT("Fuzzy Matcher: %s\n"), FuzzyMatcher ? TEXT("Active") : TEXT("Inactive"));
        Stats += FString::Printf(TEXT("Tokenizer: %s\n"), Tokenizer ? TEXT("Active") : TEXT("Inactive"));
        Stats += FString::Printf(TEXT("Pattern Engine: %s\n"), PatternEngine ? TEXT("Active") : TEXT("Inactive"));
        Stats += FString::Printf(TEXT("Knowledge Base: %s\n"), KnowledgeBase ? TEXT("Active") : TEXT("Inactive"));
        Stats += FString::Printf(TEXT("Inference Engine: %s\n"), InferenceEngine ? TEXT("Active") : TEXT("Inactive"));
        Stats += FString::Printf(TEXT("Cache Manager: %s\n"), CacheManager ? TEXT("Active") : TEXT("Inactive"));
        
        // Processor registry
        Stats += FString::Printf(TEXT("\nRegistered Processors: %d\n"), RegisteredProcessors.Num());
        for (const auto& Proc : RegisteredProcessors)
        {
            Stats += FString::Printf(TEXT("  - %s\n"), *Proc.Key.ToString());
        }
        
        // Memory usage
        int64 MemBytes = GetMemoryUsage();
        float MemMB = MemBytes / (1024.0f * 1024.0f);
        Stats += FString::Printf(TEXT("\nTotal Memory Usage: %.2f MB\n"), MemMB);
        
        // Context
        Stats += FString::Printf(TEXT("\nExecution Context: %s\n"), *GetCurrentContext());
    }
    
    return Stats;
}

void URECore::ClearAllCaches()
{
    if (CacheManager)
    {
        CacheManager->ClearAll();
    }
    
    // Also clear component-specific caches
    if (FuzzyMatcher) FuzzyMatcher->ClearCache();
    if (Tokenizer) Tokenizer->ClearCache();
    if (PatternEngine) PatternEngine->ClearCache();
    
    UE_LOG(LogReasoningEngine, Log, TEXT("All caches cleared"));
    TrackOperation(TEXT("ClearAllCaches"));
}

int64 URECore::GetMemoryUsage() const
{
    int64 TotalMemory = 0;
    
    // Base object size
    TotalMemory += GetClass()->GetStructureSize();
    
    // Component memory
    if (FuzzyMatcher) TotalMemory += FuzzyMatcher->GetMemoryUsage();
    if (Tokenizer) TotalMemory += Tokenizer->GetMemoryUsage();
    if (PatternEngine) TotalMemory += PatternEngine->GetMemoryUsage();
    if (KnowledgeBase) TotalMemory += KnowledgeBase->GetMemoryUsage();
    if (InferenceEngine) TotalMemory += InferenceEngine->GetMemoryUsage();
    if (CacheManager) TotalMemory += CacheManager->GetMemoryUsage();
    
    // Registry overhead
    TotalMemory += RegisteredProcessors.Num() * (sizeof(FName) + sizeof(void*));
    
    return TotalMemory;
}

bool URECore::PerformSelfCheck() const
{
    bool bAllOK = true;
    
    // Check each component
    if (!FuzzyMatcher || !FuzzyMatcher->IsOperational())
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Self-check: Fuzzy Matcher not operational"));
        bAllOK = false;
    }
    
    if (!Tokenizer || !Tokenizer->IsOperational())
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Self-check: Tokenizer not operational"));
        bAllOK = false;
    }
    
    if (!CacheManager || !CacheManager->IsOperational())
    {
        UE_LOG(LogReasoningEngine, Warning, TEXT("Self-check: Cache Manager not operational"));
        bAllOK = false;
    }
    
    return bAllOK;
}

void URECore::TrackOperation(const FString& OperationType) const
{
    TotalOperations.Increment();
    
    FScopeLock Lock(&StatsMutex);
    int32& Count = const_cast<URECore*>(this)->OperationCounts.FindOrAdd(OperationType);
    Count++;
}

// ========== CONTEXT DETECTION ==========

bool URECore::IsInEditorContext() const
{
    return GIsEditor && !GIsPlayInEditorWorld;
}

bool URECore::IsInRuntimeContext() const
{
    return !GIsEditor || GIsPlayInEditorWorld || IsRunningGame();
}

bool URECore::IsInPIEContext() const
{
    return GIsPlayInEditorWorld;
}

FString URECore::GetCurrentContext() const
{
    if (IsInPIEContext())
    {
        return TEXT("PIE");
    }
    else if (IsInEditorContext())
    {
        return TEXT("Editor");
    }
    else if (IsInRuntimeContext())
    {
        return TEXT("Runtime");
    }
    
    return TEXT("Unknown");
}
