param(
    [string]$Source = (Join-Path $PSScriptRoot '..\resources\icons\AppIcon.png')
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

$sourcePath = (Resolve-Path -LiteralPath $Source).Path
$outputDirectory = Split-Path $sourcePath
$sourceImage = [System.Drawing.Image]::FromFile($sourcePath)

function New-PngBytes([System.Drawing.Image]$Image, [int]$Size) {
    $bitmap = [System.Drawing.Bitmap]::new(
        $Size,
        $Size,
        [System.Drawing.Imaging.PixelFormat]::Format32bppArgb
    )
    try {
        $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
        try {
            $graphics.CompositingMode = [System.Drawing.Drawing2D.CompositingMode]::SourceCopy
            $graphics.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
            $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
            $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
            $graphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
            $graphics.DrawImage($Image, 0, 0, $Size, $Size)
        } finally {
            $graphics.Dispose()
        }

        $stream = [System.IO.MemoryStream]::new()
        try {
            $bitmap.Save($stream, [System.Drawing.Imaging.ImageFormat]::Png)
            return ,$stream.ToArray()
        } finally {
            $stream.Dispose()
        }
    } finally {
        $bitmap.Dispose()
    }
}

function Write-BigEndianUInt32([System.IO.BinaryWriter]$Writer, [uint32]$Value) {
    $Writer.Write([byte](($Value -shr 24) -band 0xff))
    $Writer.Write([byte](($Value -shr 16) -band 0xff))
    $Writer.Write([byte](($Value -shr 8) -band 0xff))
    $Writer.Write([byte]($Value -band 0xff))
}

try {
    $icoImages = @(16, 24, 32, 48, 64, 128, 256 | ForEach-Object {
        [pscustomobject]@{ Size = $_; Data = New-PngBytes $sourceImage $_ }
    })
    $icoPath = Join-Path $outputDirectory 'AppIcon.ico'
    $icoStream = [System.IO.File]::Create($icoPath)
    try {
        $writer = [System.IO.BinaryWriter]::new($icoStream)
        try {
            $writer.Write([uint16]0)
            $writer.Write([uint16]1)
            $writer.Write([uint16]$icoImages.Count)

            $offset = 6 + 16 * $icoImages.Count
            foreach ($icon in $icoImages) {
                $dimension = if ($icon.Size -eq 256) { 0 } else { $icon.Size }
                $writer.Write([byte]$dimension)
                $writer.Write([byte]$dimension)
                $writer.Write([byte]0)
                $writer.Write([byte]0)
                $writer.Write([uint16]1)
                $writer.Write([uint16]32)
                $writer.Write([uint32]$icon.Data.Length)
                $writer.Write([uint32]$offset)
                $offset += $icon.Data.Length
            }
            foreach ($icon in $icoImages) { $writer.Write($icon.Data) }
        } finally {
            $writer.Dispose()
        }
    } finally {
        $icoStream.Dispose()
    }

    $icnsDefinitions = @(
        @{ Type = 'icp4'; Size = 16 },
        @{ Type = 'icp5'; Size = 32 },
        @{ Type = 'icp6'; Size = 64 },
        @{ Type = 'ic07'; Size = 128 },
        @{ Type = 'ic08'; Size = 256 },
        @{ Type = 'ic09'; Size = 512 },
        @{ Type = 'ic11'; Size = 32 },
        @{ Type = 'ic12'; Size = 64 },
        @{ Type = 'ic13'; Size = 256 },
        @{ Type = 'ic14'; Size = 512 }
    )
    $icnsImages = @($icnsDefinitions | ForEach-Object {
        [pscustomobject]@{ Type = $_.Type; Data = New-PngBytes $sourceImage $_.Size }
    })
    $icnsLength = 8
    foreach ($icon in $icnsImages) { $icnsLength += 8 + $icon.Data.Length }

    $icnsPath = Join-Path $outputDirectory 'AppIcon.icns'
    $icnsStream = [System.IO.File]::Create($icnsPath)
    try {
        $writer = [System.IO.BinaryWriter]::new($icnsStream)
        try {
            $writer.Write([System.Text.Encoding]::ASCII.GetBytes('icns'))
            Write-BigEndianUInt32 $writer $icnsLength
            foreach ($icon in $icnsImages) {
                $writer.Write([System.Text.Encoding]::ASCII.GetBytes($icon.Type))
                Write-BigEndianUInt32 $writer (8 + $icon.Data.Length)
                $writer.Write($icon.Data)
            }
        } finally {
            $writer.Dispose()
        }
    } finally {
        $icnsStream.Dispose()
    }
} finally {
    $sourceImage.Dispose()
}

Write-Output "Generated $icoPath and $icnsPath from $sourcePath"