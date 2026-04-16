# Build.ps1 - Build script for Deform project

param(
    [switch]$Release,
    [switch]$Clean,
    [switch]$Verbose,
    [string]$Target
)

# Error handling
$ErrorActionPreference = "Continue"

# Colors for output
$ErrorColor = "Red"
$SuccessColor = "Green"
$InfoColor = "Cyan"
$WarningColor = "Yellow"

function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor $InfoColor
}

function Write-Success {
    param([string]$Message)
    Write-Host "[SUCCESS] $Message" -ForegroundColor $SuccessColor
}

function Write-Error {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor $ErrorColor
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[WARNING] $Message" -ForegroundColor $WarningColor
}

# Get script directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $ScriptDir

Write-Info "Starting build process for Deform project"
Write-Info "Working directory: $ScriptDir"

# Check if Rust is installed
function Test-RustInstalled {
    try {
        $rustcVersion = rustc --version
        $cargoVersion = cargo --version
        Write-Info "Rust installed: $rustcVersion"
        Write-Info "Cargo installed: $cargoVersion"
        return $true
    }
    catch {
        Write-Error "Rust/Cargo is not installed or not in PATH"
        Write-Error "Please install Rust from https://rustup.rs/"
        return $false
    }
}

# Check and create missing crates
function Initialize-Project {
    Write-Info "Checking project structure..."

    # Create workspace Cargo.toml FIRST (with proper structure)
    $WorkspacePath = "Cargo.toml"
    if (-not (Test-Path $WorkspacePath)) {
        Write-Info "Creating workspace Cargo.toml..."
        @"
[workspace]
resolver = "2"
members = [
    "deform_core",
    "deform-editor",
]

[workspace.package]
version = "0.1.0"
edition = "2021"
authors = ["Deform Developers"]
license = "MIT OR Apache-2.0"

# Default members for cargo commands
default-members = ["deform_core", "deform-editor"]
"@ | Out-File -FilePath $WorkspacePath -Encoding UTF8
        Write-Success "Created workspace Cargo.toml"
    }

    # Check if deform_core exists
    if (-not (Test-Path "deform_core/Cargo.toml")) {
        Write-Warning "deform_core crate not found. Creating it..."
        cargo new deform_core --lib --vcs none
        if (-not $?) {
            Write-Error "Failed to create deform_core"
            return $false
        }
        Write-Success "Created deform_core crate"
    }

    # Check if deform-editor exists
    if (-not (Test-Path "deform-editor/Cargo.toml")) {
        Write-Warning "deform-editor crate not found. Creating it..."
        cargo new deform-editor --bin --vcs none
        if (-not $?) {
            Write-Error "Failed to create deform-editor"
            return $false
        }
        Write-Success "Created deform-editor crate"
    }

    # Create module structure in deform_core if missing
    $CoreModules = @("logger", "render", "scene")
    $LibRsPath = "deform_core/src/lib.rs"

    # Check if modules need to be created
    $needsModules = $false
    foreach ($module in $CoreModules) {
        if (-not (Test-Path "deform_core/src/$module/mod.rs") -and -not (Test-Path "deform_core/src/$module.rs")) {
            $needsModules = $true
            break
        }
    }

    if ($needsModules -or -not (Test-Path $LibRsPath)) {
        Write-Info "Setting up module structure in deform_core..."

        # Create module directories and stub files
        foreach ($module in $CoreModules) {
            $modulePath = "deform_core/src/$module"
            if (-not (Test-Path $modulePath)) {
                New-Item -ItemType Directory -Force -Path $modulePath | Out-Null
            }

            $stubPath = "$modulePath/mod.rs"
            if (-not (Test-Path $stubPath)) {
                switch ($module) {
                    "logger" {
                        @"
// Logger module stub
use std::sync::Once;

static INIT: Once = Once::new();

pub fn init() {
    INIT.call_once(|| {
        println!("Logger initialized");
    });
}

pub fn info(msg: &str) {
    println!("[INFO] {}", msg);
}

pub fn error(msg: &str) {
    eprintln!("[ERROR] {}", msg);
}

pub fn debug(msg: &str) {
    #[cfg(debug_assertions)]
    println!("[DEBUG] {}", msg);
}
"@ | Out-File -FilePath $stubPath -Encoding UTF8
                    }
                    "render" {
                        @"
// Render module stub
pub trait Renderer {
    fn render(&self);
    fn clear(&self);
}

pub struct NullRenderer;

impl Renderer for NullRenderer {
    fn render(&self) {
        // Null renderer does nothing
    }

    fn clear(&self) {
        // Null renderer does nothing
    }
}

pub struct OpenGLRenderer;

impl OpenGLRenderer {
    pub fn new() -> Self {
        Self
    }
}

impl Renderer for OpenGLRenderer {
    fn render(&self) {
        println!("OpenGL rendering");
    }

    fn clear(&self) {
        println!("OpenGL clearing");
    }
}
"@ | Out-File -FilePath $stubPath -Encoding UTF8
                    }
                    "scene" {
                        @"
// Scene module stub
use std::collections::HashMap;

#[derive(Debug, Clone)]
pub struct Entity {
    pub id: u64,
    pub name: String,
}

pub struct Scene {
    entities: HashMap<u64, Entity>,
    next_id: u64,
}

impl Scene {
    pub fn new() -> Self {
        Self {
            entities: HashMap::new(),
            next_id: 0,
        }
    }

    pub fn create_entity(&mut self, name: &str) -> u64 {
        let id = self.next_id;
        self.next_id += 1;
        self.entities.insert(id, Entity {
            id,
            name: name.to_string(),
        });
        id
    }

    pub fn get_entity(&self, id: u64) -> Option<&Entity> {
        self.entities.get(&id)
    }

    pub fn remove_entity(&mut self, id: u64) -> Option<Entity> {
        self.entities.remove(&id)
    }

    pub fn entity_count(&self) -> usize {
        self.entities.len()
    }
}

impl Default for Scene {
    fn default() -> Self {
        Self::new()
    }
}
"@ | Out-File -FilePath $stubPath -Encoding UTF8
                    }
                }
                Write-Success "Created stub for $module module"
            }
        }

        # Create or update lib.rs
        @"
// Deform Core Library
// Main exports

pub mod logger;
pub mod render;
pub mod scene;

// Re-export commonly used items
pub use logger::{info, error, debug};
pub use render::{Renderer, NullRenderer};
pub use scene::Scene;

// Version information
pub const VERSION: &str = env!("CARGO_PKG_VERSION");
"@ | Out-File -FilePath $LibRsPath -Encoding UTF8

        Write-Success "Created lib.rs with module declarations"
    }

    # Update deform-editor's main.rs to use the modules
    $MainRsPath = "deform-editor/src/main.rs"
    if (Test-Path $MainRsPath) {
        $mainContent = Get-Content $MainRsPath -Raw
        if ($mainContent -notmatch "use deform_core::") {
            Write-Info "Updating deform-editor main.rs..."
            $newMainContent = @"
use deform_core::{logger, render::NullRenderer, scene::Scene};

fn main() {
    // Initialize logger
    logger::init();
    logger::info("Starting Deform Editor...");

    // Create a scene
    let mut scene = Scene::new();
    let entity_id = scene.create_entity("Test Entity");
    logger::info(&format!("Created entity with id: {}", entity_id));

    // Create a renderer
    let renderer = NullRenderer;
    renderer.clear();

    logger::info("Deform Editor initialized successfully!");
    println!("Welcome to Deform Editor v{}", deform_core::VERSION);
}
"@
            $newMainContent | Out-File -FilePath $MainRsPath -Encoding UTF8
            Write-Success "Updated main.rs with example usage"
        }
    }

    # Update deform-editor dependencies
    $EditorCargoPath = "deform-editor/Cargo.toml"
    if (Test-Path $EditorCargoPath) {
        $content = Get-Content $EditorCargoPath -Raw
        if ($content -notmatch 'deform_core\s*=\s*\{') {
            Write-Info "Adding deform_core dependency to deform-editor..."

            # Add dependency to Cargo.toml
            if ($content -match '\[dependencies\]') {
                $content = $content -replace '\[dependencies\]', "[dependencies]`ndeform_core = { path = `"../deform_core`" }"
            } else {
                $content += "`n`n[dependencies]`ndeform_core = { path = `"../deform_core`" }"
            }
            $content | Out-File -FilePath $EditorCargoPath -Encoding UTF8
            Write-Success "Added deform_core dependency"
        }
    }

    return $true
}

# Clean build artifacts
function Clean-Build {
    Write-Info "Cleaning build artifacts..."

    if (Test-Path "target") {
        Remove-Item -Path "target" -Recurse -Force -ErrorAction SilentlyContinue
        Write-Success "Removed target directory"
    }

    cargo clean -q
    Write-Success "Clean completed"
}

# Main build function
function Build-Project {
    Write-Info "Building Deform project..."

    # Build arguments
    $buildArgs = @()
    if ($Release) {
        $buildArgs += "--release"
        Write-Info "Building in release mode"
    } else {
        Write-Info "Building in debug mode"
    }

    if ($Target) {
        $buildArgs += "--target"
        $buildArgs += $Target
        Write-Info "Target: $Target"
    }

    if ($Verbose) {
        $buildArgs += "--verbose"
    }

    # Run cargo build
    $buildCmd = "cargo build $($buildArgs -join ' ')"
    Write-Info "Running: $buildCmd"

    Invoke-Expression $buildCmd

    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed with exit code $LASTEXITCODE"
        return $false
    }

    Write-Success "Build completed successfully"
    return $true
}

# Show build information
function Show-Info {
    Write-Host ""
    Write-Host "=== Deform Project Build Information ===" -ForegroundColor $InfoColor
    Write-Host "Project Root: $ScriptDir" -ForegroundColor $InfoColor

    if (Test-Path "deform_core/Cargo.toml") {
        $coreVersion = (Select-String -Path "deform_core/Cargo.toml" -Pattern '^version = "(.+)"' | ForEach-Object { $_.Matches.Groups[1].Value })
        Write-Host "deform_core version: $coreVersion" -ForegroundColor $InfoColor
    }

    if (Test-Path "deform-editor/Cargo.toml") {
        $editorVersion = (Select-String -Path "deform-editor/Cargo.toml" -Pattern '^version = "(.+)"' | ForEach-Object { $_.Matches.Groups[1].Value })
        Write-Host "deform-editor version: $editorVersion" -ForegroundColor $InfoColor
    }

    Write-Host "Build Mode: $(if ($Release) { 'Release' } else { 'Debug' })" -ForegroundColor $InfoColor
    Write-Host ""
}

# Main execution
function Main {
    # Check Rust installation
    if (-not (Test-RustInstalled)) {
        exit 1
    }

    # Clean if requested
    if ($Clean) {
        Clean-Build
        if ($Clean -and -not ($Release -or $Verbose -or $Target)) {
            # If only clean was requested, exit here
            exit 0
        }
    }

    # Initialize project structure
    if (-not (Initialize-Project)) {
        exit 1
    }

    # Show build info
    Show-Info

    # Build the project
    if (-not (Build-Project)) {
        exit 1
    }

    # Show output location
    if ($Release) {
        $outputPath = "target/release/deform-editor.exe"
    } else {
        $outputPath = "target/debug/deform-editor.exe"
    }

    if (Test-Path $outputPath) {
        Write-Success "Executable created at: $outputPath"
        Write-Info "Run with: ./$outputPath"
    }

    Write-Success "Build script completed!"
}

# Parse arguments and run
Main

# Exit with appropriate code
exit $LASTEXITCODE
