param(
    [Parameter(Mandatory = $true)]
    [string]$Tag
)

$ErrorActionPreference = "Stop"

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw "git not found in PATH"
}

$zip = "loxguard-$Tag.zip"
$sha = "$zip.sha256"

git rev-parse --is-inside-work-tree | Out-Null

git archive --format=zip --output=$zip HEAD
$hash = (Get-FileHash $zip -Algorithm SHA256).Hash.ToLower()
Set-Content -Path $sha -Value "$hash  $zip"

Write-Host "Wrote $zip"
Write-Host "Wrote $sha"
