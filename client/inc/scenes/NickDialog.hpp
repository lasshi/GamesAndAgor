#ifndef _NICKDIALOG_HPP
#define _NICKDIALOG_HPP

#include <iostream>

#include "Engine.hpp"
#include "core/Scene.hpp"

class NickDialog : public Scene {
private:
protected:
public:
    inline NickDialog() {;}
    inline virtual ~NickDialog() {}
    inline void awake() {
        gui->addText("hint", new GUIText(Engine::window->getRenderer(), Engine::R->getFont("res/fonts/OpenSans.ttf")));
        gui->addInput("nick", new GUIInput(Engine::window->getRenderer(), Engine::R->getFont("res/fonts/OpenSans.ttf")));
        gui->getText("hint")->setText("Enter username:")->setX(Engine::camera->getWidth()/2.0f)->setY(Engine::camera->getHeight()/2.0f-30);
        gui->getText("hint")->setAlign(TEXT_ALIGN::CENTER_XY);
        gui->getInput("nick")->setMaxLength(15);
        gui->getInput("nick")->setX(Engine::camera->getWidth()/2.0f-200)->setY(Engine::camera->getHeight()/2.0f+30);
    }
    inline void update(float dt) {
        if(Engine::input->isKeyPressed(SDLK_RETURN)) {
            gui->getText("hint")->setText("Checking username availability");
            gui->getInput("nick")->hide();
            // put here if connect returns true
            uint8_t testBuffer[BUFFER_SIZE];
            MessageHeader dummyGameMessageHeader;
            dummyGameMessageHeader.user_id = 0;
            dummyGameMessageHeader.gameTime = 12;

            Nick* nick = new Nick(dummyGameMessageHeader, 
                (gui->getInput("nick")->getText()).c_str()
                );
            int messageLength = nick->PackSelf(testBuffer);
            Engine::connection->send(testBuffer, messageLength);
            // Engine::connection->sendUDP(GAME_MESSAGE_TYPE::NICK, gui->getText("hint")->getText());
        }
        /*
        switch(Engine::connection->getAck(GAME_MESSAGE_TYPE::NICK)) {
            case -1: break; // not acked yet
            case 0: // negative
                gui->getText("hint")->setText("This nick is already in use, choose another one");
                gui->getInput("nick")->show();
                break;
            case 1: // positive
                Engine::startScene("Game");
                break;
            default: break;
        }
        */
    }
    inline void end() {}
    inline void draw() {}
};

#endif
