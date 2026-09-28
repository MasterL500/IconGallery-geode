#include "Icon2.hpp"
#include "../ui/IconCell.hpp"

const char *MORE_ICONS = "hiimjustin000.more_icons";

//	Returns the format based on the string
IconFormat2 formatFromString(std::string format)
{
	return (std::string_view(format) == std::string_view("Vanilla2")) ? IconFormat2::Vanilla2 : IconFormat2::MoreIcons2;
};

//	Let's create an Icon2 Object!
Icon2 *Icon2::create(std::string name, std::string author, std::string filename, std::string previewURL, int gamemode, int downloads, std::string desc, std::string format)
{
	auto icon = new Icon2();

	//	Feels the fill
	icon->m_name = name;
	icon->m_author = author;
	icon->m_fileName = filename;
	icon->m_description = desc;
	icon->m_previewURL = previewURL;

	//	Feels the fill V2
	icon->m_gamemode = IconType{gamemode};
	icon->m_format = formatFromString(format);
	icon->m_downloads = downloads;

	//	Returns the created Icon2
	return icon;
};

//	Adds the collaborators (Still unsure if it works like this)
void Icon2::addCollab(std::vector<std::string> contributors)
{
	//	Hi, wanna collab?
	this->m_collaborators = contributors;
};

//	Oh boy here we go downloading an icon!
void Icon2::downloadIcon()
{
	isDownloading = true;

	//	If a cell exists
	if (m_cell)
	{
		m_cell->m_downloadBtn->setVisible(false);
		m_downloadBar->setVisible(true);
	}

	//	Web stuff
	auto weak = geode::WeakRef(this);
	auto req = utils::web::WebRequest();
	auto url = fmt::format("{}/api/sprites/{}", Mod::get()->getSavedValue<std::string>("API"), m_fileName);

	//	Progress
	req.onProgress(
		[this, weak](web::WebProgress const &progress)
		{
			log::debug("Download Progress {}", progress.downloadProgress().value_or(0.f));

			if (m_downloadBar && weak.valid())
				m_downloadBar->setValue(progress.downloadProgress().value_or(0.01f) / 100.0f);
		});

	//	Downloading
	m_listener.spawn(
		req.get(url),
		[this, weak](web::WebResponse res)
		{
			if (!weak.lock())
				return;

			if (res.ok())
			{
				auto zipfileName = geode::utils::string::replace(m_fileName, ".gdicon", ".zip");

				if (res.into(Mod::get()->getConfigDir() / zipfileName))
				{
					isDownloading = false;
					isDownloadSuccesful = true;

					m_zipPath = Mod::get()->getConfigDir() / zipfileName;

					if (m_cell)
						m_cell->updateStatus();

					//	Next phase, unpack the icon
					if (Mod::get()->getSettingValue<bool>("auto-unpack"))
					{
						exp_unpackIcon(false);
					}
					else
					{
						auto popup = createQuickPopup(
							"Icon2 Downloaded!",
							"Do you want to unzip the files of the icon? (Note: This is still experimental and could crash)",
							"No",
							"Yes",
							[this](auto, bool btn)
							{
								if (btn)
								{
									unpackIcon();
								}
							});
					}
				}
				else
				{
					auto errorPopup = createQuickPopup(
						"Error: Icon2 Not Found",
						fmt::format("The files of this icon got corrupted. Please report the icon (<cy>{}</c>) to the Gallery Admins in the <cd>Discord Server</c>.", this->m_name).c_str(),
						"Ok",
						nullptr,
						[](auto, auto) {});
				}
			}
			else
			{
				if (res.error())
				{
					auto errorPopup = createQuickPopup(
						"Error: No Internet",
						"Couldn't download icon, try again later.",
						"Ok",
						nullptr,
						[](auto, auto) {});
				}
				else
				{
					auto errorPopup = createQuickPopup(
						"Error: Icon2 Not Found",
						fmt::format("The files of this icon got corrupted. Please report the icon (<cy>{}</c>) to the Gallery Admins in the <cd>Discord Server</c>.", this->m_name).c_str(),
						"Ok",
						nullptr,
						[](auto, auto) {});
				}

				log::debug("Error while Downloading the Icon2: {}", res.errorMessage());
			}
		});
};

//	Unpacking shenanigans
void Icon2::unpackIcon()
{
	std::vector<std::string> modeFolders = {
		"icon",
		"ship",
		"ball",
		"ufo",
		"wave",
		"robot",
		"spider",
		"swing",
		"jetpack"};
	auto gamemode = modeFolders[(int)m_gamemode];

	//	This is prone to issues, I know.
	std::filesystem::path path = Mod::get()->getSettingValue<std::filesystem::path>("icon-pack-folder");
	std::filesystem::path unzipDir = (m_format == IconFormat2::MoreIcons2) || Mod::get()->getSettingValue<bool>("vanilla-more-icons") ? (path / "config" / "hiimjustin000.more_icons" / gamemode) : (path / "icons");

	//	Special locations (if their settings are enabled)
	if (m_format == IconFormat2::Vanilla2 && Mod::get()->getSettingValue<bool>("resources-folder"))
	{
		log::debug("Resources Folder Enabled");
		unzipDir = geode::dirs::getResourcesDir() / "icons";
	}
	else if (Mod::get()->getSettingValue<bool>("more-icons-folder"))
	{
		log::debug("More Icons Folder Enabled");
		unzipDir = Loader::get()->getInstalledMod("hiimjustin000.more_icons")->getConfigDir() / gamemode;
	}

	//	Just made sum BULLSHIT!!!!
	auto unzipRes = utils::file::Unzip::create(m_zipPath);
	if (unzipRes.isErr())
	{
		return log::error("Unzip::create failed: ", unzipRes.unwrapErr());
	};

	//	Pray for me
	auto unzip = std::move(unzipRes).unwrap();
	auto entries = unzip.getEntries();

	//	Verifies if the sprites exist (they are all named the same, so)
	if (std::filesystem::exists(unzipDir / entries[0]))
	{
		auto entryName = utils::string::split(utils::string::pathToString(entries[0]), "-");

		auto popup = createQuickPopup(
			"Overwrite Icon2?",
			"There's already an icon named \"<cg>" + entryName[0] + "</c>\", are you sure you want to <cy>overwrite</c> it?",
			"No",
			"Yes", [this, unzipDir](bool no, bool yes)
			{
				if(yes){
					if (utils::file::Unzip::intoDir(m_zipPath, unzipDir, Mod::get()->getSettingValue<bool>("delete-zips")).isOk())
						{
							Notification::create("Icon2 Succesfully Added!", NotificationIcon::Success)->show();
							log::debug("Icon2 Exported to path: {}", unzipDir);
						}
				} else if(no){
					Notification::create("Icon2 Extracting cancelled", NotificationIcon::Error)->show();
				} });
	}
	else
	{
		if (utils::file::Unzip::intoDir(m_zipPath, unzipDir, Mod::get()->getSettingValue<bool>("delete-zips")).isOk())
		{
			Notification::create("Icon2 Succesfully Added!", NotificationIcon::Success)->show();
			log::debug("Icon2 Exported to path: {}", unzipDir);
		}
	}
};

void Icon2::exp_unpackIcon(bool forceMoreIcons)
{
	//	Selecting the Gamemode Folder
	std::vector<std::string> modeFolders = {
		"icon",
		"ship",
		"ball",
		"ufo",
		"wave",
		"robot",
		"spider",
		"swing",
		"jetpack"};
	auto gamemode = modeFolders[(int)m_gamemode];

	//	Path of the texture pack and default unzipping location (More Icons folder).
	std::filesystem::path packPath = Mod::get()->getSettingValue<std::filesystem::path>("icon-pack-folder");
	std::filesystem::path unzipPath = packPath / "config" / MORE_ICONS / gamemode;

	//	Other path locations.
	if (m_format == IconFormat2::Vanilla2 && !Mod::get()->getSettingValue<bool>("vanilla-more-icons"))
	{
		//	If the icon is Vanilla2 and the setting of "Vanilla2 as MI" isn't enabled.
		unzipPath = packPath / "icons";
		log::debug("* UNPACK_PATH: Assigned the Icons folder within Texture Pack.");
	}
	else if (m_format == IconFormat2::Vanilla2 && Mod::get()->getSettingValue<bool>("resources-folder"))
	{
		//	If the icon is Vanilla2 and the setting of "Resources folder" is enabled.
		unzipPath = geode::dirs::getResourcesDir() / "icons";
		log::debug("* UNPACK_PATH: Assigned the Resources Folder.");
	}
	else if (Mod::get()->getSettingValue<bool>("more-icons-folder"))
	{
		//	If the More Icons folder is enabled.
		unzipPath = Loader::get()->getInstalledMod(MORE_ICONS)->getConfigDir() / gamemode;
		log::debug("* UNPACK_PATH: Assigned the More Icons (Mod) folders.");
	}

	//	Unzipping Process.
	auto unzipRes = utils::file::Unzip::create(m_zipPath);
	if (unzipRes.isErr())
	{
		log::error("* UNZIPPING FAILED: {}", unzipRes.unwrapErr());
		return;
	}

	//	Gets the entries.
	auto unzipped = std::move(unzipRes).unwrap();
	m_tempFiles = unzipped.getEntries();

	if (!std::filesystem::exists(unzipPath / m_tempFiles[0]))
	{
		if (utils::file::Unzip::intoDir(m_zipPath, unzipPath, Mod::get()->getSettingValue<bool>("delete-zips")).isOk())
		{
			Notification::create("Icon2 Succesfully Added!", NotificationIcon::Success)->show();
			log::debug("* Icon2 succesfully exported.");
		}

		return;
	}

	log::debug("* CONFLICT FOUND.");

	//	If there's conflict, goes to check.
	auto tempRes = utils::file::Unzip::intoDir(m_zipPath, Mod::get()->getConfigDir() / "temp", Mod::get()->getSettingValue<bool>("delete-zips"));
	if (tempRes.isErr())
	{
		log::error("* UNZIPPING FAILED: {}", tempRes.unwrapErr());
		return;
	}

	auto iconFilename = utils::string::split(utils::string::pathToString(m_tempFiles[0]), "-");

	auto popup = createQuickPopup(
		"Name conflict detected",
		"There's already an icon taking the spot of \"<cg>" + iconFilename[0] + "</c>\", please input a new name for this one",
		"Ok", nullptr, [this, iconFilename](bool ok, auto)
		{
			if(ok){
				auto setTextPopup = SetTextPopup::create(iconFilename[0], "Input Name", 100, "Icon2 Filename", "Apply", true, 0);
				setTextPopup->m_delegate = this;
				setTextPopup->show();
			} });

	log::debug("* END OF CONFLICT SEGMENT.");
}

void Icon2::setTextPopupClosed(SetTextPopup *popup, gd::string text)
{
	if (!popup)
		return;

	log::debug("* TEXT POPUP CLOSED.");

	//	Selecting the Gamemode Folder
	std::vector<std::string> modeFolders = {
		"icon",
		"ship",
		"ball",
		"ufo",
		"wave",
		"robot",
		"spider",
		"swing",
		"jetpack"};
	auto gamemode = modeFolders[(int)m_gamemode];

	//	Path of the texture pack and default unzipping location (More Icons folder).
	std::filesystem::path packPath = Mod::get()->getSettingValue<std::filesystem::path>("icon-pack-folder");
	std::filesystem::path unzipPath = packPath / "config" / MORE_ICONS / gamemode;

	//	Other path locations.
	if (m_format == IconFormat2::Vanilla2 && !Mod::get()->getSettingValue<bool>("vanilla-more-icons"))
	{
		//	If the icon is Vanilla2 and the setting of "Vanilla2 as MI" isn't enabled.
		unzipPath = packPath / "icons";
		log::debug("* UNPACK_PATH: Assigned the Icons folder within Texture Pack.");
	}
	else if (m_format == IconFormat2::Vanilla2 && Mod::get()->getSettingValue<bool>("resources-folder"))
	{
		//	If the icon is Vanilla2 and the setting of "Resources folder" is enabled.
		unzipPath = geode::dirs::getResourcesDir() / "icons";
		log::debug("* UNPACK_PATH: Assigned the Resources Folder.");
	}
	else if (Mod::get()->getSettingValue<bool>("more-icons-folder"))
	{
		//	If the More Icons folder is enabled.
		unzipPath = Loader::get()->getInstalledMod(MORE_ICONS)->getConfigDir() / gamemode;
		log::debug("* UNPACK_PATH: Assigned the More Icons (Mod) folders.");
	}

	if (!m_tempFiles.empty())
	{
		for (int ii = 0; ii < m_tempFiles.size(); ii++)
		{
			auto extension = utils::string::split(utils::string::pathToString(m_tempFiles[ii]), "-")[1];
			auto newFilename = fmt::format("{}-{}", text, extension);

			std::filesystem::rename(Mod::get()->getConfigDir() / "temp" / m_tempFiles[ii], unzipPath / newFilename);
			log::debug("* FILE \"{}\" MOVED TO THE LOCATION.", newFilename);
		}

		Notification::create("Icon2 Succesfully Added!", NotificationIcon::Success)->show();
		log::debug("* Icon2 succesfully exported.");
	}
}