<#
===========================================================
 Bundle Script for ChatGPT Review - CSOPESY-MO Project
-----------------------------------------------------------
This script bundles all readable project files into a 
single `bundle.txt` file with headers and a GPT prompt.

▶ HOW TO RUN (PowerShell):
   1. Open PowerShell.
   2. cd path\to\CSOPESY-MO
   3. powershell -ExecutionPolicy Bypass -File bundle.ps1

▶ Or use: `make bundle` if Makefile is present.
===========================================================
#>

# === CONFIG ===
$projectRoot = "./src"
$outputFile = "bundle.txt"
$excludedExtensions = @('.exe', '.dll', '.obj', '.pdb')

# === GPT Prompt ===
$gptPrompt = @"
# === GPT System Overview Request ===

You are a senior developer reviewing this project for a new team member.

Please do the following:

1. **Give an overview** of what this project does and its main components.
2. **Explain how each component works**, especially:
   - main.cpp
   - Core and CoreManager classes
   - Scheduler, SchedulerStrategy, and SchedulerData
   - Process and how it interacts with the Console and Instructions
   - Command system and Shell
3. **Describe how everything fits together** during execution:
   - How a process is created, assigned, executed, and logged.
   - How the scheduler ticks and how cores are managed.
4. **Highlight entry points** and flow of control.
5. **Note any potential issues or architecture strengths/weaknesses.**

Use the FILE: and PATH: headers to locate the content.

Please write in clear sections and simple language for onboarding a new developer.

--- End of prompt ---
"@

# === Write Prompt to File ===
$gptPrompt | Out-File $outputFile -Encoding UTF8

# === Collect Files ===
$projectFullPath = Resolve-Path $projectRoot
$files = Get-ChildItem -Path $projectFullPath -Recurse -File | Where-Object {
  $excludedExtensions -notcontains $_.Extension
}

# === Append Each File ===
foreach ($file in $files) {
  $relativePath = $file.FullName.Substring($projectFullPath.Path.Length + 1)
  $header = @"
------------------------------
 FILE:  $($file.Name)
 PATH:  $relativePath
------------------------------
"@
  Add-Content -Path $outputFile -Value "`n$header`n"
  Get-Content $file.FullName | Add-Content -Path $outputFile
}

Write-Host "✅ Generated $outputFile with GPT prompt and $($files.Count) files."
