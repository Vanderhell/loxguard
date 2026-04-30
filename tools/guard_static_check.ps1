Param(
    [string]$Path = "src"
)

$forbidden = @("\bmalloc\(", "\bfree\(", "\bmemcpy\(", "\bmemset\(")
$files = Get-ChildItem -Path $Path -Filter *.c -Recurse
$failed = $false

foreach ($f in $files) {
    $content = Get-Content $f.FullName -Raw
    if ($content -notmatch "LOX_GUARDED_FILE") {
        continue
    }
    foreach ($pat in $forbidden) {
        if ($content -match $pat) {
            Write-Output "FORBIDDEN pattern '$pat' found in $($f.FullName)"
            $failed = $true
        }
    }
}

if ($failed) { exit 1 }
Write-Output "Static guard check passed"
exit 0
