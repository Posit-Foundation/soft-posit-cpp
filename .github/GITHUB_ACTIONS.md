# GitHub Actions Workflows

This document describes the GitHub Actions workflows configured for this project.

## Workflow Architecture

This project uses a modular, reusable GitHub Actions workflow architecture:

1. **Shared Setup Workflow** - Handles environment setup, dependencies, and CMake configuration
2. **Shared Build Workflow** - Handles building, testing, and benchmarking
3. **Feature-specific Workflows** - Call the shared workflows with specific parameters

This approach minimizes duplication and makes workflows easier to maintain.

## Available Workflows

### Reusable Workflows (Internal)

- **shared-setup.yml** - Sets up the environment and configures CMake
- **shared-build.yml** - Builds the project and runs tests/benchmarks

### Main CI Workflow

- **File**: `ci.yml`
- **Purpose**: Main continuous integration workflow that builds the project, runs tests and benchmarks.

### Unit Tests Workflow

- **File**: `unit-tests.yml`
- **Purpose**: Runs all C++ unit tests in both Debug and Release modes.

### Clang-Tidy Static Analysis

- **File**: `clang-tidy.yml`
- **Purpose**: Runs Clang-Tidy to detect potential bugs, style issues, and enforce coding standards through static analysis.

### Code Style

- **File**: `code-style.yml`
- **Purpose**: Enforces consistent code formatting using Clang-Format. Creates a baseline format if one doesn't exist.

### Multi-Platform Build

- **File**: `multi-platform-build.yml`
- **Purpose**: Tests building and running on multiple platforms (Ubuntu, macOS, Windows).

### Benchmarks

- **File**: `benchmarks.yml`
- **Purpose**: Runs performance benchmarks and tracks performance over time.

### Cache Builder

- **File**: `cache-build.yml`
- **Purpose**: Pre-builds and caches artifacts to speed up other workflows.

## Caching System

This project uses GitHub Actions' caching to speed up workflows and avoid unnecessary rebuilds:

1. **Cache-Builder Workflow** (`cache-build.yml`):
   - Builds and caches both Debug and Release builds when code files change
   - Creates global caches that can be reused by other workflows
   - Can be triggered manually via workflow_dispatch

2. **Workflow Optimizations**:
   - Each workflow uses cache keys to find existing build artifacts
   - Builds are skipped if the cache exists, dramatically reducing CI time
   - Cache keys include build type, platform, and a hash of source files

3. **Cache Sharing**:
   - Workflows share caches using consistent cache keys
   - The system maintains separate caches for different build configurations

To effectively use the cache system:

- Run the cache builder workflow after significant changes
- Use the same cache key format (`global-{build-type}-{commit-sha}`) in dependent workflows
- Be patient during the first build, as subsequent builds will be much faster

## Customizing Workflows

To modify the shared workflows:

1. Edit `.github/workflows/shared-setup.yml` for environment setup changes
2. Edit `.github/workflows/shared-build.yml` for build process changes

To add specific configurations to individual workflows, modify the parameters passed to the shared workflows.

## Troubleshooting

If a workflow fails:

1. Check the workflow run for specific error messages
2. Verify that all required dependencies are installed
3. Ensure all environment variables and secrets are properly configured
