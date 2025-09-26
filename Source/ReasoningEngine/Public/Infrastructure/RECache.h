#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HAL/ThreadSafeBool.h"
#include "RECache.generated.h"

/**
 * Cache entry metadata
 */
USTRUCT()
struct FRECacheEntryMeta
{
    GENERATED_BODY()
    
    FDateTime LastAccessed;
    int32 AccessCount = 0;
    int32 SizeBytes = 0;
    float Priority = 1.0f;
};

/**
 * Generic cache entry
 */
template<typename T>
struct TCacheEntry
{
    T Data;
    FRECacheEntryMeta Meta;
    
    TCacheEntry() {}
    TCacheEntry(const T& InData) : Data(InData) 
    {
        Meta.LastAccessed = FDateTime::Now();
        Meta.AccessCount = 1;
    }
};

/**
 * Thread-safe LRU cache implementation
 */
template<typename TKey, typename TValue>
class TThreadSafeCache
{
private:
    TMap<TKey, TCacheEntry<TValue>> Cache;
    mutable FCriticalSection CacheMutex;
    int32 MaxEntries;
    int64 MaxSizeBytes;
    int64 CurrentSizeBytes = 0;
    
public:
    TThreadSafeCache(int32 InMaxEntries = 10000, int64 InMaxSizeBytes = 67108864) // 64MB default
        : MaxEntries(InMaxEntries), MaxSizeBytes(InMaxSizeBytes) {}
    
    bool Get(const TKey& Key, TValue& OutValue)
    {
        FScopeLock Lock(&CacheMutex);
        
        if (TCacheEntry<TValue>* Entry = Cache.Find(Key))
        {
            Entry->Meta.LastAccessed = FDateTime::Now();
            Entry->Meta.AccessCount++;
            OutValue = Entry->Data;
            return true;
        }
        return false;
    }
    
    void Put(const TKey& Key, const TValue& Value, int32 SizeBytes = 0)
    {
        FScopeLock Lock(&CacheMutex);
        
        // Check if we need to evict
        if (Cache.Num() >= MaxEntries || (SizeBytes > 0 && CurrentSizeBytes + SizeBytes > MaxSizeBytes))
        {
            EvictLRU();
        }
        
        TCacheEntry<TValue> Entry(Value);
        Entry.Meta.SizeBytes = SizeBytes;
        
        // Update size tracking
        if (TCacheEntry<TValue>* ExistingEntry = Cache.Find(Key))
        {
            CurrentSizeBytes -= ExistingEntry->Meta.SizeBytes;
        }
        CurrentSizeBytes += SizeBytes;
        
        Cache.Add(Key, Entry);
    }
    
    void Remove(const TKey& Key)
    {
        FScopeLock Lock(&CacheMutex);
        
        if (TCacheEntry<TValue>* Entry = Cache.Find(Key))
        {
            CurrentSizeBytes -= Entry->Meta.SizeBytes;
            Cache.Remove(Key);
        }
    }
    
    void Clear()
    {
        FScopeLock Lock(&CacheMutex);
        Cache.Empty();
        CurrentSizeBytes = 0;
    }
    
    int32 Num() const
    {
        FScopeLock Lock(&CacheMutex);
        return Cache.Num();
    }
    
    int64 GetSizeBytes() const
    {
        FScopeLock Lock(&CacheMutex);
        return CurrentSizeBytes;
    }
    
private:
    void EvictLRU()
    {
        // Find least recently used entry
        TKey KeyToEvict;
        FDateTime OldestTime = FDateTime::MaxValue();
        
        for (const auto& Pair : Cache)
        {
            if (Pair.Value.Meta.LastAccessed < OldestTime)
            {
                OldestTime = Pair.Value.Meta.LastAccessed;
                KeyToEvict = Pair.Key;
            }
        }
        
        Remove(KeyToEvict);
    }
};

/**
 * Cache statistics
 */
USTRUCT(BlueprintType)
struct REASONINGENGINE_API FCacheStatistics
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int32 TotalEntries = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int64 TotalSizeBytes = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int32 HitCount = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int32 MissCount = 0;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    float HitRate = 0.0f;
    
    UPROPERTY(BlueprintReadOnly, Category="Stats")
    int32 EvictionCount = 0;
    
    float GetHitRate() const
    {
        int32 Total = HitCount + MissCount;
        return Total > 0 ? (float)HitCount / (float)Total : 0.0f;
    }
};

/**
 * Unified cache management system
 * Manages multiple typed caches with memory limits and eviction policies
 */
UCLASS(BlueprintType)
class REASONINGENGINE_API URECache : public UObject
{
    GENERATED_BODY()
    
private:
    // ========== CONFIGURATION ==========
    
    UPROPERTY()
    int32 MaxMemoryMB = 128;
    
    UPROPERTY()
    bool bEnableStatistics = true;
    
    // ========== TYPED CACHES ==========
    // Note: These would be template instantiations in actual implementation
    // Using void* for header simplicity
    
    /** String comparison cache */
    void* StringComparisonCache;
    
    /** Token stream cache */
    void* TokenStreamCache;
    
    /** Pattern match cache */
    void* PatternMatchCache;
    
    /** Query result cache */
    void* QueryResultCache;
    
    /** Generic object cache */
    TMap<FName, void*> NamedCaches;
    
    // ========== STATISTICS ==========
    
    mutable TMap<FName, FCacheStatistics> CacheStats;
    mutable FCriticalSection StatsMutex;
    
    mutable FThreadSafeCounter GlobalHits;
    mutable FThreadSafeCounter GlobalMisses;
    mutable FThreadSafeCounter GlobalEvictions;
    
    // ========== MEMORY TRACKING ==========
    
    mutable FThreadSafeCounter64 TotalMemoryUsed;
    
public:
    // ========== LIFECYCLE ==========
    
    URECache();
    virtual ~URECache();
    
    void Initialize();
    void Shutdown();
    bool IsOperational() const { return true; }
    
    // ========== CONFIGURATION ==========
    
    /**
     * Set maximum memory usage
     * @param MaxMB - Maximum memory in megabytes
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache|Config",
              meta=(DisplayName="Set Max Memory"))
    void SetMaxSizeMB(int32 MaxMB);
    
    /**
     * Get maximum memory usage
     * @return Maximum memory in megabytes
     */
    UFUNCTION(BlueprintPure, Category="MM|Cache|Config",
              meta=(DisplayName="Get Max Memory"))
    int32 GetMaxSizeMB() const { return MaxMemoryMB; }
    
    // ========== CACHE OPERATIONS ==========
    
    /**
     * Create a named cache
     * @param CacheName - Unique cache identifier
     * @param MaxEntries - Maximum entries in cache
     * @return true if cache created
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache",
              meta=(DisplayName="Create Cache"))
    bool CreateCache(FName CacheName, int32 MaxEntries = 10000);
    
    /**
     * Remove a named cache
     * @param CacheName - Cache to remove
     * @return true if cache removed
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache",
              meta=(DisplayName="Remove Cache"))
    bool RemoveCache(FName CacheName);
    
    /**
     * Check if cache exists
     * @param CacheName - Cache to check
     * @return true if cache exists
     */
    UFUNCTION(BlueprintPure, Category="MM|Cache",
              meta=(DisplayName="Has Cache"))
    bool HasCache(FName CacheName) const;
    
    /**
     * Clear specific cache
     * @param CacheName - Cache to clear
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache",
              meta=(DisplayName="Clear Cache"))
    void ClearCache(FName CacheName);
    
    /**
     * Clear all caches
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache",
              meta=(DisplayName="Clear All"))
    void ClearAll();
    
    // ========== TYPED ACCESS (C++ Only) ==========
    
    /**
     * Get typed cache
     * @param CacheName - Cache identifier
     * @return Typed cache pointer or nullptr
     */
    template<typename TKey, typename TValue>
    TThreadSafeCache<TKey, TValue>* GetTypedCache(FName CacheName);
    
    /**
     * Create typed cache
     * @param CacheName - Cache identifier
     * @param MaxEntries - Maximum entries
     * @return Created cache pointer
     */
    template<typename TKey, typename TValue>
    TThreadSafeCache<TKey, TValue>* CreateTypedCache(FName CacheName, int32 MaxEntries = 10000);
    
    // ========== STATISTICS ==========
    
    /**
     * Get cache statistics
     * @param CacheName - Cache to query (NAME_None = global)
     * @return Cache statistics
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache|Stats",
              meta=(DisplayName="Get Cache Stats"))
    FCacheStatistics GetCacheStatistics(FName CacheName = NAME_None) const;
    
    /**
     * Get all cache statistics
     * @return Map of cache names to statistics
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache|Stats",
              meta=(DisplayName="Get All Stats"))
    TMap<FName, FCacheStatistics> GetAllStatistics() const;
    
    /**
     * Reset statistics
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache|Stats",
              meta=(DisplayName="Reset Stats"))
    void ResetStatistics();
    
    // ========== MEMORY MANAGEMENT ==========
    
    /**
     * Get current memory usage
     * @return Memory usage in bytes
     */
    UFUNCTION(BlueprintPure, Category="MM|Cache|Memory",
              meta=(DisplayName="Get Memory Usage"))
    int64 GetMemoryUsage() const;
    
    /**
     * Get memory usage by cache
     * @param CacheName - Cache to query
     * @return Memory usage in bytes
     */
    UFUNCTION(BlueprintPure, Category="MM|Cache|Memory",
              meta=(DisplayName="Get Cache Memory"))
    int64 GetCacheMemoryUsage(FName CacheName) const;
    
    /**
     * Trigger memory cleanup
     * @param TargetReductionMB - Target memory to free
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache|Memory",
              meta=(DisplayName="Cleanup Memory"))
    void CleanupMemory(int32 TargetReductionMB = 0);
    
    // ========== EVICTION POLICIES ==========
    
    /**
     * Set eviction policy for cache
     * @param CacheName - Cache to configure
     * @param bUseLRU - Use LRU eviction
     * @param bUseLFU - Use LFU eviction
     * @param bUsePriority - Use priority-based eviction
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache|Eviction",
              meta=(DisplayName="Set Eviction Policy"))
    void SetEvictionPolicy(FName CacheName,
                          bool bUseLRU = true,
                          bool bUseLFU = false,
                          bool bUsePriority = false);
    
    // ========== PERSISTENCE ==========
    
    /**
     * Save cache to disk
     * @param CacheName - Cache to save
     * @param FilePath - Save location
     * @return true if saved successfully
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache|IO",
              meta=(DisplayName="Save Cache"))
    bool SaveCacheToDisk(FName CacheName, const FString& FilePath);
    
    /**
     * Load cache from disk
     * @param CacheName - Cache identifier
     * @param FilePath - Load location
     * @return true if loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category="MM|Cache|IO",
              meta=(DisplayName="Load Cache"))
    bool LoadCacheFromDisk(FName CacheName, const FString& FilePath);
    
    // ========== UTILITIES ==========
    
    /**
     * Track cache hit
     */
    void TrackHit(FName CacheName) const;
    
    /**
     * Track cache miss
     */
    void TrackMiss(FName CacheName) const;
    
    /**
     * Track eviction
     */
    void TrackEviction(FName CacheName) const;
};
