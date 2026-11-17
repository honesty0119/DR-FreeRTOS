param(
    [ValidateSet('Build', 'Rebuild')]
    [string]$Action = 'Build',
    [string]$Uv4Path = 'G:\keil_MDK\UV4\UV4.exe'
)

$ErrorActionPreference = 'Stop'
try {
    $projectRoot = Split-Path -Parent $PSScriptRoot
    $projectPath = Join-Path $projectRoot 'USER\FreeRTOS.uvprojx'
    $projectDirectory = Split-Path -Parent $projectPath
    $logPath = Join-Path $projectRoot 'OBJ\vscode-build.log'
    if (-not (Test-Path -LiteralPath $Uv4Path -PathType Leaf)) {
        throw 'Keil UV4.exe was not found. Set KeilAssistant.MDK.Uv4Path in VS Code user settings.'
    }
    [xml]$project = Get-Content -LiteralPath $projectPath -Raw -Encoding UTF8
    $target = $project.Project.Targets.Target | Where-Object { $_.TargetName -eq 'FreeRTOS' }
    if (-not $target) { throw 'The FreeRTOS target is missing from the Keil project.' }
    if ($target.uAC6 -ne '0') { throw 'This build task expects the original ARM Compiler 5 target.' }

    New-Item -ItemType Directory -Path (Split-Path -Parent $logPath) -Force | Out-Null
    if (Test-Path -LiteralPath $logPath) { Remove-Item -LiteralPath $logPath }
    $buildSwitch = '-b'
    if ($Action -eq 'Rebuild') { $buildSwitch = '-r' }
    # Start-Process waits for the GUI-subsystem executable; invoking it directly
    # from PowerShell can otherwise finish before the firmware build completes.
    $arguments = @($buildSwitch, ('"{0}"' -f $projectPath), '-j0', '-t', '"FreeRTOS"', '-o', ('"{0}"' -f $logPath))
    Write-Host "$Action FreeRTOS with $Uv4Path"
    $process = Start-Process -FilePath $Uv4Path -ArgumentList $arguments -WorkingDirectory $projectDirectory -WindowStyle Hidden -PassThru
    $printedLines = 0
    do {
        Start-Sleep -Milliseconds 500
        if (Test-Path -LiteralPath $logPath) {
            $lines = @(Get-Content -LiteralPath $logPath -Encoding UTF8)
            for ($index = $printedLines; $index -lt $lines.Count; $index++) { Write-Host $lines[$index] }
            $printedLines = $lines.Count
        }
        $process.Refresh()
    } while (-not $process.HasExited)
    $process.WaitForExit()
    if (Test-Path -LiteralPath $logPath) {
        $lines = @(Get-Content -LiteralPath $logPath -Encoding UTF8)
        for ($index = $printedLines; $index -lt $lines.Count; $index++) { Write-Host $lines[$index] }
    } else {
        throw "Keil did not produce a build log (exit code $($process.ExitCode))."
    }
    # uVision: 0 = success, 1 = warnings only, 2+ = errors.
    if ($process.ExitCode -notin 0, 1) { exit $process.ExitCode }
    Write-Host "Build finished. Firmware: OBJ\LED.axf and OBJ\LED.hex"
    exit 0
} catch {
    Write-Error $_ -ErrorAction Continue
    exit 2
}
