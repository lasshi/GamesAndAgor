#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "Engine.hpp"
#include "Inet/InetConnection.hpp"

#include "scenes/Game.hpp"
#include "scenes/IPDialog.hpp"
#include "scenes/NickDialog.hpp"

using namespace std;

enum TEST_STATES {
	JOINING, JOINING_ACK, NICKING, NICKING_ACK, GAME_RUNNING, GAME_ENDING, EXITING_GAME, EXITING_GAME_ACK
};

TEST_STATES testStates = TEST_STATES::JOINING;

void printGameStateAsString(){
	string stateAsString;
	switch (testStates) {
		case TEST_STATES::JOINING:
			stateAsString = "JOINING";
			break;
		case TEST_STATES::JOINING_ACK:
			stateAsString = "JOINING_ACK";
			break;
		case TEST_STATES::NICKING:
			stateAsString = "NICKING";
			break;
		case TEST_STATES::NICKING_ACK:
			stateAsString = "NICKING_ACK";
			break;
		case TEST_STATES::GAME_RUNNING:
			stateAsString = "GAME_RUNNING";
			break;
		case TEST_STATES::GAME_ENDING:
			stateAsString = "GAME_ENDING";
			break;
		case TEST_STATES::EXITING_GAME:
			stateAsString = "EXITING_GAME";
			break;
		case TEST_STATES::EXITING_GAME_ACK:
			stateAsString = "EXITING_GAME_ACK";
			break;
		default:
			stateAsString = "CANNOT DEFINE";
			break;
	}
	std::cout << "Entering state: " << stateAsString << std::endl;
}

void TestMessagesLoop() {
	cout << "USING MESSAGE TEST LOOP" << endl;
	testStates = TEST_STATES::JOINING;
	printGameStateAsString();

	InetConnection * connection = new InetConnection();
	connection->init();
	
//	connection->setIP("157.24.108.48");
	connection->setIP("127.0.0.1");
	connection->connectUDP();
	uint8_t testBuffer[BUFFER_SIZE];
	MessageHeader dummyGameMessageHeader = connection->createDummyHeader(0, 123123, MESSAGE_TYPE::GAME_MESSAGE, 0);
	dummyGameMessageHeader.user_id = 0;
	dummyGameMessageHeader.gameTime = 123123;
	Join * joinMessage = new Join(dummyGameMessageHeader);
	int messageLenght = joinMessage->PackSelf(testBuffer);
	connection->send(testBuffer, messageLenght);
	testStates = TEST_STATES::JOINING_ACK;
	printGameStateAsString();

	int loopCounter = 0;
	while (loopCounter < 1000) {
		std::cout << "==================================== LOOP START ====================================" << std::endl;
		connection->update();
		if (testStates != TEST_STATES::GAME_RUNNING) {
			vector<Message*> vmsgs = connection->getMessagesOfType(MESSAGE_TYPE::ACK);
			if (vmsgs.size() > 0) {
				for (auto& a : vmsgs) {
					MessageAck* ack = static_cast<MessageAck*>(a);
					memset(testBuffer, 0, BUFFER_SIZE);
					if (ack->getGameMessageType() == GAME_MESSAGE_TYPE::JOIN && testStates == TEST_STATES::JOINING_ACK) {
						JoinAck* joinAck = static_cast<JoinAck*>(ack);
						MessageHeader headerForNick = connection->createDummyHeader(joinAck->getUserID(), joinAck->getGameTime(),
								joinAck->getMessageType(), joinAck->getPayloadSize());
						connection->setID(joinAck->getUserID());
						testStates = TEST_STATES::NICKING;
						printGameStateAsString();

						memset(testBuffer, 0, BUFFER_SIZE);

						if (testStates == TEST_STATES::NICKING) {
							Nick * nick = new Nick(headerForNick, "Oskar");
							messageLenght = nick->PackSelf(testBuffer);
							connection->send(testBuffer, messageLenght);
							testStates = TEST_STATES::NICKING_ACK;
							printGameStateAsString();
						}

					} else if (ack->getGameMessageType() == GAME_MESSAGE_TYPE::NICK && testStates == TEST_STATES::NICKING_ACK) {
//						std::cout << "Main.cpp " << unsigned(ack->getMessageHeaderUserID()) << std::endl;
//						std::cout << "Main.cpp " << unsigned(ack->getMessageType()) << std::endl;
//						std::cout << "Main.cpp " << unsigned(ack->getGameMessageType()) << std::endl;
						NickAck * nickAck = static_cast<NickAck*>(ack);
						messageLenght = nickAck->PackSelf(testBuffer);
						connection->send(testBuffer, messageLenght);
						testStates = TEST_STATES::GAME_RUNNING;
						printGameStateAsString();
					}
				}
				continue;
			}
		}

		if (testStates == TEST_STATES::GAME_RUNNING) {

			if (connection->getGameEnding()) {
				testStates = TEST_STATES::GAME_ENDING;
				printGameStateAsString();
				continue;
			}

			if(loopCounter == 6){
				testStates = TEST_STATES::EXITING_GAME;
				printGameStateAsString();
				Exit exitMessage = Exit(connection->createDummyHeader(connection->getID(), 0, MESSAGE_TYPE::GAME_MESSAGE, 0));
				cout << "Main.cpp - Exit: " << exitMessage.getMessageHeaderUserID() << endl;
				cout << "Main.cpp - Exit: " << connection->getID() << endl;
				memset(testBuffer, 0, BUFFER_SIZE);
				int messageSize = exitMessage.PackSelf(testBuffer);
				connection ->send(testBuffer, messageSize);
				testStates = TEST_STATES::EXITING_GAME_ACK;
				printGameStateAsString();
			}

			std::vector<GameUpdate*> gameUpdates = connection->getGameUpdateMessages();
			if (gameUpdates.size() > 0) {
				for (auto& update : gameUpdates) {
					memset(testBuffer, 0, BUFFER_SIZE);
					std::cout << "Main.cpp - GameUpdate - Pos_X: " << update->getPosX() << std::endl;
					std::cout << "Main.cpp - GameUpdate - Pox_Y: " << update->getPosY() << std::endl;
					std::cout << "Main.cpp - GameUpdate - Dir_X: " << update->getDirX() << std::endl;
					std::cout << "Main.cpp - GameUpdate - Dir_Y: " << update->getDirY() << std::endl;
					std::cout << "Main.cpp - GameUpdate - NumberOfObjects: " << update->getNumberOfObjects() << std::endl;
					std::cout << "Main.cpp - GameUpdate - NumberOfPlayers: " << unsigned(update->getNumberOfPlayers()) << std::endl;
				}
				continue;
			}
		}

		if (testStates == TEST_STATES::GAME_ENDING) {
			std::cout << "Main.cpp - GAME_ENDING: Game ended by server" << std::endl;
			exit(0);
		}

		if(testStates == TEST_STATES::EXITING_GAME_ACK){
			std::vector<PlayerOut*> playerOuts = connection->getPLayerOutMessages();
			std::cout << "Main.cpp - EXITING_ACK: BOOBS" << std::endl;
			if(playerOuts.size() > 0){
				std::cout << "Main.cpp - EXITING: Game ended by user" << std::endl;
				PlayerOut* playerOut = playerOuts.front();
				PlayerOutAck playerOutAck = PlayerOutAck(connection->createDummyHeader(playerOut->getMessageHeaderUserID(), playerOut->getGameMessageType(), MESSAGE_TYPE::ACK, 0), playerOut->getMessageHeaderUserID());
				// Send three messages so most likely at least one will get there
				memset(testBuffer, 0, BUFFER_SIZE);
				int messageSize = playerOutAck.PackSelf(testBuffer);
				connection->send(testBuffer, messageSize);
				connection->send(testBuffer, messageSize);
				connection->send(testBuffer, messageSize);
				exit(0);
			}
		}


		std::cout << "==================================== LOOP END ====================================" << std::endl;
		loopCounter++;
	}
}

int main(void) {

#ifdef MESG_TEST
	TestMessagesLoop();
	return 0;
#endif
	std::cout << "Starting game" << std::endl;
	Engine* engine = new Engine();
	engine->addScenes( { { "Game", new Game() }, { "NickDialog", new NickDialog() }, { "IPDialog", new IPDialog() } });
	engine->run("IPDialog");
	delete engine;

	return 0;
}
