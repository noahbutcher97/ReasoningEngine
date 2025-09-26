#include "Semantic/REFuzzy.h"
#include "Core/RENormalizer.h"
#include "ReasoningEngine.h"
#include "Algo/LevenshteinDistance.h"  // Unreal's built-in for optimization

// Static member initialization
TMap<TCHAR, FVector2D> UREFuzzy::KeyboardLayout;
bool UREFuzzy::bKeyboardInitialized = false;
TMap<TCHAR, TCHAR> UREFuzzy::SoundexMap;
bool UREFuzzy::bPhoneticInitialized = false;

// ========== INITIALIZATION ==========

void UREFuzzy::Initialize()
{
    InitializeKeyboardLayout();
    InitializePhoneticMaps();
    
    UE_LOG(LogReasoningEngine, Log, TEXT("UREFuzzy initialized (Layer 0 - Always Available)"));
}

void UREFuzzy::InitializeKeyboardLayout()
{
    if (bKeyboardInitialized)
        return;
    
    KeyboardLayout.Empty(47);
    
    // QWERTY layout - Row 0 (numbers)
    KeyboardLayout.Add('1', FVector2D(0, 0));
    KeyboardLayout.Add('2', FVector2D(1, 0));
    KeyboardLayout.Add('3', FVector2D(2, 0));
    KeyboardLayout.Add('4', FVector2D(3, 0));
    KeyboardLayout.Add('5', FVector2D(4, 0));
    KeyboardLayout.Add('6', FVector2D(5, 0));
    KeyboardLayout.Add('7', FVector2D(6, 0));
    KeyboardLayout.Add('8', FVector2D(7, 0));
    KeyboardLayout.Add('9', FVector2D(8, 0));
    KeyboardLayout.Add('0', FVector2D(9, 0));
    
    // Row 1
    KeyboardLayout.Add('q', FVector2D(0, 1));
    KeyboardLayout.Add('w', FVector2D(1, 1));
    KeyboardLayout.Add('e', FVector2D(2, 1));
    KeyboardLayout.Add('r', FVector2D(3, 1));
    KeyboardLayout.Add('t', FVector2D(4, 1));
    KeyboardLayout.Add('y', FVector2D(5, 1));
    KeyboardLayout.Add('u', FVector2D(6, 1));
    KeyboardLayout.Add('i', FVector2D(7, 1));
    KeyboardLayout.Add('o', FVector2D(8, 1));
    KeyboardLayout.Add('p', FVector2D(9, 1));
    
    // Row 2 (offset by 0.25 for stagger)
    KeyboardLayout.Add('a', FVector2D(0.25, 2));
    KeyboardLayout.Add('s', FVector2D(1.25, 2));
    KeyboardLayout.Add('d', FVector2D(2.25, 2));
    KeyboardLayout.Add('f', FVector2D(3.25, 2));
    KeyboardLayout.Add('g', FVector2D(4.25, 2));
    KeyboardLayout.Add('h', FVector2D(5.25, 2));
    KeyboardLayout.Add('j', FVector2D(6.25, 2));
    KeyboardLayout.Add('k', FVector2D(7.25, 2));
    KeyboardLayout.Add('l', FVector2D(8.25, 2));
    
    // Row 3 (offset by 0.5 for stagger)
    KeyboardLayout.Add('z', FVector2D(0.5, 3));
    KeyboardLayout.Add('x', FVector2D(1.5, 3));
    KeyboardLayout.Add('c', FVector2D(2.5, 3));
    KeyboardLayout.Add('v', FVector2D(3.5, 3));
    KeyboardLayout.Add('b', FVector2D(4.5, 3));
    KeyboardLayout.Add('n', FVector2D(5.5, 3));
    KeyboardLayout.Add('m', FVector2D(6.5, 3));
    
    bKeyboardInitialized = true;
}

void UREFuzzy::InitializePhoneticMaps()
{
    if (bPhoneticInitialized)
        return;
    
    SoundexMap.Empty(26);
    
    // Soundex mappings
    SoundexMap.Add('B', '1'); SoundexMap.Add('F', '1'); 
    SoundexMap.Add('P', '1'); SoundexMap.Add('V', '1');
    
    SoundexMap.Add('C', '2'); SoundexMap.Add('G', '2'); 
    SoundexMap.Add('J', '2'); SoundexMap.Add('K', '2');
    SoundexMap.Add('Q', '2'); SoundexMap.Add('S', '2'); 
    SoundexMap.Add('X', '2'); SoundexMap.Add('Z', '2');
    
    SoundexMap.Add('D', '3'); SoundexMap.Add('T', '3');
    
    SoundexMap.Add('L', '4');
    
    SoundexMap.Add('M', '5'); SoundexMap.Add('N', '5');
    
    SoundexMap.Add('R', '6');
    
    // A, E, I, O, U, H, W, Y are not mapped (used as separators)
    
    bPhoneticInitialized = true;
}

FString UREFuzzy::PrepareString(const FString& Input, bool bNormalize)
{
    if (bNormalize)
    {
        return URENormalizer::Normalize(Input);
    }
    return Input;
}

// ========== MAIN API ==========

FREStringMatch UREFuzzy::CompareStrings(const FString& A, const FString& B, bool bNormalize)
{
    double StartTime = FPlatformTime::Seconds();
    
    FREStringMatch Result;
    Result.StringA = A;
    Result.StringB = B;
    
    // Prepare strings
    FString PreparedA = PrepareString(A, bNormalize);
    FString PreparedB = PrepareString(B, bNormalize);
    
    // Special cases
    if (PreparedA.Equals(PreparedB))
    {
        Result.NormalizedLevenshtein = 1.0f;
        Result.JaroWinklerSimilarity = 1.0f;
        Result.DiceCoefficient = 1.0f;
        Result.CosineSimilarity = 1.0f;
        Result.JaccardIndex = 1.0f;
        Result.bSoundexMatch = true;
        Result.bMetaphoneMatch = true;
        Result.ComputationTimeMS = (FPlatformTime::Seconds() - StartTime) * 1000.0f;
        return Result;
    }
    
    if (PreparedA.IsEmpty() || PreparedB.IsEmpty())
    {
        Result.ComputationTimeMS = (FPlatformTime::Seconds() - StartTime) * 1000.0f;
        return Result;
    }
    
    // Calculate all metrics
    
    // Edit distances
    Result.LevenshteinDistance = CalculateLevenshtein(PreparedA, PreparedB);
    Result.NormalizedLevenshtein = 1.0f - (float)Result.LevenshteinDistance / 
                                    FMath::Max(PreparedA.Len(), PreparedB.Len());
    Result.DamerauLevenshteinDistance = CalculateDamerauLevenshtein(PreparedA, PreparedB);
    Result.HammingDistance = CalculateHamming(PreparedA, PreparedB);
    
    // Similarity coefficients
    Result.JaroSimilarity = CalculateJaro(PreparedA, PreparedB);
    Result.JaroWinklerSimilarity = CalculateJaroWinkler(PreparedA, PreparedB);
    
    // Subsequence/substring
    Result.LongestCommonSubsequence = CalculateLCS(PreparedA, PreparedB);
    Result.LongestCommonSubstring = CalculateLCSS(PreparedA, PreparedB);
    
    // N-gram similarities
    Result.DiceCoefficient = CalculateDice(PreparedA, PreparedB);
    Result.JaccardIndex = CalculateJaccard(PreparedA, PreparedB);
    Result.CosineSimilarity = CalculateCosine(PreparedA, PreparedB);
    
    // Phonetic
    FString SoundexA = GenerateSoundex(PreparedA);
    FString SoundexB = GenerateSoundex(PreparedB);
    Result.bSoundexMatch = SoundexA.Equals(SoundexB);
    
    TArray<FString> MetaphoneA = GenerateMetaphone(PreparedA);
    TArray<FString> MetaphoneB = GenerateMetaphone(PreparedB);
    Result.bMetaphoneMatch = false;
    for (const FString& MA : MetaphoneA)
    {
        for (const FString& MB : MetaphoneB)
        {
            if (MA.Equals(MB))
            {
                Result.bMetaphoneMatch = true;
                break;
            }
        }
    }
    
    // Visual/Typo
    Result.KeyboardDistance = CalculateKeyboardDistance(PreparedA, PreparedB);
    
    Result.ComputationTimeMS = (FPlatformTime::Seconds() - StartTime) * 1000.0f;
    
    return Result;
}

FREStringMatch UREFuzzy::CompareStringsWithAlgo(
    const FString& A,
    const FString& B,
    EREFuzzyAlgorithm Algorithm,
    bool bNormalize
)
{
    FREStringMatch Result = CompareStrings(A, B, bNormalize);

    switch (Algorithm)
    {
    case EREFuzzyAlgorithm::Levenshtein:
        Result.BestSimilarity = Result.NormalizedLevenshtein;
        break;
    case EREFuzzyAlgorithm::DamerauLevenshtein:
        // Could normalize similarly
        Result.BestSimilarity = 1.f - (float)Result.DamerauLevenshteinDistance / 
                                        FMath::Max(A.Len(), B.Len());
        break;
    case EREFuzzyAlgorithm::Hamming:
        Result.BestSimilarity = 1.f - (float)Result.HammingDistance / 
                                        FMath::Max(A.Len(), B.Len());
        break;
    case EREFuzzyAlgorithm::JaroWinkler:
        Result.BestSimilarity = Result.JaroWinklerSimilarity;
        break;
    case EREFuzzyAlgorithm::Dice:
        Result.BestSimilarity = Result.DiceCoefficient;
        break;
    case EREFuzzyAlgorithm::Cosine:
        Result.BestSimilarity = Result.CosineSimilarity;
        break;
    case EREFuzzyAlgorithm::Jaccard:
        Result.BestSimilarity = Result.JaccardIndex;
        break;
    case EREFuzzyAlgorithm::Soundex:
        Result.BestSimilarity = Result.bSoundexMatch ? 1.f : 0.f;
        break;
    case EREFuzzyAlgorithm::Metaphone:
        Result.BestSimilarity = Result.bMetaphoneMatch ? 1.f : 0.f;
        break;
    default:
        Result.BestSimilarity = Result.GetBestSimilarity();
        break;
    }

    return Result;
}


float UREFuzzy::GetSimilarity(const FString& A, const FString& B, 
                              EREFuzzyAlgorithm Algorithm, bool bNormalize)
{
    FString PreparedA = PrepareString(A, bNormalize);
    FString PreparedB = PrepareString(B, bNormalize);
    
    if (PreparedA.Equals(PreparedB))
        return 1.0f;
    
    if (PreparedA.IsEmpty() || PreparedB.IsEmpty())
        return 0.0f;
    
    switch (Algorithm)
    {
        case EREFuzzyAlgorithm::Levenshtein:
        {
            int32 Distance = CalculateLevenshtein(PreparedA, PreparedB);
            return 1.0f - (float)Distance / FMath::Max(PreparedA.Len(), PreparedB.Len());
        }
        
        case EREFuzzyAlgorithm::DamerauLevenshtein:
        {
            int32 Distance = CalculateDamerauLevenshtein(PreparedA, PreparedB);
            return 1.0f - (float)Distance / FMath::Max(PreparedA.Len(), PreparedB.Len());
        }
        
        case EREFuzzyAlgorithm::Jaro:
            return CalculateJaro(PreparedA, PreparedB);
            
        case EREFuzzyAlgorithm::JaroWinkler:
            return CalculateJaroWinkler(PreparedA, PreparedB);
            
        case EREFuzzyAlgorithm::Dice:
            return CalculateDice(PreparedA, PreparedB);
            
        case EREFuzzyAlgorithm::Jaccard:
            return CalculateJaccard(PreparedA, PreparedB);
            
        case EREFuzzyAlgorithm::Cosine:
            return CalculateCosine(PreparedA, PreparedB);
            
        case EREFuzzyAlgorithm::KeyboardDistance:
            return CalculateKeyboardDistance(PreparedA, PreparedB);
            
        case EREFuzzyAlgorithm::Auto:
        default:
            // Use Jaro-Winkler as default "best" for general use
            return CalculateJaroWinkler(PreparedA, PreparedB);
    }
}

int32 UREFuzzy::GetEditDistance(const FString& A, const FString& B, 
                                 EREFuzzyAlgorithm Algorithm)
{
    switch (Algorithm)
    {
        case EREFuzzyAlgorithm::Levenshtein:
            return CalculateLevenshtein(A, B);
            
        case EREFuzzyAlgorithm::DamerauLevenshtein:
            return CalculateDamerauLevenshtein(A, B);
            
        case EREFuzzyAlgorithm::OptimalAlignment:
            return CalculateOptimalAlignment(A, B);
            
        case EREFuzzyAlgorithm::Hamming:
            return CalculateHamming(A, B);
            
        default:
            return CalculateLevenshtein(A, B);
    }
}

// ========== EDIT DISTANCE ALGORITHMS ==========

int32 UREFuzzy::CalculateLevenshtein(const FString& A, const FString& B)
{
    // Use Unreal's optimized implementation if available
    return Algo::LevenshteinDistance(A, B);
}

int32 UREFuzzy::CalculateDamerauLevenshtein(const FString& A, const FString& B)
{
    const int32 LenA = A.Len();
    const int32 LenB = B.Len();
    
    if (LenA == 0) return LenB;
    if (LenB == 0) return LenA;
    
    // Create distance matrix
    TArray<TArray<int32>> H;
    H.SetNum(LenA + 2);
    for (int32 i = 0; i < LenA + 2; i++)
    {
        H[i].SetNum(LenB + 2);
    }
    
    int32 MaxDist = LenA + LenB;
    H[0][0] = MaxDist;
    
    for (int32 i = 0; i <= LenA; i++)
    {
        H[i + 1][0] = MaxDist;
        H[i + 1][1] = i;
    }
    for (int32 j = 0; j <= LenB; j++)
    {
        H[0][j + 1] = MaxDist;
        H[1][j + 1] = j;
    }
    
    for (int32 i = 1; i <= LenA; i++)
    {
        int32 DB = 0;
        for (int32 j = 1; j <= LenB; j++)
        {
            int32 K = DB;
            int32 L = 0;
            
            if (A[i - 1] == B[j - 1])
            {
                DB = j;
                L = 0;
            }
            else
            {
                L = 1;
            }
            
            for (int32 M = 1; M <= LenA; M++)
            {
                if (A[M - 1] == B[j - 1])
                {
                    K = M;
                    break;
                }
            }
            
            H[i + 1][j + 1] = FMath::Min(
                FMath::Min3(
                    H[i][j] + L,        // Substitution
                    H[i + 1][j] + 1,    // Insertion
                    H[i][j + 1] + 1     // Deletion
                ),
                H[K][DB] + (i - K - 1) + 1 + (j - DB - 1) // Transposition
            );
        }
    }
    
    return H[LenA + 1][LenB + 1];
}

int32 UREFuzzy::CalculateOptimalAlignment(const FString& A, const FString& B)
{
    const int32 LenA = A.Len();
    const int32 LenB = B.Len();
    
    if (LenA == 0) return LenB;
    if (LenB == 0) return LenA;
    
    TArray<TArray<int32>> D;
    D.SetNum(LenA + 1);
    for (int32 i = 0; i <= LenA; i++)
    {
        D[i].SetNum(LenB + 1);
        D[i][0] = i;
    }
    for (int32 j = 0; j <= LenB; j++)
    {
        D[0][j] = j;
    }
    
    for (int32 i = 1; i <= LenA; i++)
    {
        for (int32 j = 1; j <= LenB; j++)
        {
            int32 Cost = (A[i - 1] == B[j - 1]) ? 0 : 1;
            
            D[i][j] = FMath::Min3(
                D[i - 1][j] + 1,      // Deletion
                D[i][j - 1] + 1,      // Insertion
                D[i - 1][j - 1] + Cost // Substitution
            );
            
            // Transposition (restricted)
            if (i > 1 && j > 1 && A[i - 1] == B[j - 2] && A[i - 2] == B[j - 1])
            {
                D[i][j] = FMath::Min(D[i][j], D[i - 2][j - 2] + Cost);
            }
        }
    }
    
    return D[LenA][LenB];
}

int32 UREFuzzy::CalculateHamming(const FString& A, const FString& B)
{
    if (A.Len() != B.Len())
        return -1; // Undefined for different length strings
    
    int32 Distance = 0;
    for (int32 i = 0; i < A.Len(); i++)
    {
        if (A[i] != B[i])
            Distance++;
    }
    
    return Distance;
}

// ========== SIMILARITY ALGORITHMS ==========

float UREFuzzy::CalculateJaro(const FString& A, const FString& B)
{
    if (A.IsEmpty() || B.IsEmpty()) return 0.0f;
    if (A.Equals(B)) return 1.0f;
    
    int32 LenA = A.Len();
    int32 LenB = B.Len();
    
    // Calculate match window
    int32 MatchWindow = FMath::Max(LenA, LenB) / 2 - 1;
    if (MatchWindow < 1) MatchWindow = 1;
    
    TArray<bool> MatchesA;
    MatchesA.Init(false, LenA);
    TArray<bool> MatchesB;
    MatchesB.Init(false, LenB);
    
    int32 Matches = 0;
    int32 Transpositions = 0;
    
    // Find matches
    for (int32 i = 0; i < LenA; i++)
    {
        int32 Start = FMath::Max(0, i - MatchWindow);
        int32 End = FMath::Min(i + MatchWindow + 1, LenB);
        
        for (int32 j = Start; j < End; j++)
        {
            if (MatchesB[j] || A[i] != B[j])
                continue;
                
            MatchesA[i] = true;
            MatchesB[j] = true;
            Matches++;
            break;
        }
    }
    
    if (Matches == 0) return 0.0f;
    
    // Count transpositions
    int32 K = 0;
    for (int32 i = 0; i < LenA; i++)
    {
        if (!MatchesA[i]) continue;
        
        while (!MatchesB[K]) K++;
        
        if (A[i] != B[K])
            Transpositions++;
            
        K++;
    }
    
    float Jaro = (float(Matches) / LenA + 
                  float(Matches) / LenB + 
                  float(Matches - Transpositions / 2) / Matches) / 3.0f;
    
    return Jaro;
}

float UREFuzzy::CalculateJaroWinkler(const FString& A, const FString& B, float PrefixScale)
{
    float JaroSim = CalculateJaro(A, B);
    
    if (JaroSim < 0.7f) return JaroSim;
    
    // Calculate common prefix length (max 4)
    int32 PrefixLen = 0;
    int32 MaxPrefix = FMath::Min(FMath::Min(A.Len(), B.Len()), 4);
    
    for (int32 i = 0; i < MaxPrefix; i++)
    {
        if (A[i] == B[i])
            PrefixLen++;
        else
            break;
    }
    
    return JaroSim + PrefixLen * PrefixScale * (1.0f - JaroSim);
}

// ========== SUBSEQUENCE ALGORITHMS ==========

int32 UREFuzzy::CalculateLCS(const FString& A, const FString& B)
{
    int32 LenA = A.Len();
    int32 LenB = B.Len();
    
    if (LenA == 0 || LenB == 0) return 0;
    
    TArray<TArray<int32>> LCS;
    LCS.SetNum(LenA + 1);
    for (int32 i = 0; i <= LenA; i++)
    {
        LCS[i].SetNum(LenB + 1);
        LCS[i][0] = 0;
    }
    
    for (int32 j = 0; j <= LenB; j++)
    {
        LCS[0][j] = 0;
    }
    
    for (int32 i = 1; i <= LenA; i++)
    {
        for (int32 j = 1; j <= LenB; j++)
        {
            if (A[i - 1] == B[j - 1])
            {
                LCS[i][j] = LCS[i - 1][j - 1] + 1;
            }
            else
            {
                LCS[i][j] = FMath::Max(LCS[i - 1][j], LCS[i][j - 1]);
            }
        }
    }
    
    return LCS[LenA][LenB];
}

int32 UREFuzzy::CalculateLCSS(const FString& A, const FString& B)
{
    int32 LenA = A.Len();
    int32 LenB = B.Len();
    
    if (LenA == 0 || LenB == 0) return 0;
    
    TArray<TArray<int32>> LCSuff;
    LCSuff.SetNum(LenA + 1);
    for (int32 i = 0; i <= LenA; i++)
    {
        LCSuff[i].SetNumZeroed(LenB + 1);
    }
    
    int32 Result = 0;
    
    for (int32 i = 1; i <= LenA; i++)
    {
        for (int32 j = 1; j <= LenB; j++)
        {
            if (A[i - 1] == B[j - 1])
            {
                LCSuff[i][j] = LCSuff[i - 1][j - 1] + 1;
                Result = FMath::Max(Result, LCSuff[i][j]);
            }
        }
    }
    
    return Result;
}

// ========== N-GRAM ALGORITHMS ==========

FRENGramSet UREFuzzy::GenerateNGrams(const FString& Source, int32 N)
{
    FRENGramSet Result;
    Result.N = N;
    Result.SourceString = Source;
    
    if (Source.Len() < N)
    {
        Result.Grams.Add(Source, 1);
        Result.TotalGrams = 1;
        return Result;
    }
    
    for (int32 i = 0; i <= Source.Len() - N; i++)
    {
        FString Gram = Source.Mid(i, N);
        int32& Count = Result.Grams.FindOrAdd(Gram);
        Count++;
        Result.TotalGrams++;
    }
    
    return Result;
}

float UREFuzzy::CalculateDice(const FString& A, const FString& B, int32 N)
{
    if (A.IsEmpty() || B.IsEmpty()) return 0.0f;
    if (A.Equals(B)) return 1.0f;
    
    FRENGramSet GramsA = GenerateNGrams(A, N);
    FRENGramSet GramsB = GenerateNGrams(B, N);
    
    int32 Intersection = 0;
    
    for (const auto& GramA : GramsA.Grams)
    {
        if (int32* CountB = GramsB.Grams.Find(GramA.Key))
        {
            Intersection += FMath::Min(GramA.Value, *CountB);
        }
    }
    
    return (2.0f * Intersection) / (GramsA.TotalGrams + GramsB.TotalGrams);
}

float UREFuzzy::CalculateJaccard(const FString& A, const FString& B, int32 N)
{
    if (A.IsEmpty() || B.IsEmpty()) return 0.0f;
    if (A.Equals(B)) return 1.0f;
    
    FRENGramSet GramsA = GenerateNGrams(A, N);
    FRENGramSet GramsB = GenerateNGrams(B, N);
    
    TSet<FString> Union;
    TSet<FString> Intersection;
    
    for (const auto& GramA : GramsA.Grams)
    {
        Union.Add(GramA.Key);
        if (GramsB.Grams.Contains(GramA.Key))
        {
            Intersection.Add(GramA.Key);
        }
    }
    
    for (const auto& GramB : GramsB.Grams)
    {
        Union.Add(GramB.Key);
    }
    
    if (Union.Num() == 0) return 0.0f;
    
    return float(Intersection.Num()) / float(Union.Num());
}

float UREFuzzy::CalculateCosine(const FString& A, const FString& B, int32 N)
{
    if (A.IsEmpty() || B.IsEmpty()) return 0.0f;
    if (A.Equals(B)) return 1.0f;
    
    FRENGramSet GramsA = GenerateNGrams(A, N);
    FRENGramSet GramsB = GenerateNGrams(B, N);
    
    float DotProduct = 0.0f;
    float MagnitudeA = 0.0f;
    float MagnitudeB = 0.0f;
    
    for (const auto& GramA : GramsA.Grams)
    {
        MagnitudeA += GramA.Value * GramA.Value;
        
        if (int32* CountB = GramsB.Grams.Find(GramA.Key))
        {
            DotProduct += GramA.Value * (*CountB);
        }
    }
    
    for (const auto& GramB : GramsB.Grams)
    {
        MagnitudeB += GramB.Value * GramB.Value;
    }
    
    if (MagnitudeA == 0.0f || MagnitudeB == 0.0f)
        return 0.0f;
        
    return DotProduct / (FMath::Sqrt(MagnitudeA) * FMath::Sqrt(MagnitudeB));
}

// ========== PHONETIC ALGORITHMS ==========

FString UREFuzzy::GenerateSoundex(const FString& Input)
{
    if (Input.IsEmpty()) return TEXT("0000");
    
    InitializePhoneticMaps();
    
    FString Upper = Input.ToUpper();
    FString Result;
    Result.AppendChar(Upper[0]);
    
    TCHAR LastCode = '0';
    
    for (int32 i = 1; i < Upper.Len() && Result.Len() < 4; i++)
    {
        TCHAR Ch = Upper[i];
        if (TCHAR* Code = SoundexMap.Find(Ch))
        {
            if (*Code != LastCode)
            {
                Result.AppendChar(*Code);
                LastCode = *Code;
            }
        }
        else if (!FChar::IsAlpha(Ch))
        {
            LastCode = '0';
        }
    }
    
    // Pad with zeros
    while (Result.Len() < 4)
    {
        Result.AppendChar('0');
    }
    
    return Result;
}

TArray<FString> UREFuzzy::GenerateMetaphone(const FString& Input, bool bDouble)
{
    TArray<FString> Result;
    
    if (Input.IsEmpty())
    {
        Result.Add(TEXT(""));
        return Result;
    }
    
    FString Upper = Input.ToUpper();
    FString Primary;
    FString Secondary;
    
    int32 Current = 0;
    int32 Length = Upper.Len();
    
    // Skip initial silent letters
    if (Length > 1)
    {
        if ((Upper[0] == 'G' && Upper[1] == 'N') ||
            (Upper[0] == 'K' && Upper[1] == 'N') ||
            (Upper[0] == 'P' && Upper[1] == 'N') ||
            (Upper[0] == 'W' && Upper[1] == 'R'))
        {
            Current = 1;
        }
        else if (Upper[0] == 'X')
        {
            Primary += TEXT("S");
            Secondary += TEXT("S");
            Current = 1;
        }
    }
    
    // Process remaining characters (simplified metaphone)
    while (Current < Length && Primary.Len() < 4)
    {
        TCHAR Ch = Upper[Current];
        
        switch (Ch)
        {
            case 'A': case 'E': case 'I': case 'O': case 'U': case 'Y':
                if (Current == 0)
                {
                    Primary += Ch;
                    Secondary += Ch;
                }
                break;
                
            case 'B':
                Primary += TEXT("B");
                Secondary += TEXT("B");
                if (Current + 1 < Length && Upper[Current + 1] == 'B')
                    Current++;
                break;
                
            case 'C':
                if (Current + 1 < Length)
                {
                    if (Upper[Current + 1] == 'H')
                    {
                        Primary += TEXT("X");
                        Secondary += TEXT("X");
                        Current++;
                    }
                    else if (Upper[Current + 1] == 'I' || 
                             Upper[Current + 1] == 'E' || 
                             Upper[Current + 1] == 'Y')
                    {
                        Primary += TEXT("S");
                        Secondary += TEXT("S");
                    }
                    else
                    {
                        Primary += TEXT("K");
                        Secondary += TEXT("K");
                    }
                }
                else
                {
                    Primary += TEXT("K");
                    Secondary += TEXT("K");
                }
                break;
                
            case 'D':
                Primary += TEXT("T");
                Secondary += TEXT("T");
                break;
                
            case 'F':
                Primary += TEXT("F");
                Secondary += TEXT("F");
                break;
                
            case 'G':
                if (Current + 1 < Length && Upper[Current + 1] == 'H')
                {
                    Current++;
                }
                Primary += TEXT("K");
                Secondary += TEXT("K");
                break;
                
            case 'H':
                if ((Current == 0 || !FChar::IsAlpha(Upper[Current - 1])) &&
                    (Current + 1 < Length && FChar::IsAlpha(Upper[Current + 1])))
                {
                    Primary += TEXT("H");
                    Secondary += TEXT("H");
                }
                break;
                
            case 'J':
                Primary += TEXT("J");
                Secondary += TEXT("J");
                break;
                
            case 'K':
                Primary += TEXT("K");
                Secondary += TEXT("K");
                break;
                
            case 'L':
                Primary += TEXT("L");
                Secondary += TEXT("L");
                break;
                
            case 'M':
                Primary += TEXT("M");
                Secondary += TEXT("M");
                break;
                
            case 'N':
                Primary += TEXT("N");
                Secondary += TEXT("N");
                break;
                
            case 'P':
                if (Current + 1 < Length && Upper[Current + 1] == 'H')
                {
                    Primary += TEXT("F");
                    Secondary += TEXT("F");
                    Current++;
                }
                else
                {
                    Primary += TEXT("P");
                    Secondary += TEXT("P");
                }
                break;
                
            case 'Q':
                Primary += TEXT("K");
                Secondary += TEXT("K");
                break;
                
            case 'R':
                Primary += TEXT("R");
                Secondary += TEXT("R");
                break;
                
            case 'S':
                Primary += TEXT("S");
                Secondary += TEXT("S");
                break;
                
            case 'T':
                if (Current + 1 < Length && Upper[Current + 1] == 'H')
                {
                    Primary += TEXT("0");
                    Secondary += TEXT("T");
                    Current++;
                }
                else
                {
                    Primary += TEXT("T");
                    Secondary += TEXT("T");
                }
                break;
                
            case 'V':
                Primary += TEXT("F");
                Secondary += TEXT("F");
                break;
                
            case 'W':
                Primary += TEXT("W");
                Secondary += TEXT("W");
                break;
                
            case 'X':
                Primary += TEXT("KS");
                Secondary += TEXT("KS");
                break;
                
            case 'Z':
                Primary += TEXT("S");
                Secondary += TEXT("S");
                break;
        }
        
        Current++;
    }
    
    Result.Add(Primary);
    if (bDouble && !Secondary.Equals(Primary))
    {
        Result.Add(Secondary);
    }
    
    return Result;
}

bool UREFuzzy::ArePhoneticallyEqual(const FString& A, const FString& B)
{
    FString SoundexA = GenerateSoundex(A);
    FString SoundexB = GenerateSoundex(B);
    
    if (SoundexA.Equals(SoundexB))
        return true;
    
    TArray<FString> MetaphoneA = GenerateMetaphone(A, true);
    TArray<FString> MetaphoneB = GenerateMetaphone(B, true);
    
    for (const FString& MA : MetaphoneA)
    {
        for (const FString& MB : MetaphoneB)
        {
            if (MA.Equals(MB))
                return true;
        }
    }
    
    return false;
}

// ========== TYPO/VISUAL ALGORITHMS ==========

float UREFuzzy::CalculateKeyboardDistance(const FString& A, const FString& B)
{
    InitializeKeyboardLayout();
    
    if (A.IsEmpty() || B.IsEmpty())
        return 0.0f;
    
    if (A.Equals(B))
        return 1.0f;
    
    // Calculate average keyboard distance for character substitutions
    float TotalDistance = 0.0f;
    int32 Comparisons = 0;
    
    int32 MinLen = FMath::Min(A.Len(), B.Len());
    
    for (int32 i = 0; i < MinLen; i++)
    {
        TCHAR CharA = FChar::ToLower(A[i]);
        TCHAR CharB = FChar::ToLower(B[i]);
        
        if (CharA == CharB)
        {
            TotalDistance += 1.0f; // Perfect match
        }
        else
        {
            FVector2D* PosA = KeyboardLayout.Find(CharA);
            FVector2D* PosB = KeyboardLayout.Find(CharB);
            
            if (PosA && PosB)
            {
                float Distance = FVector2D::Distance(*PosA, *PosB);
                // Normalize (max keyboard distance is about 10)
                float Normalized = FMath::Clamp(1.0f - (Distance / 10.0f), 0.0f, 1.0f);
                TotalDistance += Normalized;
            }
            // else: characters not on keyboard, contribute 0
        }
        
        Comparisons++;
    }
    
    // Penalize length differences
    float LengthPenalty = 1.0f - (float)FMath::Abs(A.Len() - B.Len()) / 
                          FMath::Max(A.Len(), B.Len());
    
    if (Comparisons > 0)
    {
        return (TotalDistance / Comparisons) * LengthPenalty;
    }
    
    return 0.0f;
}

bool UREFuzzy::AreVisualConfusables(const FString& A, const FString& B)
{
    if (A.Equals(B))
        return true;
    
    if (A.Len() != B.Len())
        return false;
    
    // Common visual confusables
    static const TMap<TCHAR, TSet<TCHAR>> Confusables = {
        {'0', {'O', 'o'}},
        {'O', {'0', 'o'}},
        {'o', {'0', 'O'}},
        {'1', {'l', 'I', '|'}},
        {'l', {'1', 'I', '|'}},
        {'I', {'1', 'l', '|'}},
        {'|', {'1', 'l', 'I'}},
        {'5', {'S', 's'}},
        {'S', {'5', 's'}},
        {'s', {'5', 'S'}},
        {'2', {'Z', 'z'}},
        {'Z', {'2', 'z'}},
        {'z', {'2', 'Z'}},
        {'8', {'B'}},
        {'B', {'8'}},
        {'6', {'G', 'b'}},
        {'G', {'6'}},
        {'b', {'6', 'd'}},
        {'d', {'b'}},
        {'m', {'n', 'rn'}},
        {'n', {'m'}},
        {'v', {'w'}},
        {'w', {'v'}}
    };
    
    for (int32 i = 0; i < A.Len(); i++)
    {
        if (A[i] == B[i])
            continue;
        
        const TSet<TCHAR>* PossibleConfusions = Confusables.Find(A[i]);
        if (!PossibleConfusions || !PossibleConfusions->Contains(B[i]))
        {
            return false;
        }
    }
    
    return true;
}

// ========== BATCH OPERATIONS ==========

TArray<FString> UREFuzzy::FindBestMatches(const FString& Query,
                                           const TArray<FString>& Candidates,
                                           int32 MaxResults,
                                           float MinSimilarity,
                                           EREFuzzyAlgorithm Algorithm)
{
    TArray<TPair<FString, float>> ScoredCandidates;
    ScoredCandidates.Reserve(Candidates.Num());
    
    for (const FString& Candidate : Candidates)
    {
        float Score = GetSimilarity(Query, Candidate, Algorithm, true);
        
        if (Score >= MinSimilarity)
        {
            ScoredCandidates.Add(TPair<FString, float>(Candidate, Score));
        }
    }
    
    // Sort by score (descending)
    ScoredCandidates.Sort([](const TPair<FString, float>& A, 
                             const TPair<FString, float>& B)
    {
        return A.Value > B.Value;
    });
    
    // Extract top results
    TArray<FString> Results;
    int32 NumResults = FMath::Min(MaxResults, ScoredCandidates.Num());
    Results.Reserve(NumResults);
    
    for (int32 i = 0; i < NumResults; i++)
    {
        Results.Add(ScoredCandidates[i].Key);
    }
    
    return Results;
}