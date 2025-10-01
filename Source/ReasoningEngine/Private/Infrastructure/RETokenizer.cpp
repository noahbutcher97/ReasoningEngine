// Source/ReasoningEngine/Private/Infrastructure/RETokenizer.cpp
#include "Infrastructure/RETokenizer.h"
#include "Infrastructure/RENormalizer.h"
#include "Semantic/REFuzzy.h"  // For typo generation
#include "ReasoningEngine.h"

// ========== PRIMARY TOKENIZATION ==========

FRETokenStream RETokenizer::Tokenize(const FString& Text)
{
    FRETokenizerConfig DefaultConfig;
    return TokenizeWithConfig(Text, DefaultConfig);
}

FRETokenStream RETokenizer::TokenizeWithConfig(const FString& Text, const FRETokenizerConfig& Config)
{
    FRETokenStream Result;
    Result.OriginalText = Text;
    
    if (Text.IsEmpty())
    {
        return Result;
    }
    
    // Detect naming convention if requested
    if (Config.bDetectNamingConvention)
    {
        Result.DetectedConvention = DetectNamingConvention(Text);
    }
    
    // Primary tokenization based on delimiters
    TArray<FString> RawTokens = SplitByDelimiters(Text, Config.Delimiters, Config.bPreserveDelimiters);
    
    // Process each raw token
    int32 CurrentPosition = 0;
    for (const FString& RawToken : RawTokens)
    {
        if (RawToken.IsEmpty())
        {
            continue;
        }
        
        TArray<FString> SubTokens;
        
        // Further split based on naming convention
        if (Config.bSplitCamelCase && 
            (Result.DetectedConvention == ERENamingConvention::CamelCase || 
             Result.DetectedConvention == ERENamingConvention::PascalCase))
        {
            SubTokens = SplitCamelCase(RawToken);
        }
        else if (Config.bSplitNumbers)
        {
            SubTokens = SplitAlphanumeric(RawToken);
        }
        else
        {
            SubTokens.Add(RawToken);
        }
        
        // Create tokens from subtokens
        for (const FString& SubToken : SubTokens)
        {
            // Apply length filters
            if (SubToken.Len() < Config.MinTokenLength)
            {
                continue;
            }
            
            FString FinalToken = SubToken;
            if (SubToken.Len() > Config.MaxTokenLength)
            {
                FinalToken = SubToken.Left(Config.MaxTokenLength);
            }
            
            // Normalize case if requested
            if (Config.bNormalizeCase)
            {
                FinalToken = RENormalizer::ToLowercase(FinalToken);
            }
            
            // Create token
            FREToken Token = CreateToken(
                FinalToken,
                CurrentPosition,
                CurrentPosition + FinalToken.Len(),
                ClassifyTokenType(FinalToken)
            );
            
            // Generate variants if requested
            if (Config.bGenerateVariants)
            {
                Token.Variants = GenerateVariants(
                    FinalToken,
                    true,  // Include typos
                    Config.bExpandAbbreviations,
                    Config.bExpandAbbreviations
                );
            }
            
            Result.Tokens.Add(Token);
            CurrentPosition += FinalToken.Len() + 1;  // +1 for assumed space
        }
    }
    
    return Result;
}

FRETokenStream RETokenizer::TokenizeWithKnowledge(
    const FString& Text,
    const FRETokenizerConfig& Config,
    const FREKnowledgeBase& Knowledge)
{
    // Note: This would integrate with Symbolic Knowledge
    // For now, just use standard tokenization
    FRETokenStream Result = TokenizeWithConfig(Text, Config);
    
    // Would enrich tokens with knowledge here
    // e.g., mark tokens that match known entities
    
    return Result;
}

// ========== TEXT SPLITTING ==========

TArray<FString> RETokenizer::SplitByDelimiters(
    const FString& Text,
    const FString& Delimiters,
    bool bKeepDelimiters)
{
    TArray<FString> Result;
    
    if (Text.IsEmpty())
    {
        return Result;
    }
    
    FString CurrentToken;
    
    for (TCHAR Char : Text)
    {
        if (IsDelimiter(Char, Delimiters))
        {
            // Add current token if not empty
            if (!CurrentToken.IsEmpty())
            {
                Result.Add(CurrentToken);
                CurrentToken.Empty();
            }
            
            // Add delimiter as token if requested
            if (bKeepDelimiters)
            {
                Result.Add(FString::Chr(Char));
            }
        }
        else
        {
            CurrentToken.AppendChar(Char);
        }
    }
    
    // Add final token
    if (!CurrentToken.IsEmpty())
    {
        Result.Add(CurrentToken);
    }
    
    return Result;
}

TArray<FString> RETokenizer::SplitCamelCase(const FString& Text)
{
    TArray<FString> Result;
    
    if (Text.IsEmpty())
    {
        return Result;
    }
    
    FString CurrentWord;
    
    for (int32 i = 0; i < Text.Len(); i++)
    {
        TCHAR Current = Text[i];
        TCHAR Prev = (i > 0) ? Text[i - 1] : 0;
        TCHAR Next = (i < Text.Len() - 1) ? Text[i + 1] : 0;
        
        if (i > 0 && IsCamelCaseBoundary(Prev, Current, Next))
        {
            if (!CurrentWord.IsEmpty())
            {
                Result.Add(CurrentWord);
                CurrentWord.Empty();
            }
        }
        
        CurrentWord.AppendChar(Current);
    }
    
    if (!CurrentWord.IsEmpty())
    {
        Result.Add(CurrentWord);
    }
    
    return Result;
}

TArray<FString> RETokenizer::SplitSnakeCase(const FString& Text)
{
    return SplitByDelimiters(Text, TEXT("_"), false);
}

TArray<FString> RETokenizer::SplitKebabCase(const FString& Text)
{
    return SplitByDelimiters(Text, TEXT("-"), false);
}

TArray<FString> RETokenizer::SplitAlphanumeric(const FString& Text)
{
    TArray<FString> Result;
    FString CurrentToken;
    bool bInNumber = false;
    
    for (TCHAR Char : Text)
    {
        bool bIsDigit = FChar::IsDigit(Char);
        
        // Transition between alpha and numeric
        if (!CurrentToken.IsEmpty() && bIsDigit != bInNumber)
        {
            Result.Add(CurrentToken);
            CurrentToken.Empty();
        }
        
        CurrentToken.AppendChar(Char);
        bInNumber = bIsDigit;
    }
    
    if (!CurrentToken.IsEmpty())
    {
        Result.Add(CurrentToken);
    }
    
    return Result;
}

// ========== NAMING CONVENTION ==========

ERENamingConvention RETokenizer::DetectNamingConvention(const FString& Text)
{
    if (Text.IsEmpty())
    {
        return ERENamingConvention::Unknown;
    }
    
    bool bHasUnderscore = Text.Contains(TEXT("_"));
    bool bHasHyphen = Text.Contains(TEXT("-"));
    bool bHasSpace = Text.Contains(TEXT(" "));
    bool bHasUppercase = false;
    bool bHasLowercase = false;
    bool bStartsWithUpper = false;
    
    for (int32 i = 0; i < Text.Len(); i++)
    {
        TCHAR Char = Text[i];
        if (FChar::IsUpper(Char))
        {
            bHasUppercase = true;
            if (i == 0) bStartsWithUpper = true;
        }
        if (FChar::IsLower(Char))
        {
            bHasLowercase = true;
        }
    }
    
    // Check patterns
    if (bHasSpace)
    {
        return ERENamingConvention::Natural;
    }
    else if (bHasUnderscore)
    {
        return bHasUppercase ? ERENamingConvention::UpperCase : ERENamingConvention::SnakeCase;
    }
    else if (bHasHyphen)
    {
        return ERENamingConvention::KebabCase;
    }
    else if (bHasUppercase && bHasLowercase)
    {
        return bStartsWithUpper ? ERENamingConvention::PascalCase : ERENamingConvention::CamelCase;
    }
    else if (!bHasUppercase)
    {
        return ERENamingConvention::LowerCase;
    }
    else if (!bHasLowercase)
    {
        return ERENamingConvention::UpperCase;
    }
    
    return ERENamingConvention::Mixed;
}

FString RETokenizer::ConvertNamingConvention(
    const FString& Text,
    ERENamingConvention FromConvention,
    ERENamingConvention ToConvention)
{
    if (FromConvention == ToConvention)
    {
        return Text;
    }
    
    // First, break into components
    TArray<FString> Components;
    
    switch (FromConvention)
    {
        case ERENamingConvention::CamelCase:
        case ERENamingConvention::PascalCase:
            Components = SplitCamelCase(Text);
            break;
            
        case ERENamingConvention::SnakeCase:
        case ERENamingConvention::UpperCase:
            Components = SplitSnakeCase(Text);
            break;
            
        case ERENamingConvention::KebabCase:
            Components = SplitKebabCase(Text);
            break;
            
        case ERENamingConvention::Natural:
            Components = SplitByDelimiters(Text, TEXT(" "), false);
            break;
            
        default:
            Components.Add(Text);
            break;
    }
    
    // Convert to target convention
    FString Result;
    
    switch (ToConvention)
    {
        case ERENamingConvention::CamelCase:
            for (int32 i = 0; i < Components.Num(); i++)
            {
                FString Component = Components[i].ToLower();
                if (i > 0 && Component.Len() > 0)
                {
                    Component[0] = FChar::ToUpper(Component[0]);
                }
                Result += Component;
            }
            break;
            
        case ERENamingConvention::PascalCase:
            for (const FString& Component : Components)
            {
                FString Comp = Component.ToLower();
                if (Comp.Len() > 0)
                {
                    Comp[0] = FChar::ToUpper(Comp[0]);
                }
                Result += Comp;
            }
            break;
            
        case ERENamingConvention::SnakeCase:
            Result = FString::Join(Components, TEXT("_")).ToLower();
            break;
            
        case ERENamingConvention::KebabCase:
            Result = FString::Join(Components, TEXT("-")).ToLower();
            break;
            
        case ERENamingConvention::UpperCase:
            Result = FString::Join(Components, TEXT("_")).ToUpper();
            break;
            
        case ERENamingConvention::LowerCase:
            Result = FString::Join(Components, TEXT("")).ToLower();
            break;
            
        case ERENamingConvention::Natural:
            Result = FString::Join(Components, TEXT(" "));
            break;
            
        default:
            Result = Text;
            break;
    }
    
    return Result;
}

FRETokenStream RETokenizer::TokenizeAnimationName(const FString& AnimationName)
{
    FRETokenizerConfig Config;
    Config.Delimiters = TEXT("_-");
    Config.bSplitNumbers = true;
    Config.bDetectNamingConvention = true;
    Config.bNormalizeCase = false;  // Preserve case for animation names
    
    FRETokenStream Result = TokenizeWithConfig(AnimationName, Config);
    
    // Mark common animation prefixes
    if (Result.Tokens.Num() > 0)
    {
        const FString& First = Result.Tokens[0].Text;
        if (First == TEXT("MM") || First == TEXT("A") || First == TEXT("BS"))
        {
            Result.Tokens[0].Type = ERETokenType::Keyword;
            Result.Tokens[0].Metadata.Add(TEXT("AnimationPrefix"), TEXT("true"));
        }
    }
    
    // Mark numeric suffixes
    if (Result.Tokens.Num() > 0)
    {
        FREToken& Last = Result.Tokens.Last();
        if (Last.Type == ERETokenType::Number)
        {
            Last.Metadata.Add(TEXT("AnimationVariant"), TEXT("true"));
        }
    }
    
    return Result;
}

// ========== TOKEN CREATION ==========

FREToken RETokenizer::CreateToken(
    const FString& Text,
    int32 StartIndex,
    int32 EndIndex,
    ERETokenType Type)
{
    FREToken Token;
    Token.Text = Text;
    Token.NormalizedText = RENormalizer::ToLowercase(Text);
    Token.Type = Type;
    Token.StartIndex = StartIndex;
    Token.EndIndex = EndIndex;
    Token.Weight = 1.0f;
    Token.Confidence = 1.0f;
    
    return Token;
}

ERETokenType RETokenizer::ClassifyTokenType(const FString& Text)
{
    if (Text.IsEmpty())
    {
        return ERETokenType::Unknown;
    }
    
    // Check if all digits
    bool bAllDigits = true;
    bool bAllAlpha = true;
    bool bHasWhitespace = false;
    
    for (TCHAR Char : Text)
    {
        if (!FChar::IsDigit(Char)) bAllDigits = false;
        if (!FChar::IsAlpha(Char)) bAllAlpha = false;
        if (FChar::IsWhitespace(Char)) bHasWhitespace = true;
    }
    
    if (bAllDigits)
    {
        return ERETokenType::Number;
    }
    else if (bAllAlpha)
    {
        return ERETokenType::Word;
    }
    else if (bHasWhitespace)
    {
        return ERETokenType::Whitespace;
    }
    else if (Text.Len() == 1 && FChar::IsPunctuation(Text[0]))
    {
        return ERETokenType::Punctuation;
    }
    else if (Text.Contains(TEXT("_")))
    {
        return ERETokenType::SnakeCase;
    }
    else
    {
        // Check for mixed alphanumeric (identifier-like)
        bool bHasAlpha = false;
        bool bHasDigit = false;
        
        for (TCHAR Char : Text)
        {
            if (FChar::IsAlpha(Char)) bHasAlpha = true;
            if (FChar::IsDigit(Char)) bHasDigit = true;
        }
        
        if (bHasAlpha && bHasDigit)
        {
            return ERETokenType::Identifier;
        }
    }
    
    return ERETokenType::Unknown;
}

// ========== VARIANT GENERATION ==========

TArray<FString> RETokenizer::GenerateVariants(
    const FString& Token,
    bool bIncludeTypos,
    bool bIncludeAbbreviations,
    bool bIncludeExpansions)
{
    TArray<FString> Variants;
    
    if (bIncludeTypos)
    {
        Variants.Append(GenerateTypos(Token, 1));
    }
    
    if (bIncludeAbbreviations)
    {
        Variants.Append(GenerateAbbreviations(Token));
    }
    
    if (bIncludeExpansions)
    {
        Variants.Append(GenerateExpansions(Token));
    }
    
    // Remove duplicates
    TSet<FString> UniqueVariants(Variants);
    Variants = UniqueVariants.Array();
    
    return Variants;
}

TArray<FString> RETokenizer::GenerateTypos(const FString& Word, int32 MaxDistance)
{
    TArray<FString> Typos;
    
    if (Word.IsEmpty() || MaxDistance <= 0)
    {
        return Typos;
    }
    
    // Common typo patterns
    
    // 1. Character deletion
    for (int32 i = 0; i < Word.Len(); i++)
    {
        FString Typo = Word.Left(i) + Word.Mid(i + 1);
        Typos.Add(Typo);
    }
    
    // 2. Character transposition
    for (int32 i = 0; i < Word.Len() - 1; i++)
    {
        FString Typo = Word;
        Swap(Typo[i], Typo[i + 1]);
        Typos.Add(Typo);
    }
    
    // 3. Character substitution (keyboard neighbors)
    // This would ideally use REFuzzy::CalculateKeyboardDistance
    // For now, just do simple vowel substitutions
    static const TMap<TCHAR, TArray<TCHAR>> Substitutions = {
        {TEXT('a'), {TEXT('e'), TEXT('o')}},
        {TEXT('e'), {TEXT('a'), TEXT('i')}},
        {TEXT('i'), {TEXT('e'), TEXT('y')}},
        {TEXT('o'), {TEXT('a'), TEXT('u')}},
        {TEXT('u'), {TEXT('o'), TEXT('i')}}
    };
    
    for (int32 i = 0; i < Word.Len(); i++)
    {
        TCHAR Char = FChar::ToLower(Word[i]);
        if (const TArray<TCHAR>* Subs = Substitutions.Find(Char))
        {
            for (TCHAR Sub : *Subs)
            {
                FString Typo = Word;
                Typo[i] = Word[i] == Char ? Sub : FChar::ToUpper(Sub);
                Typos.Add(Typo);
            }
        }
    }
    
    return Typos;
}

TArray<FString> RETokenizer::GenerateAbbreviations(const FString& Word)
{
    TArray<FString> Abbreviations;
    
    if (Word.Len() <= 2)
    {
        return Abbreviations;
    }
    
    // First letter abbreviation
    Abbreviations.Add(Word.Left(1).ToLower());
    
    // First few letters
    Abbreviations.Add(Word.Left(3).ToLower());
    
    // Consonants only (for longer words)
    if (Word.Len() > 5)
    {
        FString Consonants;
        for (TCHAR Char : Word)
        {
            if (RENormalizer::IsConsonant(Char))
            {
                Consonants.AppendChar(FChar::ToLower(Char));
            }
        }
        if (Consonants.Len() > 1 && Consonants.Len() < Word.Len())
        {
            Abbreviations.Add(Consonants);
        }
    }
    
    return Abbreviations;
}

TArray<FString> RETokenizer::GenerateExpansions(const FString& Abbreviation)
{
    TArray<FString> Expansions;
    
    // Common programming abbreviations
    static const TMap<FString, TArray<FString>> CommonExpansions = {
        {TEXT("cfg"), {TEXT("config"), TEXT("configuration")}},
        {TEXT("mgr"), {TEXT("manager")}},
        {TEXT("ctrl"), {TEXT("control"), TEXT("controller")}},
        {TEXT("btn"), {TEXT("button")}},
        {TEXT("dlg"), {TEXT("dialog")}},
        {TEXT("msg"), {TEXT("message")}},
        {TEXT("ptr"), {TEXT("pointer")}},
        {TEXT("ref"), {TEXT("reference")}},
        {TEXT("anim"), {TEXT("animation")}},
        {TEXT("char"), {TEXT("character")}},
        {TEXT("pos"), {TEXT("position")}},
        {TEXT("rot"), {TEXT("rotation")}},
        {TEXT("vel"), {TEXT("velocity")}},
        {TEXT("accel"), {TEXT("acceleration")}},
        {TEXT("fwd"), {TEXT("forward")}},
        {TEXT("bwd"), {TEXT("backward")}}
    };
    
    FString Lower = Abbreviation.ToLower();
    if (const TArray<FString>* Found = CommonExpansions.Find(Lower))
    {
        Expansions = *Found;
    }
    
    return Expansions;
}

// ========== TOKEN ANALYSIS ==========

TArray<FRETokenGroup> RETokenizer::GroupTokensBySimilarity(
    const TArray<FREToken>& Tokens,
    float SimilarityThreshold)
{
    TArray<FRETokenGroup> Groups;
    
    // Simple clustering by similarity
    // This would ideally use REFuzzy for similarity calculation
    
    for (const FREToken& Token : Tokens)
    {
        bool bAddedToGroup = false;
        
        for (FRETokenGroup& Group : Groups)
        {
            // Check similarity with group canonical form
            // For now, just check exact match
            if (Group.CanonicalForm == Token.NormalizedText)
            {
                Group.Tokens.Add(Token);
                Group.Frequency++;
                bAddedToGroup = true;
                break;
            }
        }
        
        if (!bAddedToGroup)
        {
            FRETokenGroup NewGroup;
            NewGroup.GroupName = Token.Text;
            NewGroup.CanonicalForm = Token.NormalizedText;
            NewGroup.Tokens.Add(Token);
            NewGroup.Frequency = 1;
            NewGroup.AverageSimilarity = 1.0f;
            Groups.Add(NewGroup);
        }
    }
    
    return Groups;
}

TArray<FString> RETokenizer::FindCompoundWords(
    const FRETokenStream& Stream,
    const TArray<FREVocabularyEntry>& Vocabularies)
{
    TArray<FString> Compounds;
    
    // Look for adjacent tokens that form known compounds
    for (int32 i = 0; i < Stream.Tokens.Num() - 1; i++)
    {
        FString Compound = Stream.Tokens[i].Text + Stream.Tokens[i + 1].Text;
        FString CompoundWithSpace = Stream.Tokens[i].Text + TEXT(" ") + Stream.Tokens[i + 1].Text;
        
        // Check if compound exists in vocabulary
        for (const FREVocabularyEntry& Entry : Vocabularies)
        {
            if (Entry.Term == Compound || Entry.Term == CompoundWithSpace)
            {
                Compounds.Add(Entry.Term);
                break;
            }
        }
    }
    
    return Compounds;
}

FRETokenStream RETokenizer::CalculateTokenWeights(
    const FRETokenStream& Stream,
    const TMap<FString, float>* DocumentFrequencies)
{
    FRETokenStream Result = Stream;
    
    // Calculate term frequency for this stream
    TMap<FString, int32> TermFrequency;
    for (const FREToken& Token : Result.Tokens)
    {
        TermFrequency.FindOrAdd(Token.NormalizedText)++;
    }
    
    // Calculate weights
    int32 TotalTokens = Result.Tokens.Num();
    
    for (FREToken& Token : Result.Tokens)
    {
        float TF = static_cast<float>(TermFrequency[Token.NormalizedText]) / static_cast<float>(TotalTokens);
        
        if (DocumentFrequencies)
        {
            // TF-IDF calculation
            if (const float* IDF = DocumentFrequencies->Find(Token.NormalizedText))
            {
                Token.Weight = TF * (*IDF);
            }
            else
            {
                // Unknown term, give it high IDF
                Token.Weight = TF * 10.0f;
            }
        }
        else
        {
            // Just use term frequency
            Token.Weight = TF;
        }
    }
    
    return Result;
}

// ========== STOP WORDS ==========

FRETokenStream RETokenizer::FilterStopWords(
    const FRETokenStream& Stream,
    const TSet<FString>& StopWords)
{
    FRETokenStream Result = Stream;
    Result.Tokens.RemoveAll([&StopWords](const FREToken& Token)
    {
        return StopWords.Contains(Token.NormalizedText);
    });
    
    return Result;
}

TSet<FString> RETokenizer::GetDefaultStopWords()
{
    static const TSet<FString> DefaultStopWords = {
        TEXT("a"), TEXT("an"), TEXT("and"), TEXT("are"), TEXT("as"), TEXT("at"),
        TEXT("be"), TEXT("been"), TEXT("by"), TEXT("for"), TEXT("from"),
        TEXT("has"), TEXT("have"), TEXT("he"), TEXT("in"), TEXT("is"), TEXT("it"),
        TEXT("its"), TEXT("of"), TEXT("on"), TEXT("that"), TEXT("the"), TEXT("to"),
        TEXT("was"), TEXT("will"), TEXT("with"), TEXT("the"), TEXT("this"),
        TEXT("but"), TEXT("they"), TEXT("we"), TEXT("can"), TEXT("her"), TEXT("him"),
        TEXT("his"), TEXT("how"), TEXT("if"), TEXT("may"), TEXT("or"), TEXT("she"),
        TEXT("their"), TEXT("them"), TEXT("then"), TEXT("there"), TEXT("these"),
        TEXT("those"), TEXT("what"), TEXT("when"), TEXT("where"), TEXT("which"),
        TEXT("who"), TEXT("why"), TEXT("you"), TEXT("your")
    };
    
    return DefaultStopWords;
}

bool RETokenizer::IsDefaultStopWord(const FString& Word)
{
    static const TSet<FString> StopWords = GetDefaultStopWords();
    return StopWords.Contains(Word.ToLower());
}

// ========== N-GRAMS ==========

TArray<FString> RETokenizer::GenerateCharacterNGrams(const FString& Text, int32 N)
{
    TArray<FString> NGrams;
    
    if (Text.Len() < N || N <= 0)
    {
        return NGrams;
    }
    
    for (int32 i = 0; i <= Text.Len() - N; i++)
    {
        NGrams.Add(Text.Mid(i, N));
    }
    
    return NGrams;
}

TArray<FString> RETokenizer::GenerateWordNGrams(const TArray<FREToken>& Tokens, int32 N)
{
    TArray<FString> NGrams;
    
    if (Tokens.Num() < N || N <= 0)
    {
        return NGrams;
    }
    
    for (int32 i = 0; i <= Tokens.Num() - N; i++)
    {
        TArray<FString> GramParts;
        for (int32 j = 0; j < N; j++)
        {
            GramParts.Add(Tokens[i + j].Text);
        }
        NGrams.Add(FString::Join(GramParts, TEXT(" ")));
    }
    
    return NGrams;
}

// ========== UTILITY FUNCTIONS ==========

FRETokenStream RETokenizer::MergeAdjacentTokens(const FRETokenStream& Stream)
{
    FRETokenStream Result = Stream;
    
    if (Result.Tokens.Num() <= 1)
    {
        return Result;
    }
    
    TArray<FREToken> MergedTokens;
    FREToken CurrentMerged = Result.Tokens[0];
    
    for (int32 i = 1; i < Result.Tokens.Num(); i++)
    {
        const FREToken& NextToken = Result.Tokens[i];
        
        if (NextToken.Type == CurrentMerged.Type)
        {
            // Merge tokens
            CurrentMerged.Text += NextToken.Text;
            CurrentMerged.NormalizedText += NextToken.NormalizedText;
            CurrentMerged.EndIndex = NextToken.EndIndex;
        }
        else
        {
            // Save current and start new
            MergedTokens.Add(CurrentMerged);
            CurrentMerged = NextToken;
        }
    }
    
    // Add final token
    MergedTokens.Add(CurrentMerged);
    
    Result.Tokens = MergedTokens;
    return Result;
}

FRETokenStream RETokenizer::SplitLongTokens(const FRETokenStream& Stream, int32 MaxLength)
{
    FRETokenStream Result = Stream;
    TArray<FREToken> SplitTokens;
    
    for (const FREToken& Token : Result.Tokens)
    {
        if (Token.Text.Len() > MaxLength)
        {
            // Split into chunks
            int32 NumChunks = (Token.Text.Len() + MaxLength - 1) / MaxLength;
            for (int32 i = 0; i < NumChunks; i++)
            {
                FREToken Chunk = Token;
                Chunk.Text = Token.Text.Mid(i * MaxLength, MaxLength);
                Chunk.NormalizedText = RENormalizer::ToLowercase(Chunk.Text);
                Chunk.StartIndex = Token.StartIndex + (i * MaxLength);
                Chunk.EndIndex = FMath::Min(Chunk.StartIndex + MaxLength, Token.EndIndex);
                SplitTokens.Add(Chunk);
            }
        }
        else
        {
            SplitTokens.Add(Token);
        }
    }
    
    Result.Tokens = SplitTokens;
    return Result;
}

FRETokenStream RETokenizer::RemoveShortTokens(const FRETokenStream& Stream, int32 MinLength)
{
    FRETokenStream Result = Stream;
    Result.Tokens.RemoveAll([MinLength](const FREToken& Token)
    {
        return Token.Text.Len() < MinLength;
    });
    
    return Result;
}

FString RETokenizer::TokenStreamToString(const FRETokenStream& Stream, const FString& Separator)
{
    TArray<FString> TokenStrings;
    for (const FREToken& Token : Stream.Tokens)
    {
        TokenStrings.Add(Token.Text);
    }
    
    return FString::Join(TokenStrings, *Separator);
}

FRETokenStream RETokenizer::CreateTokenStream(const TArray<FString>& Strings)
{
    FRETokenStream Stream;
    int32 Position = 0;
    
    for (const FString& Str : Strings)
    {
        FREToken Token = CreateToken(Str, Position, Position + Str.Len());
        Stream.Tokens.Add(Token);
        Position += Str.Len() + 1;  // +1 for assumed space
    }
    
    return Stream;
}

// ========== INTERNAL HELPERS ==========

bool RETokenizer::IsCamelCaseBoundary(TCHAR Prev, TCHAR Current, TCHAR Next)
{
    // Lowercase to uppercase transition
    if (FChar::IsLower(Prev) && FChar::IsUpper(Current))
    {
        return true;
    }
    
    // Uppercase followed by uppercase then lowercase (acronym boundary)
    if (FChar::IsUpper(Prev) && FChar::IsUpper(Current) && FChar::IsLower(Next))
    {
        return true;
    }
    
    return false;
}

bool RETokenizer::IsCommonAbbreviation(const FString& Text)
{
    static const TSet<FString> CommonAbbreviations = {
        TEXT("Mr"), TEXT("Mrs"), TEXT("Dr"), TEXT("Ms"), TEXT("Prof"),
        TEXT("Inc"), TEXT("Ltd"), TEXT("Co"), TEXT("Corp"),
        TEXT("vs"), TEXT("etc"), TEXT("eg"), TEXT("ie")
    };
    
    return CommonAbbreviations.Contains(Text);
}