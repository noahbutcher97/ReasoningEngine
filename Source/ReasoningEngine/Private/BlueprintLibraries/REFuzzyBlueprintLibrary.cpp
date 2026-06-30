#include "BlueprintLibraries/REFuzzyBlueprintLibrary.h"
#include "Semantic/REFuzzy.h"

int32 UREFuzzyBlueprintLibrary::BP_CalculateLevenshteinDistance(const FString& Source, const FString& Target)
{
    return REFuzzy::CalculateLevenshtein(Source, Target);
}

float UREFuzzyBlueprintLibrary::BP_CalculateJaroWinkler(const FString& Source, const FString& Target)
{
    return REFuzzy::CalculateJaroWinkler(Source, Target);
}

TArray<FREFuzzyResult> UREFuzzyBlueprintLibrary::BP_BatchFuzzyMatch(
    const TArray<FString>& Candidates,
    const FString& Query,
    EREFuzzyAlgorithm Algorithm,
    float MinScore
)
{
    TArray<FREFuzzyResult> Results;
    Results.Reserve(Candidates.Num());

    for (const FString& Candidate : Candidates)
    {
        const float Score = REFuzzy::GetSimilarity(Query, Candidate, Algorithm, true);
        if (Score < MinScore)
        {
            continue;
        }

        FREFuzzyResult Result;
        Result.Score = Score;
        Result.Algorithm = Algorithm;
        Result.Explanation = Candidate;
        Results.Add(Result);
    }

    return Results;
}

FString UREFuzzyBlueprintLibrary::BP_GenerateSoundex(const FString& Input)
{
    return REFuzzy::GenerateSoundex(Input);
}
