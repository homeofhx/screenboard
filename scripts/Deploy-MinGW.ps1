param(
    [string]$Executable = 'build/app/release/ScreenBoard.exe',
    [string]$QtBin,
    [string]$CompilerBin
)

$ErrorActionPreference = 'Stop'

# Deploy the release runtime by inspecting imports. This also works with Qt installations whose windeployqt incorrectly classifies MinGW plugins as debug.
if (-not $QtBin) {
    $QtBin = Split-Path (Get-Command qmake.exe -ErrorAction Stop).Source
}
if (-not $CompilerBin) {
    $CompilerBin = Split-Path (Get-Command g++.exe -ErrorAction Stop).Source
}
$qmakePath = Join-Path $QtBin 'qmake.exe'
$objdumpPath = Join-Path $CompilerBin 'objdump.exe'
if (-not (Test-Path -LiteralPath $objdumpPath -PathType Leaf)) {
    throw 'Use the matching MinGW compiler bin directory, containing objdump.exe.'
}
$pluginRoot = (& $qmakePath -query QT_INSTALL_PLUGINS).Trim()
if ($LASTEXITCODE -ne 0) { throw 'Unable to query the Qt plugin directory.' }
$releaseDirectory = Split-Path (Resolve-Path -LiteralPath $Executable).Path
$queue = [Collections.Generic.Queue[string]]::new()
$queue.Enqueue((Resolve-Path -LiteralPath $Executable).Path)

# PNG support is built into QtGui. SVG icons and native Windows integration require these runtime plugins alongside the application.
$plugins = @{
    platforms = 'qwindows.dll'
    iconengines = 'qsvgicon.dll'
    imageformats = 'qsvg.dll'
    styles = 'qwindowsvistastyle.dll'
}
foreach ($group in $plugins.Keys) {
    $destination = Join-Path $releaseDirectory $group
    New-Item -ItemType Directory -Path $destination -Force | Out-Null
    $source = Join-Path (Join-Path $pluginRoot $group) $plugins[$group]
    Copy-Item -LiteralPath $source -Destination $destination
    $queue.Enqueue($source)
}

$seen = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
while ($queue.Count -gt 0) {
    $binary = $queue.Dequeue()
    $imports = & $objdumpPath -p $binary
    if ($LASTEXITCODE -ne 0) { throw "Unable to inspect $binary" }
    foreach ($line in $imports) {
        if ($line -notmatch 'DLL Name:\s*(.+)$') { continue }
        $library = $Matches[1].Trim()
        if (-not $seen.Add($library)) { continue }
        $found = $false
        foreach ($directory in @($QtBin, $CompilerBin)) {
            $source = Join-Path $directory $library
            if (Test-Path -LiteralPath $source -PathType Leaf) {
                Copy-Item -LiteralPath $source -Destination $releaseDirectory
                $queue.Enqueue($source)
                $found = $true
                break
            }
        }
        if (-not $found -and $library -match '^(Qt\d|libgcc_|libstdc\+\+|libwinpthread)') {
            throw "Missing required runtime library: $library"
        }
        # Other imports are Windows system libraries and are not redistributed.
    }
}

@'
[Paths]
Prefix=.
Plugins=.
'@ | Set-Content -LiteralPath (Join-Path $releaseDirectory 'qt.conf') -Encoding ascii
Write-Output "Deployed the Qt/MinGW release runtime to $releaseDirectory"