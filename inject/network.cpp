#include "network.h"

#include "th09address.h"

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "state.h"

#include <iostream>

RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
RakNet::AddressOrGUID clientAddress;

enum GameMessages
{
    ID_SEED_MESSAGE = ID_USER_PACKET_ENUM + 1,
    ID_INPUT_MESSAGE = ID_USER_PACKET_ENUM + 2,
    ID_MENU_INPUT_MESSAGE = ID_USER_PACKET_ENUM + 3,
    ID_HOST_SIDE_MESSAGE = ID_USER_PACKET_ENUM + 4
};

namespace th09mp {

    namespace network
    {
        std::map<unsigned int, unsigned short> inputStore;
        bool connected = false;
        bool isHost = false;
        PlayerSide hostSide = PlayerSide::Side_1P;

        void ProcessPacket(RakNet::Packet* packet)
        {
            unsigned char id;
            if ((unsigned char)packet->data[0] == ID_TIMESTAMP)
                id = (unsigned char)packet->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
            else
                id = (unsigned char)packet->data[0];

            RakNet::BitStream bsIn(packet->data, packet->length, false);
            bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

            switch (id)
            {
            case ID_INPUT_MESSAGE:
            {
                inputStore.clear();
                size_t numOfInputs;
                bsIn.Read(numOfInputs);
                for (size_t idx = 0; idx < numOfInputs; idx++)
                {
                    unsigned int frameNo;
                    unsigned short keys;
                    bsIn.Read(frameNo);
                    bsIn.Read(keys);

                    inputStore[frameNo] = keys;
                }
                break;
            }
            case ID_SEED_MESSAGE:
            {
                bsIn.Read(th09mp::address::globals_ver1_5->rng[0]);
                th09mp::address::globals_ver1_5->rng[1] = 0;
                connected = true;
                break;
            }
            case ID_HOST_SIDE_MESSAGE:
            {
                bsIn.Read(network::hostSide);
                break;
            }
            case ID_MENU_INPUT_MESSAGE:
            {
                unsigned int receivedSystemKeys;
                bsIn.Read(receivedSystemKeys);
                if (receivedSystemKeys != 0)
                {
                    std::cout << "Receiving keys " << receivedSystemKeys << std::endl;
                    // This function processes all packets at once, but input packets should be applied each frame
                    
                    SetInputState(PlayerSide::Side_3P, receivedSystemKeys, true/*setSystemKeys*/);
                    if (isHost)
                    {
                        if (hostSide == PlayerSide::Side_2P)
                            SetInputState(PlayerSide::Side_1P, receivedSystemKeys, true/*setSystemKeys*/);
                        else
                            SetInputState(PlayerSide::Side_2P, receivedSystemKeys, true/*setSystemKeys*/);
                    }
                    else
                    {
                        if (hostSide == PlayerSide::Side_2P)
                            SetInputState(PlayerSide::Side_2P, receivedSystemKeys, true/*setSystemKeys*/);
                        else
                            SetInputState(PlayerSide::Side_1P, receivedSystemKeys, true/*setSystemKeys*/);
                    }
                }

                break;
            }
            case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                printf("Another client has disconnected.\n");
                break;
            case ID_REMOTE_CONNECTION_LOST:
                printf("Another client has lost the connection.\n");
                break;
            case ID_REMOTE_NEW_INCOMING_CONNECTION:
            {
                printf("Another client has connected.\n");
                clientAddress = packet->systemAddress;
                break;
            }
            case ID_CONNECTION_REQUEST_ACCEPTED:
            {
                printf("Our connection request has been accepted.\n");
                clientAddress = packet->systemAddress;
                break;
            }
            case ID_NEW_INCOMING_CONNECTION:
            {
                printf("A connection is incoming.\n");
                clientAddress = packet->systemAddress;

                SendRngSeed(th09mp::address::globals_ver1_5->rng[0]);
                th09mp::address::globals_ver1_5->rng[1] = 0;

                SendSide((int)hostSide);

                connected = true;
                break;
            }
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                printf("The server is full.\n");
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                if (isHost) {
                    printf("A client has disconnected.\n");
                }
                else {
                    printf("We have been disconnected.\n");
                }
                break;
            case ID_CONNECTION_LOST:
                if (isHost) {
                    printf("A client lost the connection.\n");
                }
                else {
                    printf("Connection lost.\n");
                }
                break;
            default:
                printf("Message with identifier %i has arrived.\n", packet->data[0]);
                break;
            }
        }

        void SendSide(unsigned int side)
        {
            RakNet::BitStream bsOut;
            bsOut.Write((RakNet::MessageID)ID_HOST_SIDE_MESSAGE);
            bsOut.Write(side);
            peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientAddress, false);
        }

        void ProcessPackets()
        {
            RakNet::Packet* packet = nullptr;
            for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
            {
                ProcessPacket(packet);
            }
        }

        void SendInputData(std::map<unsigned int, unsigned short> inputData)
        {
            RakNet::BitStream bsOut;
            bsOut.Write((RakNet::MessageID)ID_INPUT_MESSAGE);
            bsOut.Write(inputData.size());
            for (auto const& data : inputData)
            {
                bsOut.Write(data.first);
                bsOut.Write(data.second);
            }

            peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientAddress, false);
        }

        void SendMenuInput(unsigned int message, unsigned int wParam, long lParam)
        {
            if (peer->GetConnectionState(clientAddress) == RakNet::ConnectionState::IS_CONNECTED)
            {
                RakNet::BitStream bsOut;
                bsOut.Write((RakNet::MessageID)ID_MENU_INPUT_MESSAGE);
                bsOut.Write(message);
                bsOut.Write(wParam);
                bsOut.Write(lParam);

                peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientAddress, false);
            }
        }

        void SendRngSeed(unsigned int seed)
        {
            RakNet::BitStream bsOut;
            bsOut.Write((RakNet::MessageID)ID_SEED_MESSAGE);
            bsOut.Write(seed);
            peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientAddress, false);
        }

        void SendSystemKeys(unsigned int systemKeys)
        {
            RakNet::BitStream bsOut;
            bsOut.Write((RakNet::MessageID)ID_MENU_INPUT_MESSAGE);
            bsOut.Write(systemKeys);

            if (systemKeys != 0)
                std::cout << "Sending keys " << systemKeys << std::endl;

            uint32_t retCode = peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, clientAddress, false);
            if (retCode == 0)
                std::cout << "Something went wrong!" << std::endl;
        }

        void Connect(std::string ip, int port)
        {
            peer->Startup(1, &RakNet::SocketDescriptor(), 1);
            peer->Connect(ip.c_str(), port, 0, 0);
            isHost = false;
        }

        void Host(int port)
        {
            peer->Startup(10, &RakNet::SocketDescriptor(port, 0), 1);
            peer->SetMaximumIncomingConnections(10);
            isHost = true;
        }

        int GetPing()
        {
            return peer->GetAveragePing(clientAddress.systemAddress);
        }
    }
}