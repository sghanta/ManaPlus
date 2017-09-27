/*
 *  The ManaPlus Client
 *  Copyright (C) 2004-2009  The Mana World Development Team
 *  Copyright (C) 2009-2010  The Mana Developers
 *  Copyright (C) 2011-2017  The ManaPlus Developers
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

#include "gui/windows/charselectdialog.h"

#include "client.h"
#include "configuration.h"

#include "listeners/charrenamelistener.h"
#include "listeners/pincodelistener.h"

#include "gui/windows/charcreatedialog.h"
#include "gui/windows/chardeleteconfirm.h"
#include "gui/windows/editdialog.h"
#include "gui/windows/logindialog.h"
#include "gui/windows/okdialog.h"
#include "gui/windows/textdialog.h"

#include "gui/widgets/button.h"
#include "gui/widgets/characterdisplay.h"
#include "gui/widgets/characterviewnormal.h"
#include "gui/widgets/characterviewsmall.h"
#include "gui/widgets/containerplacer.h"
#include "gui/widgets/createwidget.h"

#include "net/character.h"
#include "net/charserverhandler.h"
#include "net/logindata.h"
#include "net/net.h"
#include "net/serverfeatures.h"

#include "utils/cast.h"
#include "utils/foreach.h"

#include "resources/db/unitsdb.h"

#include "debug.h"

// Character slots per row in the dialog
static const int SLOTS_PER_ROW = 5;

CharSelectDialog::CharSelectDialog(LoginData &data) :
    // TRANSLATORS: char select dialog name
    Window(strprintf(_("Account %s (last login time %s)"),
        data.username.c_str(), data.lastLogin.c_str()),
        Modal_false,
        nullptr,
        "char.xml"),
    ActionListener(),
    KeyListener(),
    mLoginData(&data),
    // TRANSLATORS: char select dialog. button.
    mSwitchLoginButton(new Button(this, _("Switch"), "switch", this)),
    // TRANSLATORS: char select dialog. button.
    mChangePasswordButton(new Button(this, _("Password"),
                          "change_password", this)),
    // TRANSLATORS: char select dialog. button.
    mPlayButton(new Button(this, _("Play"), "use", this)),
    // TRANSLATORS: char select dialog. button.
    mInfoButton(new Button(this, _("Info"), "info", this)),
    // TRANSLATORS: char select dialog. button.
    mDeleteButton(new Button(this, _("Delete"), "delete", this)),
    // TRANSLATORS: char select dialog. button.
    mRenameButton(nullptr),
    mCharacterView(nullptr),
    mCharacterEntries(0),
    mCharServerHandler(charServerHandler),
    mDeleteDialog(nullptr),
    mDeleteIndex(-1),
    mLocked(false),
    mSmallScreen(mainGraphics->getWidth() < 470
                 || mainGraphics->getHeight() < 370)
{
    setCloseButton(true);
    setFocusable(true);

    ContainerPlacer placer;
    placer = getPlacer(0, 0);

    placer(0, 0, mSwitchLoginButton);

    int n = 1;
    placer(n, 0, mChangePasswordButton);
    n ++;
    placer(n, 0, mDeleteButton);
    n ++;
#ifdef TMWA_SUPPORT
    if (Net::getNetworkType() != ServerType::TMWATHENA)
#endif
    {
        mRenameButton = new Button(this,
            // TRANSLATORS: character rename button
            _("Rename"),
            "rename",
            this);
        placer(n, 0, mRenameButton);
        n ++;
    }
    placer(n, 0, mInfoButton);
    n ++;

    for (int i = 0; i < CAST_S32(mLoginData->characterSlots); i++)
    {
        CharacterDisplay *const character = new CharacterDisplay(this, this);
        character->setVisible(Visible_false);
        mCharacterEntries.push_back(character);
    }

    placer(0, 2, mPlayButton);

    if (!mSmallScreen)
    {
        mCharacterView = new CharacterViewNormal(
            this, &mCharacterEntries, mPadding);
        placer(0, 1, mCharacterView, 10);
        int sz = 410 + 2 * mPadding;
        if (config.getIntValue("fontSize") > 18)
            sz = 500 + 2 * mPadding;
        const int width = mCharacterView->getWidth() + 2 * mPadding;
        if (sz < width)
            sz = width;
        if (sz > mainGraphics->getWidth())
            sz = mainGraphics->getWidth();
        reflowLayout(sz);
    }
    else
    {
        // TRANSLATORS: char select dialog name
        setCaption(strprintf(_("Account %s"), mLoginData->username.c_str()));
        mCharacterView = new CharacterViewSmall(
            this, &mCharacterEntries, mPadding);
        mCharacterView->setWidth(mainGraphics->getWidth()
            - 2 * getPadding());
        placer(0, 1, mCharacterView, 10);
        reflowLayout();
    }
    addKeyListener(this);
    center();

    charServerHandler->setCharSelectDialog(this);
    mCharacterView->show(0);
    updateState();
}

CharSelectDialog::~CharSelectDialog()
{
}

void CharSelectDialog::postInit()
{
    Window::postInit();
    setVisible(Visible_true);
    requestFocus();
    if (charServerHandler->isNeedCreatePin())
    {
        EditDialog *const dialog = CREATEWIDGETR(EditDialog,
            // TRANSLATORS: pin code dialog header.
            _("Please set new pincode"),
            "",
            "OK");
        dialog->addActionListener(&pincodeListener);
    }
}

void CharSelectDialog::action(const ActionEvent &event)
{
    // Check if a button of a character was pressed
    const Widget *const sourceParent = event.getSource()->getParent();
    int selected = -1;
    for (unsigned int i = 0, fsz = CAST_U32(
         mCharacterEntries.size());
         i < fsz;
         ++i)
    {
        if (mCharacterEntries[i] == sourceParent)
        {
            selected = i;
            mCharacterView->show(i);
            updateState();
            break;
        }
    }
    if (selected == -1)
        selected = mCharacterView->getSelected();

    const std::string &eventId = event.getId();

    if (selected >= 0)
    {
        if (eventId == "use")
        {
            use(selected);
            return;
        }
        else if (eventId == "delete"
                 && (mCharacterEntries[selected]->getCharacter() != nullptr))
        {
            CREATEWIDGET(CharDeleteConfirm, this, selected);
            return;
        }
        else if (eventId == "rename"
                 && (mCharacterEntries[selected]->getCharacter() != nullptr))
        {
            const LocalPlayer *const player = mCharacterEntries[
                selected]->getCharacter()->dummy;
            EditDialog *const dialog = CREATEWIDGETR(EditDialog,
                // TRANSLATORS: character rename dialog header.
                _("Please enter new name"),
                player->getName(),
                "OK");
            charRenameListener.setId(player->getId());
            charRenameListener.setDialog(dialog);
            dialog->addActionListener(&charRenameListener);
        }
        else if (eventId == "info")
        {
            Net::Character *const character = mCharacterEntries[
                selected]->getCharacter();
            if (character == nullptr)
                return;

            const LocalPlayer *const data = character->dummy;
            if (data == nullptr)
                return;

            const std::string strExp = toString(CAST_U64(
                character->data.mAttributes[Attributes::PLAYER_EXP]));
            const std::string msg = strprintf(
                // TRANSLATORS: char select dialog. player info message.
                _("Hp: %u/%u\nMp: %u/%u\nLevel: %u\n"
                "Experience: %s\nMoney: %s"),
                CAST_U32(
                character->data.mAttributes[Attributes::PLAYER_HP]),
                CAST_U32(
                character->data.mAttributes[Attributes::PLAYER_MAX_HP]),
                CAST_U32(
                character->data.mAttributes[Attributes::PLAYER_MP]),
                CAST_U32(
                character->data.mAttributes[Attributes::PLAYER_MAX_MP]),
                CAST_U32(
                character->data.mAttributes[Attributes::PLAYER_BASE_LEVEL]),
                strExp.c_str(),
                UnitsDb::formatCurrency(
                character->data.mAttributes[Attributes::MONEY]).c_str());
            CREATEWIDGET(OkDialog, data->getName(), msg,
                // TRANSLATORS: ok dialog button
                _("OK"),
                DialogType::SILENCE,
                Modal_true,
                ShowCenter_true,
                nullptr,
                260);
        }
    }
    if (eventId == "switch")
    {
        charServerHandler->clear();
        close();
    }
    else if (eventId == "change_password")
    {
        client->setState(State::CHANGEPASSWORD);
    }
    else if (eventId == "change_email")
    {
        client->setState(State::CHANGEEMAIL);
    }
    else if (eventId == "try delete character")
    {
        if ((mDeleteDialog != nullptr) && mDeleteIndex != -1)
        {
            if (serverFeatures->haveEmailOnDelete())
            {
                attemptCharacterDelete(mDeleteIndex, mDeleteDialog->getText());
                mDeleteDialog = nullptr;
            }
            else if (mDeleteDialog->getText() == LoginDialog::savedPassword)
            {
                attemptCharacterDelete(mDeleteIndex, "");
                mDeleteDialog = nullptr;
            }
            else
            {
                CREATEWIDGET(OkDialog,
                    // TRANSLATORS: error header
                    _("Error"),
                    // TRANSLATORS: error message
                    _("Incorrect password"),
                    // TRANSLATORS: ok dialog button
                    _("OK"),
                    DialogType::ERROR,
                    Modal_true,
                    ShowCenter_true,
                    nullptr,
                    260);
            }
        }
        mDeleteIndex = -1;
    }
}

void CharSelectDialog::use(const int selected)
{
    if ((mCharacterEntries[selected] != nullptr)
        && (mCharacterEntries[selected]->getCharacter() != nullptr))
    {
        attemptCharacterSelect(selected);
    }
    else
    {
        CharCreateDialog *const charCreateDialog =
            CREATEWIDGETR(CharCreateDialog, this, selected);
        mCharServerHandler->setCharCreateDialog(charCreateDialog);
    }
}

void CharSelectDialog::keyPressed(KeyEvent &event)
{
    const InputActionT actionId = event.getActionId();
    PRAGMA45(GCC diagnostic push)
    PRAGMA45(GCC diagnostic ignored "-Wswitch-enum")
    switch (actionId)
    {
        case InputAction::GUI_CANCEL:
            event.consume();
            action(ActionEvent(mSwitchLoginButton,
                mSwitchLoginButton->getActionEventId()));
            break;

        case InputAction::GUI_RIGHT:
        {
            event.consume();
            int idx = mCharacterView->getSelected();
            if (idx >= 0)
            {
                idx ++;
                if (idx == SLOTS_PER_ROW)
                    break;
                mCharacterView->show(idx);
                updateState();
            }
            break;
        }

        case InputAction::GUI_LEFT:
        {
            event.consume();
            int idx = mCharacterView->getSelected();
            if (idx >= 0)
            {
                if ((idx == 0) || idx == SLOTS_PER_ROW)
                    break;
                idx --;
                mCharacterView->show(idx);
                updateState();
            }
            break;
        }

        case InputAction::GUI_UP:
        {
            event.consume();
            int idx = mCharacterView->getSelected();
            if (idx >= 0)
            {
                if (idx < SLOTS_PER_ROW)
                    break;
                idx -= SLOTS_PER_ROW;
                mCharacterView->show(idx);
                updateState();
            }
            break;
        }

        case InputAction::GUI_DOWN:
        {
            event.consume();
            int idx = mCharacterView->getSelected();
            if (idx >= 0)
            {
                if (idx >= SLOTS_PER_ROW)
                    break;
                idx += SLOTS_PER_ROW;
                mCharacterView->show(idx);
                updateState();
            }
            break;
        }

        case InputAction::GUI_DELETE:
        {
            event.consume();
            const int idx = mCharacterView->getSelected();
            if (idx >= 0 && (mCharacterEntries[idx] != nullptr)
                && (mCharacterEntries[idx]->getCharacter() != nullptr))
            {
                CREATEWIDGET(CharDeleteConfirm, this, idx);
            }
            break;
        }

        case InputAction::GUI_SELECT:
        {
            event.consume();
            use(mCharacterView->getSelected());
            break;
        }
        default:
            break;
    }
    PRAGMA45(GCC diagnostic pop)
}

/**
 * Communicate character deletion to the server.
 */
void CharSelectDialog::attemptCharacterDelete(const int index,
                                              const std::string &email)
{
    if (mLocked)
        return;

    if (mCharacterEntries[index] != nullptr)
    {
        mCharServerHandler->deleteCharacter(
            mCharacterEntries[index]->getCharacter(),
            email);
    }
    lock();
}

void CharSelectDialog::askPasswordForDeletion(const int index)
{
    mDeleteIndex = index;
    if (serverFeatures->haveEmailOnDelete())
    {
        CREATEWIDGETV(mDeleteDialog, TextDialog,
            // TRANSLATORS: char deletion question.
            _("Enter your email for deleting character"),
            // TRANSLATORS: email label.
            _("Enter email:"),
            this, false);
    }
    else
    {
        CREATEWIDGETV(mDeleteDialog, TextDialog,
            // TRANSLATORS: char deletion question.
            _("Enter password for deleting character"),
            // TRANSLATORS: email label.
            _("Enter password:"),
            this, true);
    }
    mDeleteDialog->setActionEventId("try delete character");
    mDeleteDialog->addActionListener(this);
}

/**
 * Communicate character selection to the server.
 */
void CharSelectDialog::attemptCharacterSelect(const int index)
{
    if (mLocked || (mCharacterEntries[index] == nullptr))
        return;

    setVisible(Visible_false);
    if (mCharServerHandler != nullptr)
    {
        mCharServerHandler->chooseCharacter(
            mCharacterEntries[index]->getCharacter());
    }
    lock();
}

void CharSelectDialog::setCharacters(const Net::Characters &characters)
{
    // Reset previous characters
    FOR_EACH (STD_VECTOR<CharacterDisplay*>::const_iterator,
              iter, mCharacterEntries)
    {
        if (*iter != nullptr)
            (*iter)->setCharacter(nullptr);
    }

    FOR_EACH (Net::Characters::const_iterator, i, characters)
        setCharacter(*i);
    updateState();
}

void CharSelectDialog::setCharacter(Net::Character *const character)
{
    if (character == nullptr)
        return;
    const int characterSlot = character->slot;
    if (characterSlot >= CAST_S32(mCharacterEntries.size()))
    {
        logger->log("Warning: slot out of range: %d", character->slot);
        return;
    }

    if (mCharacterEntries[characterSlot] != nullptr)
        mCharacterEntries[characterSlot]->setCharacter(character);
}

void CharSelectDialog::lock()
{
    if (!mLocked)
        setLocked(true);
}

void CharSelectDialog::unlock()
{
    setLocked(false);
}

void CharSelectDialog::setLocked(const bool locked)
{
    mLocked = locked;

    if (mSwitchLoginButton != nullptr)
        mSwitchLoginButton->setEnabled(!locked);
    if (mChangePasswordButton != nullptr)
        mChangePasswordButton->setEnabled(!locked);
    mPlayButton->setEnabled(!locked);
    if (mDeleteButton != nullptr)
        mDeleteButton->setEnabled(!locked);

    for (size_t i = 0, sz = mCharacterEntries.size(); i < sz; ++i)
    {
        if (mCharacterEntries[i] != nullptr)
            mCharacterEntries[i]->setActive(!mLocked);
    }
}

bool CharSelectDialog::selectByName(const std::string &name,
                                    const SelectAction selAction)
{
    if (mLocked)
        return false;

    for (size_t i = 0, sz = mCharacterEntries.size(); i < sz; ++i)
    {
        if (mCharacterEntries[i] != nullptr)
        {
            const Net::Character *const character
                = mCharacterEntries[i]->getCharacter();
            if (character != nullptr)
            {
                if (character->dummy != nullptr &&
                    character->dummy->getName() == name)
                {
                    mCharacterView->show(CAST_S32(i));
                    updateState();
                    if (selAction == Choose)
                        attemptCharacterSelect(CAST_S32(i));
                    return true;
                }
            }
        }
    }

    return false;
}

void CharSelectDialog::close()
{
    client->setState(State::SWITCH_LOGIN);
    Window::close();
}

void CharSelectDialog::widgetResized(const Event &event)
{
    Window::widgetResized(event);
    mCharacterView->resize();
}

void CharSelectDialog::updateState()
{
    const int idx = mCharacterView->getSelected();
    if (idx == -1)
    {
        mPlayButton->setEnabled(false);
        return;
    }
    mPlayButton->setEnabled(true);

    if (mCharacterEntries[idx] != nullptr &&
        mCharacterEntries[idx]->getCharacter() != nullptr)
    {
        // TRANSLATORS: char select dialog. button.
        mPlayButton->setCaption(_("Play"));

        const LocalPlayer *const player = mCharacterEntries[
            idx]->getCharacter()->dummy;
        if ((player != nullptr) && (mRenameButton != nullptr))
            mRenameButton->setEnabled(player->getRename() ? true : false);
    }
    else
    {
        // TRANSLATORS: char select dialog. button.
        mPlayButton->setCaption(_("Create"));
    }
}

void CharSelectDialog::setName(const BeingId id, const std::string &newName)
{
    for (unsigned int i = 0, fsz = CAST_U32(
         mCharacterEntries.size());
         i < fsz;
         ++i)
    {
        if (mCharacterEntries[i] == nullptr)
            continue;
        CharacterDisplay *const character = mCharacterEntries[i];
        if (character == nullptr)
            continue;
        LocalPlayer *const player = character->getCharacter()->dummy;
        if ((player != nullptr) && player->getId() == id)
        {
            player->setName(newName);
            character->update();
            return;
        }
    }
}
