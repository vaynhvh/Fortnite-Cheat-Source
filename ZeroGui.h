
static wchar_t* s2wc(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}
#define ZeroGUI_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define ZeroGUI_MAX(A, B)            (((A) >= (B)) ? (A) : (B))


namespace ZeroGUI
{

	namespace Input
	{
		static bool mouseDown[5];
		static bool mouseDownAlready[256];

		static bool keysDown[256];
		static bool keysDownAlready[256];

		static bool IsAnyMouseDown()
		{
			if (mouseDown[0]) return true;
			if (mouseDown[1]) return true;
			if (mouseDown[2]) return true;
			if (mouseDown[3]) return true;
			if (mouseDown[4]) return true;

			return false;
		}

		static bool IsMouseClicked(int button, int element_id, bool repeat)
		{
			if (mouseDown[button])
			{
				if (!mouseDownAlready[element_id])
				{
					mouseDownAlready[element_id] = true;
					return true;
				}
				if (repeat)
					return true;
			}
			else
			{
				mouseDownAlready[element_id] = false;
			}
			return false;
		}
		static bool IsKeyPressed(int key, bool repeat)
		{
			if (keysDown[key])
			{
				if (!keysDownAlready[key])
				{
					keysDownAlready[key] = true;
					return true;
				}
				if (repeat)
					return true;
			}
			else
			{
				keysDownAlready[key] = false;
			}
			return false;
		}

		static void Handle()
		{
			if (GetAsyncKeyState(0x01))
				mouseDown[0] = true;
			else
				mouseDown[0] = false;
		}
	}

	namespace Colors
	{
		//Text
		static FLinearColor Text{ 1.0f, 1.0f, 1.0f, 1.0f };
		static FLinearColor Text_Shadow{ 0.0f, 0.0f, 0.0f, 0.0f };
		static FLinearColor Text_Outline{ 0.0f, 0.0f, 0.0f, 0.30f };

		//Window
		static FLinearColor Window_Background{ 0.009f, 0.009f, 0.009f, 1.0f };
		static FLinearColor Window_Header{ 0.10f, 0.15f, 0.84f, 1.0f };

		//Buttons
		static FLinearColor Button_Idle{ 5 / 255.f, 5 / 255.f, 5 / 255.f, 1.0f };
		static FLinearColor Button_Hovered{ 7 / 255.f, 7 / 255.f, 7 / 255.f, 1.0f };
		static FLinearColor Button_Active{ 8 / 255.f, 8 / 255.f, 8 / 255.f, 1.0f };

		//Combobox
		static FLinearColor Combobox_Idle{ 5 / 255.f, 5 / 255.f, 5 / 255.f, 1.0f };
		static FLinearColor Combobox_Hovered{ 7 / 255.f, 7 / 255.f, 7 / 255.f, 1.0f };
		static FLinearColor Combobox_Elements{ 5 / 255.f, 5 / 255.f, 5 / 255.f, 1.0f };

		//Sliders
		static FLinearColor Slider_Idle{ 0.17f, 0.16f, 0.23f, 1.0f };
		static FLinearColor Slider_Hovered{ 0.17f, 0.16f, 0.23f, 1.0f };
		static FLinearColor Slider_Progress{ 0.22f, 0.30f, 0.72f, 1.0f };
		static FLinearColor Slider_Button{ 0.10f, 0.15f, 0.84f, 1.0f };

		static FLinearColor ColorPicker_Background{ 0.006f, 0.006f, 0.006f, 1.0f };
	}

	namespace PostRenderer
	{
		struct DrawList
		{
			int type = -1; //1 = FilledRect, 2 = TextLeft, 3 = TextCenter, 4 = Draw_Line
			Vector2 pos;
			Vector2 size;
			FLinearColor color;
			char* name;
			bool outline;

			Vector2 from;
			Vector2 to;
			int thickness;
		};
		static DrawList drawlist[128];

		static void drawFilledRect(Vector2 pos, float w, float h, FLinearColor color)
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 1;
					drawlist[i].pos = pos;
					drawlist[i].size = Vector2{ w, h };
					drawlist[i].color = color;
					return;
				}
			}
		}
		static void TextLeft(char* name, Vector2 pos, FLinearColor color, bool outline)
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 2;
					drawlist[i].name = name;
					drawlist[i].pos = pos;
					drawlist[i].outline = outline;
					drawlist[i].color = color;
					return;
				}
			}
		}
		static void TextCenter(char* name, Vector2 pos, FLinearColor color, bool outline)
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 3;
					drawlist[i].name = name;
					drawlist[i].pos = pos;
					drawlist[i].outline = outline;
					drawlist[i].color = color;
					return;
				}
			}
		}
		static void Draw_Line(Vector2 from, Vector2 to, int thickness, FLinearColor color)
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 4;
					drawlist[i].from = from;
					drawlist[i].to = to;
					drawlist[i].thickness = thickness;
					drawlist[i].color = color;
					return;
				}
			}
		}
	}

	UCanvas* CanvasXDD;

	void drawRectA(const Vector2 initial_pos, float w, float h, const FLinearColor color, float thickness = 1.f)
	{
		CanvasXDD->K2_DrawLine(Vector3{ initial_pos.X, initial_pos.Y, 0.f }, Vector3(initial_pos.X + w, initial_pos.Y, 0), color, thickness);
		CanvasXDD->K2_DrawLine(Vector3{ initial_pos.X, initial_pos.Y, 0.f }, Vector3(initial_pos.X, initial_pos.Y + h, 0), color, thickness);


		CanvasXDD->K2_DrawLine(Vector3(initial_pos.X + w, initial_pos.Y, 0.f), Vector3(initial_pos.X + w, initial_pos.Y + h, 0.f), color, thickness);
		CanvasXDD->K2_DrawLine(Vector3(initial_pos.X, initial_pos.Y + h, 0.f), Vector3(initial_pos.X + w, initial_pos.Y + h, 0.f), color, thickness);
	}

	static bool hover_element = false;
	static Vector2 menu_pos = Vector2{ 0, 0 };
	static float offset_x = 0.0f;
	static float offset_y = 0.0f;

	static Vector2 first_element_pos = Vector2{ 0, 0 };

	static Vector2 last_element_pos = Vector2{ 0, 0 };
	static Vector2 last_element_size = Vector2{ 0, 0 };

	static int current_element = -1;
	static Vector2 current_element_pos = Vector2{ 0, 0 };
	static Vector2 current_element_size = Vector2{ 0, 0 };
	static int elements_count = 0;

	static bool sameLine = false;

	static bool pushY = false;
	static float pushYvalue = 0.0f;


	static Vector2 CursorPos()
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		return Vector2{ (float)cursorPos.x, (float)cursorPos.y };
	}
	static bool MouseInZone(Vector2 pos, Vector2 size)
	{
		Vector2 cursor_pos = CursorPos();

		if (cursor_pos.X > pos.X && cursor_pos.Y > pos.Y)
			if (cursor_pos.X < pos.X + size.X && cursor_pos.Y < pos.Y + size.Y)
				return true;

		return false;
	}

	//static void Draw_Cursor(bool toogle)
	//{
	//	if (toogle)
	//	{
	//		Vector2 cursorPos = CursorPos();
	//		CanvasXDD->K2_DrawLine(Vector3{ cursorPos.X, cursorPos.Y, 0}, Vector3{ cursorPos.X + 35, cursorPos.Y + 10,0 }, FLinearColor{ 0.30f, 0.30f, 0.80f, 1.0f }, 1);


	//		int x = 35;
	//		int y = 10;
	//		while (y != 30) //20 steps
	//		{
	//			x -= 1; if (x < 15) x = 15;
	//			y += 1; if (y > 30) y = 30;

	//			FN::CanvasFunctions::K2_DrawLine(Vector2{ cursorPos.X, cursorPos.Y }, Vector2{ cursorPos.X + x, cursorPos.Y + y }, 1, FLinearColor{ 0.30f, 0.30f, 0.80f, 1.0f });
	//		}

	//		FN::CanvasFunctions::K2_DrawLine(Vector2{ cursorPos.X, cursorPos.Y }, Vector2{ cursorPos.X + 15, cursorPos.Y + 30 }, 1, FLinearColor{ 0.30f, 0.30f, 0.80f, 1.0f });
	//		FN::CanvasFunctions::K2_DrawLine(Vector2{ cursorPos.X + 35, cursorPos.Y + 10 }, Vector2{ cursorPos.X + 15, cursorPos.Y + 30 }, 1, FLinearColor{ 0.30f, 0.30f, 0.80f, 1.0f });
	//	}
	//}

	static void SameLine()
	{
		sameLine = true;
	}
	static void PushNextElementY(float y, bool from_last_element = true)
	{
		pushY = true;
		if (from_last_element)
			pushYvalue = last_element_pos.Y + last_element_size.Y + y;
		else
			pushYvalue = menu_pos.Y + y;
	}
	static void NextColumn(float x)
	{
		offset_x = x;
		PushNextElementY(first_element_pos.Y, false);
	}
	static void PushElementX(float value)
	{
		offset_x = value;
	}
	static void PushElementY(float value)
	{
		pushY = true;
		pushYvalue = menu_pos.Y + value - 10;
	}
	static void ClearFirstPos()
	{
		first_element_pos = Vector2{ 0, 0 };
	}

	static void TextLeft(char* name, Vector2 pos, FLinearColor color, bool outline, bool big = false)
	{
		if (big)
			CanvasXDD->K2_DrawText(Vector3(pos.X, pos.Y, 0), FString(s2wc(name)), color, false, false);
		else
			CanvasXDD->K2_DrawText(Vector3(pos.X, pos.Y, 0), FString(s2wc(name)),  color, false, false);
	}
	static void TextCenter(char* name, Vector2 pos, FLinearColor color, bool outline, bool big = false)
	{
		if (big)
			CanvasXDD->K2_DrawText(Vector3(pos.X, pos.Y, 0), FString(s2wc(name)),  color, true, false);
		else
			CanvasXDD->K2_DrawText(Vector3(pos.X, pos.Y, 0), FString(s2wc(name)),  color, true, false);
	}

	static void GetColor(FLinearColor* color, float* r, float* g, float* b, float* a)
	{
		*r = color->R;
		*g = color->G;
		*b = color->B;
		*a = color->A;
	}
	static UINT32 GetColorUINT(int r, int g, int b, int a)
	{
		UINT32 result = (BYTE(a) << 24) + (BYTE(r) << 16) + (BYTE(g) << 8) + BYTE(b);
		return result;
	}

	static void Draw_Line(Vector2 from, Vector2 to, int thickness, FLinearColor color)
	{
		CanvasXDD->K2_DrawLine(Vector3{ from.X, from.Y, 0 }, Vector3{ to.X, to.Y, 0 }, color, thickness);
	}
	static void drawFilledRect(Vector2 initial_pos, float w, float h, FLinearColor color)
	{
		for (float i = 0.0f; i < h; i += 1.0f)
			CanvasXDD->K2_DrawLine(Vector3{ initial_pos.X, initial_pos.Y + i, 0 }, Vector3{ initial_pos.X + w, initial_pos.Y + i, 0 }, color, 1.0f);
	}
	static void DrawFilledCircle(Vector2 pos, float r, FLinearColor color)
	{
		float smooth = 0.07f;

		double PI = 3.14159265359;
		int size = (int)(2.0f * PI / smooth) + 1;

		float angle = 0.0f;
		int i = 0;

		for (; angle < 2 * PI; angle += smooth, i++)
		{
			Draw_Line(Vector2{ pos.X, pos.Y }, Vector2{ pos.X + cosf(angle) * r, pos.Y + sinf(angle) * r }, 1.0f, color);
		}
	}
	static void DrawCircle(Vector2 pos, int radius, int numSides, FLinearColor Color)
	{
		float PI = 3.1415927f;

		float Step = PI * 2.0 / numSides;
		int Count = 0;
		Vector2 V[128];
		for (float a = 0; a < PI * 2.0; a += Step) {
			float X1 = radius * cos(a) + pos.X;
			float Y1 = radius * sin(a) + pos.Y;
			float X2 = radius * cos(a + Step) + pos.X;
			float Y2 = radius * sin(a + Step) + pos.Y;
			V[Count].X = X1;
			V[Count].Y = Y1;
			V[Count + 1].X = X2;
			V[Count + 1].Y = Y2;
			//Draw_Line(Vector2{ pos.X, pos.Y }, Vector2{ X2, Y2 }, 1.0f, Color); // Points from Centre to ends of circle
			Draw_Line(Vector2{ V[Count].X, V[Count].Y }, Vector2{ X2, Y2 }, 1.5f, Color);// Circle Around
		}
	}

	static Vector2 dragPos;
	static bool Window(char* name, Vector2* pos, Vector2 size, bool isOpen, bool WindowInvisible)
	{
		elements_count = 0;

		if (!isOpen)
			return false;

		bool isHovered = MouseInZone(Vector2{ pos->X, pos->Y }, size);

		//Drop last element
		if (current_element != -1 && !GetAsyncKeyState(0x1))
		{
			current_element = -1;
		}

		//Drag
		if (hover_element && GetAsyncKeyState(0x1))
		{

		}
		else if ((isHovered || dragPos.X != 0) && !hover_element)
		{
			if (Input::IsMouseClicked(0, elements_count, true))
			{
				Vector2 cursorPos = CursorPos();

				cursorPos.X -= size.X;
				cursorPos.Y -= size.Y;

				if (dragPos.X == 0)
				{
					dragPos.X = (cursorPos.X - pos->X);
					dragPos.Y = (cursorPos.Y - pos->Y);
				}
				pos->X = cursorPos.X - dragPos.X;
				pos->Y = cursorPos.Y - dragPos.Y;
			}
			else
			{
				dragPos = Vector2{ 0, 0 };
			}
		}
		else
		{
			hover_element = false;
		}


		offset_x = 0.0f; offset_y = 0.0f;
		menu_pos = Vector2{ pos->X, pos->Y };
		first_element_pos = Vector2{ 0, 0 };
		current_element_pos = Vector2{ 0, 0 };
		current_element_size = Vector2{ 0, 0 };

		if (!WindowInvisible) {
			//Bg
			ZeroGUI::drawFilledRect(Vector2{ pos->X, pos->Y }, size.X, size.Y, Colors::Window_Background);
			//drawFilledRect(Vector2{ pos->X, pos->Y }, 122, size.Y, FLinearColor{ 0.006f, 0.006f, 0.006f, 1.0f });//My tabs bg

			//Header
			ZeroGUI::drawFilledRect(Vector2{ pos->X, pos->Y }, size.X, 25.0f, Colors::Window_Header);

			offset_y += 25.0f;

			//Title
			Vector2 titlePos = Vector2{ pos->X + size.X / 2, pos->Y + 25 / 2 };
			TextCenter(name, titlePos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);
		}
		else {
			offset_y += 25.0f;
		}
		return true;
	}

	static void Text(char* text, bool center = false, bool outline = false)
	{
		elements_count++;

		float size = 25;
		Vector2 padding = Vector2{ 10, 10 };
		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}

		if (!sameLine)
			offset_y += size + padding.Y;

		//Text
		Vector2 textPos = Vector2{ pos.X + 5.0f, pos.Y + size / 2 };
		if (center)
			TextCenter(text, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, outline);
		else
			TextLeft(text, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, outline);

		sameLine = false;
		last_element_pos = pos;
		//last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}
	static bool ButtonTab(char* name, Vector2 size, bool active)
	{
		elements_count++;

		Vector2 padding = Vector2{ 5, 10 };
		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2{ pos.X, pos.Y }, size);

		//Bg
		if (active)
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Active);
		}
		else if (isHovered)
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Hovered);
			hover_element = true;
		}
		else
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Idle);
		}

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		//Text
		Vector2 textPos = Vector2{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
		TextCenter(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		if (isHovered && Input::IsMouseClicked(0, elements_count, false))
			return true;

		return false;
	}
	static bool Button(char* name, Vector2 size)
	{
		elements_count++;

		Vector2 padding = Vector2{ 5, 10 };

		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };


		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2{ pos.X, pos.Y }, size);

		//Bg
		if (isHovered)
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Hovered);
			hover_element = true;
		}
		else
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Idle);
		}

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		//Text
		Vector2 textPos = Vector2{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
		//if (!TextOverlapedFromActiveElement(textPos))
		TextCenter(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		if (isHovered && Input::IsMouseClicked(0, elements_count, false))
			return true;

		return false;
	}
	static bool InvisibleButton(char* name, Vector2 size)
	{
		elements_count++;

		Vector2 padding = Vector2{ 5, 10 };
		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2{ pos.X, pos.Y }, size);

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		if (isHovered && Input::IsMouseClicked(0, elements_count, false))
			return true;

		return false;
	}
	static void FakeElement()
	{
		Vector2 size = Vector2(1, 1);
		elements_count++;

		Vector2 padding = Vector2{ 1, 1 };

		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };


		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}



		if (!sameLine)
			offset_y += size.Y + padding.Y;


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		return;
	}

	static void Checkbox(char* name, bool* value)
	{
		elements_count++;

		Vector2 size = Vector2(20, 20);
		Vector2 padding = Vector2{ 10, 10 };
		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2{ pos.X, pos.Y }, Vector2{ size.X, size.Y });

		if (!sameLine)
			offset_y += size.Y + padding.Y;


		const float check_sz = ZeroGUI_MIN(size.X, size.Y);
		const float pad = ZeroGUI_MAX(1.0f, (float)(int)(check_sz / 6.0f));


		Vector2 CheckMarkPos = Vector2(pos.X, pos.Y);


		float sz = size.X - pad * 2.0f;


		float thickness = ZeroGUI_MAX(sz / 5.0f, 1.0f);
		sz -= thickness * 0.5f;
		CheckMarkPos = CheckMarkPos + Vector2(thickness * 0.25f, thickness * 0.25f);

		float third = sz / 3.0f;
		float bx = CheckMarkPos.X + third;
		float by = CheckMarkPos.Y + sz - third * 0.5f;

		Vector2 TopLeft = Vector2(bx + third * 2 - sz, by - third * 2);
		Vector2 TopRight = Vector2(bx + third * 2, by - third * 2);
		Vector2 BottomLeft = Vector2(bx + third * 2 - sz, by - third * 2 + sz);
		Vector2 BottomRight = Vector2(bx + third * 2, by - third * 2 + sz);

		

		drawRectA(Vector2{ pos.X, pos.Y }, 18, 18, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		drawRectA(Vector2{ pos.X + 3, pos.Y + 3 }, 18 - 6, 18 - 6, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		if (*value)
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X + 3, pos.Y + 3 }, 18 - 6, 18 - 6, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		}



		//Text
		Vector2 textPos = Vector2{ pos.X + size.X + 5.0f, pos.Y + size.Y / 2 };
		//if (!TextOverlapedFromActiveElement(textPos))
		TextLeft(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);


		sameLine = false;
		last_element_pos = pos;
		//last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		if (isHovered && Input::IsMouseClicked(0, elements_count, false))
			*value = !*value;
	}
	static void SliderInt(char* name, int* value, int min, int max)
	{
		elements_count++;

		Vector2 size = Vector2{ 240, 50 };
		Vector2 slider_size = Vector2{ 200, 10 };
		Vector2 padding = Vector2{ 10, 15 };
		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size);

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		//Bg
		if (isHovered || current_element == elements_count)
		{
			//Drag
			if (Input::IsMouseClicked(0, elements_count, true))
			{
				current_element = elements_count;

				Vector2 cursorPos = CursorPos();
				*value = ((cursorPos.X - pos.X) * ((max - min) / slider_size.X)) + min;
				if (*value < min) *value = min;
				if (*value > max) *value = max;
			}

			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, Colors::Slider_Hovered);
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y + slider_size.Y + padding.Y + 5.0f }, 5.0f, 5.0f, Colors::Slider_Progress);

			hover_element = true;
		}
		else
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, Colors::Slider_Idle);
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y + slider_size.Y + padding.Y + 5.0f }, 5.0f, 5.0f, Colors::Slider_Progress);
		}


		//Value
		float oneP = slider_size.X / (max - min);
		ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y + slider_size.Y + padding.Y }, oneP * (*value - min), slider_size.Y, Colors::Slider_Progress);
		//drawFilledRect(Vector2{ pos.X + oneP * (*value - min) - 10.0f, pos.Y + slider_size.Y - 5.0f + padding.Y }, 20.0f, 20.0f, Colors::Slider_Button);
		DrawFilledCircle(Vector2{ pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 3.3f + padding.Y }, 10.0f, Colors::Slider_Button);
		DrawFilledCircle(Vector2{ pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 3.3f + padding.Y }, 5.0f, Colors::Slider_Progress);

		char buffer[32];
		sprintf_s(buffer, "%i", *value);
		Vector2 valuePos = Vector2{ pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 25 + padding.Y };
		TextCenter(buffer, valuePos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

		//Text
		Vector2 textPos = Vector2{ pos.X + 5, pos.Y + 10 };
		TextLeft(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}
	static void SliderFloat(char* name, float* value, float min, float max, const char* format = "%.0f")
	{
		elements_count++;

		Vector2 size = Vector2{ 210, 40 };
		Vector2 slider_size = Vector2{ 170, 7 };
		Vector2 adjust_zone = Vector2{ 0, 20 };
		Vector2 padding = Vector2{ 10, 15 };
		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2{ pos.X, pos.Y + slider_size.Y + padding.Y - adjust_zone.Y }, Vector2{ slider_size.X, slider_size.Y + adjust_zone.Y * 1.5f });

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		//Bg
		if (isHovered || current_element == elements_count)
		{
			//Drag
			if (Input::IsMouseClicked(0, elements_count, true))
			{
				current_element = elements_count;

				Vector2 cursorPos = CursorPos();
				*value = ((cursorPos.X - pos.X) * ((max - min) / slider_size.X)) + min;
				if (*value < min) *value = min;
				if (*value > max) *value = max;
			}

			//drawFilledRect(FVector2D{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, Colors::Slider_Hovered);
			//DrawFilledCircle(FVector2D{ pos.X, pos.Y + padding.Y + 9.3f }, 3.1f, Colors::Slider_Progress);
		//	DrawFilledCircle(FVector2D{ pos.X + slider_size.X, pos.Y + padding.Y + 9.3f }, 3.1f, Colors::Slider_Hovered);

			hover_element = true;
		}
		else
		{
			//drawFilledRect(FVector2D{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, Colors::Slider_Idle);
			//DrawFilledCircle(FVector2D{ pos.X, pos.Y + padding.Y + 9.3f }, 3.1f, Colors::Slider_Progress);
			//DrawFilledCircle(FVector2D{ pos.X + slider_size.X, pos.Y + padding.Y + 9.3f }, 3.1f, Colors::Slider_Idle);
		}


		//Text
		Vector2 textPos = Vector2{ pos.X, pos.Y + 5 };
		TextLeft(name, textPos, Colors::Text, false);

		//Empty
		float oneP = slider_size.X / (max - min);
		ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, FLinearColor(0.21f, 0.20f, 0.21f, 1.00f));

		//Value
		ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y + slider_size.Y + padding.Y }, oneP * (*value - min), slider_size.Y, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

		char buffer[32];
		sprintf_s(buffer, format, *value);
		Vector2 valuePos = Vector2{ pos.X + slider_size.X, pos.Y + 5 };
		TextCenter(buffer, valuePos, Colors::Text, false);

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}





	static bool checkbox_enabled[256];
	static void Combobox(char* name, Vector2 size, int* value, const char* arg, ...)
	{
		elements_count++;

		Vector2 padding = Vector2{ 5, 10 };
		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2{ pos.X, pos.Y }, size);

		//Bg
		if (isHovered || checkbox_enabled[elements_count])
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y }, size.X, size.Y, Colors::Combobox_Hovered);

			hover_element = true;
		}
		else
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y }, size.X, size.Y, Colors::Combobox_Idle);
		}

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		//Text
		Vector2 textPos = Vector2{ pos.X + size.X + 5.0f, pos.Y + size.Y / 2 };
		TextLeft(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

		//Elements
		bool isHovered2 = false;
		Vector2 element_pos = pos;
		int num = 0;

		if (checkbox_enabled[elements_count])
		{
			current_element_size.X = element_pos.X - 5.0f;
			current_element_size.Y = element_pos.Y - 5.0f;
		}
		va_list arguments;
		for (va_start(arguments, arg); arg != NULL; arg = va_arg(arguments, const char*))
		{
			//Selected Element
			if (num == *value)
			{
				Vector2 _textPos = Vector2{ pos.X + 5.0f, pos.Y + size.Y / 2 };
				TextLeft((char*)arg, _textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);
			}

			if (checkbox_enabled[elements_count])
			{
				element_pos.Y += 25.0f;

				isHovered2 = MouseInZone(Vector2{ element_pos.X, element_pos.Y }, Vector2{ size.X, 25.0f });
				if (isHovered2)
				{
					hover_element = true;
					PostRenderer::drawFilledRect(Vector2{ element_pos.X, element_pos.Y }, size.X, 25.0f, Colors::Combobox_Hovered);

					//Click
					if (Input::IsMouseClicked(0, elements_count, false))
					{
						*value = num;
						checkbox_enabled[elements_count] = false;
					}
				}
				else
				{
					PostRenderer::drawFilledRect(Vector2{ element_pos.X, element_pos.Y }, size.X, 25.0f, Colors::Combobox_Idle);
				}

				PostRenderer::TextLeft((char*)arg, Vector2{ element_pos.X + 5.0f, element_pos.Y + 13.0f }, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);
			}
			num++;
		}
		va_end(arguments);
		if (checkbox_enabled[elements_count])
		{
			current_element_size.X = element_pos.X + 5.0f;
			current_element_size.Y = element_pos.Y + 5.0f;
		}


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		if (isHovered && Input::IsMouseClicked(0, elements_count, false))
		{
			checkbox_enabled[elements_count] = !checkbox_enabled[elements_count];
		}
		if (!isHovered && !isHovered2 && Input::IsMouseClicked(0, elements_count, false))
		{
			checkbox_enabled[elements_count] = false;
		}
	}

	static int active_hotkey = -1;
	static bool already_pressed = false;
	static std::string VirtualKeyCodeToString(UCHAR virtualKey)
	{
		UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

		if (virtualKey == VK_LBUTTON)
		{
			return xorstr("MOUSE0");
		}
		if (virtualKey == VK_RBUTTON)
		{
			return xorstr("MOUSE1");
		}
		if (virtualKey == VK_MBUTTON)
		{
			return xorstr("MBUTTON");
		}
		if (virtualKey == VK_XBUTTON1)
		{
			return xorstr("XBUTTON1");
		}
		if (virtualKey == VK_XBUTTON2)
		{
			return xorstr("XBUTTON2");
		}

		CHAR szName[128];
		int result = 0;
		switch (virtualKey)
		{
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_RCONTROL: case VK_RMENU:
		case VK_LWIN: case VK_RWIN: case VK_APPS:
		case VK_PRIOR: case VK_NEXT:
		case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
			scanCode |= KF_EXTENDED;
		default:
			result = GetKeyNameTextA(scanCode << 16, szName, 128);
		}

		return szName;
	}
	static void Hotkey(char* name, Vector2 size, int* key)
	{
		elements_count++;

		Vector2 padding = Vector2{ 5, 10 };
		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y + (last_element_size.Y / 2) - size.Y / 2;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2{ pos.X, pos.Y }, size);

		//Bg
		if (isHovered)
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Hovered);
			hover_element = true;
		}
		else
		{
			ZeroGUI::drawFilledRect(Vector2{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Idle);
		}

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		if (active_hotkey == elements_count)
		{
			//Text
			Vector2 textPos = Vector2{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
			TextCenter(xorstr("[Press Key]"), textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

			if (!ZeroGUI::Input::IsAnyMouseDown())
			{
				already_pressed = false;
			}

			if (!already_pressed)
			{
				for (int code = 0; code < 255; code++)
				{
					if (GetAsyncKeyState(code))
					{
						*key = code;
						active_hotkey = -1;
					}
				}
			}
		}
		else
		{
			//Text
			Vector2 textPos = Vector2{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
			TextCenter((char*)VirtualKeyCodeToString(*key).c_str(), textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

			if (isHovered)
			{
				if (Input::IsMouseClicked(0, elements_count, false))
				{
					already_pressed = true;
					active_hotkey = elements_count;

					//Queue Fix
					for (int code = 0; code < 255; code++)
						if (GetAsyncKeyState(code)) {}
				}
			}
			else
			{
				if (Input::IsMouseClicked(0, elements_count, false))
				{
					active_hotkey = -1;
				}
			}
		}


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}

	static int active_picker = -1;
	static FLinearColor saved_color;
	static bool ColorPixel(Vector2 pos, Vector2 size, FLinearColor* original, FLinearColor color)
	{
		PostRenderer::drawFilledRect(Vector2{ pos.X, pos.Y }, size.X, size.Y, color);

		//????????? ????
		if (original->R == color.R && original->G == color.G && original->B == color.B)
		{
			PostRenderer::Draw_Line(Vector2{ pos.X, pos.Y }, Vector2{ pos.X + size.X - 1, pos.Y }, 1.0f, FLinearColor{ 0.0f, 0.0f, 0.0f, 1.0f });
			PostRenderer::Draw_Line(Vector2{ pos.X, pos.Y + size.Y - 1 }, Vector2{ pos.X + size.X - 1, pos.Y + size.Y - 1 }, 1.0f, FLinearColor{ 0.0f, 0.0f, 0.0f, 1.0f });
			PostRenderer::Draw_Line(Vector2{ pos.X, pos.Y }, Vector2{ pos.X, pos.Y + size.Y - 1 }, 1.0f, FLinearColor{ 0.0f, 0.0f, 0.0f, 1.0f });
			PostRenderer::Draw_Line(Vector2{ pos.X + size.X - 1, pos.Y }, Vector2{ pos.X + size.X - 1, pos.Y + size.Y - 1 }, 1.0f, FLinearColor{ 0.0f, 0.0f, 0.0f, 1.0f });
		}

		//????? ?????
		bool isHovered = MouseInZone(Vector2{ pos.X, pos.Y }, size);
		if (isHovered)
		{
			if (Input::IsMouseClicked(0, elements_count, false))
				*original = color;
		}

		return true;
	}
	static void ColorPicker(char* name, FLinearColor* color)
	{
		elements_count++;

		float size = 25;
		Vector2 padding = Vector2{ 10, 10 };
		Vector2 pos = Vector2{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2{ pos.X, pos.Y }, Vector2{ size, size });

		if (!sameLine)
			offset_y += size + padding.Y;

		if (active_picker == elements_count)
		{
			hover_element = true;

			float sizePickerX = 250;
			float sizePickerY = 250;
			bool isHoveredPicker = MouseInZone(Vector2{ pos.X, pos.Y }, Vector2{ sizePickerX, sizePickerY - 60 });

			//Background
			PostRenderer::drawFilledRect(Vector2{ pos.X, pos.Y }, sizePickerX, sizePickerY - 65, Colors::ColorPicker_Background);

			//float pixedSize = sizePickerY / pixels;
			//FLinearColor temp_color{1.0f, 1.0f, 1.0f, 1.0f};
			//float iterator = 0.0f;
			//
			//for (int y = 0; y < pixels; y++)
			//{
			//	for (int x = 0; x < pixels; x++)
			//	{
			//		ColorPixel(Vector2{ pos.X + pixedSize * x, pos.Y + pixedSize * y }, pixedSize, color, temp_color);
			//		temp_color.R -= (1.0f - saved_color.R) / pixels;
			//		temp_color.G -= (1.0f - saved_color.G) / pixels;
			//		temp_color.B -= (1.0f - saved_color.B) / pixels;
			//	}
			//	
			//	iterator += 1.0f / pixels;
			//	temp_color = FLinearColor{ 1.0f - iterator, 1.0f - iterator, 1.0f - iterator, 1.0f };
			//}

			Vector2 pixelSize = Vector2{ sizePickerX / 12, sizePickerY / 12 };

			//0
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 174 / 255.f, 235 / 255.f, 253 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 136 / 255.f, 225 / 255.f, 251 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 108 / 255.f, 213 / 255.f, 250 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 89 / 255.f, 175 / 255.f, 213 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 76 / 255.f, 151 / 255.f, 177 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 60 / 255.f, 118 / 255.f, 140 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 43 / 255.f, 85 / 255.f, 100 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 32 / 255.f, 62 / 255.f, 74 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.0f });
			}
			//1
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 175 / 255.f, 205 / 255.f, 252 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 132 / 255.f, 179 / 255.f, 252 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 90 / 255.f, 152 / 255.f, 250 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 55 / 255.f, 120 / 255.f, 250 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 49 / 255.f, 105 / 255.f, 209 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 38 / 255.f, 83 / 255.f, 165 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 28 / 255.f, 61 / 255.f, 120 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 20 / 255.f, 43 / 255.f, 86 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 247 / 255.f, 247 / 255.f, 247 / 255.f, 1.0f });
			}
			//2
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 153 / 255.f, 139 / 255.f, 250 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 101 / 255.f, 79 / 255.f, 249 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 64 / 255.f, 50 / 255.f, 230 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 54 / 255.f, 38 / 255.f, 175 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 39 / 255.f, 31 / 255.f, 144 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 32 / 255.f, 25 / 255.f, 116 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 21 / 255.f, 18 / 255.f, 82 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 16 / 255.f, 13 / 255.f, 61 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 228 / 255.f, 228 / 255.f, 228 / 255.f, 1.0f });
			}
			//3
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 194 / 255.f, 144 / 255.f, 251 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 165 / 255.f, 87 / 255.f, 249 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 142 / 255.f, 57 / 255.f, 239 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 116 / 255.f, 45 / 255.f, 184 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 92 / 255.f, 37 / 255.f, 154 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 73 / 255.f, 29 / 255.f, 121 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 53 / 255.f, 21 / 255.f, 88 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 37 / 255.f, 15 / 255.f, 63 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 203 / 255.f, 203 / 255.f, 203 / 255.f, 1.0f });
			}
			//4
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 224 / 255.f, 162 / 255.f, 197 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 210 / 255.f, 112 / 255.f, 166 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 199 / 255.f, 62 / 255.f, 135 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 159 / 255.f, 49 / 255.f, 105 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 132 / 255.f, 41 / 255.f, 89 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 104 / 255.f, 32 / 255.f, 71 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 75 / 255.f, 24 / 255.f, 51 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 54 / 255.f, 14 / 255.f, 36 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 175 / 255.f, 175 / 255.f, 175 / 255.f, 1.0f });
			}
			//5
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 235 / 255.f, 175 / 255.f, 176 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 227 / 255.f, 133 / 255.f, 135 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 219 / 255.f, 87 / 255.f, 88 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 215 / 255.f, 50 / 255.f, 36 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 187 / 255.f, 25 / 255.f, 7 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 149 / 255.f, 20 / 255.f, 6 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 107 / 255.f, 14 / 255.f, 4 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 77 / 255.f, 9 / 255.f, 3 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 144 / 255.f, 144 / 255.f, 144 / 255.f, 1.0f });
			}
			//6
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 241 / 255.f, 187 / 255.f, 171 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 234 / 255.f, 151 / 255.f, 126 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 229 / 255.f, 115 / 255.f, 76 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 227 / 255.f, 82 / 255.f, 24 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 190 / 255.f, 61 / 255.f, 15 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 150 / 255.f, 48 / 255.f, 12 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 107 / 255.f, 34 / 255.f, 8 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 79 / 255.f, 25 / 255.f, 6 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 113 / 255.f, 113 / 255.f, 113 / 255.f, 1.0f });
			}
			//7
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 245 / 255.f, 207 / 255.f, 169 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 240 / 255.f, 183 / 255.f, 122 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 236 / 255.f, 159 / 255.f, 74 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 234 / 255.f, 146 / 255.f, 37 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 193 / 255.f, 111 / 255.f, 28 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 152 / 255.f, 89 / 255.f, 22 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 110 / 255.f, 64 / 255.f, 16 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 80 / 255.f, 47 / 255.f, 12 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 82 / 255.f, 82 / 255.f, 82 / 255.f, 1.0f });
			}
			//8
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 247 / 255.f, 218 / 255.f, 170 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 244 / 255.f, 200 / 255.f, 124 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 241 / 255.f, 182 / 255.f, 77 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 239 / 255.f, 174 / 255.f, 44 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 196 / 255.f, 137 / 255.f, 34 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 154 / 255.f, 108 / 255.f, 27 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 111 / 255.f, 77 / 255.f, 19 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 80 / 255.f, 56 / 255.f, 14 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 54 / 255.f, 54 / 255.f, 54 / 255.f, 1.0f });
			}
			//9
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 254 / 255.f, 243 / 255.f, 187 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 253 / 255.f, 237 / 255.f, 153 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 253 / 255.f, 231 / 255.f, 117 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 254 / 255.f, 232 / 255.f, 85 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 242 / 255.f, 212 / 255.f, 53 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 192 / 255.f, 169 / 255.f, 42 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 138 / 255.f, 120 / 255.f, 30 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 101 / 255.f, 87 / 255.f, 22 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 29 / 255.f, 29 / 255.f, 29 / 255.f, 1.0f });
			}
			//10
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 247 / 255.f, 243 / 255.f, 185 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 243 / 255.f, 239 / 255.f, 148 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 239 / 255.f, 232 / 255.f, 111 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 235 / 255.f, 229 / 255.f, 76 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 208 / 255.f, 200 / 255.f, 55 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 164 / 255.f, 157 / 255.f, 43 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 118 / 255.f, 114 / 255.f, 31 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 86 / 255.f, 82 / 255.f, 21 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 9 / 255.f, 9 / 255.f, 9 / 255.f, 1.0f });
			}
			//11
			{
				ColorPixel(Vector2{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 218 / 255.f, 232 / 255.f, 182 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 198 / 255.f, 221 / 255.f, 143 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 181 / 255.f, 210 / 255.f, 103 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 154 / 255.f, 186 / 255.f, 76 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 130 / 255.f, 155 / 255.f, 64 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 102 / 255.f, 121 / 255.f, 50 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 74 / 255.f, 88 / 255.f, 36 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 54 / 255.f, 64 / 255.f, 26 / 255.f, 1.0f });
				ColorPixel(Vector2{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0 / 255.f, 0 / 255.f, 0 / 255.f, 1.0f });
			}



			if (isHoveredPicker)
			{
				if (Input::IsMouseClicked(0, elements_count, false))
				{

				}
			}
			else
			{
				if (Input::IsMouseClicked(0, elements_count, false))
				{
					active_picker = -1;
					//hover_element = false;
				}
			}
		}
		else
		{

			//Color
			ZeroGUI::drawFilledRect(Vector2{ pos.X + 4, pos.Y + 4 }, size - 8, size - 8, *color);

			//Text
			Vector2 textPos = Vector2{ pos.X + size + 5.0f, pos.Y + size / 2 };
			TextLeft(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

			if (isHovered && Input::IsMouseClicked(0, elements_count, false))
			{
				saved_color = *color;
				active_picker = elements_count;
			}
		}


		sameLine = false;
		last_element_pos = pos;
		//last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}


	static void Render()
	{
		for (int i = 0; i < 128; i++)
		{
			if (PostRenderer::drawlist[i].type != -1)
			{
				//Filled Rect
				if (PostRenderer::drawlist[i].type == 1)
				{
					ZeroGUI::drawFilledRect(PostRenderer::drawlist[i].pos, PostRenderer::drawlist[i].size.X, PostRenderer::drawlist[i].size.Y, PostRenderer::drawlist[i].color);
				}
				//TextLeft
				else if (PostRenderer::drawlist[i].type == 2)
				{
					ZeroGUI::TextLeft(PostRenderer::drawlist[i].name, PostRenderer::drawlist[i].pos, PostRenderer::drawlist[i].color, PostRenderer::drawlist[i].outline);
				}
				//TextCenter
				else if (PostRenderer::drawlist[i].type == 3)
				{
					ZeroGUI::TextCenter(PostRenderer::drawlist[i].name, PostRenderer::drawlist[i].pos, PostRenderer::drawlist[i].color, PostRenderer::drawlist[i].outline);
				}
				//Draw_Line
				else if (PostRenderer::drawlist[i].type == 4)
				{
					Draw_Line(PostRenderer::drawlist[i].from, PostRenderer::drawlist[i].to, PostRenderer::drawlist[i].thickness, PostRenderer::drawlist[i].color);
				}

				PostRenderer::drawlist[i].type = -1;
			}
		}
	}
}