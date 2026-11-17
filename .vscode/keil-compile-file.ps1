param(
    [Parameter(Mandatory = $true)]
    [string]$SourceFile,
    [string]$Uv4Path = 'G:\keil_MDK\UV4\UV4.exe'
)

$ErrorActionPreference = 'Stop'
try {
    $projectRoot = Split-Path -Parent $PSScriptRoot
    $projectDirectory = Join-Path $projectRoot 'USER'
    $sourcePath = (Resolve-Path -LiteralPath $SourceFile).Path
    if ([IO.Path]::GetExtension($sourcePath) -ine '.c') {
        throw 'Select a .c source file. Headers are checked by compiling a .c file that includes them.'
    }
    [xml]$project = Get-Content -LiteralPath (Join-Path $projectDirectory 'FreeRTOS.uvprojx') -Raw -Encoding UTF8
    $target = $project.Project.Targets.Target | Where-Object { $_.TargetName -eq 'FreeRTOS' }
    if (-not $target -or $target.uAC6 -ne '0') { throw 'The FreeRTOS ARM Compiler 5 target is required.' }
    $projectFiles = @($target.SelectNodes('./Groups/Group/Files/File') | Where-Object {
        $_.FileType -eq '1' -and [IO.Path]::GetFullPath((Join-Path $projectDirectory $_.FilePath)) -ieq $sourcePath
    })
    if ($projectFiles.Count -ne 1) { throw 'The selected C file must belong to the FreeRTOS target.' }
    if (-not $sourcePath.StartsWith($projectRoot + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw 'The selected source file must be inside this workspace.'
    }
    $cads = $target.TargetOption.TargetArmAds.Cads
    if ($target.SelectNodes('.//FileOption | .//GroupOption').Count -gt 0 -or
        -not [string]::IsNullOrWhiteSpace($cads.VariousControls.MiscControls)) {
        throw 'Custom file/group options or miscellaneous flags require updating this single-file task. Use the full Keil build.'
    }
    $keilRoot = Split-Path -Parent (Split-Path -Parent $Uv4Path)
    $compiler = Join-Path $keilRoot 'ARM\ARMCC\bin\armcc.exe'
    if (-not (Test-Path -LiteralPath $compiler -PathType Leaf)) { throw "ARM Compiler 5 not found: $compiler" }

    # Keep standalone objects separate from the objects managed by uVision.
    $relativeSource = $sourcePath.Substring($projectRoot.Length + 1)
    $objectPath = Join-Path (Join-Path $projectRoot 'OBJ\single-file') ([IO.Path]::ChangeExtension($relativeSource, '.o'))
    New-Item -ItemType Directory -Path (Split-Path -Parent $objectPath) -Force | Out-Null
    if (Test-Path -LiteralPath $objectPath) { Remove-Item -LiteralPath $objectPath }

    # CPU/FPU match this project's STM32F407 target. Other settings are read
    # from the same uvprojx used by the full Keil build.
    $compilerArguments = @('-c', '--cpu', 'Cortex-M4.fp.sp', '--diag_style=ide')
    $optimization = [int]$cads.Optim - 1
    if ($optimization -lt 0 -or $optimization -gt 3) { throw 'Unsupported ARMCC optimization setting.' }
    $compilerArguments += "-O$optimization"
    if ($cads.uC99 -eq '1') { $compilerArguments += '--c99' }
    if ($cads.uGnu -eq '1') { $compilerArguments += '--gnu' }
    if ($cads.interw -eq '1') { $compilerArguments += '--apcs=interwork' }
    if ($cads.OneElfS -eq '1') { $compilerArguments += '--split_sections' }
    if ($cads.Strict -eq '1') { $compilerArguments += '--strict' }
    if ($cads.EnumInt -eq '1') { $compilerArguments += '--enum_is_int' }
    if ($cads.PlainCh -eq '1') { $compilerArguments += '--signed_chars' }
    if ($cads.oTime -eq '1') { $compilerArguments += '-Otime' }
    if ($target.TargetOption.TargetCommonOption.DebugInformation -eq '1') { $compilerArguments += '-g' }
    foreach ($define in ([string]$cads.VariousControls.Define -split ',')) {
        if ($define.Trim()) { $compilerArguments += '-D' + ($define -replace '\s+', '') }
    }
    foreach ($undefine in ([string]$cads.VariousControls.Undefine -split ',')) {
        if ($undefine.Trim()) { $compilerArguments += '-U' + $undefine.Trim() }
    }
    foreach ($include in ([string]$cads.VariousControls.IncludePath -split ';')) {
        if ($include.Trim()) {
            $compilerArguments += @('-I', [IO.Path]::GetFullPath((Join-Path $projectDirectory $include.Trim())))
        }
    }
    $compilerArguments += @('-o', $objectPath, $sourcePath)
    Write-Host "Compiling only: $relativeSource"
    Push-Location $projectDirectory
    try {
        & $compiler @compilerArguments
        $compilerExitCode = $LASTEXITCODE
    } finally { Pop-Location }
    if ($compilerExitCode -ne 0) { exit $compilerExitCode }
    if (-not (Test-Path -LiteralPath $objectPath)) { throw 'The compiler did not produce an object file.' }
    Write-Host "Compile succeeded: $objectPath"
    Write-Host 'No linking or firmware generation. Use Ctrl+Shift+B to build the firmware.'
    exit 0
} catch {
    Write-Error $_ -ErrorAction Continue
    exit 2
}
