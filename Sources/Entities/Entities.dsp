# Microsoft Developer Studio Project File - Name="Entities" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Entities - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Entities.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Entities.mak" CFG="Entities - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Entities - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Entities - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Flesh/Classes/Entities", CVAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Entities - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /Ox /Ot /Og /Oi /Oy- /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"StdH.h" /FD /c
# SUBTRACT CPP /Oa /Ow /Gf /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /map /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Copying $(InputName) binaries to $(ENGINE_DIR)\Bin
InputPath=.\Release\Entities.dll
InputName=Entities
SOURCE="$(InputPath)"

"$(ENGINE_DIR)\Bin\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy Release\$(InputName).dll $(ENGINE_DIR)\Bin >nul 
	copy Release\$(InputName).map $(ENGINE_DIR)\Bin >nul 
	copy Release\$(InputName).lib $(ENGINE_DIR)\Bin >nul 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"StdH.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /map /debug /machine:I386 /out:"Debug/EntitiesD.dll" /pdbtype:sept
# Begin Custom Build - Copying $(InputName) binaries to $(ENGINE_DIR)\Bin\Debug
InputPath=.\Debug\EntitiesD.dll
InputName=EntitiesD
SOURCE="$(InputPath)"

"$(ENGINE_DIR)\Bin\Debug\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy Debug\$(InputName).dll $(ENGINE_DIR)\Bin\Debug >nul 
	copy Debug\$(InputName).map $(ENGINE_DIR)\Bin\Debug >nul 
	copy Debug\$(InputName).lib $(ENGINE_DIR)\Bin\Debug >nul 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "Entities - Win32 Release"
# Name "Entities - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "Common sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Common\Common.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\HUD.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\Particles.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\PathFinding.cpp
# End Source File
# Begin Source File

SOURCE=.\StdH\StdH.cpp
# ADD CPP /Yc"StdH.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\AirWave.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\AmmoItem.cpp
# End Source File
# Begin Source File

SOURCE=.\AmmoPack.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimationChanger.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimationHub.cpp
# End Source File
# Begin Source File

SOURCE=.\ArmorItem.cpp
# End Source File
# Begin Source File

SOURCE=.\BackgroundViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\BasicEffects.cpp
# End Source File
# Begin Source File

SOURCE=.\Beast.cpp
# End Source File
# Begin Source File

SOURCE=.\BigHead.cpp
# End Source File
# Begin Source File

SOURCE=.\BlendController.cpp
# End Source File
# Begin Source File

SOURCE=.\BloodSpray.cpp
# End Source File
# Begin Source File

SOURCE=.\Boneman.cpp
# End Source File
# Begin Source File

SOURCE=.\Bouncer.cpp
# End Source File
# Begin Source File

SOURCE=.\Bullet.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\CannonBall.cpp
# End Source File
# Begin Source File

SOURCE=.\Catman.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Copier.cpp
# End Source File
# Begin Source File

SOURCE=.\Counter.cpp
# End Source File
# Begin Source File

SOURCE=.\CrateRider.cpp
# End Source File
# Begin Source File

SOURCE=.\Cyborg.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\CyborgBike.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Damager.cpp
# End Source File
# Begin Source File

SOURCE=.\Debris.cpp
# End Source File
# Begin Source File

SOURCE=.\DestroyableArchitecture.cpp
# End Source File
# Begin Source File

SOURCE=.\Devil.cpp
# End Source File
# Begin Source File

SOURCE=.\DevilMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\DevilProjectile.cpp
# End Source File
# Begin Source File

SOURCE=.\DoorController.cpp
# End Source File
# Begin Source File

SOURCE=.\Dragonman.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\EffectMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\Effector.cpp
# End Source File
# Begin Source File

SOURCE=.\Elemental.cpp
# End Source File
# Begin Source File

SOURCE=.\EnemyBase.cpp
# End Source File
# Begin Source File

SOURCE=.\EnemyCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\EnemyDive.cpp
# End Source File
# Begin Source File

SOURCE=.\EnemyFly.cpp
# End Source File
# Begin Source File

SOURCE=.\EnemyMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\EnemyRunInto.cpp
# End Source File
# Begin Source File

SOURCE=.\EnemySpawner.cpp
# End Source File
# Begin Source File

SOURCE=.\EnvironmentBase.cpp
# End Source File
# Begin Source File

SOURCE=.\EnvironmentMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\Eruptor.cpp
# End Source File
# Begin Source File

SOURCE=.\Eyeman.cpp
# End Source File
# Begin Source File

SOURCE=.\Fish.cpp
# End Source File
# Begin Source File

SOURCE=.\Fishman.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Flame.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\FogMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\GhostBusterRay.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Gizmo.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\GradientMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\GravityMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\GravityRouter.cpp
# End Source File
# Begin Source File

SOURCE=.\HazeMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\Headman.cpp
# End Source File
# Begin Source File

SOURCE=.\HealthItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Huanman.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Item.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Light.cpp
# End Source File
# Begin Source File

SOURCE=.\Lightning.cpp
# End Source File
# Begin Source File

SOURCE=.\Mamut.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Mamutman.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Mantaman.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Marker.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageHolder.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageItem.cpp
# End Source File
# Begin Source File

SOURCE=.\MirrorMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelDestruction.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelHolder.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelHolder2.cpp
# End Source File
# Begin Source File

SOURCE=.\MovingBrush.cpp
# End Source File
# Begin Source File

SOURCE=.\MovingBrushMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicChanger.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicHolder.cpp
# End Source File
# Begin Source File

SOURCE=.\NavigationMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticlesHolder.cpp
# End Source File
# Begin Source File

SOURCE=.\Pendulum.cpp
# End Source File
# Begin Source File

SOURCE=.\Pipebomb.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerActionMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerAnimator.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerView.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerWeapons.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerWeaponsEffects.cpp
# End Source File
# Begin Source File

SOURCE=.\Projectile.cpp
# End Source File
# Begin Source File

SOURCE=.\PyramidSpaceShip.cpp
# End Source File
# Begin Source File

SOURCE=.\PyramidSpaceShipMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\Reminder.cpp
# End Source File
# Begin Source File

SOURCE=.\RobotDriving.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RobotFlying.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RollingStone.cpp
# End Source File
# Begin Source File

SOURCE=.\Scorpman.cpp
# End Source File
# Begin Source File

SOURCE=.\Ship.cpp
# End Source File
# Begin Source File

SOURCE=.\ShipMarker.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundHolder.cpp
# End Source File
# Begin Source File

SOURCE=.\StormController.cpp
# End Source File
# Begin Source File

SOURCE=.\Switch.cpp
# End Source File
# Begin Source File

SOURCE=.\Teleport.cpp
# End Source File
# Begin Source File

SOURCE=.\TouchField.cpp
# End Source File
# Begin Source File

SOURCE=.\Trigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Twister.cpp
# End Source File
# Begin Source File

SOURCE=.\VoiceHolder.cpp
# End Source File
# Begin Source File

SOURCE=.\Walker.cpp
# End Source File
# Begin Source File

SOURCE=.\Watcher.cpp
# End Source File
# Begin Source File

SOURCE=.\WatchPlayers.cpp
# End Source File
# Begin Source File

SOURCE=.\Water.cpp
# End Source File
# Begin Source File

SOURCE=.\WeaponItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Werebull.cpp
# End Source File
# Begin Source File

SOURCE=.\Woman.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldBase.cpp
# ADD CPP /Yu"StdH.h"
# End Source File
# Begin Source File

SOURCE=.\WorldLink.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldSettingsController.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "Common headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Common\Common.h
# End Source File
# Begin Source File

SOURCE=.\Common\Flags.h
# End Source File
# Begin Source File

SOURCE=.\Common\GameInterface.h
# End Source File
# Begin Source File

SOURCE=.\Common\Particles.h
# End Source File
# Begin Source File

SOURCE=.\Common\PathFinding.h
# End Source File
# Begin Source File

SOURCE=.\StdH\StdH.h
# End Source File
# Begin Source File

SOURCE=.\Common\WeaponPositions.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AirWave.h
# End Source File
# Begin Source File

SOURCE=.\AirWave_tables.h
# End Source File
# Begin Source File

SOURCE=.\AmmoItem.h
# End Source File
# Begin Source File

SOURCE=.\AmmoItem_tables.h
# End Source File
# Begin Source File

SOURCE=.\AmmoPack.h
# End Source File
# Begin Source File

SOURCE=.\AmmoPack_tables.h
# End Source File
# Begin Source File

SOURCE=.\AnimationChanger.h
# End Source File
# Begin Source File

SOURCE=.\AnimationChanger_tables.h
# End Source File
# Begin Source File

SOURCE=.\AnimationHub.h
# End Source File
# Begin Source File

SOURCE=.\AnimationHub_tables.h
# End Source File
# Begin Source File

SOURCE=.\ArmorItem.h
# End Source File
# Begin Source File

SOURCE=.\ArmorItem_tables.h
# End Source File
# Begin Source File

SOURCE=.\BackgroundViewer.h
# End Source File
# Begin Source File

SOURCE=.\BackgroundViewer_tables.h
# End Source File
# Begin Source File

SOURCE=.\BasicEffects.h
# End Source File
# Begin Source File

SOURCE=.\BasicEffects_tables.h
# End Source File
# Begin Source File

SOURCE=.\Beast.h
# End Source File
# Begin Source File

SOURCE=.\BigHead.h
# End Source File
# Begin Source File

SOURCE=.\BigHead_tables.h
# End Source File
# Begin Source File

SOURCE=.\BlendController.h
# End Source File
# Begin Source File

SOURCE=.\BlendController_tables.h
# End Source File
# Begin Source File

SOURCE=.\BloodSpray.h
# End Source File
# Begin Source File

SOURCE=.\Boneman.h
# End Source File
# Begin Source File

SOURCE=.\Boneman_tables.h
# End Source File
# Begin Source File

SOURCE=.\Bouncer.h
# End Source File
# Begin Source File

SOURCE=.\Bouncer_tables.h
# End Source File
# Begin Source File

SOURCE=.\Bullet.h
# End Source File
# Begin Source File

SOURCE=.\Bullet_tables.h
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Camera_tables.h
# End Source File
# Begin Source File

SOURCE=.\CameraMarker.h
# End Source File
# Begin Source File

SOURCE=.\CameraMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\CannonBall.h
# End Source File
# Begin Source File

SOURCE=.\CannonBall_tables.h
# End Source File
# Begin Source File

SOURCE=.\Catman.h
# End Source File
# Begin Source File

SOURCE=.\Catman_tables.h
# End Source File
# Begin Source File

SOURCE=.\Copier.h
# End Source File
# Begin Source File

SOURCE=.\Copier_tables.h
# End Source File
# Begin Source File

SOURCE=.\Counter.h
# End Source File
# Begin Source File

SOURCE=.\Counter_tables.h
# End Source File
# Begin Source File

SOURCE=.\CrateRider.h
# End Source File
# Begin Source File

SOURCE=.\CrateRider_tables.h
# End Source File
# Begin Source File

SOURCE=.\Cyborg.h
# End Source File
# Begin Source File

SOURCE=.\Cyborg_tables.h
# End Source File
# Begin Source File

SOURCE=.\CyborgBike.h
# End Source File
# Begin Source File

SOURCE=.\CyborgBike_tables.h
# End Source File
# Begin Source File

SOURCE=.\Damager.h
# End Source File
# Begin Source File

SOURCE=.\Damager_tables.h
# End Source File
# Begin Source File

SOURCE=.\Debris.h
# End Source File
# Begin Source File

SOURCE=.\Debris_tables.h
# End Source File
# Begin Source File

SOURCE=.\DestroyableArchitecture.h
# End Source File
# Begin Source File

SOURCE=.\DestroyableArchitecture_tables.h
# End Source File
# Begin Source File

SOURCE=.\Devil.h
# End Source File
# Begin Source File

SOURCE=.\Devil_tables.h
# End Source File
# Begin Source File

SOURCE=.\DevilMarker.h
# End Source File
# Begin Source File

SOURCE=.\DevilMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\DevilProjectile.h
# End Source File
# Begin Source File

SOURCE=.\DevilProjectile_tables.h
# End Source File
# Begin Source File

SOURCE=.\DoorController.h
# End Source File
# Begin Source File

SOURCE=.\DoorController_tables.h
# End Source File
# Begin Source File

SOURCE=.\Dragonman.h
# End Source File
# Begin Source File

SOURCE=.\Dragonman_tables.h
# End Source File
# Begin Source File

SOURCE=.\EffectMarker.h
# End Source File
# Begin Source File

SOURCE=.\EffectMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\Effector.h
# End Source File
# Begin Source File

SOURCE=.\Effector_tables.h
# End Source File
# Begin Source File

SOURCE=.\Elemental.h
# End Source File
# Begin Source File

SOURCE=.\Elemental_tables.h
# End Source File
# Begin Source File

SOURCE=.\EnemyBase.h
# End Source File
# Begin Source File

SOURCE=.\EnemyBase_tables.h
# End Source File
# Begin Source File

SOURCE=.\EnemyCounter.h
# End Source File
# Begin Source File

SOURCE=.\EnemyCounter_tables.h
# End Source File
# Begin Source File

SOURCE=.\EnemyDive.h
# End Source File
# Begin Source File

SOURCE=.\EnemyDive_tables.h
# End Source File
# Begin Source File

SOURCE=.\EnemyFly.h
# End Source File
# Begin Source File

SOURCE=.\EnemyFly_tables.h
# End Source File
# Begin Source File

SOURCE=.\EnemyMarker.h
# End Source File
# Begin Source File

SOURCE=.\EnemyMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\EnemyRunInto.h
# End Source File
# Begin Source File

SOURCE=.\EnemyRunInto_tables.h
# End Source File
# Begin Source File

SOURCE=.\EnemySpawner.h
# End Source File
# Begin Source File

SOURCE=.\EnemySpawner_tables.h
# End Source File
# Begin Source File

SOURCE=.\EnvironmentBase.h
# End Source File
# Begin Source File

SOURCE=.\EnvironmentBase_tables.h
# End Source File
# Begin Source File

SOURCE=.\EnvironmentMarker.h
# End Source File
# Begin Source File

SOURCE=.\EnvironmentMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\Eruptor.h
# End Source File
# Begin Source File

SOURCE=.\Eruptor_tables.h
# End Source File
# Begin Source File

SOURCE=.\Eyeman.h
# End Source File
# Begin Source File

SOURCE=.\Eyeman_tables.h
# End Source File
# Begin Source File

SOURCE=.\Fish.h
# End Source File
# Begin Source File

SOURCE=.\Fisherman.h
# End Source File
# Begin Source File

SOURCE=.\Fisherman_tables.h
# End Source File
# Begin Source File

SOURCE=.\Fishman.h
# End Source File
# Begin Source File

SOURCE=.\Fishman_tables.h
# End Source File
# Begin Source File

SOURCE=.\Flame.h
# End Source File
# Begin Source File

SOURCE=.\Flame_tables.h
# End Source File
# Begin Source File

SOURCE=.\FogMarker.h
# End Source File
# Begin Source File

SOURCE=.\FogMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\GhostBusterRay.h
# End Source File
# Begin Source File

SOURCE=.\GhostBusterRay_tables.h
# End Source File
# Begin Source File

SOURCE=.\Gizmo.h
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\Global_tables.h
# End Source File
# Begin Source File

SOURCE=.\GradientMarker.h
# End Source File
# Begin Source File

SOURCE=.\GradientMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\GravityMarker.h
# End Source File
# Begin Source File

SOURCE=.\GravityMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\GravityRouter.h
# End Source File
# Begin Source File

SOURCE=.\GravityRouter_tables.h
# End Source File
# Begin Source File

SOURCE=.\HazeMarker.h
# End Source File
# Begin Source File

SOURCE=.\HazeMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\Headman.h
# End Source File
# Begin Source File

SOURCE=.\Headman_tables.h
# End Source File
# Begin Source File

SOURCE=.\HealthItem.h
# End Source File
# Begin Source File

SOURCE=.\HealthItem_tables.h
# End Source File
# Begin Source File

SOURCE=.\Huanman.h
# End Source File
# Begin Source File

SOURCE=.\Huanman_tables.h
# End Source File
# Begin Source File

SOURCE=.\Item.h
# End Source File
# Begin Source File

SOURCE=.\Item_tables.h
# End Source File
# Begin Source File

SOURCE=.\KeyItem.h
# End Source File
# Begin Source File

SOURCE=.\KeyItem_tables.h
# End Source File
# Begin Source File

SOURCE=.\Light.h
# End Source File
# Begin Source File

SOURCE=.\Light_tables.h
# End Source File
# Begin Source File

SOURCE=.\Lightning.h
# End Source File
# Begin Source File

SOURCE=.\Lightning_tables.h
# End Source File
# Begin Source File

SOURCE=.\Mamut.h
# End Source File
# Begin Source File

SOURCE=.\Mamut_tables.h
# End Source File
# Begin Source File

SOURCE=.\Mamutman.h
# End Source File
# Begin Source File

SOURCE=.\Mamutman_tables.h
# End Source File
# Begin Source File

SOURCE=.\Mantaman.h
# End Source File
# Begin Source File

SOURCE=.\Mantaman_tables.h
# End Source File
# Begin Source File

SOURCE=.\Marker.h
# End Source File
# Begin Source File

SOURCE=.\Marker_tables.h
# End Source File
# Begin Source File

SOURCE=.\MessageHolder.h
# End Source File
# Begin Source File

SOURCE=.\MessageHolder_tables.h
# End Source File
# Begin Source File

SOURCE=.\MessageItem.h
# End Source File
# Begin Source File

SOURCE=.\MessageItem_tables.h
# End Source File
# Begin Source File

SOURCE=.\MirrorMarker.h
# End Source File
# Begin Source File

SOURCE=.\MirrorMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\ModelDestruction.h
# End Source File
# Begin Source File

SOURCE=.\ModelDestruction_tables.h
# End Source File
# Begin Source File

SOURCE=.\ModelHolder.h
# End Source File
# Begin Source File

SOURCE=.\ModelHolder2.h
# End Source File
# Begin Source File

SOURCE=.\ModelHolder2_tables.h
# End Source File
# Begin Source File

SOURCE=.\ModelHolder_tables.h
# End Source File
# Begin Source File

SOURCE=.\MovingBrush.h
# End Source File
# Begin Source File

SOURCE=.\MovingBrush_tables.h
# End Source File
# Begin Source File

SOURCE=.\MovingBrushMarker.h
# End Source File
# Begin Source File

SOURCE=.\MovingBrushMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\MusicChanger.h
# End Source File
# Begin Source File

SOURCE=.\MusicChanger_tables.h
# End Source File
# Begin Source File

SOURCE=.\MusicHolder.h
# End Source File
# Begin Source File

SOURCE=.\MusicHolder_tables.h
# End Source File
# Begin Source File

SOURCE=.\NavigationMarker.h
# End Source File
# Begin Source File

SOURCE=.\NavigationMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\ParticlesHolder.h
# End Source File
# Begin Source File

SOURCE=.\ParticlesHolder_tables.h
# End Source File
# Begin Source File

SOURCE=.\Pendulum.h
# End Source File
# Begin Source File

SOURCE=.\Pendulum_tables.h
# End Source File
# Begin Source File

SOURCE=.\Pipebomb.h
# End Source File
# Begin Source File

SOURCE=.\Pipebomb_tables.h
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\Player_tables.h
# End Source File
# Begin Source File

SOURCE=.\PlayerActionMarker.h
# End Source File
# Begin Source File

SOURCE=.\PlayerActionMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\PlayerAnimator.h
# End Source File
# Begin Source File

SOURCE=.\PlayerAnimator_tables.h
# End Source File
# Begin Source File

SOURCE=.\PlayerMarker.h
# End Source File
# Begin Source File

SOURCE=.\PlayerMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\PlayerView.h
# End Source File
# Begin Source File

SOURCE=.\PlayerView_tables.h
# End Source File
# Begin Source File

SOURCE=.\PlayerWeapons.h
# End Source File
# Begin Source File

SOURCE=.\PlayerWeapons_tables.h
# End Source File
# Begin Source File

SOURCE=.\PlayerWeaponsEffects.h
# End Source File
# Begin Source File

SOURCE=.\PlayerWeaponsEffects_tables.h
# End Source File
# Begin Source File

SOURCE=.\Projectile.h
# End Source File
# Begin Source File

SOURCE=.\Projectile_tables.h
# End Source File
# Begin Source File

SOURCE=.\PyramidSpaceShip.h
# End Source File
# Begin Source File

SOURCE=.\PyramidSpaceShip_tables.h
# End Source File
# Begin Source File

SOURCE=.\PyramidSpaceShipMarker.h
# End Source File
# Begin Source File

SOURCE=.\PyramidSpaceShipMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\Reminder.h
# End Source File
# Begin Source File

SOURCE=.\Reminder_tables.h
# End Source File
# Begin Source File

SOURCE=.\RobotDriving.h
# End Source File
# Begin Source File

SOURCE=.\RobotDriving_tables.h
# End Source File
# Begin Source File

SOURCE=.\RobotFlying.h
# End Source File
# Begin Source File

SOURCE=.\RobotFlying_tables.h
# End Source File
# Begin Source File

SOURCE=.\RollingStone.h
# End Source File
# Begin Source File

SOURCE=.\RollingStone_tables.h
# End Source File
# Begin Source File

SOURCE=.\Scorpman.h
# End Source File
# Begin Source File

SOURCE=.\Scorpman_tables.h
# End Source File
# Begin Source File

SOURCE=.\Ship.h
# End Source File
# Begin Source File

SOURCE=.\Ship_tables.h
# End Source File
# Begin Source File

SOURCE=.\ShipMarker.h
# End Source File
# Begin Source File

SOURCE=.\ShipMarker_tables.h
# End Source File
# Begin Source File

SOURCE=.\SoundHolder.h
# End Source File
# Begin Source File

SOURCE=.\SoundHolder_tables.h
# End Source File
# Begin Source File

SOURCE=.\StormController.h
# End Source File
# Begin Source File

SOURCE=.\StormController_tables.h
# End Source File
# Begin Source File

SOURCE=.\Switch.h
# End Source File
# Begin Source File

SOURCE=.\Switch_tables.h
# End Source File
# Begin Source File

SOURCE=.\Teleport.h
# End Source File
# Begin Source File

SOURCE=.\Teleport_tables.h
# End Source File
# Begin Source File

SOURCE=.\TouchField.h
# End Source File
# Begin Source File

SOURCE=.\TouchField_tables.h
# End Source File
# Begin Source File

SOURCE=.\Trigger.h
# End Source File
# Begin Source File

SOURCE=.\Trigger_tables.h
# End Source File
# Begin Source File

SOURCE=.\Twister.h
# End Source File
# Begin Source File

SOURCE=.\Twister_tables.h
# End Source File
# Begin Source File

SOURCE=.\VoiceHolder.h
# End Source File
# Begin Source File

SOURCE=.\VoiceHolder_tables.h
# End Source File
# Begin Source File

SOURCE=.\Walker.h
# End Source File
# Begin Source File

SOURCE=.\Walker_tables.h
# End Source File
# Begin Source File

SOURCE=.\Watcher.h
# End Source File
# Begin Source File

SOURCE=.\Watcher_tables.h
# End Source File
# Begin Source File

SOURCE=.\WatchPlayers.h
# End Source File
# Begin Source File

SOURCE=.\WatchPlayers_tables.h
# End Source File
# Begin Source File

SOURCE=.\Water.h
# End Source File
# Begin Source File

SOURCE=.\Water_tables.h
# End Source File
# Begin Source File

SOURCE=.\WeaponItem.h
# End Source File
# Begin Source File

SOURCE=.\WeaponItem_tables.h
# End Source File
# Begin Source File

SOURCE=.\Werebull.h
# End Source File
# Begin Source File

SOURCE=.\Werebull_tables.h
# End Source File
# Begin Source File

SOURCE=.\Woman.h
# End Source File
# Begin Source File

SOURCE=.\Woman_tables.h
# End Source File
# Begin Source File

SOURCE=.\WorldBase.h
# End Source File
# Begin Source File

SOURCE=.\WorldBase_tables.h
# End Source File
# Begin Source File

SOURCE=.\WorldLink.h
# End Source File
# Begin Source File

SOURCE=.\WorldLink_tables.h
# End Source File
# Begin Source File

SOURCE=.\WorldSettingsController.h
# End Source File
# Begin Source File

SOURCE=.\WorldSettingsController_tables.h
# End Source File
# End Group
# Begin Group "Class Files"

# PROP Default_Filter "es"
# Begin Group "AI"

# PROP Default_Filter "es"
# Begin Source File

SOURCE=.\NavigationMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\NavigationMarker.es
InputName=NavigationMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\NavigationMarker.es
InputName=NavigationMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Reminder.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Reminder.es
InputName=Reminder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Reminder.es
InputName=Reminder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Watcher.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Watcher.es
InputName=Watcher

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Watcher.es
InputName=Watcher

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WatchPlayers.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\WatchPlayers.es
InputName=WatchPlayers

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\WatchPlayers.es
InputName=WatchPlayers

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Brushes"

# PROP Default_Filter "es"
# Begin Source File

SOURCE=.\Bouncer.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Bouncer.es
InputName=Bouncer

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Bouncer.es
InputName=Bouncer

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DestroyableArchitecture.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\DestroyableArchitecture.es
InputName=DestroyableArchitecture

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\DestroyableArchitecture.es
InputName=DestroyableArchitecture

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MovingBrush.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MovingBrush.es
InputName=MovingBrush

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MovingBrush.es
InputName=MovingBrush

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MovingBrushMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MovingBrushMarker.es
InputName=MovingBrushMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MovingBrushMarker.es
InputName=MovingBrushMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Pendulum.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Pendulum.es
InputName=Pendulum

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Pendulum.es
InputName=Pendulum

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ship.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Ship.es
InputName=Ship

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Ship.es
InputName=Ship

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ShipMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ShipMarker.es
InputName=ShipMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ShipMarker.es
InputName=ShipMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WorldBase.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\WorldBase.es
InputName=WorldBase

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\WorldBase.es
InputName=WorldBase

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Effects"

# PROP Default_Filter "es"
# Begin Source File

SOURCE=.\BasicEffects.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\BasicEffects.es
InputName=BasicEffects

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\BasicEffects.es
InputName=BasicEffects

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BlendController.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\BlendController.es
InputName=BlendController

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\BlendController.es
InputName=BlendController

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BloodSpray.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\BloodSpray.es
InputName=BloodSpray

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\BloodSpray.es
InputName=BloodSpray

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Debris.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Debris.es
InputName=Debris

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Debris.es
InputName=Debris

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EffectMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EffectMarker.es
InputName=EffectMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EffectMarker.es
InputName=EffectMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Effector.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Effector.es
InputName=Effector

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Effector.es
InputName=Effector

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Lightning.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Lightning.es
InputName=Lightning

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Lightning.es
InputName=Lightning

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PyramidSpaceShip.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PyramidSpaceShip.es
InputName=PyramidSpaceShip

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PyramidSpaceShip.es
InputName=PyramidSpaceShip

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PyramidSpaceShipMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PyramidSpaceShipMarker.es
InputName=PyramidSpaceShipMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PyramidSpaceShipMarker.es
InputName=PyramidSpaceShipMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RollingStone.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\RollingStone.es
InputName=RollingStone

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\RollingStone.es
InputName=RollingStone

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                              ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StormController.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\StormController.es
InputName=StormController

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                  ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\StormController.es
InputName=StormController

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                  ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WorldSettingsController.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\WorldSettingsController.es
InputName=WorldSettingsController

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                  ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\WorldSettingsController.es
InputName=WorldSettingsController

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                  ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Enemies"

# PROP Default_Filter "es"
# Begin Source File

SOURCE=.\Beast.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Beast.es
InputName=Beast

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Beast.es
InputName=Beast

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BigHead.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\BigHead.es
InputName=BigHead

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\BigHead.es
InputName=BigHead

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Boneman.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Boneman.es
InputName=Boneman

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Boneman.es
InputName=Boneman

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Catman.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\CrateRider.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\CrateRider.es
InputName=CrateRider

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\CrateRider.es
InputName=CrateRider

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Cyborg.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\CyborgBike.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Devil.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Devil.es
InputName=Devil

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Devil.es
InputName=Devil

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DevilMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\DevilMarker.es
InputName=DevilMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\DevilMarker.es
InputName=DevilMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dragonman.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Elemental.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Elemental.es
InputName=Elemental

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Elemental.es
InputName=Elemental

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EnemyBase.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyBase.es
InputName=EnemyBase

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyBase.es
InputName=EnemyBase

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EnemyCounter.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyCounter.es
InputName=EnemyCounter

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyCounter.es
InputName=EnemyCounter

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EnemyDive.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyDive.es
InputName=EnemyDive

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyDive.es
InputName=EnemyDive

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EnemyFly.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyFly.es
InputName=EnemyFly

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyFly.es
InputName=EnemyFly

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EnemyMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyMarker.es
InputName=EnemyMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyMarker.es
InputName=EnemyMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EnemyRunInto.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyRunInto.es
InputName=EnemyRunInto

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemyRunInto.es
InputName=EnemyRunInto

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EnemySpawner.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemySpawner.es
InputName=EnemySpawner

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnemySpawner.es
InputName=EnemySpawner

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Eyeman.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Eyeman.es
InputName=Eyeman

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Eyeman.es
InputName=Eyeman

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Fish.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Fish.es
InputName=Fish

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Fish.es
InputName=Fish

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Fishman.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Gizmo.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Gizmo.es
InputName=Gizmo

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Gizmo.es
InputName=Gizmo

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Headman.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Headman.es
InputName=Headman

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Headman.es
InputName=Headman

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Huanman.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Mamut.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Mamutman.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Mantaman.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RobotDriving.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RobotFixed.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RobotFlying.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Scorpman.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Scorpman.es
InputName=Scorpman

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Scorpman.es
InputName=Scorpman

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Walker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Walker.es
InputName=Walker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Walker.es
InputName=Walker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Werebull.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Werebull.es
InputName=Werebull

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Werebull.es
InputName=Werebull

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Woman.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Woman.es
InputName=Woman

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Woman.es
InputName=Woman

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Environment"

# PROP Default_Filter "es"
# Begin Source File

SOURCE=.\EnvironmentBase.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnvironmentBase.es
InputName=EnvironmentBase

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnvironmentBase.es
InputName=EnvironmentBase

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EnvironmentMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnvironmentMarker.es
InputName=EnvironmentMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\EnvironmentMarker.es
InputName=EnvironmentMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Items"

# PROP Default_Filter "es"
# Begin Source File

SOURCE=.\AmmoItem.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\AmmoItem.es
InputName=AmmoItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\AmmoItem.es
InputName=AmmoItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AmmoPack.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\AmmoPack.es
InputName=AmmoPack

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\AmmoPack.es
InputName=AmmoPack

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ArmorItem.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ArmorItem.es
InputName=ArmorItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ArmorItem.es
InputName=ArmorItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HealthItem.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\HealthItem.es
InputName=HealthItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\HealthItem.es
InputName=HealthItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Item.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Item.es
InputName=Item

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Item.es
InputName=Item

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KeyItem.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\KeyItem.es
InputName=KeyItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\KeyItem.es
InputName=KeyItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MessageItem.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MessageItem.es
InputName=MessageItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MessageItem.es
InputName=MessageItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WeaponItem.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\WeaponItem.es
InputName=WeaponItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\WeaponItem.es
InputName=WeaponItem

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Players"

# PROP Default_Filter "es"
# Begin Source File

SOURCE=.\Player.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Player.es
InputName=Player

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Player.es
InputName=Player

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlayerActionMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerActionMarker.es
InputName=PlayerActionMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerActionMarker.es
InputName=PlayerActionMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlayerAnimator.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerAnimator.es
InputName=PlayerAnimator

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerAnimator.es
InputName=PlayerAnimator

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlayerMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerMarker.es
InputName=PlayerMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerMarker.es
InputName=PlayerMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlayerView.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerView.es
InputName=PlayerView

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerView.es
InputName=PlayerView

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlayerWeapons.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerWeapons.es
InputName=PlayerWeapons

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerWeapons.es
InputName=PlayerWeapons

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlayerWeaponsEffects.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerWeaponsEffects.es
InputName=PlayerWeaponsEffects

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\PlayerWeaponsEffects.es
InputName=PlayerWeaponsEffects

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Tools"

# PROP Default_Filter "es"
# Begin Source File

SOURCE=.\AnimationChanger.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\AnimationChanger.es
InputName=AnimationChanger

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\AnimationChanger.es
InputName=AnimationChanger

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AnimationHub.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\AnimationHub.es
InputName=AnimationHub

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\AnimationHub.es
InputName=AnimationHub

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BackgroundViewer.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\BackgroundViewer.es
InputName=BackgroundViewer

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\BackgroundViewer.es
InputName=BackgroundViewer

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Camera.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Camera.es
InputName=Camera

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Camera.es
InputName=Camera

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CameraMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\CameraMarker.es
InputName=CameraMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\CameraMarker.es
InputName=CameraMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Copier.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Copier.es
InputName=Copier

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Copier.es
InputName=Copier

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Counter.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Counter.es
InputName=Counter

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Counter.es
InputName=Counter

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Damager.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Damager.es
InputName=Damager

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                             ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Damager.es
InputName=Damager

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                             ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DoorController.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\DoorController.es
InputName=DoorController

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\DoorController.es
InputName=DoorController

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Eruptor.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Eruptor.es
InputName=Eruptor

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Eruptor.es
InputName=Eruptor

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FogMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\FogMarker.es
InputName=FogMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\FogMarker.es
InputName=FogMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GradientMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\GradientMarker.es
InputName=GradientMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\GradientMarker.es
InputName=GradientMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GravityMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\GravityMarker.es
InputName=GravityMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\GravityMarker.es
InputName=GravityMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GravityRouter.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\GravityRouter.es
InputName=GravityRouter

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\GravityRouter.es
InputName=GravityRouter

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HazeMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\HazeMarker.es
InputName=HazeMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\HazeMarker.es
InputName=HazeMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Light.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Light.es
InputName=Light

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Light.es
InputName=Light

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Marker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Marker.es
InputName=Marker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Marker.es
InputName=Marker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MessageHolder.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MessageHolder.es
InputName=MessageHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MessageHolder.es
InputName=MessageHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MirrorMarker.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MirrorMarker.es
InputName=MirrorMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MirrorMarker.es
InputName=MirrorMarker

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ModelDestruction.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ModelDestruction.es
InputName=ModelDestruction

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ModelDestruction.es
InputName=ModelDestruction

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ModelHolder.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ModelHolder.es
InputName=ModelHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ModelHolder.es
InputName=ModelHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ModelHolder2.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ModelHolder2.es
InputName=ModelHolder2

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ModelHolder2.es
InputName=ModelHolder2

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MusicChanger.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MusicChanger.es
InputName=MusicChanger

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MusicChanger.es
InputName=MusicChanger

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MusicHolder.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MusicHolder.es
InputName=MusicHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\MusicHolder.es
InputName=MusicHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ParticlesHolder.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ParticlesHolder.es
InputName=ParticlesHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\ParticlesHolder.es
InputName=ParticlesHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SoundHolder.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\SoundHolder.es
InputName=SoundHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\SoundHolder.es
InputName=SoundHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Switch.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Switch.es
InputName=Switch

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Switch.es
InputName=Switch

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Teleport.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Teleport.es
InputName=Teleport

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Teleport.es
InputName=Teleport

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TouchField.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\TouchField.es
InputName=TouchField

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\TouchField.es
InputName=TouchField

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Trigger.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Trigger.es
InputName=Trigger

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Trigger.es
InputName=Trigger

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VoiceHolder.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\VoiceHolder.es
InputName=VoiceHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\VoiceHolder.es
InputName=VoiceHolder

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WorldLink.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\WorldLink.es
InputName=WorldLink

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\WorldLink.es
InputName=WorldLink

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Weapons"

# PROP Default_Filter "es"
# Begin Source File

SOURCE=.\AirWave.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Bullet.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Bullet.es
InputName=Bullet

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Bullet.es
InputName=Bullet

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CannonBall.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\CannonBall.es
InputName=CannonBall

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\CannonBall.es
InputName=CannonBall

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DevilProjectile.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\DevilProjectile.es
InputName=DevilProjectile

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\DevilProjectile.es
InputName=DevilProjectile

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Flame.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GhostBusterRay.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Pipebomb.es
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Projectile.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Projectile.es
InputName=Projectile

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Projectile.es
InputName=Projectile

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Twister.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Twister.es
InputName=Twister

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Twister.es
InputName=Twister

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Water.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Water.es
InputName=Water

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Water.es
InputName=Water

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Global.es

!IF  "$(CFG)" == "Entities - Win32 Release"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Global.es
InputName=Global

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                            ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Entities - Win32 Debug"

# Begin Custom Build - Entities/$(InputName).es
InputPath=.\Global.es
InputName=Global

"$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd                                                                  ..\  
	ecc Entities/$(InputName).es 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LastFileID.txt
# End Source File
# End Group
# Begin Group "Links To Package"

# PROP Default_Filter "ecl"
# Begin Source File

SOURCE=..\..\Flesh\Classes\Acid.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\AirWave.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\AmmoItem.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\AmmoPack.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\AnimationChanger.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\AnimationHub.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\ArmorItem.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\BackgroundViewer.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\BasicEffect.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Beast.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\BigHead.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\BlendController.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\BloodSpray.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Boneman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Bouncer.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Bullet.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Camera.ecl
# End Source File
# Begin Source File

SOURCE=..\..\FLESH\Classes\CameraMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\CannonBall.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Catman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Copier.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Counter.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\CrateRider.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Cyborg.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\CyborgBike.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Damager.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Debris.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\DestroyableArchitecture.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Devil.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\DevilMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\DevilProjectile.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\DoorController.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Dragonman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\EffectMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Effector.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Elemental.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\EnemyBase.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\EnemyCounter.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\EnemyDive.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\EnemyFly.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\EnemyMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\EnemyRunInto.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\EnemySpawner.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\EnvironmentBase.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\EnvironmentMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Eruptor.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Eyeman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Fish.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Fishman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Flame.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\FogMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\GhostBusterRay.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Gizmo.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\GradientMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\GravityMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\GravityRouter.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\HazeMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Headman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\HealthItem.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Huanman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Item.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\KeyItem.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Light.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Lightning.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Mamut.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Mamutman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Mantaman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Marker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\MessageHolder.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\MessageItem.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\MirrorMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\ModelDestruction.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\ModelHolder.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\ModelHolder2.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\MovingBrush.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\MovingBrushMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\MusicChanger.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\MusicHolder.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\NavigationMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\ParticlesHolder.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Pendulum.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Pipebomb.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Player.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\PlayerActionMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\PlayerAnimator.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\PlayerMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\PlayerView.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\PlayerWeapons.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\PlayerWeaponsEffects.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Projectile.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\PyramidSpaceShip.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\PyramidSpaceShipMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Reminder.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\RobotDriving.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\RobotFixed.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\RobotFlying.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\RollingStone.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Scorpman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Ship.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\ShipMarker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\SoundHolder.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Switch.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Teleport.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\TouchField.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Trigger.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Twister.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\VoiceHolder.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Walker.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Watcher.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\WatchPlayers.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Water.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\WeaponItem.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Werebull.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\Woman.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\WorldBase.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\WorldLink.ecl
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Classes\WorldSettingsController.ecl
# End Source File
# End Group
# Begin Group "Messages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\BeastBig.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\BeastNormal.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\Bomberman.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\Boneman.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\Bull.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\Devil.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\ElementalLava.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\EyemanGreen.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\EyemanPurple.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\Firecracker.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\Fish.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\Gizmo.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\Kamikaze.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\Rocketman.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\ScorpmanGeneral.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\ScorpmanSoldier.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\WalkerBig.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\WalkerSmall.txt
# End Source File
# Begin Source File

SOURCE=..\..\Flesh\Data\Messages\Enemies\Woman.txt
# End Source File
# End Group
# End Target
# End Project
