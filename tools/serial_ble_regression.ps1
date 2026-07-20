param([Parameter(Mandatory = $true)][string]$Port)

$serial = New-Object System.IO.Ports.SerialPort $Port,115200,'None',8,'One'
$serial.DtrEnable = $false
$serial.RtsEnable = $false
$serial.Open()
$output = New-Object System.Text.StringBuilder
function Collect-Serial([int]$Milliseconds) {
  $until = (Get-Date).AddMilliseconds($Milliseconds)
  while ((Get-Date) -lt $until) {
    if ($serial.BytesToRead -gt 0) { [void]$output.Append($serial.ReadExisting()) }
    Start-Sleep -Milliseconds 50
  }
}

Collect-Serial 18000
$serial.Write("app rfwatch`r`n")
Collect-Serial 700
$serial.Write("press secondary`r`n")
Collect-Serial 18000
$serial.Write("press primary`r`n")
Collect-Serial 8000
$serial.Write("status`r`n")
Collect-Serial 1000
$serial.Close()
$output.ToString()
