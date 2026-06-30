#include "Semantic/RESimilarity.h"
#include "Semantic/REFuzzy.h"

FRESimilarityResult RESimilarity::CompareStringsWithAlgo(
    const FString& A,
    const FString& B,
    ERESimilarityMetric Metric,
    bool bNormalize
)
{
    const FREStringMatch FuzzyMatch = REFuzzy::CompareStrings(A, B, bNormalize);

    FRESimilarityResult Result;
    Result.InputA = A;
    Result.InputB = B;
    Result.FuzzyScore = FuzzyMatch.GetBestSimilarity();
    Result.OverallScore = Result.FuzzyScore;
    Result.UsedMetric = Metric;
    Result.bUsedFuzzy = true;
    return Result;
}
