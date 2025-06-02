// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FairyGUIModule.h"
#if WITH_EDITOR
#include "Editor.h"
#include "FairyApplication.h"
#endif
#include "UI/UIConfig.h"

#define LOCTEXT_NAMESPACE "FFairyGUIModule"

void FFairyGUIModule::StartupModule()
{
}

void FFairyGUIModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFairyGUIModule, FairyGUI)