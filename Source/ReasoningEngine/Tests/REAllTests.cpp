#include "Misc/AutomationTest.h"
#include "Semantic/REFuzzy.h"
#include "Semantic/RESimilarity.h"
#include "Semantic/Data/REStringTypes.h"
#include "Semantic/Data/RESemanticTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRESemanticAllHarnessTest,
    "ReasoningEngine.Semantic.AllHarness",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRESemanticAllHarnessTest::RunTest(const FString& Parameters)
{
    FString A = TEXT("apple");
    FString B = TEXT("applesauce");

    // ======================
    // Fuzzy algorithms
    // ======================
    UE_LOG(LogTemp, Display, TEXT("=== Running Fuzzy Algorithms ==="));

    for (int32 AlgoIndex = 0; AlgoIndex < static_cast<int32>(EREFuzzyAlgorithm::Auto) + 1; ++AlgoIndex)
    {
        static_cast<EREFuzzyAlgorithm>(AlgoIndex);

        FREStringMatch Result = UREFuzzy::CompareStrings(A, B, true);

        FString AlgoName = StaticEnum<EREFuzzyAlgorithm>()->GetNameStringByValue(AlgoIndex);
        UE_LOG(LogTemp, Display,
            TEXT("[Fuzzy] %s | Lev=%d | NormLev=%.3f | JW=%.3f | Dice=%.3f | Jac=%.3f | Cos=%.3f"),
            *AlgoName,
            Result.LevenshteinDistance,
            Result.NormalizedLevenshtein,
            Result.JaroWinklerSimilarity,
            Result.DiceCoefficient,
            Result.JaccardIndex,
            Result.CosineSimilarity);

        TestTrue(FString::Printf(TEXT("Fuzzy %s NormLev valid"), *AlgoName),
                 Result.NormalizedLevenshtein >= 0.f && Result.NormalizedLevenshtein <= 1.f);
    }

    // ======================
    // Similarity metrics
    // ======================
    UE_LOG(LogTemp, Display, TEXT("=== Running Similarity Metrics ==="));

    for (int32 MetricIndex = 0; MetricIndex < static_cast<int32>(ERESimilarityMetric::Auto) + 1; ++MetricIndex)
    {
        ERESimilarityMetric Metric = static_cast<ERESimilarityMetric>(MetricIndex);

        FRESimilarityResult Result = URESimilarity::CompareStringsWithAlgo(A, B, Metric, true);

        FString MetricName = StaticEnum<ERESimilarityMetric>()->GetNameStringByValue(MetricIndex);
        UE_LOG(LogTemp, Display,
            TEXT("[Similarity] %s | Overall=%.3f | Fuzzy=%.3f | Token=%.3f | Vector=%.3f | Flags(F=%d,T=%d,V=%d)"),
            *MetricName,
            Result.OverallScore,
            Result.FuzzyScore,
            Result.TokenScore,
            Result.VectorScore,
            Result.bUsedFuzzy,
            Result.bUsedTokens,
            Result.bUsedVectors);

        TestTrue(FString::Printf(TEXT("Similarity %s Overall valid"), *MetricName),
                 Result.OverallScore >= 0.f && Result.OverallScore <= 1.f);

        TestTrue(FString::Printf(TEXT("Similarity %s UsedMetric correct"), *MetricName),
                 Result.UsedMetric == Metric);
    }

    return true;
}
