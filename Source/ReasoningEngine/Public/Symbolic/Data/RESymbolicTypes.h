#pragma once

#include "CoreMinimal.h"
#include "RESymbolicTypes.generated.h"

/**
 * Unified symbolic type definitions for ReasoningEngine.
 * Consolidates knowledge schemas, pattern schemas, and reasoning strategies.
 */

// =========================================================================
// Pattern Schemas (formerly from REPatternTypes.h)
// =========================================================================

/**
 * Pattern match mode
 */
UENUM(BlueprintType)
enum class EREPatternMatchMode : uint8
{
    Exact           UMETA(DisplayName = "Exact Match"),
    Fuzzy           UMETA(DisplayName = "Fuzzy Match"),
    Regex           UMETA(DisplayName = "Regular Expression"),
    Wildcard        UMETA(DisplayName = "Wildcard"),
    Semantic        UMETA(DisplayName = "Semantic"),
    Token           UMETA(DisplayName = "Token-based")
};

/**
 * Pattern type
 */
UENUM(BlueprintType)
enum class EREPatternType : uint8
{
    Simple          UMETA(DisplayName = "Simple String"),
    Template        UMETA(DisplayName = "Template"),
    Regex           UMETA(DisplayName = "Regular Expression"),
    StateMachine    UMETA(DisplayName = "State Machine"),
    Graph           UMETA(DisplayName = "Graph Pattern")
};

/**
 * Token type enumeration
 */
UENUM(BlueprintType)
enum class ERETokenType : uint8
{
    Unknown         UMETA(DisplayName = "Unknown"),
    Word            UMETA(DisplayName = "Word"),
    Number          UMETA(DisplayName = "Number"),
    Symbol          UMETA(DisplayName = "Symbol"),
    Delimiter       UMETA(DisplayName = "Delimiter"),
    Operator        UMETA(DisplayName = "Operator"),
    Keyword         UMETA(DisplayName = "Keyword"),
    Whitespace      UMETA(DisplayName = "Whitespace"),
    Punctuation     UMETA(DisplayName = "Punctuation"),
    CamelCase       UMETA(DisplayName = "CamelCase"),
    SnakeCase       UMETA(DisplayName = "SnakeCase"),
    Identifier      UMETA(DisplayName = "Identifier"),
    Literal         UMETA(DisplayName = "Literal")
};

/**
 * Pattern template for matching
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREPatternTemplate
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Pattern")
    FName PatternID;
    
    UPROPERTY(BlueprintReadWrite, Category="Pattern")
    FString PatternString;
    
    UPROPERTY(BlueprintReadWrite, Category="Pattern")
    EREPatternType PatternType = EREPatternType::Simple;
    
    UPROPERTY(BlueprintReadWrite, Category="Pattern")
    TArray<ERETokenType> ExpectedTokenTypes;
    
    UPROPERTY(BlueprintReadWrite, Category="Pattern")
    bool bAllowPartialMatch = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Pattern")
    float MinConfidence = 0.7f;
    
    UPROPERTY(BlueprintReadWrite, Category="Pattern")
    TMap<FString, FString> CaptureGroups;
    
    UPROPERTY(BlueprintReadWrite, Category="Pattern")
    bool bCaseSensitive = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Pattern")
    int32 Priority = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Metadata")
    TMap<FString, FString> Metadata;
};

/**
 * Pattern match result
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREPatternMatch
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    FName PatternID;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    bool bMatched = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    float Confidence = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    int32 StartIndex = -1;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    int32 EndIndex = -1;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    int32 StartTokenIndex = -1;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    int32 EndTokenIndex = -1;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    TMap<FString, FString> CapturedValues;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    FString MatchedText;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    EREPatternMatchMode MatchMode = EREPatternMatchMode::Exact;
    
    int32 Length() const { return EndIndex - StartIndex; }
    int32 TokenLength() const { return EndTokenIndex - StartTokenIndex; }
};

/**
 * Pattern state for state machine patterns
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREPatternState
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="State")
    FName StateID;
    
    UPROPERTY(BlueprintReadWrite, Category="State")
    FString StateName;
    
    UPROPERTY(BlueprintReadWrite, Category="State")
    TArray<ERETokenType> AcceptedTokenTypes;
    
    UPROPERTY(BlueprintReadWrite, Category="State")
    TArray<FString> AcceptedValues;
    
    UPROPERTY(BlueprintReadWrite, Category="State")
    bool bIsTerminal = false;
    
    UPROPERTY(BlueprintReadWrite, Category="State")
    bool bIsOptional = false;
    
    UPROPERTY(BlueprintReadWrite, Category="State")
    bool bIsRepeatable = false;
    
    UPROPERTY(BlueprintReadWrite, Category="State")
    TMap<FName, FName> Transitions;
    
    UPROPERTY(BlueprintReadWrite, Category="State")
    FString CaptureGroup;
};

/**
 * Pattern state machine
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREPatternStateMachine
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Machine")
    FName MachineID;
    
    UPROPERTY(BlueprintReadWrite, Category="Machine")
    TMap<FName, FREPatternState> States;
    
    UPROPERTY(BlueprintReadWrite, Category="Machine")
    FName StartState;
    
    UPROPERTY(BlueprintReadWrite, Category="Machine")
    TSet<FName> FinalStates;
    
    UPROPERTY(BlueprintReadWrite, Category="Machine")
    float MinConfidence = 0.7f;
    
    UPROPERTY(BlueprintReadWrite, Category="Machine")
    bool bAllowPartialMatch = false;
    
    bool IsValid() const
    {
        return !StartState.IsNone() && States.Num() > 0 && FinalStates.Num() > 0;
    }
};

/**
 * Pattern rule for complex matching
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREPatternRule
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    FName RuleID;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    FString Description;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    TArray<FREPatternTemplate> RequiredPatterns;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    TArray<FREPatternTemplate> OptionalPatterns;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    TArray<FREPatternTemplate> ForbiddenPatterns;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    float MinConfidence = 0.7f;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    bool bEnabled = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    int32 Priority = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    TMap<FString, FString> Actions;
};

/**
 * Pattern collection result
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREPatternCollection
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Collection")
    TArray<FREPatternMatch> Matches;
    
    UPROPERTY(BlueprintReadOnly, Category="Collection")
    float OverallConfidence = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Collection")
    bool bAllRequiredMatched = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Collection")
    TMap<FString, FString> CombinedCaptures;
    
    void AddMatch(const FREPatternMatch& Match)
    {
        Matches.Add(Match);
        
        // Merge captured values
        for (const auto& Capture : Match.CapturedValues)
        {
            CombinedCaptures.Add(Capture.Key, Capture.Value);
        }
        
        // Recalculate overall confidence
        if (Matches.Num() > 0)
        {
            float Sum = 0.0f;
            for (const auto& M : Matches)
            {
                Sum += M.Confidence;
            }
            OverallConfidence = Sum / Matches.Num();
        }
    }
    
    int32 Num() const { return Matches.Num(); }
    bool IsEmpty() const { return Matches.Num() == 0; }
};

/**
 * Pattern engine configuration
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREPatternConfig
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    EREPatternMatchMode DefaultMatchMode = EREPatternMatchMode::Fuzzy;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    float DefaultMinConfidence = 0.7f;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bAllowPartialMatches = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bCaseSensitive = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bUseTokenization = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    int32 MaxPatternDepth = 5;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    bool bCacheResults = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Config")
    int32 RegexTimeoutMS = 1000;
};

// =========================================================================
// Knowledge Schemas (formerly from REKnowledgeTypes.h)
// =========================================================================

/**
 * Knowledge query type
 */
UENUM(BlueprintType)
enum class EREKnowledgeQueryType : uint8
{
    Facts           UMETA(DisplayName = "Facts"),
    Concepts        UMETA(DisplayName = "Concepts"),
    Relations       UMETA(DisplayName = "Relations"),
    Inferences      UMETA(DisplayName = "Inferences"),
    All             UMETA(DisplayName = "All")
};

/**
 * Inference method type
 */
UENUM(BlueprintType)
enum class EREInferenceMethod : uint8
{
    ForwardChaining     UMETA(DisplayName = "Forward Chaining"),
    BackwardChaining    UMETA(DisplayName = "Backward Chaining"),
    FuzzyLogic          UMETA(DisplayName = "Fuzzy Logic"),
    Probabilistic       UMETA(DisplayName = "Probabilistic"),
    Hybrid              UMETA(DisplayName = "Hybrid")
};

/**
 * Knowledge fact (subject-predicate-object triple)
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREFact
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Fact")
    FString Subject;
    
    UPROPERTY(BlueprintReadWrite, Category="Fact")
    FString Predicate;
    
    UPROPERTY(BlueprintReadWrite, Category="Fact")
    FString Object;
    
    UPROPERTY(BlueprintReadWrite, Category="Fact")
    float Confidence = 1.0f;
    
    UPROPERTY(BlueprintReadWrite, Category="Fact")
    FName Namespace;
    
    UPROPERTY(BlueprintReadWrite, Category="Fact")
    FDateTime Timestamp;
    
    UPROPERTY(BlueprintReadWrite, Category="Fact")
    FString Source;
    
    UPROPERTY(BlueprintReadWrite, Category="Metadata")
    TMap<FString, FString> Metadata;
    
    FString ToString() const
    {
        return FString::Printf(TEXT("%s %s %s"), *Subject, *Predicate, *Object);
    }
    
    bool IsValid() const
    {
        return !Subject.IsEmpty() && !Predicate.IsEmpty() && !Object.IsEmpty();
    }
};

/**
 * Concept in knowledge graph
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREConcept
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Concept")
    FName ConceptID;
    
    UPROPERTY(BlueprintReadWrite, Category="Concept")
    FString Name;
    
    UPROPERTY(BlueprintReadWrite, Category="Concept")
    FString Description;
    
    UPROPERTY(BlueprintReadWrite, Category="Concept")
    TArray<FString> Synonyms;
    
    UPROPERTY(BlueprintReadWrite, Category="Concept")
    TArray<FString> Categories;
    
    UPROPERTY(BlueprintReadWrite, Category="Concept")
    TMap<FString, FString> Properties;
    
    UPROPERTY(BlueprintReadWrite, Category="Concept")
    float Weight = 1.0f;
    
    UPROPERTY(BlueprintReadWrite, Category="Concept")
    float ActivationLevel = 0.0f;
    
    bool HasSynonym(const FString& Term) const
    {
        return Synonyms.Contains(Term);
    }
};

/**
 * Relation between concepts
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FRERelation
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Relation")
    FName FromConcept;
    
    UPROPERTY(BlueprintReadWrite, Category="Relation")
    FName ToConcept;
    
    UPROPERTY(BlueprintReadWrite, Category="Relation")
    FString RelationType;
    
    UPROPERTY(BlueprintReadWrite, Category="Relation")
    float Strength = 1.0f;
    
    UPROPERTY(BlueprintReadWrite, Category="Relation")
    bool bBidirectional = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Relation")
    bool bTransitive = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Metadata")
    TMap<FString, FString> Metadata;
    
    bool IsValid() const
    {
        return !FromConcept.IsNone() && !ToConcept.IsNone() && !RelationType.IsEmpty();
    }
};

/**
 * Inference rule
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREInferenceRule
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    FName RuleID;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    FString Description;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    TArray<FREFact> Conditions;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    TArray<FREFact> Conclusions;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    float MinConfidence = 0.7f;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    bool bEnabled = true;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    int32 Priority = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Rule")
    EREInferenceMethod Method = EREInferenceMethod::ForwardChaining;
    
    bool CanFire(float CurrentConfidence) const
    {
        return bEnabled && CurrentConfidence >= MinConfidence;
    }
};

/**
 * Inference result
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREInference
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Inference")
    FREFact InferredFact;
    
    UPROPERTY(BlueprintReadOnly, Category="Inference")
    TArray<FREFact> SupportingFacts;
    
    UPROPERTY(BlueprintReadOnly, Category="Inference")
    FName AppliedRule;
    
    UPROPERTY(BlueprintReadOnly, Category="Inference")
    float Confidence = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Inference")
    FDateTime Timestamp;
    
    UPROPERTY(BlueprintReadOnly, Category="Inference")
    int32 InferenceDepth = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Inference")
    TArray<FString> ReasoningPath;
};

/**
 * Knowledge query structure
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREKnowledgeQuery
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Query")
    EREKnowledgeQueryType Type = EREKnowledgeQueryType::All;
    
    UPROPERTY(BlueprintReadWrite, Category="Query")
    FString Subject;
    
    UPROPERTY(BlueprintReadWrite, Category="Query")
    FString Predicate;
    
    UPROPERTY(BlueprintReadWrite, Category="Query")
    FString Object;
    
    UPROPERTY(BlueprintReadWrite, Category="Query")
    FName Namespace;
    
    UPROPERTY(BlueprintReadWrite, Category="Query")
    float MinConfidence = 0.5f;
    
    UPROPERTY(BlueprintReadWrite, Category="Query")
    int32 MaxResults = 100;
    
    UPROPERTY(BlueprintReadWrite, Category="Query")
    bool bIncludeInferred = true;
};

/**
 * Knowledge graph node
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREKnowledgeNode
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Node")
    FName NodeID;
    
    UPROPERTY(BlueprintReadWrite, Category="Node")
    FREConcept Concept;
    
    UPROPERTY(BlueprintReadWrite, Category="Node")
    TArray<FName> IncomingRelations;
    
    UPROPERTY(BlueprintReadWrite, Category="Node")
    TArray<FName> OutgoingRelations;
    
    UPROPERTY(BlueprintReadWrite, Category="Node")
    float ActivationLevel = 0.0f;
    
    UPROPERTY(BlueprintReadWrite, Category="Node")
    int32 Depth = 0;
    
    UPROPERTY(BlueprintReadWrite, Category="Node")
    bool bVisited = false;
};

/**
 * Hypothesis structure for reasoning
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREHypothesis
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Hypothesis")
    FString Statement;
    
    UPROPERTY(BlueprintReadWrite, Category="Hypothesis")
    TArray<FREFact> SupportingFacts;
    
    UPROPERTY(BlueprintReadWrite, Category="Hypothesis")
    TArray<FREFact> ContradictingFacts;
    
    UPROPERTY(BlueprintReadWrite, Category="Hypothesis")
    float Confidence = 0.0f;
    
    UPROPERTY(BlueprintReadWrite, Category="Hypothesis")
    TArray<FString> AssumptionsMade;
    
    UPROPERTY(BlueprintReadWrite, Category="Hypothesis")
    bool bProven = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Hypothesis")
    bool bDisproven = false;
};

/**
 * Inference context for reasoning
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREInferenceContext
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    TArray<FREFact> KnownFacts;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    TArray<FREInferenceRule> ActiveRules;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    float MinConfidence = 0.5f;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    int32 MaxInferenceDepth = 5;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    bool bAllowAssumptions = false;
    
    UPROPERTY(BlueprintReadWrite, Category="Context")
    EREInferenceMethod PreferredMethod = EREInferenceMethod::ForwardChaining;
};

/**
 * Rule collection wrapper for TMap
 */
USTRUCT()
struct FRERuleCollection
{
    GENERATED_BODY()
    
    UPROPERTY()
    TArray<FREInferenceRule> Rules;
};

/**
 * Fact collection wrapper for TMap
 */
USTRUCT()
struct FREFactCollection
{
    GENERATED_BODY()
    
    UPROPERTY()
    TArray<FREFact> Facts;
};


/**
 * Fact pair for contradiction tracking
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREFactContradiction
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Contradiction")
    FREFact FactA;
    
    UPROPERTY(BlueprintReadOnly, Category="Contradiction")
    FREFact FactB;
    
    UPROPERTY(BlueprintReadOnly, Category="Contradiction")
    FString Reason;
};

// =========================================================================
// Reasoning Strategies (NEW - for strategy-driven reasoning)
// =========================================================================

/**
 * Reasoning strategy for orchestrating semantic and symbolic fallbacks
 * This struct defines the execution strategy that Core will use
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FREReasoningStrategy
{
    GENERATED_BODY()

    /** Order of semantic algorithms to try (e.g., "Embedding", "TokenSim", "Fuzzy") */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strategy|Semantic",
              meta=(DisplayName="Semantic Fallback Order"))
    TArray<FName> SemanticFallbackOrder;

    /** Order of symbolic methods to try (e.g., "Inference", "Pattern", "ExactMatch") */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strategy|Symbolic",
              meta=(DisplayName="Symbolic Fallback Order"))  
    TArray<FName> SymbolicFallbackOrder;

    /** Fusion rule description (e.g., "Prefer symbolic if confidence > 0.8") */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strategy|Fusion",
              meta=(DisplayName="Fusion Rule", MultiLine="true"))
    FString FusionRule;

    /** Minimum confidence threshold for accepting results */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strategy|Thresholds",
              meta=(DisplayName="Minimum Confidence", ClampMin=0.0, ClampMax=1.0))
    float MinConfidenceThreshold = 0.5f;

    /** Weight for semantic results in fusion (0-1) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strategy|Fusion",
              meta=(DisplayName="Semantic Weight", ClampMin=0.0, ClampMax=1.0))
    float SemanticWeight = 0.5f;

    /** Weight for symbolic results in fusion (0-1) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strategy|Fusion",
              meta=(DisplayName="Symbolic Weight", ClampMin=0.0, ClampMax=1.0))
    float SymbolicWeight = 0.5f;

    /** Whether to run semantic and symbolic branches in parallel */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strategy|Execution",
              meta=(DisplayName="Enable Parallel Execution"))
    bool bEnableParallelExecution = false;

    /** Maximum time to wait for results (in seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strategy|Execution",
              meta=(DisplayName="Timeout Seconds", ClampMin=0.1, ClampMax=30.0))
    float TimeoutSeconds = 5.0f;

    /** Strategy name for identification */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strategy",
              meta=(DisplayName="Strategy Name"))
    FName StrategyName;

    /** Strategy description */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strategy",
              meta=(DisplayName="Description", MultiLine="true"))
    FString Description;

    /**
     * Helper function to validate the strategy configuration
     */
    bool IsValid() const
    {
        return (SemanticFallbackOrder.Num() > 0 || SymbolicFallbackOrder.Num() > 0) &&
               (FMath::Abs(SemanticWeight + SymbolicWeight - 1.0f) < 0.01f || 
                SemanticWeight == 1.0f || SymbolicWeight == 1.0f);
    }

    /**
     * Get a default strategy for basic operation
     */
    static FREReasoningStrategy GetDefault()
    {
        FREReasoningStrategy Default;
        Default.StrategyName = TEXT("Default");
        Default.Description = TEXT("Balanced semantic and symbolic reasoning");
        Default.SemanticFallbackOrder = { TEXT("Fuzzy"), TEXT("TokenSim") };
        Default.SymbolicFallbackOrder = { TEXT("Pattern"), TEXT("Knowledge") };
        Default.FusionRule = TEXT("Average semantic and symbolic scores");
        Default.MinConfidenceThreshold = 0.5f;
        Default.SemanticWeight = 0.5f;
        Default.SymbolicWeight = 0.5f;
        return Default;
    }
};