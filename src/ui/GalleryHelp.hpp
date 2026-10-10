#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/loader/Mod.hpp>
using namespace geode::prelude;

class GalleryHelpPopup : public Popup
{
protected:
	MDTextArea *m_textArea = nullptr;
	CCMenu *m_selectMenu = nullptr;

	bool init();

	void onPage(CCObject *);
	void onLanguage(CCObject *);
	void setInfo();

public:
	unsigned int m_page = 0;
	bool m_spanish = false;

	static GalleryHelpPopup *create();
};

///	Yes, I moved all the text here because it's annoying
constexpr const char *aboutENG = R"(# About This
The <co>**Icon Gallery**</c> started as a website made to act as a museum of public <cp>custom icons</c>, so people could download them and add them in their games.

<cc>*Please be aware, that the servers of this mod might not be stable enough to handle many requests at once, so if Icons don't load properly in the gallery, this is the reason*</c>)";

constexpr const char *aboutSPA = R"(# Acerca de
La <co>**Icon Gallery**</c> empezo como una pagina web que actua como un museo de <cp>iconos custom</c> publicos, para que la gente pueda descargarlos y agregarlos en sus juegos.

<cc>*Ten en cuenta que es posible que los servidores de este mod no sean lo suficientemente estables para manejar muchas solicitudes al mismo tiempo, asi que si los iconos no se cargan correctamente en la galleria, esa es la razon.*</c>)";

constexpr const char *tutorialENG = R"(# How to Use
The mod will automatically set up a <cc>Texture Pack</c> upon start-up, where all the icons downloaded from the gallery will be saved by default.
***
### Icon Pack Folder:
In the settings (Gear button at the bottom right corner) there's a setting called <cy>**Icon Pack Folder**</c>.

If this wasn't set automatically, please set yourself to point a valid <cg>Texture Loader Pack</c>, so then you could start downloading icons.

The mod does **NOT** automatically apply the texture pack, so you have to do that yourself inside the <cg>Texture Loader</c> manager.
***
### Other Locations:
You can enable some extra locations where downloaded icons end up in the settings.

Locations such as the folders within the <cl>More Icons</c> Mod (in general) or the original resources folder of the game (for <cf>Vanilla</c> icons).

<cr>Be advised, that you could override icons that are already there</c>.)";

constexpr const char *tutorialSPA = R"(# Como Usar
El mod te crea automaticamente un <cc>Texture Pack</c> al inicio, ahi es donde todos los iconos descargados de la galeria seran guardados por default.
***
### Icon Pack Folder:
En la configuracion (Boton de engranaje a la esquina inferior derecha de la galeria) se encuentra la opcion de <cy>**Icon Pack Folder**</c>.

Si no se ha configurado automaticamente, asignale un <cg>Texture Pack del Loader</c> para que puedas comenzar a descargar iconos.

El mod **NO** aplica automaticamente el Texture Pack, tienes que hacerlo tu mismo dentro del manager de <cg>Texture Loader</c>.
***
### Otros Lugares:
En la configuracion, tu puedes activar otros lugares donde los iconos descargados terminaran si no quieres un texture pack.

Lugares como las carpetas dentro del <cl>Mod de More Icons</c> (en general) o los recursos originales del juego (para iconos <cf>Vanilla</c>).

<cr>Ten en cuenta, que podrias reemplazar iconos que ya esten ahi</c>.)";

constexpr const char *downloadENG = R"(# Downloading Icons
Once you find the icons you wish to download, simply click the \"<co>Download</c>\" button at the right side of the icon.
***
### Formats:
* **Icons with the <cf>Vanilla</c> format** are found inside the \"Icons\" folder of the Texture Pack.
* **Icons with the <cp>More Icons</c> format** are found inside their gamemode-specific folder of <cl>More Icons</c>.
***
### Vanilla as More Icons
If you have the <cy>**Vanilla as More Icons**</c> setting enabled, all icons you download will be treated as the <cp>More Icons</c> format.)";

constexpr const char *downloadSPA = R"(# Descargar Iconos
Si encuentras un icono que deseas descargar, presiona el boton de <co>Descarga</c> a la derecha del icono.
***
### Formatos:
* **Iconos con el formato \"<cf>Vanilla</c>\"** se encuentran en la carpeta de \"Icons\" del texture pack.
* **Iconos con el formato \"<cp>More Icons</c>\"** se encuentran en su carpeta especifica del Gamemode de <cl>More Icons</c>.
***
### Vanilla como More Icons
Si tienes la configuracion <cy>**Vanilla as More Icons**</c> activa, todos los iconos descargados se trataran como el formato <cp>More Icons</c>.)";

constexpr const char *uploadENG = R"(# Uploading Icons
If you wish to upload your own icons, visit the <cg>Website version of the Gallery</c> go to the submitting page (via the bottom right button at the website) and follow the steps to submission.

<cc>*Please* ***read the guidelines*** *and make sure your icon follows the right format. Any icon that breaks either will have higher chances of rejection.*</c>)";

constexpr const char *uploadSPA = R"(# Subir Iconos
Si deseas publicar tu propio icono aqui, visita la <cg>Pagina web de la Icon Gallery</c>, ve al boton inferior derecho de la misma para ir a la pagina de subir icono y sigues los pasos en como subir tus iconos.

<cc>*Por favor,* ***lee las Guidelines*** *de la galeria y asegurate que tu icono sigue el formato correcto. Cualquier icono que rompe ambas cosas tiene mayores riesgos de rechazo.*</c>)";

constexpr const char *faqENG = R"(# FAQ
This is a list of frequently asked questions, just in case you need to look for more help about the mod, join the discord server.
***
### I can't find the icons I downloaded.
Here are some possible solutions:
- Apply the texture pack. Go to <cg>Texture Loader</c> and move the pack named \"<co>**Downloaded Icons**</c>\" and refresh.
- Some icons use the \"<cf>Vanilla</c>\" format, thus they take place of existing vanilla designs.
- Reload the textures if you recently downloaded some.
***
### I want to report an Icon
If you came across with an icon that doesn't load properly, or has incorrect author credits, make sure to report it to the admins of the Gallery via the <cd>Discord Server</c>.
***
### I'm having connection issues
If the gallery doesn't load for the first time, <cg>leave and open it again to refresh it</c>.

If it keeps failing to load, please be patient. It's possible that you have internet issues, limited connection or a server-side issue of the Gallery.)";

constexpr const char *faqSPA = R"(# FAQ	
Esta es una lista de preguntas frecuentes, se ira actualizando al traves del tiempo. Si necesitas mas ayuda sobre el mod, unite al servidor de Discord.	
***
### No encuentro un icono que descargue	
Posibles soluciones:
- Aplica el texture pack. Ve al <cg>Texture Loader</c> y mueve el Pack llamado \"<co>**Downloaded Icons**</c>\" (Iconos descargados) a la columna derecha del loader y recarga las texturas.
- Algunos iconos pueden ser de formato \"<cf>Vanilla</c>\" y toman el lugar de iconos existentes del juego.
- Recarga las texturas si has descargado algun icono reciente.
***
### Quiero reportar un Icono	
Si te haz encontrado un icono que no carga, o cuyos creditos no son los correctos, reportalo a los admins de la Galeria en el <cd>Servidor de Discord</c>.	
***
### Tengo problemas de conexion	
Si la galeria no se carga la primera vez, <cg>sal de ella y abrela de nuevo para actualizarla</c>.

Si sigue sin cargarse, ten paciencia. Es posible que tengas problemas con internet, una conexion limitada o un problema del lado del servidor de la galeria.)";

constexpr const char *supportENG = R"(# Support
If you come across bugs with the mod itself or help in the development of the Icon Gallery, make sure to contact us through the [GitHub repository](https://github.com/MasterL500/IconGallery-geode) or through [Asterveila's Discord Server](https://discord.gg/dceY3uvGzD)
***
### Other Methods:
If you like the <co> Icon Gallery</c> and want to show some monetary support, you can do it by [Buying Aster a ko-fi](https://ko-fi.com/asterveila).

That's all for now, thank you for enjoying the Mod!
***
### Developers socials:
<cp>Asterveila</c>:
- [Twitter](https://x.com/Asterveila)
- [Youtube](https://www.youtube.com/@Asterveila)

<cj>ML500's</c>:
- [Twitter](https://x.com/MasterL500)
- [Youtube](https://www.youtube.com/@MasterL500))";

constexpr const char *supportSPA = R"(# Soporte
Si encuentras un bug del mod mismo o deseas ayudar el desarrollo de la galeria (ya sea el Mod o la pagina web), asegurate de contactarnos a traves del [Repositorio Github](https://github.com/MasterL500/IconGallery-geode) o por medio del [Servidor de Discord](https://discord.gg/dceY3uvGzD).
***
### Otros Metodos:
Si te gusta la <co>Icon Gallery</c> y quieres darle un soporte monetaria, puedes hacerlo al [Donar Aster en Ko-fi](https://ko-fi.com/asterveila).

Eso es todo, muchas gracias por disfrutar del Mod!
***
### Redes sociales de los devs:
<cp>Asterveila</c>:
- [Twitter](https://x.com/Asterveila)
- [Youtube](https://www.youtube.com/@Asterveila)

<cj>ML500's</c>:
- [Twitter](https://x.com/MasterL500)
- [Youtube](https://www.youtube.com/@MasterL500))";