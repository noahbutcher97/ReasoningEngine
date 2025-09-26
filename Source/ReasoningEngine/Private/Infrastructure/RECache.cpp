#include "Core/RECache.h"

URECache::URECache()
{
}

URECache::~URECache()
{
}

void URECache::Initialize()
{
}

void URECache::Shutdown()
{
}

void URECache::SetMaxSizeMB(int32 MaxMB)
{
}

bool URECache::CreateCache(FName CacheName, int32 MaxEntries)
{
    return false;
}

bool URECache::RemoveCache(FName CacheName)
{
    return false;
}

bool URECache::HasCache(FName CacheName) const
{
    return false;
}

void URECache::ClearCache(FName CacheName)
{
}

void URECache::ClearAll()
{
}

FCacheStatistics URECache::GetCacheStatistics(FName CacheName) const
{
    return FCacheStatistics();
}

TMap<FName, FCacheStatistics> URECache::GetAllStatistics() const
{
    return TMap<FName, FCacheStatistics>();
}

void URECache::ResetStatistics()
{
}

int64 URECache::GetMemoryUsage() const
{
    return 0;
}

int64 URECache::GetCacheMemoryUsage(FName CacheName) const
{
    return 0;
}

void URECache::CleanupMemory(int32 TargetReductionMB)
{
}

void URECache::SetEvictionPolicy(FName CacheName, bool bUseLRU, bool bUseLFU, bool bUsePriority)
{
}

bool URECache::SaveCacheToDisk(FName CacheName, const FString& FilePath)
{
    return false;
}

bool URECache::LoadCacheFromDisk(FName CacheName, const FString& FilePath)
{
    return false;
}

void URECache::TrackHit(FName CacheName) const
{
}

void URECache::TrackMiss(FName CacheName) const
{
}

void URECache::TrackEviction(FName CacheName) const
{
}
