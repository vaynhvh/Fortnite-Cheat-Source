#include "Includes.h"
#include "SDK.h"
#include "../../Downloads/Neutron.gg-Soruce-master/Neutron.gg-Soruce-master/minhook/MinHook.h"
#include "ZeroGui.h"
#pragma comment(lib, "minhook.lib")
#include "NewGUI.h"
#include <chrono>

bool ShowMenu = false;

Vector3 o_CamRot;
Vector3 o_CamLoc;
Vector3 OriginalLocation;
Vector3 OriginalRotation;
Vector3 udboxA;

struct FMinimalViewInfo {
	Vector3 Location;
	Vector3 Rotation;
	float FOV;
	float OrthoWidth;
	float OrthoNearClipPlane;
	float OrthoFarClipPlane;
	float AspectRatio;
};


Vector2 pos = { 50, 50 };

uintptr_t BaseAddy = 0;
uintptr_t LocalPawn = 0;
void(*o_GetPlayerViewPoint)(uintptr_t, Vector3*, Vector3*) = nullptr;
void hk_GetPlayerViewPoint(uintptr_t player_controller, Vector3* Location, Vector3* Rotation)
{
	o_GetPlayerViewPoint(player_controller, Location, Rotation);

	OriginalLocation = *Location;
	OriginalRotation = *Rotation;

	if (LocalPawn)
	{

		if (!variables::menu::opened)
		{

			if (StickySilent)
			{
				if (GetAsyncKeyState(VK_RBUTTON))
				{
					Vector3 camloc = *Location;

					Vector3 VectorPos;
					VectorPos.x = udboxA.x - camloc.x;
					VectorPos.y = udboxA.y - camloc.y;
					VectorPos.z = udboxA.z - camloc.z;

					float distance = (double)(sqrtf(VectorPos.x * VectorPos.x + VectorPos.y * VectorPos.y + VectorPos.z * VectorPos.z));

					Vector3 rot;
					rot.x = -((acosf(VectorPos.z / distance) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510)) - 90.f);
					rot.y = atan2f(VectorPos.y, VectorPos.x) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510);
					rot.z = 0;

					*Rotation = rot;
				}
			}

			if (NormalSilent)
			{
				if (GetAsyncKeyState(VK_LBUTTON))
				{
					Vector3 camloc = *Location;

					Vector3 VectorPos;
					VectorPos.x = udboxA.x - camloc.x;
					VectorPos.y = udboxA.y - camloc.y;
					VectorPos.z = udboxA.z - camloc.z;

					float distance = (double)(sqrtf(VectorPos.x * VectorPos.x + VectorPos.y * VectorPos.y + VectorPos.z * VectorPos.z));

					Vector3 rot;
					rot.x = -((acosf(VectorPos.z / distance) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510)) - 90.f);
					rot.y = atan2f(VectorPos.y, VectorPos.x) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510);
					rot.z = 0;

					*Rotation = rot;
				}
			}
		}


	}

}


uintptr_t gCWeapon = 0;
BOOL InstantReloadInProgress = FALSE;
static bool InstantReloadOnce = false;

void(*o_GetViewPoint)(uintptr_t, FMinimalViewInfo*, BYTE) = nullptr;
void hk_GetViewPoint(uintptr_t this_LocalPlayer, FMinimalViewInfo* OutViewInfo, BYTE StereoPass)
{
	o_GetViewPoint(this_LocalPlayer, OutViewInfo, StereoPass);

	if (LocalPawn)
	{
		if (!variables::menu::opened)
		{
			if (NormalSilent)
			{
				if (GetAsyncKeyState(VK_LBUTTON))
				{
					OutViewInfo->Rotation = OriginalRotation;
					OutViewInfo->Location = OriginalLocation;
				}
			}
		}
	}

}

boolean bIsInRectangle(double centerX, double centerY, double radius, double x, double y) {
	return x >= centerX - radius && x <= centerX + radius &&
		y >= centerY - radius && y <= centerY + radius;
}

uintptr_t closestPawn = NULL;
uintptr_t closestPingPawn = NULL;
typedef signed int(__thiscall* tPostRender) (UObject*, UCanvas*);
tPostRender oPostRender = NULL;

bool menu_open = 0;
int  tabs = 0;

signed int __stdcall hPostRender(UObject* pCallObject, UCanvas* pUFunc)
{
	ZeroGUI::CanvasXDD = pUFunc;
	CanvasA = pUFunc;

	if (GetAsyncKeyState((DWORD)VK_INSERT) & 1) {
		menu_open = !menu_open;
	}

	static Vector2 pos = Vector2(700, 700);

	if (ZeroGUI::Window(NULL, &pos, Vector2{ 390.f, 390.f }, menu_open, true))
	{
		ZeroGUI::drawFilledRect(Vector2(pos.X - 4, pos.Y), 390.f, 390, FLinearColor(0.00392, 0.00392, 0.00392, 1.0f));
		drawRect(Vector2(pos.X - 4, pos.Y), 390.f, 390, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		ZeroGUI::Draw_Line(Vector2(pos.X + 2, pos.Y + 40), Vector2(pos.X + 378, pos.Y + 40), 1.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		ZeroGUI::TextLeft(xorstr("NEGRONITE BUILT DATE: " __TIME__ " " __DATE__), Vector2(pos.X + 15, pos.Y + 15), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), false, true);

		ZeroGUI::PushElementY(62);
		if (ZeroGUI::Button(xorstr("Aimbot"), Vector2(120, 30))) tabs = 0;

		ZeroGUI::PushElementY(62);
		ZeroGUI::PushElementX(124);
		if (ZeroGUI::Button(xorstr("Visuals"), Vector2(120, 30))) tabs = 1;

		ZeroGUI::PushElementY(62);
		ZeroGUI::PushElementX(248);
		if (ZeroGUI::Button(xorstr("Exploits"), Vector2(120, 30))) tabs = 2;

		if (tabs == 0)
		{
			ZeroGUI::PushElementX(0);
			ZeroGUI::Checkbox(xorstr("Memory Aimbot"), &MemoryAimbot);
			ZeroGUI::Checkbox(xorstr("Sticky Aimbot"), &StickySilent);
			ZeroGUI::Checkbox(xorstr("Silent Aimbot"), &NormalSilent);
			ZeroGUI::Checkbox(xorstr("Fov Circle"), &FovCircle);
			ZeroGUI::FakeElement();
			ZeroGUI::SliderFloat(xorstr("Fov Circle"), &ThrreFOV, 0.f, 800.f, xorstr("%.1f"));
			ZeroGUI::SliderFloat(xorstr("Aim Smooth"), &AimSmoothness, 0.f, 50.f, xorstr("%.1f"));
		}

		if (tabs == 1)
		{
			ZeroGUI::PushElementX(0);
			ZeroGUI::Checkbox(xorstr("Box 2D"), &Box2D);
			ZeroGUI::Checkbox(xorstr("Box Corner"), &Box);
			ZeroGUI::Checkbox(xorstr("Skeleton"), &Skeleton);
			ZeroGUI::Checkbox(xorstr("Name"), &PlayerName);
			ZeroGUI::Checkbox(xorstr("Weapon / Active (Ammo)"), &PlayerAmmo);
			ZeroGUI::FakeElement();
			ZeroGUI::Checkbox(xorstr("Loot ESP"), &LootESP);
			ZeroGUI::SliderFloat(xorstr("Loot Render Distance (X)"), &LootRenderDist, 0.f, 200.f, xorstr("%.1f"));
		}

		static bool once = 0;
			
		if (tabs == 2)
		{

			if (!once)
			{
				ZeroGUI::PushElementX(0);
				if (ZeroGUI::Button(xorstr("Enable at Own Risk!"), Vector2(130, 30))) once = 1;
			}

			if (once)
			{
				ZeroGUI::PushElementX(0);
				ZeroGUI::Checkbox(xorstr("Rapid Fire"), &NoSpread);
				ZeroGUI::FakeElement();
				ZeroGUI::Checkbox(xorstr("Instant Weapon Reload"), &NoReload);
				ZeroGUI::FakeElement();
				ZeroGUI::Checkbox(xorstr("Spinbot (360 Radius)"), &Spinbot);
				ZeroGUI::FakeElement();
				ZeroGUI::Checkbox(xorstr("Projectile Teleport"), &ProjectileA);
				ZeroGUI::FakeElement();
				ZeroGUI::Checkbox(xorstr("Vehicle Teleport (Wallkill)"), &VehicleFly);
				ZeroGUI::FakeElement();
				ZeroGUI::Checkbox(xorstr("Invisible Yourself"), &Invisible);
				ZeroGUI::FakeElement();
				ZeroGUI::Checkbox(xorstr("Infinite Ammo"), &InfiniteAmmo);
			}
		}
	}

	ZeroGUI::Input::Handle();

	/*CanvasA->K2_DrawTextAMK(Vector3(ZeroGUI::CursorPos().X, ZeroGUI::CursorPos().Y, 0), L"+", Colors::Cyan, false, true);*/

	ZeroGUI::Render();

	if (FovCircle)
		Render_Circle(Vector2(Width / 2, Height / 2), ThrreFOV, Colors::Red);

	CanvasA->K2_DrawLine(Vector3(Width / 2 + 10, Height / 2, 0), Vector3(Width / 2 - 10, Height / 2, 0), Colors::Red, 1.f);
	CanvasA->K2_DrawLine(Vector3(Width / 2, Height / 2 + 10, 0), Vector3(Width / 2, Height / 2 - 10, 0), Colors::Red, 1.f);

	float closestDistance = FLT_MAX;
	bool closestPawnVisible = false;

	uintptr_t MyTeamIndex = 0, EnemyTeamIndex = 0;
	uintptr_t GWorld = read<uintptr_t>(UWORLD); if (!GWorld) return false;

	uintptr_t Gameinstance = read<uint64_t>(GWorld + StaticOffsets::OwningGameInstance); if (!Gameinstance) return false;

	uintptr_t LocalPlayers = read<uint64_t>(Gameinstance + StaticOffsets::LocalPlayers); if (!LocalPlayers) return false;

	uintptr_t LocalPlayer = read<uint64_t>(LocalPlayers); if (!LocalPlayer) return false;

	uintptr_t PlayerController = read<uint64_t>(LocalPlayer + StaticOffsets::PlayerController); if (!PlayerController) return false;

	uintptr_t PlayerCameraManager = read<uint64_t>(PlayerController + StaticOffsets::PlayerCameraManager); if (!PlayerCameraManager) return false;

	LocalPawn = read<uint64_t>(PlayerController + StaticOffsets::AcknowledgedPawn);

	uintptr_t Ulevel = read<uintptr_t>(GWorld + StaticOffsets::PersistentLevel); if (!Ulevel) return false;

	uintptr_t AActors = read<uintptr_t>(Ulevel + StaticOffsets::AActors); if (!AActors) return false;

	uintptr_t ActorCount = read<int>(Ulevel + StaticOffsets::ActorCount); if (!ActorCount) return false;
	uintptr_t LocalRootComponent;
	Vector3 LocalRelativeLocation;

	Vector3 HeadPos, Headbox, bottom, bulletpos;


	auto levels = *(TArray<UObject*>*)((uintptr_t)GWorld + StaticOffsets::Levels);
	for (int a = 0; a < levels.Num(); a++) {
		auto level = levels[a];
		if (!level) continue;

		auto GActorArray = (TArray<UObject*>*)((uintptr_t)level + 0x98);
		auto actors = *GActorArray;
		for (int i = 0; i < actors.Num(); i++) {
			auto CurrentActorA = actors[i];
			if (!CurrentActorA)
				continue;

			auto CurrentActor = (uintptr_t)CurrentActorA;

			auto name = GetObjectName(CurrentActor);

			if (LocalPawn)
			{
				if (ProjectileA && strstr(name, ("B_Prj_")))
				{

					auto root = read<uintptr_t>(CurrentActor + 0x190);
					auto loc = read<Vector3>(root + 0x128);

					WorldToScreen(PlayerController, loc, &loc);

					pUFunc->K2_DrawText(loc, L"Projectile [?m]", FLinearColor(1.f, 0.f, 0.f, 1.f), 1, 0);

					FHitResult xxxx;
					K2_SetActorLocation(CurrentActor, bulletpos, false, true, &xxxx);

					if (GetAsyncKeyState(VK_LBUTTON) && ProjectileA)
					{
						INPUT input = {};
						input.type = INPUT_MOUSE;
						input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
						SendInput(1, &input, sizeof(INPUT));
					}
				}
			}
			//	if (LootESP && strstr(name, ("FortPickup")) || strstr(name, ("Fort_Pickup_Creative_C")))
			//	{
			//		auto Definition = read<uintptr_t>(CurrentActor + 0x318 + 0x18);

			//		auto root = read<uintptr_t>(CurrentActor + 0x190);
			//		auto loc = read<Vector3>(root + 0x128);

			//		int dist = loc.DistanceFrom(o_CamLoc) / 100;

			//		if (dist > LootRenderDist)
			//			continue;

			//		if (Definition)
			//		{
			//			auto root = read<uintptr_t>(CurrentActor + 0x190);
			//			auto loc = read<Vector3>(root + 0x128);
			//			Vector3 ItemPosA;
			//			WorldToScreen(PlayerController, loc, &ItemPosA);
			//			BYTE tier = *(BYTE*)(Definition + 0x73);
			//			auto name = *(FText*)(Definition + 0x90);
			//			auto naam = name.Get();
			//			if (naam)
			//			{
			//				pUFunc->K2_DrawText(ItemPosA, naam, getColorFromTier(tier), 1, 0);
			//			}
			//		}
			//	}
			//}


			if (strstr(name, "BP_PlayerPawn") || strstr(name, "PlayerPawn"))
			{
				if (LocalPawn)
				{
					GetBoneLocation(CurrentActor, 68, &HeadPos);
					GetBoneLocation(CurrentActor, 0, &bottom);

					WorldToScreen(PlayerController, Vector3(HeadPos.x, HeadPos.y, HeadPos.z + 15), &Headbox);
					WorldToScreen(PlayerController, Vector3(bottom.x, bottom.y, bottom.z - 10), &bottom);

					if (Headbox.x == 0 && Headbox.y == 0) continue;
					if (bottom.x == 0 && bottom.y == 0) continue;
					if (CurrentActor == LocalPawn) continue;

					Vector3 NAAAA;
					GetBoneLocation(CurrentActor, 68, &NAAAA);
					WorldToScreen(PlayerController, NAAAA, &NAAAA);
					auto dx = NAAAA.x - (Width / 2);
					auto dy = NAAAA.y - (Height / 2);
					auto dist = sqrtf(dx * dx + dy * dy);
					if (dist < ThrreFOV && dist < closestDistance) {
						closestDistance = dist;
						closestPawn = CurrentActor;
					}

					GetBoneLocation(closestPawn, 68, &udboxA);


					if (NoReload)
					{
						gCWeapon = read<uintptr_t>(LocalPawn + 0x8d8);

						auto xd = read<bool>(gCWeapon + 0x329);

						if (xd)
						{
							write<float>(LocalPawn + 0x64, 90);
						}
						else
						{
							write<float>(LocalPawn + 0x64, 1);
						}
					}
					GetBoneLocation(closestPawn, 68, &bulletpos);

					if (MemoryAimbot)
					{
						auto camerlocation = GetCameraLocation(PlayerCameraManager);
						auto camerrotation = GetCameraRotation(PlayerCameraManager);

						Vector3 VectorPos = udboxA - camerlocation;
						float distance = VectorPos.Length();

						Vector3 RetVector;

						RetVector.x = -((acosf(VectorPos.z / distance) * (float)(180.f / M_PI)) - 90.f);
						RetVector.y = atan2f(VectorPos.y, VectorPos.x) * (float)(180.f / M_PI);

						if (RetVector.x == 0 and RetVector.y == 0) continue;

						RetVector.x = (RetVector.x - camerrotation.x) / 1.0f + AimSmoothness + camerrotation.x;
						RetVector.y = (RetVector.y - camerrotation.y) / 1.0f + AimSmoothness + camerrotation.y;


						/*if (AimbotLine)
						{
							Vector3 Eichel = RetVector;

							WorldToScreen(PlayerController, Eichel, &Eichel);

							pUFunc->K2_DrawLine(Vector3(Height / 2, Width / 2, 0.f), Vector3(Eichel.x, Eichel.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
						}*/

						if (GetAsyncKeyState(VK_RBUTTON))
							SetControlRotation(PlayerController, RetVector, false);
					}

					if (Spinbot)
					{
						uintptr_t Mesh = read<uintptr_t>((uintptr_t)LocalPawn + 0x310);

						if (Spinbot)
						{
							write<Vector3>(Mesh + 0x140, Vector3(0, -rand() % (int)360, 0));
						}
						else
						{
							write<Vector3>(Mesh + 0x140, Vector3(0, -90, 0));
						}
					}

					if (InfiniteAmmo)
					{
						gCWeapon = read<uintptr_t>(LocalPawn + 0x8d8);
						write<int32_t>(gCWeapon + 0xB6C, 999);
					}

					if (NoSpread)
					{
						gCWeapon = read<uintptr_t>(LocalPawn + 0x8d8);
						auto ScaleX = read<float>(gCWeapon + 0xab0);
						auto ScaleY = read<float>(gCWeapon + 0xAB4);
						write<float>(gCWeapon + 0xAB0, ScaleX + ScaleY - 2);
					}

					if (FovChanger)
					{
						FOV(PlayerController, FovChangerA);
					}

					if (CarFly)
					{
						uint64_t VEHICLE_STATS = read<uint64_t>(LocalPawn + 0x2310);
						if (GetAsyncKeyState(VK_SHIFT))write<char>(VEHICLE_STATS + 0x6AA, 1);
					}

					if (CarSpeed)
					{
						uintptr_t LocalVehicle = read<uintptr_t>(LocalPawn + 0x2310);
						if (LocalVehicle)
						{
							write<float>(LocalVehicle + 0x9A0, -0.100);
							write<float>(LocalVehicle + 0x9A4, -0.100);
						}
					}

					if (Invisible)
					{
						SetVisibilty(LocalPawn, true, false);
					}

					if (VehicleFly)
					{
						auto CurrentVehicle = *(uintptr_t*)(LocalPawn + 0x2310);
						if (!CurrentVehicle) return false;
						if (GetAsyncKeyState(VK_SHIFT))
						{
							if (closestPawn)
							{
								K2_SetActorLocation(CurrentVehicle, udboxA, false, true, 0);
							}
						}
					}
				}

				Vector3 HeadPosA, HeadboxA, bottomA;
				Vector3 chesti, chestatright;
				Vector3 head2, botoom, neck, pelvis, chest, leftShoulder, rightShoulder, leftElbow, rightElbow, leftHand, rightHand, leftLeg, rightLeg, leftThigh, rightThigh, leftFoot, rightFoot, leftFeet, rightFeet, leftFeetFinger, rightFeetFinger;

				GetBoneLocation(CurrentActor, 68, &HeadPosA);
				GetBoneLocation(CurrentActor, 0, &bottomA);

				WorldToScreen(PlayerController, Vector3(HeadPosA.x, HeadPosA.y, HeadPosA.z + 20), &HeadboxA);
				WorldToScreen(PlayerController, Vector3(bottomA.x, bottomA.y, bottomA.z - 10), &bottomA);

				GetBoneLocation(CurrentActor, 68, &head2);
				GetBoneLocation(CurrentActor, 0, &botoom);
				GetBoneLocation(CurrentActor, 66, &neck);
				GetBoneLocation(CurrentActor, 2, &pelvis);
				GetBoneLocation(CurrentActor, 37, &chesti);
				GetBoneLocation(CurrentActor, 8, &chestatright);
				GetBoneLocation(CurrentActor, 38, &leftShoulder);
				GetBoneLocation(CurrentActor, 9, &rightShoulder);
				GetBoneLocation(CurrentActor, 39, &leftElbow);
				GetBoneLocation(CurrentActor, 10, &rightElbow);
				GetBoneLocation(CurrentActor, 62, &leftHand);
				GetBoneLocation(CurrentActor, 33, &rightHand);
				GetBoneLocation(CurrentActor, 78, &leftLeg);
				GetBoneLocation(CurrentActor, 71, &rightLeg);
				GetBoneLocation(CurrentActor, 84, &leftThigh);
				GetBoneLocation(CurrentActor, 77, &rightThigh);
				GetBoneLocation(CurrentActor, 81, &leftFoot);
				GetBoneLocation(CurrentActor, 73, &rightFoot);
				GetBoneLocation(CurrentActor, 82, &leftFeet);
				GetBoneLocation(CurrentActor, 86, &rightFeet);
				GetBoneLocation(CurrentActor, 83, &leftFeetFinger);
				GetBoneLocation(CurrentActor, 76, &rightFeetFinger);

				Vector3 Top;
				WorldToScreen(PlayerController, { head2.x, head2.y, head2.z + 20 }, &Top);

				Vector3 Bottom;
				WorldToScreen(PlayerController, { botoom.x, botoom.y, botoom.z - 15 }, &Bottom);

				WorldToScreen(PlayerController, head2, &head2);
				WorldToScreen(PlayerController, botoom, &botoom);
				WorldToScreen(PlayerController, neck, &neck);
				WorldToScreen(PlayerController, pelvis, &pelvis);
				WorldToScreen(PlayerController, chesti, &chesti);
				WorldToScreen(PlayerController, chestatright, &chestatright);
				WorldToScreen(PlayerController, leftShoulder, &leftShoulder);
				WorldToScreen(PlayerController, rightShoulder, &rightShoulder);
				WorldToScreen(PlayerController, leftElbow, &leftElbow);
				WorldToScreen(PlayerController, rightElbow, &rightElbow);
				WorldToScreen(PlayerController, leftHand, &leftHand);
				WorldToScreen(PlayerController, rightHand, &rightHand);
				WorldToScreen(PlayerController, leftLeg, &leftLeg);
				WorldToScreen(PlayerController, rightLeg, &rightLeg);
				WorldToScreen(PlayerController, leftThigh, &leftThigh);
				WorldToScreen(PlayerController, rightThigh, &rightThigh);
				WorldToScreen(PlayerController, leftFoot, &leftFoot);
				WorldToScreen(PlayerController, rightFoot, &rightFoot);
				WorldToScreen(PlayerController, leftFeet, &leftFeet);
				WorldToScreen(PlayerController, rightFeet, &rightFeet);
				WorldToScreen(PlayerController, leftFeetFinger, &leftFeetFinger);
				WorldToScreen(PlayerController, rightFeetFinger, &rightFeetFinger);

				chest.x = chesti.x + ((chestatright.x - chesti.x) / 2);
				chest.y = chesti.y;

				float TextOffset_Top;

				TextOffset_Top = Top.y;

				float TextOffset_Bottom;

				TextOffset_Bottom = Bottom.y;

				float BoxHeight = (float)(botoom.y - head2.y);
				float BoxWidth = BoxHeight * 0.50f;

				float BottomLeftX = (float)head2.x - BoxWidth / 2;
				float BottomLeftY = (float)botoom.y;

				float BottomRightX = (float)head2.x + BoxWidth / 2;
				float BottomRightY = (float)botoom.y;

				float TopRightX = (float)head2.x + BoxWidth / 2;
				float TopRightY = (float)head2.y;

				float TopLeftX = (float)head2.x - BoxWidth / 2;
				float TopLeftY = (float)head2.y;

				float TextOffset_Y = head2.y;
				float TextOffset_X = TopRightX + 5;

				if (PlayerName)
				{
					auto playerState = read<uintptr_t>((uintptr_t)CurrentActor + 0x2a8);

					if (playerState)
					{
						auto name = GetPlayerName(playerState);
						if (name.c_str())
						{
							int ifespisonon = 8;
							TextOffset_Top -= pUFunc->K2_TextSize(name.c_str()).Y + ifespisonon;
							Vector2 calculation = Vector2(Top.x, TextOffset_Top);
							pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), name.c_str(), FLinearColor(1.f, 1.f, 0.f, 1.f), 1, 0);
						}
					}
				}

				int ifespison = 30;

				if (PlayerPlattform)
				{
					auto Platform = GetPlatform(read<uintptr_t>(CurrentActor + 0x2a8));
					if (!Platform.c_str()) continue;
					auto platform_name = Platform.c_str();
					int ifespisonon = 10;

					TextOffset_Top -= pUFunc->K2_TextSize(platform_name).Y + ifespisonon;
					Vector2 calculation = Vector2(Top.x, TextOffset_Top);
					if (wcsstr(platform_name, (L"PSN"))) {
						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), (L"Play Station"), Colors::Blue, true, false);
					}
					else if (wcsstr(platform_name, (L"XBL"))) {
						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), (L"Xbox"), Colors::LightGreen, true, false);
					}
					else if (wcsstr(platform_name, (L"XSX"))) {
						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), (L"Xbox Series S"), Colors::LightGreen, true, false);
					}
					else if (wcsstr(platform_name, (L"WIN"))) {
						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), (L"Windows"), Colors::Red, true, false);
					}
					else if (wcsstr(platform_name, (L"MAC"))) {
						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), (L"Mac OS"), Colors::Yellow, true, false);
					}
					else if (wcsstr(platform_name, (L"LNX"))) {
						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), (L"Linux"), Colors::Yellow, true, false);
					}
					else if (wcsstr(platform_name, (L"IOS"))) {
						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), (L"Ios"), Colors::LightGreen, true, false);
					}
					else if (wcsstr(platform_name, (L"AND"))) {
						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), (L"Android"), Colors::LightGreen, true, false);
					}
					else if (wcsstr(platform_name, (L"SWT"))) {
						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), (L"Nintendo Switch"), Colors::Red, true, false);
					}
				}
				if (Distance)
				{
					if (LocalPawn)
					{
						int ifespisonon = 12;

						auto Distance = (int)(head2.DistanceFrom(o_CamLoc) / 100);
						static char memes[128];
						static wchar_t wmemes[128];
						sprintf(memes, "%d m", Distance);
						AnsiToWide(memes, wmemes);


						TextOffset_Top -= pUFunc->K2_TextSize(wmemes).Y + ifespisonon;
						Vector2 calculation = Vector2(Top.x, TextOffset_Top);

						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), wmemes, FLinearColor(1.f, 1.f, 0.f, 1.f), 1, 0);
					}
					else
					{
						int ifespisonon = 12;

						TextOffset_Top -= pUFunc->K2_TextSize(L"5 m").Y + ifespisonon;
						Vector2 calculation = Vector2(Top.x, TextOffset_Top);

						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), L"5 m", FLinearColor(1.f, 1.f, 0.f, 1.f), 1, 0);
					}
				}

				if (LocalPawn)
				{
					if (PlayerWeapon)
					{
						auto weapon = read<uint64_t>((uintptr_t)CurrentActor + 0x8d8);
						if (weapon)
						{
							auto wdata = read<uint64_t>(weapon + 0x3f0);
							if (wdata)
							{
								auto name = read<FText>(wdata + 0x90);
								auto tier = read<BYTE>(wdata + 0x73);
								auto naam = name.Get();
								if (naam)
								{
									int ifespisonon = 8;

									TextOffset_Bottom -= pUFunc->K2_TextSize(naam).Y - ifespisonon;
									Vector2 calculation = Vector2(Bottom.x, TextOffset_Bottom);

									pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), naam, getColorFromTier(tier), 1, 0);

								}
							}
						}
						auto CurrentAmmo = GetMagazineAmmoCount(weapon);
						auto PerClip = GetBulletsPerClip(weapon);

						static char memes[128];
						static wchar_t wmemes[128];
						sprintf(memes, "%d/%d", CurrentAmmo, PerClip);
						AnsiToWide(memes, wmemes);

						int ifespisonon = 10;

						TextOffset_Bottom -= pUFunc->K2_TextSize(wmemes).Y - ifespisonon;
						Vector2 calculation = Vector2(Bottom.x, TextOffset_Bottom);

						pUFunc->K2_DrawText(Vector3(calculation.X, calculation.Y, 0), wmemes, Colors::Yellow, 1, 0);
					}
				}

				if (Box2D)
				{
					float BoxHeight = bottomA.y - HeadboxA.y;
					float BoxWidth = BoxHeight / 2.1f;
					pUFunc->Draw2DBoundingBox(HeadboxA, BoxWidth, BoxHeight, Colors::Red);
				}

				if (Box)
				{
					float BoxHeight = bottomA.y - HeadboxA.y;
					float BoxWidth = BoxHeight / 2.1f;
					pUFunc->DrawCorneredBox(HeadboxA.x - (BoxWidth / 2), HeadboxA.y, BoxWidth, BoxHeight, Colors::Red, 2);
				}

				if (Skeleton)
				{
					pUFunc->K2_DrawLine(Vector3(head2.x, head2.y, 0.f), Vector3(neck.x, neck.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(neck.x, neck.y, 0.f), Vector3(chest.x, chest.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(chest.x, chest.y, 0.f), Vector3(pelvis.x, pelvis.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(chest.x, chest.y, 0.f), Vector3(leftShoulder.x, leftShoulder.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(chest.x, chest.y, 0.f), Vector3(rightShoulder.x, rightShoulder.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(leftShoulder.x, leftShoulder.y, 0.f), Vector3(leftElbow.x, leftElbow.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(rightShoulder.x, rightShoulder.y, 0.f), Vector3(rightElbow.x, rightElbow.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(leftElbow.x, leftElbow.y, 0.f), Vector3(leftHand.x, leftHand.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(rightElbow.x, rightElbow.y, 0.f), Vector3(rightHand.x, rightHand.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(pelvis.x, pelvis.y, 0.f), Vector3(leftLeg.x, leftLeg.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(pelvis.x, pelvis.y, 0.f), Vector3(rightLeg.x, rightLeg.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(leftLeg.x, leftLeg.y, 0.f), Vector3(leftThigh.x, leftThigh.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(rightLeg.x, rightLeg.y, 0.f), Vector3(rightThigh.x, rightThigh.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(leftThigh.x, leftThigh.y, 0.f), Vector3(leftFoot.x, leftFoot.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(rightThigh.x, rightThigh.y, 0.f), Vector3(rightFoot.x, rightFoot.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(leftFoot.x, leftFoot.y, 0.f), Vector3(leftFeet.x, leftFeet.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(rightFoot.x, rightFoot.y, 0.f), Vector3(rightFeet.x, rightFeet.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(leftFeet.x, leftFeet.y, 0.f), Vector3(leftFeetFinger.x, leftFeetFinger.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(rightFeet.x, rightFeet.y, 0.f), Vector3(rightFeetFinger.x, rightFeetFinger.y, 0.f), FLinearColor(1.f, 0.f, 0.f, 1.f), 1.f);
				}

				if (Line)
				{
					pUFunc->K2_DrawLine(Vector3(Width / 2, Height, 0), Vector3(botoom.x, botoom.y, 0), FLinearColor(1.f, 0.f, 0.f, 1.f), 0.8f);
				}


			}
		}
	}

	return oPostRender(pCallObject, pUFunc);
}

/*else if (Boxes == 2)
				{
					Vector3 BottomNoW2S;
					Vector3 HeadNoW2S;

					GetBoneLocation(CurrentActor, 68, &HeadNoW2S);
					GetBoneLocation(CurrentActor, 0, &BottomNoW2S);


					Vector3 bottom1;
					Vector3 bottom2;
					Vector3 bottom3;
					Vector3 bottom4;

					WorldToScreen(PlayerController, Vector3(BottomNoW2S.x + 55, BottomNoW2S.y - 55, BottomNoW2S.z), &bottom1);
					WorldToScreen(PlayerController, Vector3(BottomNoW2S.x - 55, BottomNoW2S.y - 55, BottomNoW2S.z), &bottom2);
					WorldToScreen(PlayerController, Vector3(BottomNoW2S.x - 55, BottomNoW2S.y + 55, BottomNoW2S.z), &bottom3);
					WorldToScreen(PlayerController, Vector3(BottomNoW2S.x + 55, BottomNoW2S.y + 55, BottomNoW2S.z), &bottom4);

					Vector3 top1;
					Vector3 top2;
					Vector3 top3;
					Vector3 top4;

					WorldToScreen(PlayerController, Vector3(HeadNoW2S.x + 55, HeadNoW2S.y - 55, HeadNoW2S.z), &top1);
					WorldToScreen(PlayerController, Vector3(HeadNoW2S.x - 55, HeadNoW2S.y - 55, HeadNoW2S.z), &top2);
					WorldToScreen(PlayerController, Vector3(HeadNoW2S.x - 55, HeadNoW2S.y + 55, HeadNoW2S.z), &top3);
					WorldToScreen(PlayerController, Vector3(HeadNoW2S.x + 55, HeadNoW2S.y + 55, HeadNoW2S.z), &top4);

					pUFunc->K2_DrawLine(Vector3(bottom1.x, bottom1.y, bottom1.z), Vector3(top1.x, top1.y, top1.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(bottom2.x, bottom2.y, bottom2.z), Vector3(top2.x, top2.y, top2.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(bottom3.x, bottom3.y, bottom3.z), Vector3(top3.x, top3.y, top3.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(bottom4.x, bottom4.y, bottom4.z), Vector3(top4.x, top4.y, top4.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);


					pUFunc->K2_DrawLine(Vector3(bottom1.x, bottom1.y, bottom1.z), Vector3(bottom2.x, bottom2.y, bottom2.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(bottom2.x, bottom2.y, bottom2.z), Vector3(bottom3.x, bottom3.y, bottom3.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(bottom3.x, bottom3.y, bottom3.z), Vector3(bottom4.x, bottom4.y, bottom4.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(bottom4.x, bottom4.y, bottom4.z), Vector3(bottom1.x, bottom1.y, bottom1.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);

					pUFunc->K2_DrawLine(Vector3(top1.x, top1.y, top1.z), Vector3(top2.x, top2.y, top2.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(top2.x, top2.y, top2.z), Vector3(top3.x, top3.y, top3.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(top3.x, top3.y, top3.z), Vector3(top4.x, top4.y, top4.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);
					pUFunc->K2_DrawLine(Vector3(top4.x, top4.y, top3.z), Vector3(top1.x, top1.y, top4.z), FLinearColor(1.f, 1.f, 0.f, 1.f), 1.f);
				}*/


float* (*CalculateShot)(PVOID, PVOID, PVOID) = nullptr;
float* CalculateShotHook(PVOID arg0, PVOID arg1, PVOID arg2) {
	auto ret = CalculateShot(arg0, arg1, arg2);

	if (GetAsyncKeyState(VK_LBUTTON) && ProjectileA)
	{
		INPUT input = {};
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, &input, sizeof(INPUT));
	}

	return ret;
}

//using process_event_t = void(*)(UObject* object, UObject* func, PVOID params, PVOID result);
//process_event_t process_event_riginal = 0;
//
//void process_event_hook(UObject* object, UObject* func, PVOID params, PVOID result)
//{
//	
//	return process_event_riginal(object, func, params, result);
//}


BOOL InitMain()
{
	Width = GetSystemMetrics(0);
	Height = GetSystemMetrics(1);

	//AllocConsole();
	//static_cast<VOID>(freopen(("CONIN$"), ("r"), stdin));
	//static_cast<VOID>(freopen(("CONOUT$"), ("w"), stdout));
	//static_cast<VOID>(freopen(("CONOUT$"), ("w"), stderr));

	BaseAddy = (uintptr_t)GetModuleHandleA(0);

	UWORLD = MemoryHelper::PatternScan("48 89 05 ? ? ? ? 48 8B 4B 78");
	UWORLD = RVA(UWORLD, 7);
	if (!UWORLD)
		MessageBoxA(0, "UWORLD!", "", 0);

	FREEFN = MemoryHelper::PatternScan(xorstr("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0"));
	if (!FREEFN)
		MessageBoxA(0, "FREEFN!", "", 0);
	PROJECTWORLDTOSCREEN = MemoryHelper::PatternScan(xorstr("40 53 55 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 33 DB 45 8A F1"));
	//PROJECTWORLDTOSCREEN = RVA(PROJECTWORLDTOSCREEN, 5);
	if (!PROJECTWORLDTOSCREEN)
		MessageBoxA(0, "PROJECTWORLDTOSCREEN!", "", 0);

	GETNAMEBYINDEX = MemoryHelper::PatternScan(xorstr("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B"));
	if (!GETNAMEBYINDEX)
		MessageBoxA(0, "PROJECTWORLDTOSCREEN!", "", 0);

	BONEMATRIX = MemoryHelper::PatternScan(xorstr("E8 ? ? ? ? 0F 10 40 68"));
	BONEMATRIX = RVA(BONEMATRIX, 5);
	if (!BONEMATRIX)
		MessageBoxA(0, "BONEMATRIX!", "", 0);

	GOBJECT = MemoryHelper::PatternScan(xorstr("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1"));
	GOBJECT = RVA(GOBJECT, 7);
	if (!GOBJECT)
		MessageBoxA(0, "GOBJECT!", "", 0);

	objects = (GObjects*)GOBJECT;

	ROBOTOFONT = (UObject*)FindAllObjects(xorstr("Fonts/Roboto"), xorstr("Roboto")); //0x70
	std::cout << "ROBOTOFONT: " << ROBOTOFONT << std::endl;
	DRAWTEXT = (UObject*)FindObject("K2_DrawText");
	std::cout << "DRAWTEXT: " << DRAWTEXT << std::endl;
	DRAWLINE = (UObject*)FindObject("K2_DrawLine");
	std::cout << "DRAWLINE: " << DRAWLINE << std::endl;
	Projectile = (UClass*)FindObject("FortniteGame.FortProjectileBase");
	std::cout << "Projectile: " << Projectile << std::endl;
	Loot = (UClass*)FindObject("FortniteGame.FortPickup");
	std::cout << "FortPickup: " << Loot << std::endl;
	PawnAthena = (UClass*)FindObject("FortniteGame.FortPlayerPawnAthena");
	std::cout << "PawnAthena: " << PawnAthena << std::endl;

	//WeaponStatHandle = (uintptr_t)FindObject("GetWeaponStatHandle");
	//std::cout << "WeaponStatHandle: " << WeaponStatHandle << std::endl;
	//WeaponStatHandle = read<uintptr_t>(WeaponStatHandle);

	//gayVehicle = (UObject*)FindObject2("FortPlayerPawn", "GetVehicle");
	//std::cout << "gayVehicle: " << gayVehicle << std::endl;

	auto UWorld = *(uintptr_t*)(UWORLD);
	std::cout << "UWorld: " << UWorld << std::endl;
	auto GameInstance = *(uintptr_t*)(UWorld + 0x1b8);
	std::cout << "GameInstance: " << GameInstance << std::endl;
	auto LocalPlayers = *(uintptr_t*)(GameInstance + 0x38);
	std::cout << "LocalPlayers: " << LocalPlayers << std::endl;
	auto ULocalPlayer = *(uintptr_t*)(LocalPlayers);
	std::cout << "ULocalPlayer: " << ULocalPlayer << std::endl;
	auto PlayerController = *(uintptr_t*)(ULocalPlayer + StaticOffsets::PlayerController);
	std::cout << "PlayerController: " << PlayerController << std::endl;
	auto UViewportClient = *(uintptr_t*)(ULocalPlayer + 0x78);
	std::cout << "UViewportClient: " << UViewportClient << std::endl;
	auto UViewportClientVtable = *(void***)(UViewportClient);


	DWORD PEOutput;
	VirtualProtect(&UViewportClientVtable[0x6D], 8, 0x40, &PEOutput);
	oPostRender = reinterpret_cast<decltype(oPostRender)>(UViewportClientVtable[0x6D]);
	UViewportClientVtable[0x6D] = &hPostRender;
	VirtualProtect(&UViewportClientVtable[0x6D], 8, PEOutput, 0);


	auto GetPlayerViewPoint_VFTable = *(uintptr_t*)(uintptr_t(PlayerController));
	auto GetPlayerViewPoint_addr = *(uintptr_t*)(GetPlayerViewPoint_VFTable + 0x7B0);

	MH_Initialize();
	MH_CreateHook((PVOID)GetPlayerViewPoint_addr, hk_GetPlayerViewPoint, reinterpret_cast<PVOID*>(&o_GetPlayerViewPoint));
	MH_EnableHook((PVOID)GetPlayerViewPoint_addr);

	auto GetViewPoint_VFTable = *(uintptr_t*)(uintptr_t(ULocalPlayer));
	auto GetViewPoint_addr = *(uintptr_t*)(GetViewPoint_VFTable + 0x2B8);

	MH_CreateHook((PVOID)GetViewPoint_addr, hk_GetViewPoint, reinterpret_cast<PVOID*>(&o_GetViewPoint));
	MH_EnableHook((PVOID)GetViewPoint_addr);


	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitMain();
	}
	return TRUE;
}
