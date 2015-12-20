#include "r3dpch.h"
#include "r3d.h"

#include "ChemLight.h"
#include "ObjectsCode\Effects\obj_ParticleSystem.h"
#include "..\world\DecalChief.h"
#include "..\world\MaterialTypes.h"
#include "ExplosionVisualController.h"
#include "FlashbangVisualController.h"
#include "Gameplay_Params.h"

#include "multiplayer/P2PMessages.h"

#include "..\..\multiplayer\ClientGameLogic.h"
#include "..\ai\AI_Player.H"
#include "WeaponConfig.h"
#include "Weapon.h"

IMPLEMENT_CLASS(obj_ChemLight, "obj_ChemLight", "Object");
AUTOREGISTER_CLASS(obj_ChemLight);

obj_ChemLight::obj_ChemLight()
{
}

void obj_ChemLight::OnExplode()
{
	// Do Nothing!
}
