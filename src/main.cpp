////////////////////////////////////////////////////////////////////////////
//
// Matrix Trails Screensaver for XBox Media Center
// Copyright (c) 2005 Joakim Eriksson <je@plane9.com>
//
// Thanks goes to Warren for his 'TestXBS' program!
// Matrix Symbol Font by Lexandr (mCode 1.5 - http://www.deviantart.com/deviation/2040700/)
//
// To run the screensaver copy over the MatrixTrails.xbs, MatrixTrails.tga
// and MatrixTrails.xml to the screensaver dir in xbmc
//
////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "matrixtrails.h"
#include "XmlDocument.h"
#include "include/xbmc_scr_dll.h"
#include "include/xbmc_addon_cpp_dll.h"
#include "timer.h"
#include <time.h>

#define CONFIG_FILE "special://home/addons/screensaver.matrixtrails/config.xml"

static char gScrName[1024];

CMatrixTrails*	gMatrixTrails = null;
CRenderD3D		gRender;
CTimer*			gTimer = null;
CConfig			gConfig;

extern "C" void ADDON_Stop();

#define TEXTURESIZE		256				// Width & height of the texture we are using

////////////////////////////////////////////////////////////////////////////
// XBMC has loaded us into memory, we should set our core values
// here and load any settings we may have from our config file
//
extern "C" ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!props)
    return ADDON_STATUS_UNKNOWN;

  SCR_PROPS* scrprops = (SCR_PROPS*)props;

	strcpy(gScrName,scrprops->name);
	gConfig.SetDefaults();
	gConfig.LoadSettings();

	gRender.m_D3dDevice = (LPDIRECT3DDEVICE8)scrprops->device;
	gRender.m_Width	= scrprops->width;
	gRender.m_Height= scrprops->height;

  return ADDON_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////
// XBMC tells us we should get ready to start rendering. This function
// is called once when the screensaver is activated by XBMC.
//
extern "C" void Start()
{
	srand((u32)time(null));
	gMatrixTrails = new CMatrixTrails();
	if (!gMatrixTrails)
		return;
	gTimer = new CTimer();
	gTimer->Init();
	if (!gMatrixTrails->RestoreDevice(&gRender))
		ADDON_Stop();
}

////////////////////////////////////////////////////////////////////////////
// XBMC tells us to render a frame of our screensaver. This is called on
// each frame render in XBMC, you should render a single frame only - the DX
// device will already have been cleared.
//
extern "C" void Render()
{
	if (!gMatrixTrails)
		return;
	gTimer->Update();
	gMatrixTrails->Update(gTimer->GetDeltaTime());
	gMatrixTrails->Draw(&gRender);
}

////////////////////////////////////////////////////////////////////////////
// XBMC tells us to stop the screensaver we should free any memory and release
// any resources we have created.
//
extern "C" void ADDON_Stop()
{
	if (!gMatrixTrails)
		return;
	gMatrixTrails->InvalidateDevice(&gRender);
	SAFE_DELETE(gMatrixTrails);
	SAFE_DELETE(gTimer);
}

////////////////////////////////////////////////////////////////////////////
// not used, but can be used to pass info back to XBMC if required in the future
//
extern "C" void GetInfo(SCR_INFO* pInfo)
{
	return;
}

//-- Destroy-------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Destroy()
{
}

//-- HasSettings --------------------------------------------------------------
// Returns true if this add-on use settings
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" bool ADDON_HasSettings()
{
  return false;
}

//-- GetStatus ---------------------------------------------------------------
// Returns the current Status of this visualisation
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

//-- GetSettings --------------------------------------------------------------
// Return the settings for XBMC to display
//-----------------------------------------------------------------------------

extern "C" unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}

//-- FreeSettings --------------------------------------------------------------
// Free the settings struct passed from XBMC
//-----------------------------------------------------------------------------
extern "C" void ADDON_FreeSettings()
{
}

//-- UpdateSetting ------------------------------------------------------------
// Handle setting change request from XBMC
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_SetSetting(const char* id, const void* value)
{
  return ADDON_STATUS_UNKNOWN;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
void	CConfig::SetDefaults()
{
	m_CharDelayMin	= 0.015f;
	m_CharDelayMax	= 0.060f;
	m_FadeSpeedMin	= 1.0f;
	m_FadeSpeedMax	= 1.5f;
	m_NumColumns	= 50;
	m_NumRows		= 40;
	m_CharCol.Set(0.0f, 1.0f, 0.0f, 1.0f);

	m_NumChars		= 32;
	m_CharSizeTex.x = 32.0f/TEXTURESIZE;
	m_CharSizeTex.y = 26.0f/TEXTURESIZE;
}

////////////////////////////////////////////////////////////////////////////
// Load settings from the [screensavername].xml configuration file
// the name of the screensaver (filename) is used as the name of
// the xml file - this is sent to us by XBMC when the Init func is called.
//
void	CConfig::LoadSettings()
{
	XmlNode node, childNode; //, grandChild;
	CXmlDocument doc;
	
	char szXMLFile[1024];
#ifdef _TEST
	strcpy(szXMLFile, "MatrixTrails.xml");
#else
  strcpy(szXMLFile, CONFIG_FILE);
#endif

	// Load the config file
	if (doc.Load(szXMLFile) >= 0)
	{
		node = doc.GetNextNode(XML_ROOT_NODE);
		while(node > 0)
		{
			if (strcmpi(doc.GetNodeTag(node),"screensaver"))
			{
				node = doc.GetNextNode(node);
				continue;
			}

			if (childNode = doc.GetChildNode(node,"CharDelayMin"))	m_CharDelayMin	= (f32)atof(doc.GetNodeText(childNode));
			if (childNode = doc.GetChildNode(node,"CharDelayMax"))	m_CharDelayMax	= (f32)atof(doc.GetNodeText(childNode));
			if (childNode = doc.GetChildNode(node,"FadeSpeedMin"))	m_FadeSpeedMin	= (f32)atof(doc.GetNodeText(childNode));
			if (childNode = doc.GetChildNode(node,"FadeSpeedMax"))	m_FadeSpeedMax	= (f32)atof(doc.GetNodeText(childNode));

			if (childNode = doc.GetChildNode(node,"NumColumns"))	m_NumColumns	= atoi(doc.GetNodeText(childNode));
			if (childNode = doc.GetChildNode(node,"NumRows"))		m_NumRows		= atoi(doc.GetNodeText(childNode));

			if (childNode = doc.GetChildNode(node,"CharCol"))		sscanf(doc.GetNodeText(childNode), "%f %f %f", &m_CharCol.r, &m_CharCol.g, &m_CharCol.b);

			if (childNode = doc.GetChildNode(node,"NumChars"))		m_NumChars	= atoi(doc.GetNodeText(childNode));

			if (childNode = doc.GetChildNode(node,"CharSizeTexX"))	m_CharSizeTex.x	= (f32)atof(doc.GetNodeText(childNode))/TEXTURESIZE;
			if (childNode = doc.GetChildNode(node,"CharSizeTexY"))	m_CharSizeTex.y	= (f32)atof(doc.GetNodeText(childNode))/TEXTURESIZE;

			node = doc.GetNextNode(node);
		}
		doc.Close();
	}
}
