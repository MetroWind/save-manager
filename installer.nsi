Name "Simple Save Manager"
OutFile "ssm-installer.exe"
Page license
Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles
InstallDir "$PROGRAMFILES64\Simple Save Manager"
LicenseData "COPYING.txt"

Section "Simple Save Manager"
  SetOutPath $INSTDIR
  File /r ${__FILEDIR__}\build\dist\*
  WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Uninstall"
  RMDir /R $INSTDIR\platforms
  RMDir /R $INSTDIR\sqldrivers
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\*.exe
  Delete $INSTDIR\games.yaml
  Delete $INSTDIR\COPYING.txt
  RMDir $INSTDIR
SectionEnd
