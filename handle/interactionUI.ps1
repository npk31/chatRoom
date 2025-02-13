param (
    [string]$RID,
    [string]$UN,
    [string]$P1,
    [string]$P2,
    [string]$Title = "Chat Room"
)

# Tải assembly System.Windows.Forms
Add-Type -AssemblyName System.Windows.Forms

# Tạo biến $command
$command = '--title "' + $Title + '" -p "Command Prompt" cmd /k "' + $P1 + ' ' + $RID + ' ' + $UN + '" ' + 
           '; split-pane --title "' + $Title + '" -p "Command Prompt" -H --size 0.2 cmd /k "' + $P2 + ' ' + $RID + ' ' + $UN + '"'


# Mở Windows Terminal với hai pane CMD và tiêu đề cụ thể
Start-Process -FilePath "wt.exe" -ArgumentList $command
Start-Sleep -Seconds 2

# Lấy handle của cửa sổ Windows Terminal
$terminal = Get-Process -Name "WindowsTerminal" | Select-Object -First 1
$hwnd = $terminal.MainWindowHandle

# Tải thư viện user32.dll để thay đổi kích thước cửa sổ và gửi thông điệp
Add-Type @"
    using System;
    using System.Runtime.InteropServices;
    public class User32 {
        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool MoveWindow(IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);
    }
"@

# Lấy kích thước màn hình
$screen = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds

# Tính toán kích thước cửa sổ mới (40% kích thước màn hình)
$newWidth = [math]::Round($screen.Width * 0.4)
$newHeight = [math]::Round($screen.Height * 0.4)

# Tính toán vị trí để đặt cửa sổ ở giữa màn hình
$newX = [math]::Round(($screen.Width - $newWidth) / 2)
$newY = [math]::Round(($screen.Height - $newHeight) / 2)

# Di chuyển và thay đổi kích thước cửa sổ
[User32]::MoveWindow($hwnd, $newX, $newY, $newWidth, $newHeight, $true)

Clear-Host

Write-Output "Room ID: $RID"
Write-Output "User Name: $UN"