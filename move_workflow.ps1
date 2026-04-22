# Move migration-workflow.yml into .github/workflows/migration.yml
$src = Join-Path (Get-Location) 'migration-workflow.yml'
$destDir = Join-Path (Get-Location) '.github\workflows'
if (-not (Test-Path $src)) {
  Write-Error "Source migration-workflow.yml not found in repo root."
  exit 1
}
if (-not (Test-Path $destDir)) {
  New-Item -ItemType Directory -Path $destDir -Force | Out-Null
}
$dest = Join-Path $destDir 'migration.yml'
Move-Item -Path $src -Destination $dest -Force
Write-Output "Moved migration-workflow.yml to $dest"
