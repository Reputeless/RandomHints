//-----------------------------------------------
//
//	RandomHints
//
//	Copyright (c) 2018 Ryo Suzuki
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D.hpp> // OpenSiv3D v0.3.1

class Error
{
private:

	String m_what;

public:

	Error(const String& what)
		: m_what(what) {}

	const String& what() const
	{
		return m_what;
	}
};

// ロゴ画像をプログラムで作成
Image CreateLogo()
{
	Image image(400, 400, ColorF(255, 0));
	Rect(20, 20, 360, 120).overwrite(image, ColorF(0.5, 0.6, 0.7));
	Rect(20, 160, 360, 60).overwrite(image, ColorF(0.5, 0.6, 0.7));
	Rect(20, 240, 360, 60).overwrite(image, ColorF(0.5, 0.6, 0.7));
	Rect(20, 320, 360, 60).overwrite(image, ColorF(0.5, 0.6, 0.7));
	return image;
}

Array<String> ReadItems(const FilePath& path)
{
	TextReader reader(path);

	if (!reader)
	{
		throw Error(U"アイテムリスト `{}` が読み込めませんでした"_fmt(path));
	}

	Array<String> items;

	String line;

	while (reader.readLine(line))
	{
		if (line.trim())
		{
			items << line;
		}
	}

	if (!items)
	{
		throw Error(U"アイテムリスト `{}` の項目がありません"_fmt(path));
	}

	return items;
}

void Main()
{
	// アイコン用の画像を出力
	// CreateLogo().save(U"logo.png");

	// タイトル
	Window::SetTitle(U"RandomHints");

	// 背景色と文字
	Window::Resize(800, 440);
	Graphics::SetBackground(ColorF(0.8, 0.9, 1.0));
	const Font fontApp(40, Typeface::Bold);
	const Font fontItem(28, Typeface::Bold);
	const Font fontPattern(16);
	const Font fontTitle(26);
	const Texture textureLogo(CreateLogo(), TextureDesc::Mipped);

	const Rect applicationRect(60, 40, 680, 80);
	const Rect targetRect(60, 140, 680, 50);
	const Rect objectRect(60, 200, 680, 50);
	const Rect actionRect(60, 260, 680, 50);

	Array<String> applications, targets, objects, actions;

	// ファイルからアイテムを読み込む
	try
	{
		applications = ReadItems(U"applications.txt");
		targets = ReadItems(U"targets.txt");
		objects = ReadItems(U"objects.txt");
		actions = ReadItems(U"actions.txt");
	}
	catch (const Error& error)
	{
		System::ShowMessageBox(error.what(), MessageBoxStyle::Error);
		return;
	}

	String application = applications.choice();
	String target = targets.choice();
	String object = objects.choice();
	String action = actions.choice();
	ColorF color = RandomColorF();

	// 何パターン作れるかを計算 (A の個数 x B の個数 x C の個数 x D の個数)
	size_t patterns = (applications.size() * targets.size() * objects.size() * actions.size());

	while (System::Update())
	{
		// 「生成」ボタンを押したら再生成
		if (SimpleGUI::Button(U"生成", Vec2(60, 330)))
		{
			application = applications.choice();
			target = targets.choice();
			object = objects.choice();
			action = actions.choice();
			color = RandomColorF();
		}

		// 「保存」ボタンを押したらスクリーンショットを保存
		if (SimpleGUI::Button(U"保存", Vec2(160, 330)))
		{
			const FilePath path = U"Save/{}.png"_fmt(DateTime::Now().format(U"yyyy-MM-dd-HHmmss-SS"));
			ScreenCapture::SaveCurrentFrame(path);
		}

		// 「リスト再読み込み」ボタンを押したらアイテムファイルをリロード
		if (SimpleGUI::Button(U"リスト再読み込み", Vec2(260, 330)))
		{
			try
			{
				applications = ReadItems(U"applications.txt");
				targets = ReadItems(U"targets.txt");
				objects = ReadItems(U"objects.txt");
				actions = ReadItems(U"actions.txt");
				patterns = (applications.size() * targets.size() * objects.size() * actions.size());
			}
			catch (const Error& error)
			{
				System::ShowMessageBox(error.what(), MessageBoxStyle::Error);
			}
		}

		// 結果を表示
		applicationRect.rounded(8).draw().drawFrame(4, 0, ColorF(color, 0.5));
		targetRect.draw();
		objectRect.draw();
		actionRect.draw();

		fontApp(application)
			.drawAt(applicationRect.center(), ColorF(0.25));
		fontItem(target).draw(80, 145, ColorF(0.25));
		fontItem(object).draw(80, 205, ColorF(0.25));
		fontItem(action).draw(80, 265, ColorF(0.25));
		
		fontPattern(U"Applications:{}\nTargets:{}\nObjects:{}\nActions:{}"_fmt(
			applications.size(), targets.size(), objects.size(), actions.size()))
			.draw(480, 330, ColorF(0.25));

		fontPattern(U"\n\n生成可能:\n{} パターン"_fmt(patterns)).draw(600, 330, ColorF(0.25));

		Rect(60, 390, 30)(textureLogo).draw();
		fontTitle(U"RandomHints").draw(96, 388, ColorF(0.4, 0.5, 0.6));
	}
}