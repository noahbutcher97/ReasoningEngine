# Reasoning Engine

A domain-agnostic semantic processing engine for Unreal Engine 5.6+ that provides advanced text analysis, pattern matching, fuzzy string comparison, knowledge representation, and logical inference capabilities.

## Overview

Reasoning Engine is an Unreal Engine plugin that brings powerful natural language processing and reasoning capabilities to your Unreal Engine projects. It provides a comprehensive suite of tools for semantic analysis, pattern recognition, knowledge management, and logical inference operations.

### Key Features

- **Semantic Processing**: Advanced text tokenization and semantic analysis
- **Fuzzy String Matching**: Multiple algorithms including Levenshtein distance, Jaro-Winkler, Dice coefficient, and Jaccard index
- **Pattern Matching**: Flexible pattern recognition with state machine-based matching
- **Knowledge Base**: Store and query semantic facts, concepts, and relations
- **Inference Engine**: Logical reasoning with hypothesis testing and rule-based inference
- **Caching System**: High-performance caching for optimized operations
- **Blueprint Support**: Full Blueprint integration for designers and artists
- **Thread-Safe**: Singleton architecture with thread-safe operations

## Architecture

The Reasoning Engine is built around a core singleton (`URECore`) that orchestrates several specialized components:

### Core Components

- **REFuzzy**: Fuzzy string matching with multiple algorithms
- **RETokenizer**: Text tokenization and vocabulary management
- **REPatterns**: Pattern matching engine with state machines
- **REKnowledge**: Knowledge base for semantic facts and relations
- **REInferences**: Inference engine for logical reasoning
- **RECache**: Performance-optimized caching system

### Module Structure

```
ReasoningEngine/
├── Source/
│   └── ReasoningEngine/
│       ├── Public/
│       │   ├── Core/              # Core engine and orchestration
│       │   ├── Semantic/          # Semantic processing (fuzzy matching, similarity, vectorization)
│       │   ├── Symbolic/          # Symbolic reasoning (patterns, knowledge, inference)
│       │   ├── Infrastructure/    # Tokenization, caching, utilities
│       │   ├── Configuration/     # Engine settings and configuration
│       │   ├── Interfaces/        # Abstract interfaces for extensibility
│       │   └── BlueprintLibraries/ # Blueprint function libraries
│       ├── Private/               # Implementation files
│       └── Tests/                 # Automated tests
├── Config/                        # Configuration files
└── Resources/                     # Plugin resources (icons, etc.)
```

## Installation

### Requirements

- Unreal Engine 5.6.0 or later
- Supported platforms: Windows (Win64), Mac, Linux

### Installation Steps

1. Clone or download this repository
2. Copy the `ReasoningEngine` folder to your project's `Plugins` directory
3. Regenerate your project files
4. Build your project
5. Enable the plugin in Unreal Engine's Plugin Manager

## Usage

### C++ Usage

#### Getting Started

```cpp
#include "Core/RECore.h"

// Get the singleton instance
URECore* Engine = URECore::Get();

// Check if available
if (URECore::IsAvailable())
{
    // Use the engine...
}
```

#### Fuzzy String Matching

```cpp
#include "Semantic/REFuzzy.h"

// Compare two strings
FREStringMatch Result = REFuzzy::CompareStrings(TEXT("apple"), TEXT("applesauce"), true);

// Access various similarity metrics
float LevenshteinSimilarity = Result.NormalizedLevenshtein;
float JaroWinkler = Result.JaroWinklerSimilarity;
float Dice = Result.DiceCoefficient;
float Jaccard = Result.JaccardIndex;
```

#### Tokenization

```cpp
URETokenizer* Tokenizer = Engine->GetTokenizer();
// Use tokenizer for text processing...
```

#### Pattern Matching

```cpp
UREPatterns* PatternEngine = Engine->GetPatternEngine();
// Use pattern matching...
```

#### Knowledge Base

```cpp
UREKnowledge* KB = Engine->GetKnowledgeBase();
// Store and query semantic knowledge...
```

#### Inference Engine

```cpp
UREInferences* InferenceEngine = Engine->GetInferenceEngine();
// Perform logical reasoning...
```

### Blueprint Usage

The Reasoning Engine is fully Blueprint-compatible. Access the engine through the **Get Semantic Engine** node and use its component accessors:

- **Get Fuzzy Matcher**: Access fuzzy string comparison
- **Get Tokenizer**: Text tokenization
- **Get Pattern Engine**: Pattern matching operations
- **Get Knowledge Base**: Semantic knowledge management
- **Get Inference Engine**: Logical reasoning capabilities
- **Get Cache Manager**: Cache management

### Custom Processors

You can extend the engine by registering custom processors:

```cpp
// Register a custom processor
Engine->RegisterProcessor(TEXT("MyProcessor"), MyProcessorInstance);

// Use the processor
TScriptInterface<IREProcessor> Processor = Engine->GetProcessor(TEXT("MyProcessor"));

// Find best processor for input
FName BestProcessor = Engine->FindBestProcessor(TEXT("some input text"));
```

## Configuration

### Runtime Configuration

Configure the engine at runtime:

```cpp
Engine->ConfigureRuntime(
    64,    // MaxCacheSizeMB
    true,  // bEnableMultithreading
    4      // ThreadPoolSize
);
```

### Configuration Files

Engine-specific configuration is stored in `Config/DefaultReasoningEngine.ini`. This file contains class and struct redirects for backward compatibility.

## Performance & Diagnostics

The engine includes built-in performance monitoring:

```cpp
// Get performance statistics
FString Stats = Engine->GetPerformanceStats();

// Get memory usage
int64 MemoryUsage = Engine->GetMemoryUsage();

// Clear all caches
Engine->ClearAllCaches();

// Perform self-diagnostic check
bool bHealthy = Engine->PerformSelfCheck();
```

## Testing

The plugin includes automated tests for core functionality:

- **REFuzzyTests**: Fuzzy matching algorithm tests
- **RESimilarityTests**: Similarity metric tests
- **REAllTests**: Comprehensive test suite

Run tests through Unreal Engine's Session Frontend or using the Automation system.

## Context Detection

The engine is context-aware and can detect its execution environment:

```cpp
bool bInEditor = Engine->IsInEditorContext();
bool bInRuntime = Engine->IsInRuntimeContext();
bool bInPIE = Engine->IsInPIEContext();
FString Context = Engine->GetCurrentContext();
```

## Thread Safety

The Reasoning Engine uses a thread-safe singleton pattern with critical sections to ensure safe concurrent access. All core operations are designed to be thread-safe.

## Hot Reload Note

**Important**: The Reasoning Engine does not support hot reload due to its singleton architecture. For iterative development:
- Use Live Coding for C++ iteration
- Restart the editor for plugin changes

## License

[Add your license information here]

## Credits

Created by Noah Butcher

## Support

For issues, questions, or contributions, please [add your support information here].

## Version History

### 1.0.0
- Initial release
- Core semantic processing engine
- Fuzzy string matching with multiple algorithms
- Pattern matching system
- Knowledge base and inference engine
- Full Blueprint support
- Thread-safe singleton architecture
