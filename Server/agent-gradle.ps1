param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]] $GradleArgs
)

$ErrorActionPreference = "Stop"

$serverDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Split-Path -Parent $serverDir
$jdkHome = "C:\Program Files\Java\jdk-21"

if (-not (Test-Path -LiteralPath (Join-Path $jdkHome "bin\java.exe"))) {
    throw "JDK 21 was not found at $jdkHome"
}

$env:JAVA_HOME = $jdkHome
$env:Path = "$env:JAVA_HOME\bin;$env:Path"
if (-not $env:GRADLE_USER_HOME) {
    $env:GRADLE_USER_HOME = Join-Path $repoRoot ".gradle_home"
}

Push-Location $serverDir
try {
    & .\gradlew.bat @GradleArgs
    exit $LASTEXITCODE
}
finally {
    Pop-Location
}
