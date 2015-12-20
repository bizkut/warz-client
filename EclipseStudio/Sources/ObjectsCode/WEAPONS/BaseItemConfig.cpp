#include "r3dPCH.h"
#include "r3d.h"

#include "BaseItemConfig.h"
#include "LangMngr.h"

bool BaseItemConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	r3d_assert(m_Description==NULL);
	r3d_assert(m_StoreIcon==NULL);
	r3d_assert(m_StoreName==NULL);

	category = (STORE_CATEGORIES)xmlItem.attribute("category").as_int();

	m_Weight = xmlItem.attribute("Weight").as_float()/1000.0f; // convert from grams into kg

	m_StoreIcon = strdup(xmlItem.child("Store").attribute("icon").value());
	m_LevelRequired = xmlItem.child("Store").attribute("LevelRequired").as_int();
	
	m_ResWood  = xmlItem.child("Res").attribute("r1").as_int();
	m_ResStone = xmlItem.child("Res").attribute("r2").as_int();
	m_ResMetal = xmlItem.child("Res").attribute("r3").as_int();
	
	bool loadNameFromLangFile = true;
	if(category == storecat_LootBox)
		loadNameFromLangFile = false;

#ifndef WO_SERVER
	if(loadNameFromLangFile)
	{
		char tmpStr[64];
		sprintf(tmpStr, "%d_name", m_itemID);
		m_StoreName = strdup(gLangMngr.getString(tmpStr));
		sprintf(tmpStr, "%d_desc", m_itemID);
		m_Description = strdup(gLangMngr.getString(tmpStr));
	}
	else
#endif
	{
		const char* desc = xmlItem.child("Store").attribute("desc").value();
		r3d_assert(desc);
		m_Description = strdup(desc);
		m_StoreName = strdup(xmlItem.child("Store").attribute("name").value());
	}

	/*FILE* tempFile = fopen_for_write("lang.txt", "ab");
	char tmpStr[2048];
	sprintf(tmpStr, "%d_name=%s\n", m_itemID, m_StoreName);
	fwrite(tmpStr, 1, strlen(tmpStr), tempFile);
	sprintf(tmpStr, "%d_desc=%s\n", m_itemID, m_Description);
	fwrite(tmpStr, 1, strlen(tmpStr), tempFile);
	fclose(tempFile);*/


	return true;
}

bool ModelItemConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	BaseItemConfig::loadBaseFromXml(xmlItem);
	r3d_assert(m_ModelPath==NULL);
	if(!xmlItem.child("Model").attribute("file").empty())
		m_ModelPath = strdup(xmlItem.child("Model").attribute("file").value());

	return true;
}

bool LootBoxConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	BaseItemConfig::loadBaseFromXml(xmlItem);
	return true;
}

bool FoodConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	ModelItemConfig::loadBaseFromXml(xmlItem);
	Health = xmlItem.child("Property").attribute("health").as_float();
	Toxicity = xmlItem.child("Property").attribute("toxicity").as_float();
	Water= xmlItem.child("Property").attribute("water").as_float();
	Food = xmlItem.child("Property").attribute("food").as_float();
	Stamina = R3D_CLAMP(xmlItem.child("Property").attribute("stamina").as_float()/100.0f, 0.0f, 1.0f);

	m_ShopStackSize = xmlItem.child("Property").attribute("shopSS").as_int();

	return true;
}

bool CraftComponentConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	return ModelItemConfig::loadBaseFromXml(xmlItem);
}

bool CraftRecipeConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	ModelItemConfig::loadBaseFromXml(xmlItem);

	// hard coded for now
	switch(m_itemID)
	{
	case 301340: // bandage
		{
			components[numComponents++] = CraftComponent(301331, 1); // rag
			components[numComponents++] = CraftComponent(301366, 1); // ointment
			components[numComponents++] = CraftComponent(301319, 1); // duct tape
			craftedItemID = 101405;
		}
		break;
	case 301341: // silencer
		{
			components[numComponents++] = CraftComponent(301320, 1); // empty plastic bottle
			components[numComponents++] = CraftComponent(301331, 3); // rag
			components[numComponents++] = CraftComponent(301319, 2); // duct tape
			craftedItemID = 400159;
		}
		break;
	case 301342: // canoe paddle
		{
			components[numComponents++] = CraftComponent(101344, 1); // canoe paddle
			components[numComponents++] = CraftComponent(101267, 5); // knife
			components[numComponents++] = CraftComponent(301319, 5); // duct tape
			craftedItemID = 101401; // canoe paddle with knife
		}
		break;
	case 301343: // police baton
		{
			components[numComponents++] = CraftComponent(101389, 1); // police baton
			components[numComponents++] = CraftComponent(301332, 1); // razer wire
			components[numComponents++] = CraftComponent(301319, 1); // duct tape
			craftedItemID = 101406;
		}
		break;
	case 301344: // gas mask
		{
			components[numComponents++] = CraftComponent(301331, 1); // rag
			components[numComponents++] = CraftComponent(101299, 1); // water
			components[numComponents++] = CraftComponent(301357, 1); // charcoal
			components[numComponents++] = CraftComponent(301370, 2); // rope
			craftedItemID = 20206;
		}
		break;
	case 301345: // shiv
		{
			components[numComponents++] = CraftComponent(301335, 1); // scissors
			components[numComponents++] = CraftComponent(301331, 1); // rag
			components[numComponents++] = CraftComponent(301319, 1); // duct tape
			craftedItemID = 101402;
		}
		break;
	case 301346: // spear
		{
			components[numComponents++] = CraftComponent(301355, 1); // broom
			components[numComponents++] = CraftComponent(101267, 1); // knife
			components[numComponents++] = CraftComponent(301319, 1); // duct tape
			craftedItemID = 101407;
		}
		break;
	case 301347: // shrapnel bomb
		{
			components[numComponents++] = CraftComponent(301318, 1); // empty can
			components[numComponents++] = CraftComponent(301327, 3); // metal scrap
			components[numComponents++] = CraftComponent(301324, 2); // gunpowder
			craftedItemID = 101403;
		}
		break;
	case 301348: // trip wire shrapnel bomb
		{
			components[numComponents++] = CraftComponent(301318, 1); // empty can
			components[numComponents++] = CraftComponent(301327, 3); // metal scrap
			components[numComponents++] = CraftComponent(301324, 2); // gunpowder
			components[numComponents++] = CraftComponent(301370, 1); // rope
			components[numComponents++] = CraftComponent(301328, 1); // nails
			craftedItemID = 101404;
		}
		break;
	case 301389: // wooden door block
		{
			components[numComponents++] = CraftComponent(301388, 50); // wood
			craftedItemID = 101352;
		}
		break;
	case 301390: // metal wall 
		{
			components[numComponents++] = CraftComponent(301386, 50); // metal
			craftedItemID = 101353;
		}
		break;
	case 301391: // brick wall 
		{
			components[numComponents++] = CraftComponent(301387, 50); // stone
			craftedItemID = 101354;
		}
		break;
	case 301392: // wood wall 
		{
			components[numComponents++] = CraftComponent(301388, 25); // wood
			craftedItemID = 101355;
		}
		break;
	case 301393: // short brick wall 
		{
			components[numComponents++] = CraftComponent(301387, 25); // stone
			craftedItemID = 101356;
		}
		break;
	case 301394: // farm
		{
			components[numComponents++] = CraftComponent(301387, 100); // stone
			components[numComponents++] = CraftComponent(301386, 50); // metal
			components[numComponents++] = CraftComponent(301388, 100); // wood
			craftedItemID = 101357;
		}
		break;
	case 301395: // solar water
		{
			components[numComponents++] = CraftComponent(301387, 100); // stone
			components[numComponents++] = CraftComponent(301386, 100); // metal
			components[numComponents++] = CraftComponent(301388, 100); // wood
			craftedItemID = 101360;
		}
		break;
	case 301396: // light
		{
			components[numComponents++] = CraftComponent(301387, 50); // stone
			components[numComponents++] = CraftComponent(301386, 50); // metal
			components[numComponents++] = CraftComponent(301388, 25); // wood
			craftedItemID = 101361;
		}
		break;
	case 301397: // small power
		{
			components[numComponents++] = CraftComponent(301387, 75); // stone
			components[numComponents++] = CraftComponent(301386, 50); // metal
			components[numComponents++] = CraftComponent(301388, 75); // wood
			craftedItemID = 101359;
		}
		break;
	case 301398: // big power
		{
			components[numComponents++] = CraftComponent(301387, 150); // stone
			components[numComponents++] = CraftComponent(301386, 100); // metal
			components[numComponents++] = CraftComponent(301388, 150); // wood
			craftedItemID = 101358;
		}
		break;
	default:
		r3d_assert(false && "Unsupported craft recipe!!!");
		break;
	}

	return true;
}

bool VehicleInfoConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	BaseItemConfig::loadBaseFromXml(xmlItem);
	
	m_FNAME = strdup(xmlItem.child("Property").attribute("fname").value());

	//r3dOutToLog("########## !!! Properties: Name:%s, weight:%.4f\n", m_FNAME, m_Weight);

	return true;
}
