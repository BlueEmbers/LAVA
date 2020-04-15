#pragma once
#ifndef FE_H
#define FE_H
struct nk_command_buffer* canvas;
struct nk_rect total_space;
std::set<std::string> advancedScanPaths;
#define DEFAULT_FONT_SIZE 28
//#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_STANDARD_IO
//#define NK_INCLUDE_STANDARD_VARARGS
//#define NK_INCLUDE_DEFAULT_ALLOCATOR
//#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
//#define NK_INCLUDE_FONT_BAKING
//#define NK_INCLUDE_DEFAULT_FONT
//#define NK_IMPLEMENTATION
//#define NK_SFML_GL2_IMPLEMENTATION
//
//#include "nuklear.h"
//#include "nuklear_sfml_gl2.h"
//
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//
//#define WINDOW_WIDTH 1200
//#define WINDOW_HEIGHT 800
//#define IMG_COLOR nk_rgba(255, 255, 255, 255)

static const char* Days[] = { "Sunday","Monday","Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

/* Use this function to allocate space for text below an image. see how the back arrow is displayed in the DisplayScansPage function is used
*/
struct nk_rect SubRectTextBelow(struct nk_rect *big, struct nk_rect *sub ) {
	return nk_rect(sub->x, sub->h, sub->w, big->h-sub->h+sub->x);
}

/* opening file dialog stuff */
std::string ToNarrow(const wchar_t* s, char dfault = '?',
	const std::locale& loc = std::locale())
{
	std::ostringstream stm;

	while (*s != L'\0') {
		stm << std::use_facet< std::ctype<wchar_t> >(loc).narrow(*s++, dfault);
	}
	return stm.str();
}

// global struct. prob shouldnt do this but who will see this, lets be honest
struct
{
	WNDPROC oldWndProc;
	TCHAR   szLastSelection[MAX_PATH];
	UINT    cFolders;
} g_Multi;


/*
 * Subclassed window procedure for dialog created by SHBrowseForFolder.
 */
static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED)
	{
		TCHAR szText[50];

		if (GetWindowText((HWND)lParam, szText, 50) &&
			0 == lstrcmp(szText, TEXT("Add File/Dir")))
		{
			if (0 != lstrcmp(g_Multi.szLastSelection, TEXT("")))
			{
				g_Multi.cFolders++;

				/* User has clicked the "Add Folder" button.
				 * Add the contents of szLastSelection to your list. */
				//printf("%s\n", g_Multi.szLastSelection);
				advancedScanPaths.insert(ToNarrow(g_Multi.szLastSelection));
			}
			else
			{
				/* A non-file system folder is selected. eg. Network Neighbourhood. */
				printf("Please select a valid file folder and try again.\n");
			}

			/* Eat the BN_CLICKED message so the dialog is not closed. */
			return 0;
		}
	}

	return CallWindowProc(g_Multi.oldWndProc, hwnd, uMsg, wParam, lParam);
}


/*
 * Callback function for SHBrowseForFolder.
 */
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
	{
		/* Change the text for the OK button and subclass the dialog. */
		LPCTSTR startFolder = reinterpret_cast<LPCTSTR>(lpData);
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, reinterpret_cast<LPARAM>(startFolder));
		SendMessage(hwnd, BFFM_SETOKTEXT, 0, (LPARAM)L"Add File/Dir");
		g_Multi.oldWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)SubclassProc);
		break;
	}

	case BFFM_SELCHANGED:
	{
		/* Record the selection, so we can record it if the user clicks "Add Folder". */
		if (!SHGetPathFromIDList((LPCITEMIDLIST)lParam, g_Multi.szLastSelection))
			g_Multi.szLastSelection[0] = TEXT('\0');
		break;
	}
	}

	return 0;
}


/*
 * Displays a directory selection dialog that allows the user to select
 * multiple folders. CoInitialize must be called before calling this function.
 * hWnd may be NULL. Returns the number of folders that were selected.
 * This function is not thread safe. It must not be called from more than
 * one thread at a time.
 */
UINT MultiFolderSelect(HWND hWnd, LPCTSTR szTitle, CString folder=L"C:\\")
{
	LPITEMIDLIST pidl = NULL;
	BROWSEINFO   bi = { 0 };
	TCHAR        buf[MAX_PATH];

	bi.hwndOwner = hWnd;
	bi.pszDisplayName = buf;
	bi.pidlRoot = 0;
	bi.lpszTitle = szTitle;
	bi.ulFlags = BIF_BROWSEFORPRINTER | BIF_BROWSEINCLUDEFILES | BIF_BROWSEINCLUDEURLS | 
		BIF_NEWDIALOGSTYLE | BIF_SHAREABLE | 
		BIF_NONEWFOLDERBUTTON | BIF_USENEWUI;
	bi.lParam = reinterpret_cast<LPARAM>(static_cast<LPCTSTR>(folder));
	bi.lpfn = BrowseCallbackProc;
	//bi.lParam = (LPARAM)L"C:\\";

	g_Multi.cFolders = 0;
	g_Multi.szLastSelection[0] = TEXT('\0');

	if ((pidl = SHBrowseForFolder(&bi)) != NULL)
		CoTaskMemFree(pidl);

	return g_Multi.cFolders;
}



struct pics {
	const char* squareLogo;
	const char* rectLogo;
	const char* trapLogo;
	const char* scan;
	const char* history;
	const char* backArrow;
	const char* chooseScan;
	const char* triangleButton;
	const char* addButton;
	const char* calendar;
	const char* trash;
	const char* purpleFwd;
	const char* purpleBack;
	const char* done;
};

/* FE CLASS */
class FE : public LavaScan
{
private:
public:
	/* CONSTRUCTORS */
	bool init(sf::Window *win);
	FE();

	/* MEMBER VARS */
	struct nk_context* ctx;
	struct nk_command_buffer* canvas;
	// font stuff
	struct nk_font_atlas* atlas; //reg
	struct nk_font_atlas* atlas2; //large
	struct nk_font_atlas* atlas3; //small
	struct nk_font_atlas* atlas4; //medium large, size 84
	struct nk_font_atlas* atlas5; // small medium...less than reg..22
	struct nk_font* font;
	struct nk_font* font2;
	struct nk_font* font3;
	struct nk_font* font4;
	struct nk_font* font5;
	const char* font_path = "../Assets/font.ttf";
	//struct nk_font_config* fontCFG;// can be null so commenting out for mem
	struct nk_colorf bg;
	struct nk_colorf whiteFont;
	struct nk_rect total_space;
	// pics to be loaded (pngs)
	pics pp;
	struct nk_image scanImage;
	struct nk_image rectImage; 
	struct nk_image trapImage; 
	struct nk_image squareImage; 
	struct nk_image historyImage;
	struct nk_image backArrow;
	struct nk_image chooseScan;
	struct nk_image triangleLogo;
	struct nk_image addLogo;
	struct nk_image calendarLogo;
	struct nk_image trashIcon;
	struct nk_image purpleFwd;
	struct nk_image purpleBack;
	struct nk_image done;
	//std::string currentScanGoing;
	std::queue<int> scanTasks;
	std::vector<std::vector<std::string>> scanHistorySet;
	int maxfiles=0;
	// view screen switch. view ScanViews scanview member for furthger info
	unsigned int view;
	/*
		0 : logo screen
		1 : scans screen
		2 : history screen
		3 : Scan in Pro gress!
		4 : schedule advance scan
		5 : quarentine // how tf can i spell this holy fucking shit
	*/
	unsigned int m_scanViews;
	/*
		0 : plz maam pls choose a scan
		1 : coimplete scan
		2 : quick scan
		3 : advanced scan
	*/
	unsigned int m_schedulerViews;
	/*
		0 : choose one time, monthly, weekly, or daily
		1 : daily
		2 : weekly
		3 : monthly
	*/

	// if type=daily or once user startDate startTime and reccuring(0 if one time)
	// if type=weekly use startDate startTime recurring and daysOfWeek
	// if type=montly use startdate starttime months then either days OR On
	struct schedulerInfo {
		unsigned int type; //0=once, 1=daily, 2=weekly, 3=monthly
		std::string startDate;
		std::string startTime;
		int reccuring; // happen every x days or x weeks or x months
		std::set<int> daysOfWeek; //what days to do weekly scan. ex: every two weeks occur on mon and fri at 6pm starting 4/20/20 {0->6}
		std::set<int> months; // if months size =0 include all, else do only whats in the set
		bool days_on_switch; // false days true on
		std::set<int> days; //days is 1-31
		std::set < std::pair<std::string, std::set<std::string>>> on; //on first, second, last friday and monday. set of pairs(first/second/.. , <days to happen>)
	} _schedulerInfo;
	//ProgressMonitor pm;
	
	/* MEMBER FUNCTIONS */
	static struct nk_image icon_load(const char* filename, bool flip = false);
	bool dateInput();
	bool drawImage(struct nk_image *img);
	bool drawImageSubRect(struct nk_image* img, struct nk_rect* r);
	bool DrawMainPage();
	bool DrawScansPage();
	bool Display();
	bool NoScanView();
	bool CompleteScanView();
	bool viewSwap();
	bool QuickScansView();
	bool AdvancedScanView();
	bool DrawHistoryPage();
	bool DrawInProgressScan();
	bool printset(std::set<std::string> s);
	/*static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	UINT MultiFolderSelect(HWND hWnd, LPCTSTR szTitle);*/
	void UIPrintSet(std::set<std::string> s);
	bool ChangeFontSize(float s); //s is size, default is 28.
	bool QuarantineView();
	bool ScheduleAdvScanView();
	bool displayScheduleArrows();
	bool displayScheduleType();
};
inline struct nk_image FE::icon_load(const char* filename, bool flip)
{
	int x, y, n;
	GLuint tex;
	if (flip) {
		stbi_set_flip_vertically_on_load(true);
	}
	unsigned char* data = stbi_load(filename, &x, &y, &n, STBI_rgb_alpha);
	if (!data) std::cerr << "[SDL]: failed to load image\n";
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return nk_image_id((int)tex);
}

inline bool FE::drawImage(struct nk_image *img)
{
	canvas = nk_window_get_canvas(this->ctx);
	total_space = nk_window_get_content_region(this->ctx);
	nk_draw_image(canvas, total_space, img, IMG_COLOR);
	return true;
}

inline bool FE::dateInput() {
	/*
	char buf[256] = {0};
	// in window
	nk_edit_string_zero_terminated (ctx, NK_EDIT_FIELD, buf, sizeof(buf) - 1, nk_filter_default);
	if (nk_button_label (ctx, "Done"))
		printf ("%s\n", buf);
	*/
	int d;
	int m;
	int y;
	std::cin >> d; // read the day
	if (std::cin.get() != '/') // make sure there is a slash between DD and MM
	{
		std::cout << "expected /\n";
		return 1;
	}
	std::cin >> m; // read the month
	if (std::cin.get() != '/') // make sure there is a slash between MM and YYYY
	{
		std::cout << "expected /\n";
		return 1;
	}
	std::cin >> y; // read the year
	std::cout << "input date: " << d << "/" << m << "/" << y << "\n";

	return true;
}

inline bool FE::drawImageSubRect(struct nk_image* img, struct nk_rect *r)
{
	canvas = nk_window_get_canvas(this->ctx);
	nk_draw_image(canvas, *r, img, IMG_COLOR);
	return true;
}

inline bool FE::Display() {
	if (this->view == 0) { // logo page
		this->DrawMainPage();
		advancedScanPaths.clear();
	}
	else if (this->view == 1) // scan page
		this->DrawScansPage();
	else if (this->view == 2) // history page
		this->DrawHistoryPage();
	else if (this->view == 3) // in-prog scan page
		this->DrawInProgressScan();
	else if (this->view == 4) {
		//adv scan sched pg
		this->ScheduleAdvScanView();
	}
	else if (this->view == 5) //quarentine view
		this->QuarantineView();
	else
		std::cout << "ERRRRRRORRRRR\n";
	return true;
}

inline bool FE::DrawMainPage()
{
	/* LOGO */
	struct nk_rect r_logo = nk_rect(25, 25, WINDOW_HEIGHT * .33, WINDOW_HEIGHT * .33);
	if (nk_begin(this->ctx, "lavalogo",r_logo,
		NK_WINDOW_NO_SCROLLBAR)) {
		this->drawImage(&this->squareImage);
	}
	nk_end(this->ctx);
	/* LAVA TEXT TEXT */
	nk_style_set_font(this->ctx, &this->font4->handle);
	struct nk_rect LAVATEXT = nk_rect(r_logo.x+ r_logo.w+10, r_logo.y, 600, 84);
	struct nk_rect LAVATEXT2 = nk_rect(r_logo.x + r_logo.w + 10, r_logo.y+90, 600, 84);
	struct nk_rect LAVATEXT3 = nk_rect(r_logo.x + r_logo.w + 10, r_logo.y+180, 600, 84);
	if (nk_begin(this->ctx, "lavatext", LAVATEXT, NK_WINDOW_NO_SCROLLBAR)) {
		nk_draw_text(nk_window_get_canvas(this->ctx), LAVATEXT, " Lightweight", 12, &this->font4->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);
	if (nk_begin(this->ctx, "lavatext2", LAVATEXT2, NK_WINDOW_NO_SCROLLBAR)) {
		nk_draw_text(nk_window_get_canvas(this->ctx), LAVATEXT2, " Anti-Virus", 11, &this->font4->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);
	if (nk_begin(this->ctx, "lavatext3", LAVATEXT3, NK_WINDOW_NO_SCROLLBAR)) {
		nk_draw_text(nk_window_get_canvas(this->ctx), LAVATEXT3, " Application", 12, &this->font4->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);
	nk_style_set_font(this->ctx, &this->font->handle);


	///* LAVA TEXT ********************************************************/
	//if (nk_begin(this->ctx, "Lightweight Anti-Virus Application", nk_rect(400, WINDOW_HEIGHT * .04, 470, 300),
	//	NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
	//{
	//}
	//nk_end(this->ctx);

	/* SCAN ICON */
	if (nk_begin(this->ctx, "scan", nk_rect(WINDOW_WIDTH * .2, WINDOW_HEIGHT * .49, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3),
		NK_WINDOW_NO_SCROLLBAR)) {
		// hidden button to press behind icon
		nk_layout_row_static(ctx, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3, 1);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "scans pressed\n");
			this->view = 1; // scans page
			this->m_scanViews = 0; // choose a scan sub view
			nk_clear(this->ctx);
		}
		this->drawImage(&this->scanImage);
	}
	nk_end(this->ctx);
	/* SCAN TEXT */
	struct nk_rect SCANTEXT = nk_rect(WINDOW_WIDTH * .2-20, WINDOW_HEIGHT * .49+ WINDOW_HEIGHT * .3+15, 400, 84);
	if (nk_begin(this->ctx, "SCANTEXT", SCANTEXT, NK_WINDOW_NO_SCROLLBAR)) {
		nk_draw_text(nk_window_get_canvas(this->ctx), SCANTEXT, " Scans", 6, &this->font4->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	/* history ICON */
	if (nk_begin(this->ctx, "history", nk_rect(WINDOW_WIDTH * .6, WINDOW_HEIGHT * .49, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3),
		NK_WINDOW_NO_SCROLLBAR)) {
		// hidden button behind icon to press
		nk_layout_row_static(ctx, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3, 1);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "history pressed\n");
			std::thread t1 = std::thread([this] {this->UpdatePreviousScans(); });
			t1.detach();
			this->view = 2;
			nk_clear(this->ctx);
		}
		this->drawImage(&this->historyImage);
	}
	nk_end(this->ctx);
	/* HISTORY TEXT */
	struct nk_rect HISTTEXT = nk_rect(WINDOW_WIDTH * .6-50, WINDOW_HEIGHT * .49+ WINDOW_HEIGHT * .3+15, 400, 84);
	if (nk_begin(this->ctx, "HISTTEXT", HISTTEXT, NK_WINDOW_NO_SCROLLBAR)) {
		nk_draw_text(nk_window_get_canvas(this->ctx), HISTTEXT, " History", 8, &this->font4->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	return true;
}

bool FE::NoScanView() {
	int filled_width = WINDOW_WIDTH * .08 * 2 + WINDOW_WIDTH * .075; // remaining width ofscreen after side menu
	int delta = WINDOW_WIDTH - filled_width;
	/* CHOOSE A SCAN */
	struct nk_rect center = nk_rect(filled_width + delta * .2, WINDOW_HEIGHT * .075, delta * .6, WINDOW_HEIGHT * .7);
	struct nk_rect centerAndText = nk_rect(center.x+center.w*.2+50, center.h+35, center.w, 150); //36 for font size!
	if (nk_begin(this->ctx, "ChooseScan", center, NK_WINDOW_NO_SCROLLBAR)) {
		this->drawImageSubRect(&this->chooseScan, &center);
	}
	nk_end(this->ctx);
	/* TEXT TO CHOOSE A SCAN*/
	if (nk_begin(this->ctx, "ChooseScantxt", centerAndText, NK_WINDOW_NO_SCROLLBAR))
	{
		/*nk_layout_row_dynamic(this->ctx, 40, 1);
		nk_label_wrap(this->ctx, "                                                        ");
		nk_layout_row_dynamic(this->ctx, 80, 1);
		nk_label_wrap(this->ctx, "Chose a Scan, Please!");*/
		// old way of drawing txt...low level api
		nk_draw_text(nk_window_get_canvas(this->ctx), centerAndText, "  Choose A Scan, Please!", 24, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	return true;
}

bool FE::CompleteScanView() {
	int filled_width = WINDOW_WIDTH * .08 * 2 + WINDOW_WIDTH * .075; // remaining width ofscreen after side menu
	int delta = WINDOW_WIDTH - filled_width;

	/* quick scan header and text */
	if (nk_begin(this->ctx, "Complete Scan", nk_rect(300, WINDOW_HEIGHT * .06, 600, 300),
		NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_dynamic(this->ctx, 40, 1);
		nk_label_wrap(this->ctx, "                                                        ");
		nk_layout_row_dynamic(this->ctx, 80, 1);
		nk_label_wrap(this->ctx, "This scan will traverse all files on the current file-system!");
	}
	nk_end(this->ctx);

	/* SCAN NOW BUTTON AND TEXT */
	struct nk_rect scanButton = nk_rect(delta * .5, WINDOW_HEIGHT * .5, 250, 36);
	if (nk_begin(this->ctx, "Complete Scan Button", scanButton, NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_static(ctx, 65, 400, 1);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "do complete scan pressed\n");
			this->currentScanGoing = "Complete Scan";
			this->view = 3;
			//this->CompleteScan();
			this->scanTasks.push(1);
			nk_clear(this->ctx);
		}
	    // draw txt 
		struct nk_rect textArea = nk_rect(scanButton.x, scanButton.y, scanButton.w, scanButton.h);
		nk_draw_text(nk_window_get_canvas(this->ctx), textArea, " Complete Scan! ", 16, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		// draw triangle
		this->drawImageSubRect(&this->triangleLogo, &nk_rect(textArea.x + textArea.w - textArea.h-5, textArea.y, textArea.h, textArea.h));
	}
	nk_end(this->ctx);

	return true;
}

bool FE::QuickScansView() {
	int filled_width = WINDOW_WIDTH * .08 * 2 + WINDOW_WIDTH * .075; // remaining width ofscreen after side menu
	int delta = WINDOW_WIDTH - filled_width;

	/* quick scan header and text */
	if (nk_begin(this->ctx, "Quick Scan", nk_rect(300, WINDOW_HEIGHT * .06, 600, 300),
		NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_dynamic(this->ctx, 40, 1);
		nk_label_wrap(this->ctx, "                                                        ");
		nk_layout_row_dynamic(this->ctx, 120, 1);
		nk_label_wrap(this->ctx, "This scan will traverse the most commonly affected areas on your system.");
	}
	nk_end(this->ctx);

	/* SCAN NOW BUTTON AND TEXT */
	struct nk_rect scanButton = nk_rect(delta * .5, WINDOW_HEIGHT * .5, 200, 36);
	if (nk_begin(this->ctx, "Quick Scan Button", scanButton, NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_static(ctx, 65, 400, 1);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "do quick scan pressed\n");
			this->currentScanGoing = "Quick Scan";
			this->view = 3;
			//this->QuickScan();
			this->scanTasks.push(2);
			nk_clear(this->ctx);
		}
		// draw txt 
		struct nk_rect textArea = nk_rect(scanButton.x, scanButton.y, scanButton.w, scanButton.h);
		nk_draw_text(nk_window_get_canvas(this->ctx), textArea, " Quick Scan! ", 13, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		// draw triangle
		this->drawImageSubRect(&this->triangleLogo, &nk_rect(textArea.x + textArea.w - textArea.h -5,  textArea.y, textArea.h, textArea.h));
	}
	nk_end(this->ctx);

	return true;

}

inline bool FE::AdvancedScanView() {
	int filled_width = WINDOW_WIDTH * .08 * 2 + WINDOW_WIDTH * .075; // remaining width ofscreen after side menu
	int delta = WINDOW_WIDTH - filled_width;

	/* advanced scan header and text */
	if (nk_begin(this->ctx, "Advanced Scan", nk_rect(300, WINDOW_HEIGHT * .06, 600, 300),
		NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_dynamic(this->ctx, 120, 1);
		nk_label_wrap(this->ctx, "This scan will allow you to choose where and when to scan!");
	}
	nk_end(this->ctx);

	///* checkbox date switch */
	//if (nk_begin(this->ctx, "Overview", nk_rect(600, 600, 400, 600), NK_WINDOW_DYNAMIC))
	//{
	//	static const float ratio[] = { 120, 150 };
	//	static char field_buffer[64];
	//	static char text[9][64];
	//	static int text_len[9];
	//	static char box_buffer[512];
	//	static int field_len;
	//	static int box_len;
	//	nk_flags active;

	//	//text box and enter
	//	nk_layout_row(ctx, NK_STATIC, 100, 2, ratio);
	//	//nk_label(ctx, "test:", NK_TEXT_LEFT);
	//	nk_edit_string(ctx, NK_EDIT_SIMPLE, text[0], &text_len[0], 64, nk_filter_ascii);
	//	if (nk_button_label(ctx, "Done")) {
	//		printf("%s\n", text[0]);
	//		this->printset(advancedScanPaths);
	//	}
	//		
	//}
	//nk_end(this->ctx);
	
	/* displaying shit */
	struct nk_list_view view; //view.count = 2; 
	if (nk_begin(this->ctx, "Selected Files/Folders...", nk_rect(WINDOW_WIDTH - delta + 20, WINDOW_HEIGHT * .235, delta-40, 400), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_SCROLL_AUTO_HIDE | NK_WINDOW_DYNAMIC ))
	{
		int h = advancedScanPaths.size() * DEFAULT_FONT_SIZE + DEFAULT_FONT_SIZE;
		if (h < 400) { h = 400; }
		nk_layout_row_dynamic(ctx, h, 1);
		if (nk_list_view_begin(ctx, &view, "test", NK_WINDOW_BORDER, 25, 2)) {
			nk_layout_row_dynamic(ctx, 25, 1);
			UIPrintSet(advancedScanPaths);
			//printset(advancedScanPaths);
			/*nk_label(this->ctx, list[0], NK_TEXT_ALIGN_LEFT);
			nk_label(this->ctx, list[1], NK_TEXT_ALIGN_LEFT);*/
			nk_list_view_end(&view);
		}
	}
	nk_end(this->ctx);

	/* add folders/files button */
	struct nk_rect AddFoF = nk_rect(WINDOW_WIDTH - delta+10, WINDOW_HEIGHT * .235+405, 17*12+41, 36);
	if (nk_begin(this->ctx, "add folders", AddFoF, NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_static(ctx, 65, AddFoF.w, 1);
		if (nk_button_label(ctx, "")) {
			// openfolderdiag
			// POGGERS THIS WORKED
			std::thread t1([this]() {
				MultiFolderSelect(GetActiveWindow(), TEXT("SELECT SOME FILES/FOLDERS"));
			});
			t1.detach();
			nk_clear(this->ctx);
		}
		// draw txt 
		struct nk_rect textArea = nk_rect(AddFoF.x, AddFoF.y, AddFoF.w, AddFoF.h);
		nk_draw_text(nk_window_get_canvas(this->ctx), textArea, "  Add Files/Dirs ", 17, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		// draw add
		this->drawImageSubRect(&this->addLogo, &nk_rect(AddFoF.x + AddFoF.w - 39, AddFoF.y, AddFoF.h, AddFoF.h));
	}
	nk_end(this->ctx);

	/* adv scan now */
	struct nk_rect scanButton = nk_rect(AddFoF.x+125, AddFoF.y+75, 250, 36);
	if (nk_begin(this->ctx, "advscannow", scanButton, NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_static(ctx, 65, scanButton.w, 1);
		if (nk_button_label(ctx, "")) {
			// run adv scan now
			if (advancedScanPaths.size() > 0) {
				this->currentScanGoing = "Advanced";
				this->view = 3;
				//this->AdvanceScanNow(advancedScanPaths);
				this->scanTasks.push(3);
			}
			nk_clear(this->ctx);
		}
		// draw txt 
		struct nk_rect textArea2 = nk_rect(scanButton.x, scanButton.y, scanButton.w, scanButton.h);
		nk_draw_text(nk_window_get_canvas(this->ctx), textArea2, "  Adv. Scan Now ", 16, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		// draw triangle
		this->drawImageSubRect(&this->triangleLogo, &nk_rect(scanButton.x + scanButton.w - 40, scanButton.y, scanButton.h, scanButton.h));
	}
	nk_end(this->ctx);

	/* adv scan LATER */
	struct nk_rect scanButtonLater = nk_rect(scanButton.x+scanButton.w+50, scanButton.y, 16 * 12 + 60, 36);
	if (nk_begin(this->ctx, "advscannl8r", scanButtonLater, NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_MOVABLE))
	{
		nk_layout_row_static(ctx, 65, scanButtonLater.w, 1);
		if (nk_button_label(ctx, "")) {
			if (advancedScanPaths.size() > 0) {
				// run adv scan l8r
				this->currentScanGoing = "Scheduled Scan";
				this->_schedulerInfo = {};
				this->_schedulerInfo.type = -1;
				this->view = 4;
			}
			nk_clear(this->ctx);
		}
		// draw txt 
		struct nk_rect textArea3 = nk_rect(scanButtonLater.x, scanButtonLater.y, scanButtonLater.w, scanButtonLater.h);
		nk_draw_text(nk_window_get_canvas(this->ctx), textArea3, "  Schedule Scan ", 16, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		// draw calendar
		this->drawImageSubRect(&this->calendarLogo, &nk_rect(scanButtonLater.x + scanButtonLater.w - 40, scanButtonLater.y, scanButtonLater.h, scanButtonLater.h));
	}
	nk_end(this->ctx);

	return true;
}

inline bool FE::DrawHistoryPage()
{
	// test cout what we got
	/*std::vector<std::vector<std::string>> test = read_log();
	for (auto ss : test) {
		for (auto s : ss)
			std::cout << " " << s;
		std::cout << std::endl;
	}
	getch();*/

	/* BACK ARROW ICON */
	struct nk_rect bar = nk_rect(0, 0, WINDOW_WIDTH * .08, WINDOW_HEIGHT * .08);
	struct nk_rect backArrowAndText = nk_rect(bar.x, bar.y, bar.w, bar.h + 36); //36 for font size!
	if (nk_begin(this->ctx, "barrow", backArrowAndText,
		NK_WINDOW_NO_SCROLLBAR)) {

		/* hidden button behind icon to press */
		nk_layout_row_static(ctx, bar.y + bar.h + 36, bar.x + bar.w, 2);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "back arrow\n");
			this->view = 0;
			advancedScanPaths.clear();
			nk_clear(this->ctx);
		}
		this->drawImageSubRect(&this->backArrow, &bar);
		nk_draw_text(nk_window_get_canvas(this->ctx), SubRectTextBelow(&backArrowAndText, &bar), " BACK ", 6, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	/* lava logo */
	if (nk_begin(this->ctx, "scan", nk_rect(WINDOW_WIDTH*.5-95-95,5,95,95),
		NK_WINDOW_NO_SCROLLBAR)) {
		this->drawImage(&this->squareImage);
	}
	nk_end(this->ctx);

	/* LAVA HISTORY TEXT */
	nk_style_set_font(this->ctx, &this->font2->handle);
	struct nk_rect historyscantxt = nk_rect(WINDOW_WIDTH*.5-95+5,5,375,95);
	if (nk_begin(this->ctx, "historytextlogo", historyscantxt, NK_WINDOW_NO_SCROLLBAR)) {
		nk_draw_text(nk_window_get_canvas(this->ctx), historyscantxt, " HISTORY", 8, &this->font2->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);
	nk_style_set_font(this->ctx, &this->font->handle);

	//nk_stroke_line(this->canvas, 10, historyscantxt.h + 15, historyscantxt.w + 15, historyscantxt.h + 15, 2, nk_rgb(255, 255, 255));

	/* LAVA ACTUAL HISTORY SECTION */
	// make first col a seperate entity and skinnier...might fit rest of text this wway.
	//struct nk_rect view0 = nk_rect(5, bar.h + 50, 200, WINDOW_HEIGHT - bar.h - 55);
	//struct nk_rect scanshistory = nk_rect(view0.w,bar.h+50,WINDOW_WIDTH-5-view0.w,WINDOW_HEIGHT-bar.h-55);
	struct nk_rect biggieCheese = nk_rect(5, bar.h + 50, WINDOW_WIDTH - 10, WINDOW_HEIGHT - bar.h - 55); // total area...i give up on this for now im too tired
	struct nk_list_view view; //view.count = 2; 	

	if (nk_begin(this->ctx, "testlmao", biggieCheese,
		NK_WINDOW_SCROLL_AUTO_HIDE ))
	{
		/* HORIZANTAL LINE */
		//nk_stroke_line(this->canvas, 20, biggieCheese.y + 50, WINDOW_WIDTH - 20, biggieCheese.y + 50, 20, nk_rgb(255, 255, 255));

		nk_style_set_font(this->ctx, &this->font5->handle);
		nk_layout_row_dynamic(ctx, WINDOW_HEIGHT - 10, 5); // wrapping row
		if (nk_group_begin(ctx, "column1", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) { // column 1
			nk_layout_row_dynamic(ctx, 36, 1); // nested row
			nk_label(ctx, "Scan Type", NK_TEXT_CENTERED);

			nk_style_set_font(this->ctx, &this->font3->handle);
			for (int i = 0; i < this->PreviousScans.size(); i++) {
				nk_layout_row_dynamic(ctx, 30, 1);
				nk_label(ctx, this->PreviousScans[i][0].c_str(), NK_TEXT_CENTERED);
			}
			nk_style_set_font(this->ctx, &this->font5->handle);
			nk_group_end(ctx);
		}

		if (nk_group_begin(ctx, "column2", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) { // column 2
			nk_layout_row_dynamic(ctx, 36, 1);
			nk_label(ctx, "Scan Start", NK_TEXT_CENTERED);

			nk_style_set_font(this->ctx, &this->font3->handle);
			for (int i = 0; i < this->PreviousScans.size(); i++) {
				nk_layout_row_dynamic(ctx, 30, 1);
				nk_label(ctx, this->PreviousScans[i][1].c_str(), NK_TEXT_CENTERED);
			}
			nk_style_set_font(this->ctx, &this->font5->handle);
			nk_group_end(ctx);
		}

		if (nk_group_begin(ctx, "column3", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) { // column 3
			nk_layout_row_dynamic(ctx, 36, 1); // nested row
			nk_label(ctx, "Scan End", NK_TEXT_CENTERED);

			nk_style_set_font(this->ctx, &this->font3->handle);
			for (int i = 0; i < this->PreviousScans.size(); i++) {
				nk_layout_row_dynamic(ctx, 30, 1);
				nk_label(ctx, this->PreviousScans[i][2].c_str(), NK_TEXT_CENTERED);
			}
			nk_style_set_font(this->ctx, &this->font5->handle);
			nk_group_end(ctx);
		}

		if (nk_group_begin(ctx, "column4", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) { // column 4
			nk_layout_row_dynamic(ctx, 36, 1);
			nk_label(ctx, "Viruses Found", NK_TEXT_CENTERED);

			nk_style_set_font(this->ctx, &this->font3->handle);
			for (int i = 0; i < this->PreviousScans.size(); i++) {
				nk_layout_row_dynamic(ctx, 30, 1);
				nk_label(ctx, this->PreviousScans[i][3].c_str(), NK_TEXT_CENTERED);
			}
			nk_style_set_font(this->ctx, &this->font5->handle);
			nk_group_end(ctx);
		}

		if (nk_group_begin(ctx, "column5", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) { // column 5
			nk_layout_row_dynamic(ctx, 36, 1); // nested row
			nk_label(ctx, "Viruses Removed", NK_TEXT_CENTERED);

			nk_style_set_font(this->ctx, &this->font3->handle);
			for (int i = 0; i < this->PreviousScans.size(); i++) {
				nk_layout_row_dynamic(ctx, 30, 1);
				nk_label(ctx, this->PreviousScans[i][4].c_str(), NK_TEXT_CENTERED);
			}
			nk_style_set_font(this->ctx, &this->font5->handle);
			nk_group_end(ctx);
		}
	}
	nk_end(this->ctx);
	nk_style_set_font(this->ctx, &this->font->handle);
	
	return true;
}

inline bool FE::DrawInProgressScan()
{
	//std::cout << "\n\t cf : " << CurrentScanFile;
	// grab the top task ,if exists
	if (!this->scanTasks.empty()) {
		this->maxfiles = 0;
		std::thread t1([this]() {
			int scan = this->scanTasks.front();
			scanTasks.pop();
			switch (scan) {
			case 1: //complete
				//this->maxfiles = this->FileCount("C:\\");
				this->CompleteScan();
				break;
			case 2: //quick
				//this->maxfiles = this->TotalSetFileCount(this->countQuarantineContents());
				this->QuickScan();
				break;
			case 3: //adv
				//this->maxfiles = this->TotalSetFileCount(advancedScanPaths);
				//pm.Reccommend();
				this->AdvanceScanNow(advancedScanPaths);
				break;
			default:
				break;
			}
		});
		t1.detach();
	}


	/* LOGO */
	struct nk_rect traplogo = nk_rect(WINDOW_WIDTH * .22, WINDOW_HEIGHT * .08, WINDOW_WIDTH * .56, WINDOW_HEIGHT * .28);
	if (nk_begin(this->ctx, "lavalogo", traplogo,
		NK_WINDOW_NO_SCROLLBAR)) {
		this->drawImage(&this->trapImage);
	}
	nk_end(this->ctx);

	/* SCAN TYPE TEXT */
	if (nk_begin(this->ctx, "type", nk_rect(traplogo.x, traplogo.y+25+traplogo.h, 600, 50),
		NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_dynamic(this->ctx, 120, 1);
		nk_label_wrap(this->ctx, std::string("Scan Type : "+this->currentScanGoing).c_str());
	}
	nk_end(this->ctx);

	/* Current scanning items TEXT */
	if (nk_begin(this->ctx, "currentscanfile", nk_rect(traplogo.x, traplogo.y + 25 + traplogo.h+75, 800, 150),
		NK_WINDOW_DYNAMIC| NK_WINDOW_MOVABLE))
	{
		nk_layout_row_dynamic(this->ctx, 150, 1);
		// const char* fn = CurrentScanFile.c_str();
		auto f = std::string("file: ").append(CurrentScanFile);
		const char* fn = f.c_str();
		// std::cout << "file: " << fn;
		nk_label_wrap(this->ctx, fn);
	}
	nk_end(this->ctx);

	/* prog bbar */
	if (nk_begin(this->ctx, "progbar", nk_rect(traplogo.x-5, traplogo.y+275+traplogo.h, traplogo.w, 65),
		NK_WINDOW_NO_SCROLLBAR))
	{
		/*nk_size currentValue = this->pm.GetPercentage();*/
		nk_size currentValue = 69;
		nk_size maxValue = 100;
		nk_modify modifyable = NK_FIXED;
		nk_layout_row_dynamic(this->ctx, traplogo.w, 1);
		nk_progress(ctx, &currentValue, 100, NULL);
		std::cout << "\n  " << currentValue;
	}
	nk_end(this->ctx);

	if (this->isScanDone) { // if current scan done display our done button!
		
		/* trash icon */
		/*struct nk_rect traplogo = nk_rect(WINDOW_WIDTH * .4, WINDOW_HEIGHT - WINDOW_WIDTH * .1 - 20, WINDOW_WIDTH * .1, WINDOW_WIDTH * .1);
		if (nk_begin(this->ctx, "quarentinelogo", traplogo,
			NK_WINDOW_NO_SCROLLBAR)) {
			this->drawImage(&this->trashIcon);
		}
		nk_end(this->ctx);*/

		int b_w = 680; //width of button for done
		struct nk_rect r = nk_rect(WINDOW_WIDTH * .5 - b_w / 2, WINDOW_HEIGHT - 110, b_w, 100);
		if (nk_begin(this->ctx, "donebutton", r,
			NK_WINDOW_NO_SCROLLBAR))
		{
			nk_layout_row_static(ctx, r.h, r.w, 1);
			if (nk_button_label(ctx, "")) {
				// run adv scan now
				if (advancedScanPaths.size() > 0) {
					//fprintf(stdout, "testestestest\n");
					this->view = 5;
					nk_clear(this->ctx);
				}
			}
			// draw txt 
			struct nk_rect textArea2 = nk_rect(r.x+110, WINDOW_HEIGHT - 75, r.w, 36);
			nk_draw_text(nk_window_get_canvas(this->ctx), textArea2, " Click Here to Remove any Viruses Found!", 40, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
			// draw trash shit
			this->drawImageSubRect(&this->trashIcon, &nk_rect(r.x, r.y, 100, 100));
		}
		nk_end(this->ctx);
	}

	return true;
}

inline bool FE::printset(std::set<std::string> s)
{
	std::cout << "\ncontents: \n";
	for (std::string const& str : s)
	{
		std::cout << "\t" << str << "\n";
	}
	return true;
}

inline bool FE::viewSwap() {
	if (this->m_scanViews == 0) {
		//fprintf(stdout, "we is in default scan view bruh\n");
		NoScanView();
	}
	else if (this->m_scanViews == 1) {
		//fprintf(stdout, "we is in compelte scan view bruh\n");
		CompleteScanView();
	}
	else if (this->m_scanViews == 2) {
		//fprintf(stdout, "we is in quick scan view bruh\n");
		QuickScansView();
	}
	else if (this->m_scanViews == 3) {
		//fprintf(stdout, "we is in advanced scan view bruh\n");
		//advancedScanPaths.clear();
		AdvancedScanView();
	}
	else {
		fprintf(stderr, "this shouldnt of happened, mr. freeman\n");
	}
	return true;
}

inline bool FE::DrawScansPage()
{
	/* BACK ARROW ICON */
	struct nk_rect bar = nk_rect(0, 0, WINDOW_WIDTH * .08, WINDOW_HEIGHT * .08);
	struct nk_rect backArrowAndText = nk_rect(bar.x, bar.y, bar.w, bar.h + 36); //36 for font size!
	if (nk_begin(this->ctx, "barrow", backArrowAndText,
		NK_WINDOW_NO_SCROLLBAR)) {

		/* hidden button behind icon to press */
		nk_layout_row_static(ctx, bar.y + bar.h + 36, bar.x + bar.w, 2);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "back arrow\n");
			this->view = 0;
			advancedScanPaths.clear();
			nk_clear(this->ctx);
		}
		this->drawImageSubRect(&this->backArrow, &bar);
		nk_draw_text(nk_window_get_canvas(this->ctx), SubRectTextBelow(&backArrowAndText, &bar), " BACK ", 6, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	// vertical line
	nk_stroke_line(this->canvas, bar.w * 2 + WINDOW_WIDTH * .075, WINDOW_HEIGHT - WINDOW_HEIGHT * .06,
		bar.w * 2 + WINDOW_WIDTH * .075, 0+WINDOW_HEIGHT * .06, 2, nk_rgb(255, 255, 255));

	int w_space = bar.w * 2 + WINDOW_WIDTH * .075;
	/* COMPLETE SCAN */
	struct nk_rect completeScanTextRect = nk_rect(.025*w_space, WINDOW_HEIGHT*.30, w_space*.95, 36);
	if (nk_begin(this->ctx, "CompleteScan", completeScanTextRect, NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(this->ctx, completeScanTextRect.y + completeScanTextRect.h, completeScanTextRect.x + completeScanTextRect.w, 2);
		if (nk_button_label(this->ctx, "")) {
			//fprintf(stdout, "Complete Scan\n");
			this->m_scanViews = 1;
			nk_clear(this->ctx);
		}
		nk_draw_text(nk_window_get_canvas(this->ctx), completeScanTextRect, " Complete Scan", 14, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	/* QUICK SCAN */
	struct nk_rect quickScanTextRect = nk_rect(.025 * w_space, WINDOW_HEIGHT * .45, w_space * .95, 36);
	if (nk_begin(this->ctx, "QuickScan", quickScanTextRect, NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(this->ctx, quickScanTextRect.y + quickScanTextRect.h, quickScanTextRect.x + quickScanTextRect.w, 2);
		if (nk_button_label(this->ctx, "")) {
			//fprintf(stdout, "Quick Scan\n");
			this->m_scanViews = 2;
			nk_clear(this->ctx);
		}
		nk_draw_text(nk_window_get_canvas(this->ctx), quickScanTextRect, " Quick Scan   ", 14, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	/* ADVANCED SCAN */
	struct nk_rect advScanTextRect = nk_rect(.025 * w_space, WINDOW_HEIGHT * .60, w_space * .95, 36);
	if (nk_begin(this->ctx, "AdvancedScan", advScanTextRect, NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(this->ctx, advScanTextRect.y + advScanTextRect.h, advScanTextRect.x + advScanTextRect.w, 2);
		if (nk_button_label(this->ctx, "")) {
			//fprintf(stdout, "Advanced Scan\n");
			this->m_scanViews = 3;
			nk_clear(this->ctx);
		}
		nk_draw_text(nk_window_get_canvas(this->ctx), advScanTextRect, " Advanced Scan", 14, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	this->viewSwap();

	return true;
}

inline FE::FE() {
	//currentScanGoing = "";
	this->view = 0;
	this->m_scanViews = 0;
	this->_schedulerInfo = {};
	this->_schedulerInfo.type = -1;

	/* INIT IMAGES */
	this->pp.scan = "../Assets/scan2.png";
	this->pp.rectLogo = "../Assets/rectLogo.png";
	this->pp.trapLogo = "../Assets/trapLogo.png";
	this->pp.squareLogo = "../Assets/squareLogo.png";
	this->pp.history = "../Assets/historylarger.png";
	this->pp.backArrow = "../Assets/back_arrow.png";
	//this->pp.chooseScan = "../Assets/chooseScan.png";
	this->pp.chooseScan = "../Assets/choosescan2.png";
	this->pp.triangleButton = "../Assets/triangle.png";
	this->pp.addButton = "../Assets/add.png";
	this->pp.calendar = "../Assets/calendar.png";
	this->pp.trash = "../Assets/trash.png";
	this->pp.purpleBack = "../Assets/goback.png";
	this->pp.purpleFwd = "../Assets/cont.png";
	this->pp.done = "../Assets/done.png";
	this->scanHistorySet = read_log();
}

inline void FE::UIPrintSet(std::set<std::string> s)
{
	for (auto f : s) {
		nk_label(this->ctx, f.c_str(), NK_TEXT_ALIGN_LEFT);
	}

}

inline bool FE::ChangeFontSize(float s = 28) {

	{//struct nk_font_atlas* atlas;
		nk_sfml_font_stash_begin(&this->atlas);
		//this->font = nk_font_atlas_add_from_file(this->atlas, this->font_path, 18, NULL);
		struct nk_font* droid = nk_font_atlas_add_from_file(atlas, "../Assets/font.ttf", s, 0);
		nk_sfml_font_stash_end();
		nk_style_set_font(this->ctx, &droid->handle);
		//nk_init_default(this->ctx, &font->handle);
	}

	return true;
}

static int all = 0;
inline bool FE::QuarantineView()
{
	//int sel = 0;
	// 1. get list of viruses found
	// 2. checkbox list
	// 3. delete selected. on done btn
	
	/* BACK ARROW ICON */
	struct nk_rect bar = nk_rect(0, 0, WINDOW_WIDTH * .08, WINDOW_HEIGHT * .08);
	struct nk_rect backArrowAndText = nk_rect(bar.x, bar.y, bar.w, bar.h + 36); //36 for font size!
	if (nk_begin(this->ctx, "barrow", backArrowAndText,
		NK_WINDOW_NO_SCROLLBAR)) {

		/* hidden button behind icon to press */
		nk_layout_row_static(ctx, bar.y + bar.h + 36, bar.x + bar.w, 2);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "back arrow\n");
			// move files back
			advancedScanPaths.clear();
			std::set<q_entry> q;
			int i = 0;
			for (auto thing : this->QuarantineContents) {
				try {
					q.insert(thing);
				}
				catch (int e) {
					std::cout << "shoot we messed up quaranting!\n";
				}
				i++;
			}
			all = 0; //for next scan
			this->num_removed = 0;
			//this->remove_quarantined_files(toRemove);
			this->moveQuarantineHome(q);
			this->log_scan();
			this->view = 0;
			nk_clear(this->ctx);
		}
		this->drawImageSubRect(&this->backArrow, &bar);
		nk_draw_text(nk_window_get_canvas(this->ctx), SubRectTextBelow(&backArrowAndText, &bar), " BACK ", 6, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	// text to chose delete shit
	if (nk_begin(this->ctx, "choseshittodelete", nk_rect(bar.w+50,5,900,30),
		NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(ctx, 30, WINDOW_WIDTH-150, 1);
		nk_label_wrap(this->ctx, "Please select viruses to remove! Press Done when you're ready!");
	}
	nk_end(this->ctx);

	static std::vector<int> array(this->QuarantineContents.size());
	// CHECK BOXES
	if (nk_begin(this->ctx, "checkboxes", nk_rect(bar.w+10,45, WINDOW_WIDTH-15-bar.x-bar.w, WINDOW_HEIGHT-150),
		NK_WINDOW_BORDER | NK_WINDOW_SCROLL_AUTO_HIDE)) {
		if (this->QuarantineContents.size() > 0) {
			int i = 0; 
			// print check boxes
			for (auto s : this->QuarantineContents) {
				nk_layout_row_static(ctx, 30, 32*(s.origin_directory.length() + s.old_file_name.length()), 1);
				nk_checkbox_label(ctx, ("  (" + s.virus_name + "): " + s.origin_directory+s.old_file_name).c_str(), &array[i]);
				//try {
				//	if (array.at(i) == 1) {
				//		//std::cout << s.old_file_name << " checked \n";
				//		sel++;
				//	}
				//	else {
				//		sel--;
				//	}
				//	//std::cout << s.old_file_name << " (" << array[i] << ") \n";
				//	
				//}
				//catch (int e) {
				//	//std::cout << "\nout of bounds\n";
				//	
				//}
				//std::cout << "()"+s.origin_directory + s.old_file_name <<std::endl;
				i++;
			}
			//sk_layout_row_dynamic(ctx, 15, 1);
			//nk_label(this->ctx, "                                                                                                                         ", NK_TEXT_ALIGN_LEFT);
		}
		else {
			//std::cout << "no viruses found!\n";
			nk_layout_row_dynamic(this->ctx, 250, 1);
			//nk_label_wrap(this->ctx, "No Viruses Found! Either Press the back button or the Done button.");
			nk_draw_text(nk_window_get_canvas(this->ctx), nk_rect(150, 150, 800, 100), " NO VIRUSES FOUND", 17, &this->font2->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
			nk_draw_text(nk_window_get_canvas(this->ctx), nk_rect(150, 250, 800, 100), "    Please Hit The Done/Back button", 35, &this->font->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));

		}
		
	}
	nk_end(this->ctx);

	// SELECT ALL/DONT
	if (nk_begin(this->ctx, "all", nk_rect(bar.x+bar.w+10, WINDOW_HEIGHT-100+2, 200, 50),
		NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(ctx, 50, 200, 1);
		if (all == 0) {
			//display sel all
			if (nk_button_label(this->ctx, "Select All")) {
				//std::cout << "sell pressed" << std::endl;
				std::fill(array.begin(), array.end(), 1);
				all = 1;
			}
		}
		else {
			// all=1
			// disaply unsel all
			if (nk_button_label(ctx, "Unselect All")) {
				//std::cout << "unsel pressed" << std::endl;
				std::fill(array.begin(), array.end(), 0);
				all = 0;
			}
		}
	}
	nk_end(this->ctx);
	
	// done button
	if (nk_begin(this->ctx, "submit", nk_rect(WINDOW_WIDTH-15-200, WINDOW_HEIGHT - 100 + 2, 200, 50),
		NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(ctx, 50, 200, 1);

		if (nk_button_label(ctx, "Done")) {
			// loop thru the array and if element is 1, push to a set to ret to our mans
			std::set<std::string>toRemove;
			std::set<q_entry> q;
			int i = 0;
			for (auto thing : this->QuarantineContents) {
				try {
					if (array.at(i) == 1)
						toRemove.insert(thing.old_file_name);
					else
						q.insert(thing);
				}
				catch (int e) {
					std::cout << "shoot we messed up quaranting!\n";
				}
				i++;
			}
			all = 0; //for next scan
			this->num_removed = toRemove.size();
			this->remove_quarantined_files(toRemove);
			this->moveQuarantineHome(q);
			this->log_scan();
			this->view = 0;
		}
		
	}
	nk_end(this->ctx);

	return true;
}

int changeHrFormat(int hr, bool am) {
	if (am) {
		if (hr == 12) {
			hr = 0;
		}
	}
	else { // pm
		if (hr == 12) {
			hr = 23;
		}
		else {
			hr = hr + 12;
		}
	}

	return hr;
}


inline bool FE::ScheduleAdvScanView()
{
	/* BACK ARROW ICON */
	struct nk_rect bar = nk_rect(0, 0, WINDOW_WIDTH * .08, WINDOW_HEIGHT * .08);
	struct nk_rect backArrowAndText = nk_rect(bar.x, bar.y, bar.w, bar.h + 36); //36 for font size!
	if (nk_begin(this->ctx, "barrow", backArrowAndText,
		NK_WINDOW_NO_SCROLLBAR)) {

		/* hidden button behind icon to press */
		nk_layout_row_static(ctx, bar.y + bar.h + 36, bar.x + bar.w, 2);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "back arrow\n");
			this->view = 1;
			//advancedScanPaths.clear();
			nk_clear(this->ctx);
		}
		this->drawImageSubRect(&this->backArrow, &bar);
		nk_draw_text(nk_window_get_canvas(this->ctx), SubRectTextBelow(&backArrowAndText, &bar), " EXIT ", 6, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	unsigned static short int trigger_type= 3; // init state, make user choose what they want :) but start at one time
	/*
	0 : daily
	1 : weekly
	2 : monthly
	3 : one time bro
	*/

	static const char* items[] = { "Daily","Weekly","Monthly", "One Time" };

	/* schedule logo */
	struct nk_rect schedlogo = nk_rect(225,25,115,115);
	if (nk_begin(this->ctx, "schedlogo", schedlogo,
		NK_WINDOW_NO_SCROLLBAR)) {
		this->drawImage(&this->calendarLogo);
	}
	nk_end(this->ctx);

	/* sched text */
	if (nk_begin(this->ctx, "schedtext", nk_rect(225+schedlogo.w+7, 25+10, 720, 95),
		NK_WINDOW_NO_SCROLLBAR)) {
		nk_style_set_font(this->ctx, &this->font2->handle);
		nk_draw_text(nk_window_get_canvas(this->ctx), nk_rect(225 + schedlogo.w + 7, 25 + 10, 720, 95), " Scan Scheduler", 15, 
			&this->font2->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		nk_style_set_font(this->ctx, &this->font->handle);
	}
	nk_end(this->ctx);

	switch (this->_schedulerInfo.type) {
		//std::cout << "\n\tcurrent select: " << this->_schedulerInfo.type;
	case -1:
		try {
			// select trigger type
			if (nk_begin(this->ctx, "triggertxt", nk_rect(WINDOW_WIDTH * .5 - 150, WINDOW_HEIGHT * .5 - 50, 300, 28),
				NK_WINDOW_NO_SCROLLBAR)) {
				nk_draw_text(nk_window_get_canvas(this->ctx), nk_rect(WINDOW_WIDTH * .5 - 150, WINDOW_HEIGHT * .5 - 50, 300, 200),
					" Scheduler Type", 15,
					&this->font->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
			}
			nk_end(this->ctx);

			/* try selectable */
			if (nk_begin(this->ctx, "triggertype", nk_rect(WINDOW_WIDTH * .5 - 150, WINDOW_HEIGHT * .5, 300, 60),
				NULL)) {

				// default combo box
				nk_layout_row_static(ctx, 30, 160, 1);
				trigger_type = nk_combo(ctx, items, 4, trigger_type, 30, nk_vec2(160, 200));
				/*nk_layout_row_static(ctx, 50, 300, 1);
				int i = 0;
				if (nk_combo_begin_label(ctx, items[trigger_type], nk_vec2(200, 200))) {
					nk_layout_row_dynamic(ctx, 25, 1);
					for (i = 0; i < 4; ++i)
						if (nk_combo_item_label(ctx, items[i], NK_TEXT_LEFT))
							trigger_type = i;
					nk_combo_end(ctx);
				}*/
				//std::cout << "\n\ttype chosen: " << items[trigger_type] << "\n";
			}
			nk_end(this->ctx);

			/* FORWARD ARROW AT BOTTOM */
			struct nk_rect bar1 = nk_rect(WINDOW_WIDTH * .5 - 60, 650, 120, 65);
			//struct nk_rect bar1 = nk_rect(600,600,120,65);
			if (nk_begin(this->ctx, "purparrowforward", bar1,
				NK_WINDOW_NO_SCROLLBAR)) {
				/* hidden button behind icon to press */
				nk_layout_row_static(ctx, bar1.h, bar1.w, 1);
				if (nk_button_image(this->ctx, this->purpleFwd)) {
					//this->view = 1;
					this->_schedulerInfo.type = (int)trigger_type;
					//std::cout << "\n\t " << trigger_type << "   " << this->_schedulerInfo.type ;
				}
				//this->drawImage(&this->purpleFwd);
			}
			nk_end(this->ctx);
			return true;
		}
		catch (int e) {
			fprintf(stdout, "flip we messed up the selectable");
		}
		break;

	case 0: // DAILY BRUV
		try {
			// type : daily
			static bool op = true;
			this->displayScheduleType();
			// ask for time
			//nk_clear(this->ctx);

			if (nk_begin(this->ctx, "hrsstext", nk_rect(125, 180, 100, 30),
				NK_WINDOW_SCROLL_AUTO_HIDE|NK_WINDOW_NO_SCROLLBAR)) {
				// default combo box
				nk_layout_row_dynamic(this->ctx, 30, 1);
				nk_label_wrap(this->ctx, "Hour: ");
			}
			nk_end(this->ctx);
			// AM OR PM
			if (nk_begin(this->ctx, "amPM", nk_rect(125 + 100 + 2 + 300, 180, 300, 60),
				NULL)) {
				nk_layout_row_dynamic(this->ctx, 30, 2);
				if (nk_option_label(this->ctx, "AM", op == true)) op = false;
				if (nk_option_label(this->ctx, "PM", op == true)) op = false;
			}
			nk_end(this->ctx);
			unsigned static short int trigger_type = 0; // init state, make user choose what they want :) but start at one time
			static const char* hrs[] = { "1","2","3","4","5","6","7","8","9","10","11","12" };
			/* try selectable */
			if (nk_begin(this->ctx, "hrs", nk_rect(125 + 100 + 2, 180, 300, 60),
				NULL)) {
				// default combo box
				nk_layout_row_static(ctx, 30, 160, 1);
				trigger_type = nk_combo(ctx, hrs, 12, trigger_type, 30, nk_vec2(160, 200));
				std::cout << "\n\t hr : " << changeHrFormat(trigger_type + 1, op);
			}
			nk_end(this->ctx);


			this->displayScheduleArrows();
		}
		catch (int e) {
			fprintf(stdout, "flip we messed up the selectable");
		}
		break;

	case 1://weekly
		try {
			this->displayScheduleType();
			this->displayScheduleArrows();
		}
		catch (int e) {
			fprintf(stdout, "flip we messed up the selectable");
		}
		break;

	case 2: //mohnhtly
		try {
			this->displayScheduleType();
			this->displayScheduleArrows();
		}
		catch (int e) {
			fprintf(stdout, "flip we messed up the selectable");
		}
		break;

	case 3: //one time
		try {
			this->displayScheduleType();
			this->displayScheduleArrows();
		}
		catch (int e) {
			fprintf(stdout, "flip we messed up the selectable");
		}
		break;

	case 4:
		try {
			this->displayScheduleArrows();
		}
		catch (int e) {
			fprintf(stdout, "flip we messed up the selectable");
		}
		break;

	default: 
		std::cout << "\n  flip";
		break;
	}

	return true;
}

inline bool FE::displayScheduleArrows()
{
	/* back arrow AT BOTTOM */
	struct nk_rect bar1 = nk_rect(WINDOW_WIDTH * .5 - 120 - 5, 650, 120, 65);
	//struct nk_rect bar1 = nk_rect(600,600,120,65);
	if (nk_begin(this->ctx, "purparrowback", bar1,
		NK_WINDOW_NO_SCROLLBAR)) {
		/* hidden button behind icon to press */
		nk_layout_row_static(ctx, bar1.h, bar1.w, 1);
		if (nk_button_image(this->ctx, this->purpleBack)) {
			//std::cout << "\n\t " << trigger_type;
			//this->view = 1;
			this->_schedulerInfo = {};
			this->_schedulerInfo.type = -1;
		}
		//this->drawImage(&this->purpleFwd);
	}
	nk_end(this->ctx);
	/* FRONT arrow AT BOTTOM */
	struct nk_rect bar2 = nk_rect(WINDOW_WIDTH * .5 + 5, 650, 140, 65);
	//struct nk_rect bar1 = nk_rect(600,600,120,65);
	if (nk_begin(this->ctx, "purparrowfor", bar2,
		NK_WINDOW_NO_SCROLLBAR)) {
		/* hidden button behind icon to press */
		nk_layout_row_static(ctx, bar2.h, bar2.w, 1);
		if (nk_button_image(this->ctx, this->done)) {
			//std::cout << "\n\t " << trigger_type;
			//this->view = 1;
			this->_schedulerInfo = {};
			this->_schedulerInfo.type = -1;
		}
		//this->drawImage(&this->purpleFwd);
	}
	nk_end(this->ctx);
	return true;
}

inline bool FE::displayScheduleType()
{
	try {
		// add logo
		/*if (nk_begin(this->ctx, "lavalogo", r_logo,
			NK_WINDOW_NO_SCROLLBAR)) {
			this->drawImage(&this->squareImage);
		}
		nk_end(this->ctx);*/

		std::string typ = "Schedule Type: ";
		switch (this->_schedulerInfo.type) {
		case 0:
			typ.append( "Daily ");
			break;
		case 1:
			typ.append("Weekly ");
			break;
		case 2:
			typ.append("Monthly ");
			break;
		case 3:
			typ.append("One Time ");
			break;
		default:
			typ.append("failed ");
			break;
		}

		// text
		if (nk_begin(this->ctx, "txt", nk_rect(125,150,800, 30),
			NK_WINDOW_NO_SCROLLBAR)) {
			/*nk_layout_row_dynamic(this->ctx, 80, 1);
			nk_label_wrap(this->ctx, "Chose a Scan, Please!");*/
			nk_layout_row_dynamic(this->ctx,30,1);
			//std::cout << "\n\t" << typ;
			nk_label_wrap(this->ctx, typ.c_str());
		}
		nk_end(this->ctx);
	}
	catch (int e) {
		std::cout << "failed the sched type bruv" << std::endl;
	}
	return true;
}

inline bool FE::init(sf::Window *win) {
	glewExperimental = 1;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to setup GLEW\n");
		std::cout << glewGetErrorString(glewInit());
		exit(1);
	}
	/* GUI */
	//struct nk_context* ctx;
	this->ctx = nk_sfml_init(win);
	// ihstory header font
	{
		nk_sfml_font_stash_begin(&this->atlas2);
		this->font2 = nk_font_atlas_add_from_file(this->atlas2, "../Assets/font.ttf", 95, 0);
		nk_sfml_font_stash_end();
	}
	// smaller text
	{
		nk_sfml_font_stash_begin(&this->atlas3);
		this->font3 = nk_font_atlas_add_from_file(this->atlas3, "../Assets/font.ttf", 18, 0);
		nk_sfml_font_stash_end();
	}
	{//logo text size 88;
		nk_sfml_font_stash_begin(&this->atlas4);
		this->font4 = nk_font_atlas_add_from_file(this->atlas4, "../Assets/font.ttf", 84, 0);
		nk_sfml_font_stash_end();
	}
	{//logo text size 88;
		nk_sfml_font_stash_begin(&this->atlas5);
		this->font5 = nk_font_atlas_add_from_file(this->atlas5, "../Assets/font.ttf", 22, 0);
		nk_sfml_font_stash_end();
	}
	{//struct nk_font_atlas* atlas;
		nk_sfml_font_stash_begin(&this->atlas);
		//this->font = nk_font_atlas_add_from_file(this->atlas, this->font_path, 18, NULL);
		this->font= nk_font_atlas_add_from_file(this->atlas, "../Assets/font.ttf", DEFAULT_FONT_SIZE, 0);
		nk_sfml_font_stash_end();
		nk_style_set_font(this->ctx, &this->font->handle);
		//nk_init_default(this->ctx, &font->handle);
	}

	struct nk_color table[NK_COLOR_COUNT];
	table[NK_COLOR_TEXT] = nk_rgba(175, 175, 175, 255);
	table[NK_COLOR_WINDOW] = nk_rgba(45, 45, 45, 255);
	table[NK_COLOR_HEADER] = nk_rgba(40, 40, 40, 255);
	table[NK_COLOR_BORDER] = nk_rgba(65, 65, 65, 255);
	table[NK_COLOR_BUTTON] = nk_rgba(50, 50, 50, 255);
	table[NK_COLOR_BUTTON_HOVER] = nk_rgba(40, 40, 40, 255);
	table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(35, 35, 35, 255);
	table[NK_COLOR_TOGGLE] = nk_rgba(100, 100, 100, 255);
	table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(140,138,128, 255);
	table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(255, 69, 0, 255);
	table[NK_COLOR_SELECT] = nk_rgba(45, 45, 45, 255);
	table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(208, 7, 27, 255);
	table[NK_COLOR_SLIDER] = nk_rgba(208, 119, 126, 255);
	table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(255, 69, 0, 255);
	table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(255, 69, 0, 255);
	table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(255, 69, 0, 255);
	table[NK_COLOR_PROPERTY] = nk_rgba(38, 38, 38, 255);
	table[NK_COLOR_EDIT] = nk_rgba(38, 38, 38, 255);
	table[NK_COLOR_EDIT_CURSOR] = nk_rgba(175, 175, 175, 255);
	table[NK_COLOR_COMBO] = nk_rgba(45, 45, 45, 255);
	table[NK_COLOR_CHART] = nk_rgba(120, 120, 120, 255);
	table[NK_COLOR_CHART_COLOR] = nk_rgba(45, 45, 45, 255);
	table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
	table[NK_COLOR_SCROLLBAR] = nk_rgba(40, 40, 40, 255);
	table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(100, 100, 100, 255);
	table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] =nk_rgba(120, 120, 120, 255);
	table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(150, 150, 150, 255);
	table[NK_COLOR_TAB_HEADER] = nk_rgba(40, 40, 40, 255);
	nk_style_from_table(ctx, table);

	// load some image stuff
	//struct nk_buffer cmds;
	////nk_buffer_init_default(&cmds);
	glEnable(GL_TEXTURE_2D);

	// bg color
	this->bg.r = 45 / 255.0f; this->bg.g = 45 / 255.0f; this->bg.b = 45 / 255.0f; this->bg.a = 1.0f;
	// white txt color
	this->whiteFont.r = 0; this->whiteFont.g =0; this->whiteFont.b = 0; this->whiteFont.a = 1;
	
	// load pngs 
	this->scanImage = this->icon_load(pp.scan);
	this->squareImage = this->icon_load(pp.squareLogo);
	this->historyImage = this->icon_load(pp.history);
	this->backArrow = this->icon_load(pp.backArrow);
	this->chooseScan = this->icon_load(pp.chooseScan);
	this->triangleLogo = this->icon_load(pp.triangleButton);
	this->addLogo = this->icon_load(pp.addButton);
	this->trapImage = this->icon_load(pp.trapLogo);
	this->calendarLogo = this->icon_load(pp.calendar);
	this->trashIcon = this->icon_load(pp.trash);
	this->purpleBack = this->icon_load(pp.purpleBack);
	this->purpleFwd = this->icon_load(pp.purpleFwd);
	this->done = this->icon_load(pp.done);
	return true;
}

#endif