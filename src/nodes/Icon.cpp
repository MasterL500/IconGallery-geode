#include "Icon.hpp"
#include "../ui/IconCell.hpp"

const char *MOREICONS = "hiimjustin000.more_icons";

//	Returns the format based on the string
IconFormat formatFromString(std::string format)
{
	return (std::string_view(format) == std::string_view("Vanilla")) ? IconFormat::Vanilla : IconFormat::MoreIcons;
};

//	Let's create an Icon Object!
Icon *Icon::create(std::string name, std::string author, std::string filename, std::string previewURL, int gamemode, int downloads, std::string desc, std::string format)
{
	auto icon = new Icon();

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

	//	Returns the created Icon
	return icon;
};

//	Adds the collaborators (Still unsure if it works like this)
void Icon::addCollab(std::vector<std::string> contributors)
{
	//	Hi, wanna collab?
	this->m_collaborators = contributors;
};

//	Oh boy here we go downloading an icon!
void Icon::downloadIcon()
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
						if (Mod::get()->getSettingValue<bool>("experimental-export"))
						{
							exp_unpackIcon();
						}
						else
						{
							unpackIcon();
						}
					}
					else
					{
						auto popup = createQuickPopup(
							"Icon Downloaded!",
							"Do you want to unzip the files of the icon? (Note: This is still experimental and could crash)",
							"No",
							"Yes",
							[this](auto, bool btn)
							{
								if (btn)
								{
									if (Mod::get()->getSettingValue<bool>("experimental-export"))
									{
										exp_unpackIcon();
									}
									else
									{
										unpackIcon();
									}
								}
							});
					}
				}
				else
				{
					auto errorPopup = createQuickPopup(
						"Error: Icon Not Found",
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
						"Error: Icon Not Found",
						fmt::format("The files of this icon got corrupted. Please report the icon (<cy>{}</c>) to the Gallery Admins in the <cd>Discord Server</c>.", this->m_name).c_str(),
						"Ok",
						nullptr,
						[](auto, auto) {});
				}

				log::debug("Error while Downloading the Icon: {}", res.errorMessage());
			}
		});
};

//	Unpacking shenanigans
void Icon::unpackIcon()
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
	std::filesystem::path unzipDir = (m_format == IconFormat::MoreIcons) || Mod::get()->getSettingValue<bool>("vanilla-more-icons") ? (path / "config" / "hiimjustin000.more_icons" / gamemode) : (path / "icons");

	//	Special locations (if their settings are enabled)
	if (m_format == IconFormat::Vanilla && Mod::get()->getSettingValue<bool>("resources-folder"))
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
			"Overwrite Icon?",
			"There's already an icon named \"<cg>" + entryName[0] + "</c>\", are you sure you want to <cy>overwrite</c> it?",
			"No",
			"Yes", [this, unzipDir](bool no, bool yes)
			{
				if(yes){
					if (utils::file::Unzip::intoDir(m_zipPath, unzipDir, Mod::get()->getSettingValue<bool>("delete-zips")).isOk())
						{
							Notification::create("Icon Succesfully Added!", NotificationIcon::Success)->show();
							log::debug("Icon Exported to path: {}", unzipDir);
						}
				} else if(no){
					Notification::create("Icon Extracting cancelled", NotificationIcon::Error)->show();
				} });
	}
	else
	{
		if (utils::file::Unzip::intoDir(m_zipPath, unzipDir, Mod::get()->getSettingValue<bool>("delete-zips")).isOk())
		{
			Notification::create("Icon Succesfully Added!", NotificationIcon::Success)->show();
			log::debug("Icon Exported to path: {}", unzipDir);
		}
	}
};

//	Experimental feature to unpacking the Icon
void Icon::exp_unpackIcon()
{
	//	Creates the temporal folder for the Icon.
	auto tempPath = Mod::get()->getConfigDir() / "temp" / geode::utils::string::replace(m_fileName, ".gdicon", "");

	//	Unzips the content into the newly created folder.
	auto tempRes = utils::file::Unzip::intoDir(m_zipPath, tempPath, Mod::get()->getSettingValue<bool>("delete-zips"));
	if (tempRes.isErr())
	{
		Notification::create("Error while Unzipping...", NotificationIcon::Error)->show();
		log::error("* ERROR WHILE UNZIPPING AT TEMP FOLDER: {}", tempRes.unwrapErr());
		return;
	}

	//	Reads the directory
	auto tempDirRes = utils::file::readDirectory(tempPath, false);
	if (tempDirRes.isErr())
	{
		Notification::create("Error while Unzipping...", NotificationIcon::Error)->show();
		log::error("* FAILED READING DIRECTORY PATH.");
		return;
	};

	//	Gets the vector of the filenames
	m_tempFiles = std::move(tempDirRes).unwrap();
	auto unzipPath = exp_getUnzipLocation();

	if (!std::filesystem::exists(unzipPath / m_tempFiles[0].filename()))
	{
		if (!std::filesystem::exists(unzipPath))
		{
			std::filesystem::create_directory(unzipPath);
		}

		for (int ii = 0; ii < m_tempFiles.size(); ii++)
		{
			std::filesystem::rename(m_tempFiles[ii], unzipPath / m_tempFiles[ii].filename());
			log::debug("* FILE \"{}\" MOVED TO LOCATION.", m_tempFiles[ii].filename());
		}

		Notification::create("Icon Succesfully Added!", NotificationIcon::Success)->show();
		log::debug("* Icon succesfully exported.");

		std::error_code ec;
		std::filesystem::remove_all(tempPath, ec);

		return;
	}

	log::debug("* CONFLICT FOUND.");

	//	Opens the popup to prompt the renaming.
	auto iconFilename = utils::string::split(utils::string::pathToString(m_tempFiles[0].stem()), "-");
	auto popup = createQuickPopup(
		"Name Conflict",
		"There's already an icon taking the spot of \"<cg>" + iconFilename[0] + "</c>\", please input a new name for this one.",
		"Ok", nullptr, [this, iconFilename](bool ok, auto)
		{
			if(ok){
				auto setTextPopup = SetTextPopup::create(iconFilename[0], "Input Name", 100, "Icon Filename", "Apply", true, 0);
				setTextPopup->m_delegate = this;
				setTextPopup->show();
			} });

	log::debug("* END OF UNZIP FUNCTION.");
}

//	When the popup is closed
void Icon::setTextPopupClosed(SetTextPopup *popup, gd::string text)
{
	if (!popup)
		return;

	if (popup->m_cancelled)
	{
		//	Notifies the user the process has been cancelled
		Notification::create("Process cancelled", NotificationIcon::Error)->show();
		log::debug("* The user has cancelled the process");

		//	Removes the temporal files (Because why bother?).
		auto tempPath = Mod::get()->getConfigDir() / "temp" / geode::utils::string::replace(m_fileName, ".gdicon", "");
		std::error_code ec;
		std::filesystem::remove_all(tempPath, ec);

		return;
	}

	//	If the temporary files weren't saved, skips this process.
	if (!m_tempFiles.empty())
	{
		auto unzipPath = exp_getUnzipLocation(true);

		//	Renames every file of this icon and moves it to the destined path.
		for (int ii = 0; ii < m_tempFiles.size(); ii++)
		{
			//	log::debug("ENTRY: {}", m_tempFiles[ii].filename());

			//	Fetches the segment of the filename that states the quality (-hd or -uhd).
			auto extension = utils::string::split(utils::string::pathToString(m_tempFiles[ii].filename()), "-")[1];
			auto newFilename = fmt::format("{}-{}", text, extension);

			//	Then moves the file into the destined folder.
			std::filesystem::rename(m_tempFiles[ii], unzipPath / newFilename);
			log::debug("* FILE \"{}\" MOVED TO LOCATION.", newFilename);
		}

		//	If the icon was Vanilla, give the heads up.
		if (m_format == IconFormat::Vanilla)
		{
			auto popup = createQuickPopup(
				"Heads up",
				"The icon will be added, but treated in the <cp>More Icons</c> format due to it's rename (no longer works as Vanilla icon). Hope you understand.",
				"Ok",
				nullptr,
				[](bool ok, auto)
				{
					if (ok)
					{
						Notification::create("Icon Succesfully Added!", NotificationIcon::Success)->show();
						log::debug("* Icon succesfully renamed and exported.");
					}
				});
		}
		else
		{
			Notification::create("Icon Succesfully Added!", NotificationIcon::Success)->show();
			log::debug("* Icon succesfully renamed and exported.");
		}

		//	Cleans up the temporal files after the process is done.
		auto tempPath = Mod::get()->getConfigDir() / "temp" / geode::utils::string::replace(m_fileName, ".gdicon", "");
		std::error_code ec;
		std::filesystem::remove_all(tempPath, ec);
	}
}

//	Function to get the Unzip location
std::filesystem::path Icon::exp_getUnzipLocation(bool forceMoreIcons)
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
	std::filesystem::path unzipPath = packPath / "config" / MOREICONS / gamemode;

	//	Other path locations.
	if (m_format == IconFormat::Vanilla && !Mod::get()->getSettingValue<bool>("vanilla-more-icons") && !forceMoreIcons)
	{
		//	If the icon is Vanilla2 and the setting of "Vanilla2 as MI" isn't enabled.
		unzipPath = packPath / "icons";
		log::debug("* UNPACK_PATH: Assigned the Icons folder within Texture Pack.");
	}
	else if (m_format == IconFormat::Vanilla && Mod::get()->getSettingValue<bool>("resources-folder") && !forceMoreIcons)
	{
		//	If the icon is Vanilla2 and the setting of "Resources folder" is enabled.
		unzipPath = geode::dirs::getResourcesDir() / "icons";
		log::debug("* UNPACK_PATH: Assigned the Resources Folder.");
	}
	else if (Mod::get()->getSettingValue<bool>("more-icons-folder"))
	{
		//	If the More Icons folder is enabled.
		unzipPath = Loader::get()->getInstalledMod(MOREICONS)->getConfigDir() / gamemode;
		log::debug("* UNPACK_PATH: Assigned the More Icons (Mod) folders.");
	}

	//	Returns the selected location.
	return unzipPath;
};
