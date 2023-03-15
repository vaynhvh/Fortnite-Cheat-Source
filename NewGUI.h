#pragma once
#include <iomanip>

namespace menu_framework {
	void group_box(double x, double y, double w, double h, unsigned long font, const std::wstring wstring, bool show_label);
	void tab(double x, double y, double w, double h, unsigned long font, const std::wstring, double& tab, double count, bool show_outline);
	void check_box(double x, double y, double position, unsigned long font, const std::wstring wstring, bool& value);
	void slider(double x, double y, double position, unsigned long font, const std::wstring wstring, float& value, float min_value, float max_value);
	void menu_movement(double& x, double& y, double w, double h);

	inline bool should_drag = false;
	inline bool should_move = false;
};

namespace menu {
	inline double current_tab{ 0 };

	void render();
	void toggle();
};

namespace variables {
	inline bool test_bool = false;
	inline float test_float = 0.f;

	namespace menu {
		inline bool opened = false;
		inline double x = 140, y = 140;
		inline double w = 400, h = 300;
	}
}

POINT cursor;
POINT cursor_corrected;
UCanvas* CanvasA;

void drawFilledRect(Vector2 initial_pos, float w, float h, FLinearColor color)
{
	for (float i = 0.0f; i < h; i += 1.0f)
		CanvasA->K2_DrawLine(Vector3{ initial_pos.X, initial_pos.Y + i, 0.f }, Vector3{ initial_pos.X + w, initial_pos.Y + i, 0.f }, color, 1.0f);
}

void drawRect(const Vector2 initial_pos, float w, float h, const FLinearColor color, float thickness = 1.f)
{
	CanvasA->K2_DrawLine(Vector3{ initial_pos.X, initial_pos.Y, 0.f }, Vector3(initial_pos.X + w, initial_pos.Y, 0), color, thickness);
	CanvasA->K2_DrawLine(Vector3{ initial_pos.X, initial_pos.Y, 0.f }, Vector3(initial_pos.X, initial_pos.Y + h, 0), color, thickness);


	CanvasA->K2_DrawLine(Vector3(initial_pos.X + w, initial_pos.Y, 0.f), Vector3(initial_pos.X + w, initial_pos.Y + h, 0.f), color, thickness);
	CanvasA->K2_DrawLine(Vector3(initial_pos.X, initial_pos.Y + h, 0.f), Vector3(initial_pos.X + w, initial_pos.Y + h, 0.f), color, thickness);
}


void menu_framework::group_box(double x, double y, double w, double h, unsigned long font, const std::wstring wstring, bool show_label) {
	//groupbox background
	drawFilledRect(Vector2(x, y), w, h, Colors::DarkGray);

	//groupbox outline
	drawRect(Vector2(x, y), w, h, Colors::Black);

	//groupbox label
	if (show_label)
		CanvasA->K2_DrawText(Vector3(x - 1, y - 12, 0), wstring.c_str(), Colors::Black, 0, 0);
}

void menu_framework::tab(double x, double y, double w, double h, unsigned long font, const std::wstring wstring, double& tab, double count, bool show_outline) {
	GetCursorPos(&cursor);

	if ((cursor.x > x) && (cursor.x < x + w) && (cursor.y > y) && (cursor.y < y + h) && (GetAsyncKeyState(VK_LBUTTON) & 1))
		tab = count;

	//tab background
	drawRect(Vector2(x, y), w, h, tab == count ? Colors::Cyan : Colors::Black);

	//tab label
	CanvasA->K2_DrawText(Vector3(x - CanvasA->K2_TextSize(wstring.c_str()).X / 2 + 32, y + h / 2 - 8, 0), wstring.c_str(), show_outline ? Colors::Black : tab == count ? Colors::Cyan : Colors::Black, 0, 0);
}

void menu_framework::check_box(double x, double y, double position, unsigned long font, const std::wstring wstring, bool& value) {
	GetCursorPos(&cursor);

	int w = 10, h = 10;

	if ((cursor.x > position) && (cursor.x < position + w) && (cursor.y > y) && (cursor.y < y + h) && GetAsyncKeyState(VK_LBUTTON) & 1)
		value = !value;

	//checkbox background
	drawFilledRect(Vector2(position, y), w, h, Colors::Gray);
	if (value)
	{
		drawFilledRect(Vector2(position + 2, y + 2), 6, 6, Colors::Cyan);
	}
	//checkbox label
	CanvasA->K2_DrawText(Vector3(x + 2, y - 1, 0), wstring.c_str(), Colors::Black, 0, 0);
}

void menu_framework::slider(double x, double y, double position, unsigned long font, const std::wstring wstring, float& value, float min_value, float max_value) {
	GetCursorPos(&cursor);

	int ix = x + 140;
	int yi = y + 4;

	if ((cursor.x > ix) && (cursor.x < ix + position) && (cursor.y > yi) && (cursor.y < yi + 6) && (GetAsyncKeyState(VK_LBUTTON)))
		value = (cursor.x - ix) / (float(position) / float(max_value));

	//slider background
	drawFilledRect(Vector2(ix, yi), position, 6, Colors::Gray);
	drawFilledRect(Vector2(ix, yi), value * (float(position) / float(max_value)), 6, Colors::Cyan);

	//slider label
	std::wstring aua = (std::wstringstream{ } << wstring << ": " << std::setprecision(3) << value).str();

	CanvasA->K2_DrawText(Vector3(x + 2, y - 1, 0), aua.c_str(), Colors::Black, 0, 0);
}

void menu_framework::menu_movement(double& x, double& y, double w, double h) {
	GetCursorPos(&cursor);

	if (GetAsyncKeyState(VK_LBUTTON) < 0 && (cursor.x > x && cursor.x < x + w && cursor.y > y && cursor.y < y + h)) {
		should_drag = true;

		if (!should_move) {
			cursor_corrected.x = cursor.x - x;
			cursor_corrected.y = cursor.y - y;
			should_move = true;
		}
	}

	if (should_drag) {
		x = cursor.x - cursor_corrected.x;
		y = cursor.y - cursor_corrected.y;
	}

	if (GetAsyncKeyState(VK_LBUTTON) == 0) {
		should_drag = false;
		should_move = false;
	}
}

Vector2 CursorPos()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	return Vector2{ (float)cursorPos.x, (float)cursorPos.y };
}

static void Draw_Cursor()
{
	Vector2 cursorPos = CursorPos();
	drawFilledRect(Vector2(cursorPos.X, cursorPos.Y), 6.f, 6.f, Colors::Black);
}

float Width, Height = 0;

bool MemoryAimbot = true;
bool StickySilent = false;
bool NormalSilent = false;
bool FovCircle = true;
float ThrreFOV = 200;
float AimSmoothness = 0;

bool Box = true;
bool Box2D = false;
int Boxes = false;
bool Skeleton = true;
bool Line = false;
bool AimbotLine = true;
bool Distance = true;
bool PlayerName = false;
bool PlayerWeapon = false;
bool PlayerAmmo = false;
bool PlayerPlattform = false;
bool LootESP = false;
float LootRenderDist = 100;

bool FovChanger = false;
bool Spinbot = false;
bool InfiniteAmmo = false;
bool DMREXPLOIT = false;
bool ProjectileA = false;
bool NoSpread = false;
bool NoReload = false;
bool VehicleFly = false;
bool ShootTroughWalls = false;
bool TPUWU = false;
bool CarTpToPing = false;
bool CarSpeed = false;
bool CarFly = false;
bool Invisible = false;
float FovChangerA = 0;

int AimBone_Selector = 0;

#define M_PI 3.14159265358979323846264338327950288419716939937510582f
#define D2R(d) (d / 180.f) * M_PI
#define MAX_SEGMENTS 180

void Render_Circle(Vector2 pos, int r, FLinearColor color)
{
	float Step = M_PI * 2.0 / 60;
	int Count = 0;
	Vector2 V[128];
	for (float a = 0; a < M_PI * 2.0; a += Step)
	{
		float X1 = ThrreFOV * cosf(a) + pos.X;
		float Y1 = ThrreFOV * sinf(a) + pos.Y;
		float X2 = ThrreFOV * cosf(a + Step) + pos.X;
		float Y2 = ThrreFOV * sinf(a + Step) + pos.Y;
		V[Count].X = X1;
		V[Count].Y = Y1;
		V[Count + 1].X = X2;
		V[Count + 1].Y = Y2;
		CanvasA->K2_DrawLine(Vector3({ V[Count].X, V[Count].Y,0 }), Vector3({ X2, Y2 , 0}), color, 1.0f);
	}
}


void menu::render() {
	if (!variables::menu::opened)
		return;

	drawFilledRect(Vector2(variables::menu::x, variables::menu::y), variables::menu::w, variables::menu::h, Colors::DarkGray);
	drawFilledRect(Vector2(variables::menu::x, variables::menu::y), variables::menu::w, 30, Colors::DarkGray);
	drawFilledRect(Vector2(variables::menu::x, variables::menu::y + 30), variables::menu::w, 2, Colors::DarkGray);
	CanvasA->K2_DrawText(Vector3(variables::menu::x + 10, variables::menu::y + 8, 0), L"ios ist best oida", Colors::Black, 0, 0);

	menu_framework::group_box(variables::menu::x + 5, variables::menu::y + 35, 100, 260, 0, L"tabs", false); {
		menu_framework::tab(variables::menu::x + 5, variables::menu::y + (260 / 2) - 35, 100, 30, 0, L"aimbot", menu::current_tab, 0, false);
		menu_framework::tab(variables::menu::x + 5, variables::menu::y + (260 / 2), 100, 30, 0, L"visuals", menu::current_tab, 1, false);
		menu_framework::tab(variables::menu::x + 5, variables::menu::y + (260 / 2) + 35, 100, 30, 0, L"misc", menu::current_tab, 2, false);
	}

	switch ((int)menu::current_tab) {
	case 0:
		menu_framework::group_box(variables::menu::x + 110, variables::menu::y + 35, 285, 260, 0, L"aimbot", false); {
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 45, variables::menu::x + 375, 0, L"Memory Aimbot", MemoryAimbot); 
			if (MemoryAimbot) 
			{ 
				StickySilent = false;
				NormalSilent = false;
			}
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 60, variables::menu::x + 375, 0, L"Sticky Silent", StickySilent);
			if (StickySilent)
			{
				MemoryAimbot = false;
				NormalSilent = false;
			}
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 75, variables::menu::x + 375, 0, L"Silent Aimbot", NormalSilent);
			if (NormalSilent)
			{
				MemoryAimbot = false;
				StickySilent = false;
			}
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 90, variables::menu::x + 375, 0, L"Fov Circle", FovCircle);
			menu_framework::slider(variables::menu::x + 120, variables::menu::y + 105, 125, 0, L"FOV", ThrreFOV, 0, 360);
			menu_framework::slider(variables::menu::x + 120, variables::menu::y + 120, 125, 0, L"Aimbot Smoothness", AimSmoothness, 0, 100);
		}
		break;
	case 1:
		menu_framework::group_box(variables::menu::x + 110, variables::menu::y + 35, 285, 260, 0, L"visuals", false); {
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 45, variables::menu::x + 375, 0, L"Box", Box);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 60, variables::menu::x + 375, 0, L"Box 2D", Box2D);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 75, variables::menu::x + 375, 0, L"Name", PlayerName);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 90, variables::menu::x + 375, 0, L"Skeleton", Skeleton);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 105, variables::menu::x + 375, 0, L"Weapon/Active item", PlayerWeapon);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 120, variables::menu::x + 375, 0, L"Snapline", Line);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 135, variables::menu::x + 375, 0, L"Aimbot Line", AimbotLine);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 165, variables::menu::x + 375, 0, L"Loot (Map)", LootESP);
			menu_framework::slider(variables::menu::x + 120, variables::menu::y + 180, 125, 0, L"Loot distance", LootRenderDist, 0, 500);
		}
		break;
	case 2:
		menu_framework::group_box(variables::menu::x + 110, variables::menu::y + 35, 285, 260, 0, L"misc", false); {
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 45, variables::menu::x + 375, 0, L"Spinbot", Spinbot);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 60, variables::menu::x + 375, 0, L"Projectile Teleport", ProjectileA);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 75, variables::menu::x + 375, 0, L"Rapid Fire (BRR!)", NoSpread);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 90, variables::menu::x + 375, 0, L"No Weapon Reload", NoReload);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 105, variables::menu::x + 375, 0, L"Magic Bullet (In Vehicle!)", ShootTroughWalls);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 120, variables::menu::x + 375, 0, L"Teleport To Waypoint", CarTpToPing);
			menu_framework::check_box(variables::menu::x + 120, variables::menu::y + 135, variables::menu::x + 375, 0, L"Field Of View Changer", FovChanger);
			menu_framework::slider(variables::menu::x + 120, variables::menu::y + 150, 125, 0, L"FOV slider", FovChangerA, 0, 160);
		}
		break;
	}

	menu_framework::menu_movement(variables::menu::x, variables::menu::y, variables::menu::w, 30);
	Draw_Cursor();
}

void menu::toggle() {
	if (GetAsyncKeyState(VK_INSERT) & 1)
		variables::menu::opened = !variables::menu::opened;
}