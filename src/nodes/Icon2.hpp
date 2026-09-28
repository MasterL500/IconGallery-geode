#pramga once
#include <Geode/Geode.hpp>
#include <Geode/loader/Dirs.hpp>

using namespace geode::prelude;

enum IconFormat2
{
	Vanilla2,
	MoreIcons2
};

class IconCell;

class Icon2 : public CCObject, public SetTextPopupDelegate
{
	IconCell *m_cell = nullptr;

	//	Main Info
	std::string m_name;
	std::string m_author;
	std::string m_fileName;
	std::string m_description;
	std::string m_previewURL;
	IconFormat2 m_format;
	IconType m_gamemode;
	int m_downloads;

	//	Extra Info
	std::filesystem::path m_zipPath;
	std::vector<std::string> m_collaborators = {};

	//	Creating and adding data
	static Icon *create(std::string name, std::string author, std::string filename, std::string previewURL, int gamemode, int downloads = 0, std::string desc = "", std::string format = "More Icons");
	void addCollab(std::vector<std::string> contributors);

	//	Downloading Processes
	bool isDownloading = false;
	bool isDownloadSuccesful = false;

	Slider *m_downloadBar = nullptr;
	TaskHolder<geode::utils::web::WebResponse> m_listener;

	void downloadIcon();
	void unpackIcon();

	//	Experimental resources
	std::vector<std::filesystem::path> m_tempFiles = {};

	void exp_getUnzipLocation();
	void exp_unpackIcon(bool forceMoreIcons);
	void setTextPopupClosed(SetTextPopup *popup, gd::string text) override;
}