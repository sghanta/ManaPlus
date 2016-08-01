/*
 *  The ManaPlus Client
 *  Copyright (C) 2013-2016  The ManaPlus Developers
 *
 *  This file is part of The ManaPlus Client.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "catch.hpp"
#include "client.h"
#include "configuration.h"
#include "graphicsmanager.h"
#include "settings.h"

#include "being/actorsprite.h"

#include "gui/gui.h"
#include "gui/userpalette.h"

#include "gui/popups/beingpopup.h"
#include "gui/popups/itempopup.h"
#include "gui/popups/popupmenu.h"
#include "gui/popups/spellpopup.h"
#include "gui/popups/textboxpopup.h"
#include "gui/popups/textpopup.h"

#include "gui/widgets/desktop.h"
#include "gui/widgets/createwidget.h"
#include "gui/widgets/windowcontainer.h"

#include "gui/windows/connectiondialog.h"
#include "gui/windows/didyouknowwindow.h"
#include "gui/windows/helpwindow.h"
#include "gui/windows/logindialog.h"
#include "gui/windows/setupwindow.h"
#include "gui/windows/serverdialog.h"

#include "input/touch/touchmanager.h"

#include "render/sdlgraphics.h"

#include "resources/sdlimagehelper.h"

#include "resources/resourcemanager/resourcemanager.h"

#include "utils/delete2.h"
#include "utils/env.h"
#include "utils/gettext.h"
#include "utils/physfstools.h"

#include "debug.h"

TEST_CASE("Windows tests", "windowmanager")
{
    setEnv("SDL_VIDEODRIVER", "dummy");

    client = new Client;
    PHYSFS_init("manaplus");
    dirSeparator = "/";
    XML::initXML();
    SDL_Init(SDL_INIT_VIDEO);
    logger = new Logger();
    ResourceManager::init();
    resourceManager->addToSearchPath("data", Append_false);
    resourceManager->addToSearchPath("../data", Append_false);
    branding.setValue("onlineServerFile", "test/serverlistplus.xml");
    mainGraphics = new SDLGraphics;
    imageHelper = new SDLImageHelper;
    userPalette = new UserPalette;
    theme = new Theme;
    ActorSprite::load();
    gui = new Gui();
    gui->postInit(mainGraphics);
    touchManager.init();

    mainGraphics->setVideoMode(640, 480, 1, 8, false, false, false, false);

    SECTION("setupWindow")
    {
        CREATEWIDGETV0(setupWindow, SetupWindow);
        delete2(setupWindow);
    }
    SECTION("helpWindow")
    {
        CREATEWIDGETV0(helpWindow, HelpWindow);
        delete2(helpWindow);
    }
    SECTION("didYouKnowWindow")
    {
        CREATEWIDGETV0(didYouKnowWindow, DidYouKnowWindow);
        delete2(didYouKnowWindow);
    }
    SECTION("popupMenu")
    {
        CREATEWIDGETV0(popupMenu, PopupMenu);
        delete2(popupMenu);
    }
    SECTION("beingPopup")
    {
        CREATEWIDGETV0(beingPopup, BeingPopup);
        delete2(beingPopup);
    }
    SECTION("textPopup")
    {
        CREATEWIDGETV0(textPopup, TextPopup);
        delete2(textPopup);
    }
    SECTION("textBoxPopup")
    {
        CREATEWIDGETV0(textBoxPopup, TextBoxPopup);
        delete2(textBoxPopup);
    }
    SECTION("itemPopup")
    {
        CREATEWIDGETV0(itemPopup, ItemPopup);
        delete2(itemPopup);
    }
    SECTION("spellPopup")
    {
        CREATEWIDGETV0(spellPopup, SpellPopup);
        delete2(spellPopup);
    }
    SECTION("desktop")
    {
        CREATEWIDGETV(desktop, Desktop, nullptr);
        delete2(desktop);
    }
    SECTION("serversDialog")
    {
        ServerInfo mCurrentServer;
        settings.configDir = PhysFs::getRealDir("test/serverlistplus.xml");;
        ServerDialog *serverDialog = CREATEWIDGETR(ServerDialog,
            &mCurrentServer,
            settings.configDir);
        delete2(serverDialog);
    }
    SECTION("connectionDialog")
    {
        ConnectionDialog *connectionDialog = CREATEWIDGETR(ConnectionDialog,
            // TRANSLATORS: connection dialog header
            _("Connecting to server"),
            State::SWITCH_SERVER);
        delete2(connectionDialog);
    }
    SECTION("loginDialog")
    {
        ServerInfo mCurrentServer;
        LoginDialog *loginDialog = CREATEWIDGETR(LoginDialog,
            loginData,
            &mCurrentServer,
            &settings.options.updateHost);
        delete2(loginDialog);
    }
    SECTION("connectionDialog")
    {
        ConnectionDialog *connectionDialog = CREATEWIDGETR(ConnectionDialog,
            // TRANSLATORS: connection dialog header
            _("Logging in"),
            State::SWITCH_SERVER);
        delete2(connectionDialog);
    }

    gui->draw();
    mainGraphics->updateScreen();

    delete2(userPalette);
    delete2(client);
    windowContainer = nullptr;
}
