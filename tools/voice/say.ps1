# Synthèse vocale Windows SAPI : voix française lisant un texte anglais (accent français).
param([string]$Text = "For sure!", [string]$Out, [int]$Rate = -1)
Add-Type -AssemblyName System.Speech
$s = New-Object System.Speech.Synthesis.SpeechSynthesizer
$fr = $s.GetInstalledVoices() | Where-Object { $_.VoiceInfo.Culture.Name -like "fr*" } | Select-Object -First 1
if ($fr) { $s.SelectVoice($fr.VoiceInfo.Name) }
$s.Rate = $Rate
$s.SetOutputToWaveFile($Out)
$s.Speak($Text)
$s.Dispose()
