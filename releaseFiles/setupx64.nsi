; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Kactus2"
!define PRODUCT_VERSION "2.1 64-bit"
!define PRODUCT_PUBLISHER "TUT"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Kactus2.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor bzip2

!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!include "MultiUser.nsh"
!include "MUI2.nsh"
!include "x64.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "license.txt"
; Multiuser page
!insertmacro MULTIUSER_PAGE_INSTALLMODE
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\Kactus2.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
;!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME}"
OutFile "Kactus2SetupWin64bit.exe"
InstallDir "$PROGRAMFILES64\Kactus2"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "..\x64\executable\Qt5CLucene.dll"
  File "..\x64\executable\Qt5Core.dll"
  File "..\x64\executable\Qt5Gui.dll"
  File "..\x64\executable\Qt5Help.dll"
  File "..\x64\executable\Qt5Network.dll"
  File "..\x64\executable\Qt5PrintSupport.dll"
  File "..\x64\executable\Qt5Sql.dll"
  File "..\x64\executable\Qt5Widgets.dll"
  File "..\x64\executable\Qt5Xml.dll"
  File "..\x64\executable\Qt5XmlPatterns.dll"
  File "..\x64\executable\libEGL.dll"
  File "..\x64\executable\libGLESv2.dll"
  File "..\x64\executable\icuin50.dll"
  File "..\x64\executable\icudt50.dll"
  File "..\x64\executable\icuuc50.dll"
  File "..\x64\executable\D3DCompiler_43.dll"
  File "..\x64\executable\Kactus2.exe"
  File "license.txt"
  File "release_notes.txt"
  File "readme.txt"
  File "..\executable\upgrade.cfg"
  File /oname=DefaultSettings.ini "DefaultSettingsWin.ini"
  
  SetOutPath "$INSTDIR\platforms"
  SetOverwrite on
  File "..\x64\executable\platforms\qminimal.dll"
  File "..\x64\executable\platforms\qwindows.dll"
  
  SetOutPath "$INSTDIR\sqldrivers"
  SetOverwrite on
  File "..\x64\executable\sqldrivers\qsqlite.dll"

  SetOutPath "$INSTDIR\Plugins"
  SetOverwrite on
  File "..\x64\executable\Plugins\MCAPICodeGenerator.dll"
  File "..\x64\executable\Plugins\CppSourceAnalyzer.dll"
  File "..\x64\executable\Plugins\VHDLSourceAnalyzer.dll"
  File "..\x64\executable\Plugins\AlteraBSPGenerator.dll"
  File "..\x64\executable\Plugins\MemoryMapHeaderGenerator.dll"

  SetOutPath "$INSTDIR\Help"
  SetOverwrite on
  File "..\Help\Kactus2Help.qhc"
  File "..\Help\Kactus2Help.qch"

  SetOutPath "$APPDATA\TUT"
  SetOverwrite off
  File /oname=Kactus2.ini "DefaultSettingsWin.ini"
  
  SetOutPath "$INSTDIR\Library\TUT\global.communication\mcapi\1.063"
  SetOverwrite on
  File "Library\TUT\global.communication\mcapi\1.063\mcapi.1.063.xml"

  SetOutPath "$INSTDIR\Library\TUT\ip.swp.api\mcapi.apiDef\1.063"
  SetOverwrite on
  File "Library\TUT\ip.swp.api\mcapi.apiDef\1.063\mcapi.apiDef.1.063.xml"

  SetOutPath "$INSTDIR"
  CreateDirectory "$SMPROGRAMS\Kactus2"
  CreateShortCut "$SMPROGRAMS\Kactus2\Kactus2.lnk" "$INSTDIR\Kactus2.exe"
  CreateShortCut "$SMPROGRAMS\Kactus2\License.lnk" "$INSTDIR\license.txt"
  CreateShortCut "$SMPROGRAMS\Kactus2\Release Notes.lnk" "$INSTDIR\release_notes.txt"
  CreateShortCut "$SMPROGRAMS\Kactus2\Readme.lnk" "$INSTDIR\readme.txt"
  CreateShortCut "$DESKTOP\Kactus2.lnk" "$INSTDIR\Kactus2.exe"
SectionEnd

Section "DependencySection" SEC02
  SetOutPath "$TEMP\Kactus2Installer"
  SetOverwrite ifnewer
  File "vcredist_x64.exe"
  ExecWait '"$OUTDIR\vcredist_x64.exe" /q /norestart'
  SetOutPath "$INSTDIR"
  RMDir /r "$TEMP\Kactus2Installer"
SectionEnd

Section -AdditionalIcons
  CreateShortCut "$SMPROGRAMS\Kactus2\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Kactus2.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Kactus2.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function .onInit
  !insertmacro MULTIUSER_INIT
  ${IfNot} ${RunningX64}
     MessageBox MB_OK "This program requires a 64-bit version of Windows. Setup will now exit."
     Abort
  ${EndIf}
  ${DisableX64FSRedirection}
  SetRegView 64
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  !insertmacro MULTIUSER_UNINIT
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\Library\TUT\global.communication\mcapi\1.063\mcapi.1.063.xml"
  Delete "$INSTDIR\Library\TUT\ip.swp.api\mcapi.apiDef\1.063\mcapi.apiDef.1.063.xml"
  Delete "$INSTDIR\Help\Kactus2Help.qch"
  Delete "$INSTDIR\Help\Kactus2Help.qhc"
  Delete "$INSTDIR\Plugins\MemoryMapHeaderGenerator.dll"
  Delete "$INSTDIR\Plugins\AlteraBSPGenerator.dll"
  Delete "$INSTDIR\Plugins\VHDLSourceAnalyzer.dll"
  Delete "$INSTDIR\Plugins\CppSourceAnalyzer.dll"
  Delete "$INSTDIR\Plugins\MCAPICodeGenerator.dll"
  Delete "$INSTDIR\sqldrivers\qsqlite.dll"
  Delete "$INSTDIR\platforms\qminimal.dll"
  Delete "$INSTDIR\platforms\qwindows.dll"
  Delete "$INSTDIR\DefaultSettings.ini"
  Delete "$INSTDIR\upgrade.cfg"
  Delete "$INSTDIR\readme.txt"
  Delete "$INSTDIR\release_notes.txt"
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\Kactus2.exe"
  Delete "$INSTDIR\D3DCompiler_43.dll"
  Delete "$INSTDIR\icuuc50.dll"
  Delete "$INSTDIR\icudt50.dll"
  Delete "$INSTDIR\icuin50.dll"
  Delete "$INSTDIR\libGLESv2.dll"
  Delete "$INSTDIR\libEGL.dll"
  Delete "$INSTDIR\Qt5XmlPatterns.dll"
  Delete "$INSTDIR\Qt5Xml.dll"
  Delete "$INSTDIR\Qt5Widgets.dll"
  Delete "$INSTDIR\Qt5Sql.dll"
  Delete "$INSTDIR\Qt5PrintSupport.dll"
  Delete "$INSTDIR\Qt5Network.dll"
  Delete "$INSTDIR\Qt5Help.dll"
  Delete "$INSTDIR\Qt5Gui.dll"
  Delete "$INSTDIR\Qt5Core.dll"
  Delete "$INSTDIR\Qt5CLucene.dll"

  Delete "$SMPROGRAMS\Kactus2\Uninstall.lnk"
  Delete "$DESKTOP\Kactus2.lnk"
  Delete "$SMPROGRAMS\Kactus2\Readme.lnk"
  Delete "$SMPROGRAMS\Kactus2\Release Notes.lnk"
  Delete "$SMPROGRAMS\Kactus2\License.lnk"
  Delete "$SMPROGRAMS\Kactus2\Kactus2.lnk"

  RMDir "$SMPROGRAMS\Kactus2"
  RMDir "$INSTDIR\Help"
  RMDir "$INSTDIR\Plugins"
  RMDir "$INSTDIR\sqldrivers"
  RMDir "$INSTDIR\platforms"
  RMDir "$INSTDIR\Library\TUT\ip.swp.api\mcapi.apiDef\1.063"
  RMDir "$INSTDIR\Library\TUT\ip.swp.api\mcapi.apiDef"
  RMDir "$INSTDIR\Library\TUT\ip.swp.api"
  RMDir "$INSTDIR\Library\TUT\global.communication\mcapi\1.063"
  RMDir "$INSTDIR\Library\TUT\global.communication\mcapi"
  RMDir "$INSTDIR\Library\TUT\global.communication"
  RMDir "$INSTDIR\Library\TUT"
  RMDir "$INSTDIR\Library"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd