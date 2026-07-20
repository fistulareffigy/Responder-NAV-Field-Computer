param([Parameter(Mandatory = $true)][string]$Port)
$serial = New-Object System.IO.Ports.SerialPort $Port,115200,'None',8,'One'
$serial.DtrEnable = $false
$serial.RtsEnable = $false
$serial.Open()
$output = New-Object System.Text.StringBuilder
$until = (Get-Date).AddSeconds(17)
while ((Get-Date) -lt $until) {
  if ($serial.BytesToRead -gt 0) { [void]$output.Append($serial.ReadExisting()) }
  Start-Sleep -Milliseconds 50
}
$serial.Write("press back`r`n")
$until = (Get-Date).AddSeconds(18)
while ((Get-Date) -lt $until) {
  if ($serial.BytesToRead -gt 0) { [void]$output.Append($serial.ReadExisting()) }
  Start-Sleep -Milliseconds 50
}
$serial.Write("status`r`n")
Start-Sleep -Milliseconds 500
if ($serial.BytesToRead -gt 0) { [void]$output.Append($serial.ReadExisting()) }
$serial.Close()
$output.ToString()
