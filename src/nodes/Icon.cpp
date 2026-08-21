#include "Icon.hpp"
#include "../ui/IconCell.hpp"

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
						unpackIcon();
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
									unpackIcon();
								}
							});
					}
				};
			}
			else
			{
				Notification::create("There was an error", NotificationIcon::Error)->show();
				log::error("Failed on loading data - {}", res.errorMessage());
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
	std::filesystem::path unzipDir = (m_format == IconFormat::MoreIcons) ? (path / "config" / "hiimjustin000.more_icons" / gamemode) : (path / "icons");

	//	Special locations (if their settings are enabled)
	if(m_format == IconFormat::Vanilla && Mod::get()->getSettingValue<bool>("resources-folder")){
		unzipDir = geode::dirs::getResourcesDir() / "icons";
	} else if(Mod::get()->getSettingValue<bool>("more-icons-folder")){
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