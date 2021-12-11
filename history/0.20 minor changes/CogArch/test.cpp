//#define Test
#ifdef Test

// CGAFile test.

#include "file/cga_file.h"
#include "block/block_manager.h"

#include "../WndDesignHelper/WndDesignHelper.h"
using namespace WndDesignHelper;

#ifdef _DEBUG
#pragma comment(lib, "../build/x64/Debug/WndDesignHelper.lib")
#else
#pragma comment(lib, "../build/x64/Release/WndDesignHelper.lib")
#endif


void Init(uint argc, const wchar_t* argv[]) {
	//wstring file = L"1.cga";
	//if (argc == 2) { file = argv[1]; }
	//
	//CGAFormat::CGAFile _file;
	//BlockManager _block_manager;

	//_file.Open(file, File::CreateMode::OpenAlways, File::AccessMode::ReadWrite, File::ShareMode::ReadOnly);
	//_file.LoadBlockManager(_block_manager);
	//auto& b1 = _block_manager.CreateBlock();
	//auto& b2 = _block_manager.CreateBlock();
	//auto& b3 = _block_manager.CreateBlock();
	//auto& b4 = _block_manager.CreateBlock();
	//auto& b5 = _block_manager.CreateBlock();

	//b1.Content() = L"Root";
	//b2.Content() = L"Who am I?";
	//b3.Content() = L"Hello World!";
	//b4.Content() = L"What's your problem?";
	//b5.Content() = L"Ð¦ËÀÎÒÁË£¡";

	//_block_manager.CreateInheritanceRelation(b1, b2, Point(120, 240), child_width_auto);
	//_block_manager.CreateInheritanceRelation(b1, b3, Point(201, 75), child_width_auto);
	//_block_manager.CreateInheritanceRelation(b1, b4, Point(-74, 10), child_width_auto);
	//_block_manager.CreateInheritanceRelation(b3, b5, child_position_auto, child_width_auto);

	//_file.SaveBlockManager(_block_manager);

	WndStyle style;
	style.size.setFixed(800px, 480px);
	style.position.setHorizontalCenter().setVerticalCenter();
	style.background.setColor(ColorSet::White);
	Wnd* wnd = new Wnd(nullwnd, style);

	style.size.setFixed(400px, 240px);
	style.background.setColor(ColorSet::AliceBlue);
	style.render.z_index(10);
	WndBase* inner = new WndBase(*wnd, style);

	desktop->AddChild(wnd->GetWnd());
}

void Final() {

}

#endif