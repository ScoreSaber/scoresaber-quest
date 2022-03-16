# if (Test-Path ./log.log)
# {
#     & del log.log
# }
# & adb logcat -c
# & adb logcat > log.log

Write-Output("Started logging")

& adb logcat | findstr ScoreSaber