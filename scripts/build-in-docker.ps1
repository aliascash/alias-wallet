# Build the modernized Alias wallet inside Docker.
#
# Usage:
#   pwsh -File scripts/build-in-docker.ps1                  # Linux build (default)
#   pwsh -File scripts/build-in-docker.ps1 -Target windows  # Windows cross-compile
#
# Output:
#   ./dist/linux-x86_64/aliaswalletd
#   ./dist/linux-x86_64/aliaswallet
#   ./dist/windows-x86_64/*.exe   (when -Target windows)
#
# Requirements:
#   - Docker Desktop installed and running
#   - ~4 GB free disk space for the Linux build image,
#     ~10 GB for the Windows cross-compile (depends/ builds Qt+Boost+OpenSSL).
#   - First Linux build: ~30 min. First Windows build: ~100-130 min.

[CmdletBinding()]
param(
    [ValidateSet("linux","windows")]
    [string]$Target = "linux",
    [int]$Jobs = 4
)

$ErrorActionPreference = "Stop"

# Sanity check Docker is up.
$d = Get-Command docker -ErrorAction SilentlyContinue
if (-not $d) {
    Write-Error "Docker is not on PATH. Install Docker Desktop and re-open this shell."
    exit 1
}
try {
    docker info --format '{{.ServerVersion}}' | Out-Null
} catch {
    Write-Error "Docker daemon is not running. Start Docker Desktop and try again."
    exit 1
}

switch ($Target) {
    "linux"   { $image = "alias-modernized:latest";      $dockerfile = "Dockerfile";     $outArch = "linux-x86_64" }
    "windows" { $image = "alias-modernized-win:latest";  $dockerfile = "Dockerfile.win"; $outArch = "windows-x86_64" }
}

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
Set-Location $repoRoot

Write-Host "Target:      $Target"
Write-Host "Repo root:   $repoRoot"
Write-Host "Image tag:   $image"
Write-Host "Dockerfile:  $dockerfile"
Write-Host "Output dir:  dist/$outArch/"
Write-Host "Jobs:        $Jobs"
Write-Host ""

# Build
Write-Host "==> docker build (this is the long step)"
docker build --progress=plain --build-arg JOBS=$Jobs -t $image -f $dockerfile .
if ($LASTEXITCODE -ne 0) {
    Write-Error "docker build failed."
    exit $LASTEXITCODE
}

# Extract artifacts to dist/<os>-<arch>/
$outDir = Join-Path $repoRoot "dist/$outArch"
if (-not (Test-Path $outDir)) { New-Item -ItemType Directory -Path $outDir -Force | Out-Null }

Write-Host ""
Write-Host "==> extracting artifacts to $outDir"
docker run --rm -v "${outDir}:/host-out" $image sh -c "cp -v /build-out/* /host-out/ 2>/dev/null || true"

Write-Host ""
Write-Host "==> artifacts:"
Get-ChildItem $outDir | Select-Object Name, @{N='MB';E={[math]::Round($_.Length/1MB,1)}}, LastWriteTime
