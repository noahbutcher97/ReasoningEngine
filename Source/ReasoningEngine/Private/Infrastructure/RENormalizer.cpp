#include "Core/RENormalizer.h"
#include "ReasoningEngine.h"

// Static member initialization
TMap<TCHAR, TCHAR> URENormalizer::AccentMap;
bool URENormalizer::bAccentMapInitialized = false;

void URENormalizer::InitializeAccentMap()
{
    if (bAccentMapInitialized)
    {
        return;
    }
    
    AccentMap.Empty(256);
    
    // Lowercase accents
    AccentMap.Add(TEXT('à'), TEXT('a'));
    AccentMap.Add(TEXT('á'), TEXT('a'));
    AccentMap.Add(TEXT('â'), TEXT('a'));
    AccentMap.Add(TEXT('ã'), TEXT('a'));
    AccentMap.Add(TEXT('ä'), TEXT('a'));
    AccentMap.Add(TEXT('å'), TEXT('a'));
    AccentMap.Add(TEXT('æ'), TEXT('a'));
    
    AccentMap.Add(TEXT('ç'), TEXT('c'));
    
    AccentMap.Add(TEXT('è'), TEXT('e'));
    AccentMap.Add(TEXT('é'), TEXT('e'));
    AccentMap.Add(TEXT('ê'), TEXT('e'));
    AccentMap.Add(TEXT('ë'), TEXT('e'));
    
    AccentMap.Add(TEXT('ì'), TEXT('i'));
    AccentMap.Add(TEXT('í'), TEXT('i'));
    AccentMap.Add(TEXT('î'), TEXT('i'));
    AccentMap.Add(TEXT('ï'), TEXT('i'));
    
    AccentMap.Add(TEXT('ñ'), TEXT('n'));
    
    AccentMap.Add(TEXT('ò'), TEXT('o'));
    AccentMap.Add(TEXT('ó'), TEXT('o'));
    AccentMap.Add(TEXT('ô'), TEXT('o'));
    AccentMap.Add(TEXT('õ'), TEXT('o'));
    AccentMap.Add(TEXT('ö'), TEXT('o'));
    AccentMap.Add(TEXT('ø'), TEXT('o'));
    
    AccentMap.Add(TEXT('ù'), TEXT('u'));
    AccentMap.Add(TEXT('ú'), TEXT('u'));
    AccentMap.Add(TEXT('û'), TEXT('u'));
    AccentMap.Add(TEXT('ü'), TEXT('u'));
    
    AccentMap.Add(TEXT('ý'), TEXT('y'));
    AccentMap.Add(TEXT('ÿ'), TEXT('y'));
    
    // Uppercase accents
    AccentMap.Add(TEXT('À'), TEXT('A'));
    AccentMap.Add(TEXT('Á'), TEXT('A'));
    AccentMap.Add(TEXT('Â'), TEXT('A'));
    AccentMap.Add(TEXT('Ã'), TEXT('A'));
    AccentMap.Add(TEXT('Ä'), TEXT('A'));
    AccentMap.Add(TEXT('Å'), TEXT('A'));
    AccentMap.Add(TEXT('Æ'), TEXT('A'));
    
    AccentMap.Add(TEXT('Ç'), TEXT('C'));
    
    AccentMap.Add(TEXT('È'), TEXT('E'));
    AccentMap.Add(TEXT('É'), TEXT('E'));
    AccentMap.Add(TEXT('Ê'), TEXT('E'));
    AccentMap.Add(TEXT('Ë'), TEXT('E'));
    
    AccentMap.Add(TEXT('Ì'), TEXT('I'));
    AccentMap.Add(TEXT('Í'), TEXT('I'));
    AccentMap.Add(TEXT('Î'), TEXT('I'));
    AccentMap.Add(TEXT('Ï'), TEXT('I'));
    
    AccentMap.Add(TEXT('Ñ'), TEXT('N'));
    
    AccentMap.Add(TEXT('Ò'), TEXT('O'));
    AccentMap.Add(TEXT('Ó'), TEXT('O'));
    AccentMap.Add(TEXT('Ô'), TEXT('O'));
    AccentMap.Add(TEXT('Õ'), TEXT('O'));
    AccentMap.Add(TEXT('Ö'), TEXT('O'));
    AccentMap.Add(TEXT('Ø'), TEXT('O'));
    
    AccentMap.Add(TEXT('Ù'), TEXT('U'));
    AccentMap.Add(TEXT('Ú'), TEXT('U'));
    AccentMap.Add(TEXT('Û'), TEXT('U'));
    AccentMap.Add(TEXT('Ü'), TEXT('U'));
    
    AccentMap.Add(TEXT('Ý'), TEXT('Y'));
    AccentMap.Add(TEXT('Ÿ'), TEXT('Y'));
    
    bAccentMapInitialized = true;
}

// ========== MAIN NORMALIZATION ==========

FString URENormalizer::Normalize(const FString& Text)
{
    return NormalizeWithConfig(Text, GetDefaultConfig());
}

FString URENormalizer::NormalizeWithConfig(const FString& Text, const FReNormalizationConfig& Config)
{
    if (Text.IsEmpty())
    {
        return Text;
    }
    
    FString Result = Text;
    
    // Apply normalization in specific order for best results
    
    // 1. Remove accents first (before case changes)
    if (Config.bRemoveAccents)
    {
        Result = RemoveAccents(Result);
    }
    
    // 2. Case normalization
    if (Config.bLowercase && !Config.bPreserveCase)
    {
        Result = ToLowercase(Result);
    }
    
    // 3. Remove unwanted characters
    if (Config.bRemovePunctuation)
    {
        Result = RemovePunctuation(Result, true);
    }
    
    if (Config.bRemoveNumbers)
    {
        Result = RemoveNumbers(Result);
    }
    
    // 4. Whitespace normalization (do this last)
    if (Config.bTrimWhitespace)
    {
        Result = TrimWhitespace(Result);
    }
    
    if (Config.bCollapseWhitespace)
    {
        Result = CollapseWhitespace(Result);
    }
    
    return Result;
}

FString URENormalizer::NormalizeWithMode(const FString& Text, ERENormalizationMode Mode)
{
    switch (Mode)
    {
        case ERENormalizationMode::None:
            return Text;
            
        case ERENormalizationMode::Lowercase:
            return ToLowercase(Text);
            
        case ERENormalizationMode::Uppercase:
            return ToUppercase(Text);
            
        case ERENormalizationMode::TrimWhitespace:
            return TrimWhitespace(Text);
            
        case ERENormalizationMode::RemoveAccents:
            return RemoveAccents(Text);
            
        case ERENormalizationMode::Full:
            return NormalizeWithConfig(Text, GetDefaultConfig());
            
        default:
            return Text;
    }
}

// ========== INDIVIDUAL OPERATIONS ==========

FString URENormalizer::ToLowercase(const FString& Text)
{
    return Text.ToLower();
}

FString URENormalizer::ToUppercase(const FString& Text)
{
    return Text.ToUpper();
}

FString URENormalizer::TrimWhitespace(const FString& Text)
{
    return Text.TrimStartAndEnd();
}

FString URENormalizer::CollapseWhitespace(const FString& Text)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    bool bLastWasSpace = false;
    
    for (TCHAR Ch : Text)
    {
        if (FChar::IsWhitespace(Ch))
        {
            if (!bLastWasSpace)
            {
                Result.AppendChar(TEXT(' '));
                bLastWasSpace = true;
            }
        }
        else
        {
            Result.AppendChar(Ch);
            bLastWasSpace = false;
        }
    }
    
    return Result;
}

FString URENormalizer::RemoveAccents(const FString& Text)
{
    InitializeAccentMap();
    
    FString Result;
    Result.Reserve(Text.Len());
    
    for (TCHAR Ch : Text)
    {
        TCHAR NormalizedChar = RemoveAccentFromChar(Ch);
        Result.AppendChar(NormalizedChar);
    }
    
    return Result;
}

FString URENormalizer::RemovePunctuation(const FString& Text, bool bKeepSpaces)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    for (TCHAR Ch : Text)
    {
        if (FChar::IsAlnum(Ch) || (bKeepSpaces && FChar::IsWhitespace(Ch)))
        {
            Result.AppendChar(Ch);
        }
    }
    
    return Result;
}

FString URENormalizer::RemoveNumbers(const FString& Text)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    for (TCHAR Ch : Text)
    {
        if (!FChar::IsDigit(Ch))
        {
            Result.AppendChar(Ch);
        }
    }
    
    return Result;
}

FString URENormalizer::KeepAlphanumeric(const FString& Text, bool bKeepSpaces)
{
    FString Result;
    Result.Reserve(Text.Len());
    
    for (TCHAR Ch : Text)
    {
        if (FChar::IsAlnum(Ch) || (bKeepSpaces && FChar::IsWhitespace(Ch)))
        {
            Result.AppendChar(Ch);
        }
    }
    
    return Result;
}

// ========== CHARACTER OPERATIONS ==========

FString URENormalizer::NormalizeChar(TCHAR Char, const FReNormalizationConfig& Config)
{
    // Handle accent removal
    if (Config.bRemoveAccents)
    {
        Char = RemoveAccentFromChar(Char);
    }
    
    // Handle case conversion
    if (Config.bLowercase && !Config.bPreserveCase)
    {
        Char = FChar::ToLower(Char);
    }
    
    // Handle character filtering
    if (Config.bRemovePunctuation && FChar::IsPunctuation(Char))
    {
        return TEXT("");
    }
    
    if (Config.bRemoveNumbers && FChar::IsDigit(Char))
    {
        return TEXT("");
    }
    
    // Handle whitespace
    if (FChar::IsWhitespace(Char))
    {
        if (Config.bCollapseWhitespace)
        {
            return TEXT(" ");
        }
    }
    
    return FString::Chr(Char);
}

TCHAR URENormalizer::RemoveAccentFromChar(TCHAR Char)
{
    InitializeAccentMap();
    
    if (TCHAR* Normalized = AccentMap.Find(Char))
    {
        return *Normalized;
    }
    
    return Char;
}

bool URENormalizer::IsAccentedChar(TCHAR Char)
{
    InitializeAccentMap();
    return AccentMap.Contains(Char);
}

// ========== CONFIGURATIONS ==========

FReNormalizationConfig URENormalizer::GetDefaultConfig()
{
    FReNormalizationConfig Config;
    Config.bLowercase = true;
    Config.bTrimWhitespace = true;
    Config.bRemoveAccents = true;
    Config.bCollapseWhitespace = true;
    Config.bRemovePunctuation = false;
    Config.bRemoveNumbers = false;
    Config.bPreserveCase = false;
    
    return Config;
}

FReNormalizationConfig URENormalizer::GetAggressiveConfig()
{
    FReNormalizationConfig Config;
    Config.bLowercase = true;
    Config.bTrimWhitespace = true;
    Config.bRemoveAccents = true;
    Config.bCollapseWhitespace = true;
    Config.bRemovePunctuation = true;
    Config.bRemoveNumbers = true;
    Config.bPreserveCase = false;
    
    return Config;
}

FReNormalizationConfig URENormalizer::GetMinimalConfig()
{
    FReNormalizationConfig Config;
    Config.bLowercase = false;
    Config.bTrimWhitespace = true;
    Config.bRemoveAccents = false;
    Config.bCollapseWhitespace = false;
    Config.bRemovePunctuation = false;
    Config.bRemoveNumbers = false;
    Config.bPreserveCase = true;
    
    return Config;
}