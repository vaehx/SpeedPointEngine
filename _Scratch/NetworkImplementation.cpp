namespace SpeedPoint
{
    struct IApplication
    {
        virtual void Update(float fTime) = 0;
    };

    namespace Net
    {
        namespace Common
        {
            // SPMX is the SpeedPoint Messsage Exchange Protocol
            struct SPMXHeader
            {
                unsigned short clientId;
                unsigned short messageId;
                unsigned short lastAckMessageId; // last acknowledged message id from other participant

                /*
                    0 - Invalid Packet (-> ignore)
                    1 - Connection request
                    2 - Connection acknowledgement
                    3 - Connection termination/refuse
                    4 - Connection ping request (also used as heartbeat, sent periodically to measure performance, e.g. each 5 seconds)
                    5 - Connection ping response
                    1000 - World Snapshot
                    2000 - Client command
                 */
                unsigned short messageType;
            }; 
        }

        // Client-Side implementations
        namespace Client
        {
            class Client
            {
            };
        }

        // Server-Side implementations
        namespace Server
        {
            struct ConnectedClientInfo
            {
                unsigned long address;
                unsigned short port; // Port on client side
            };

            class ConnectedClient
            {
            protected:
                ConnectedClientInfo m_Info;

                virtual void OnClientCommandReceived() = 0;
            public:
				// Listens to messages from the client and parses them and sends world snapshots
                void DoClientThread();
            };

            class Server
            {
            protected:
                vector<ConnectedClient*> m_ConnectedClients;
                int m_Socket;

                virtual ConnectedClient* CreateConnectedClientInstance(const ConnectedClientInfo& info) = 0;
            public:
                // Waits for client messages to arrive and passes them to higher-level processing threads (DoClientThread()) in a thread-safe queue.
                void DoServerThread();
            };

            


            void ConnectedClient::DoClientThread()
            {
				// - Wait for commands in receivedCommandQueue and call dispatchClientCommand() for each command (ONCE!!)
				//		- ... while still maintaining the snapshot interval! --> high resolution clock?
            }

            void Server::DoServerThread()
            {
				// - Wait for inbound messages and queue them to be processed by client threads
				// - Send snapshots periodically. Too many inbound messages must not alter the snapshot interval

				/*

						lastSnapshot = 0;
						for (;;)
						{
							currentTime = now();
							timeUntilNextSnapshot = min(SNAPSHOT_INTERVAL, lastSnapshot + SNAPSHOT_INTERVAL - currentTime);
							
							timeout = timeUntilNextSnapshot							
							if (select(socket, &readFds, timeout)) // returns true if there are messages on the socket, false if timed out
							{
								messageQueue.push_back(read(socket));
							}

							if (currentTime - lastSnapshot >= SNAPSHOT_INTERVAL)
							{
								sendSnapshot();
								lastSnapshot = currentTime;
							}
						}


				*/
            }
        }
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Game
{
    class Actor : public IComponent
    {
    public:
        virtual void Update(float fTime);
    };

    class Player : public Actor
    {
    public:
        virtual void Update(float fTime);
    };


    // This is the client application implementation
    class Game : public IApplication
    {
    public:
        virtual void Update(float fTime);
    }



    class ConnectedGameClient : public ConnectedClient
    {
    private:
        Player* m_pPlayer;
        KeyStates* m_KeyStates;
    protected:
        virtual void OnClientCommandReceived();
    public:
        ConnectedGameClient(const ConnectedClientInfo& info)
            : m_Info(info) {}

        void Update(float fTime);
    };

    class GameServer : public SpeedPoint::Net::Server::Server, public SpeedPoint::IApplication
    {
    protected:
        virtual SpeedPoint::Net::Server::ConnectedClient* CreateConnectedClientInstance(const SpeedPoint::Net::Server::ConnectedClientInfo& info);
    public:
        virtual void Update(float fTime);
    }





    void Player::Update(float fTime, KeyStates* keyStates)
    {
        if (IsClient())
        {
            // TODO: Send commands to server
        }
        else
        {
            if (keyStates->IsKeyPressed(KEY_MOVE_FORWARD))
                m_pEntity->pos += m_pEntity->GetForward() * m_Speed * fTime;
        }
    }



    void Game::Update(float fTime)
    {
        m_pPlayer->Update(fTime, m_pEngine->GetInputSystem()->GetKeyStates());
    }




    void ConnectedGameClient::Update(float fTime)
    {
        m_pPlayer->Update(fTime, m_KeyStates);
    }



    ConnectedClient* GameServer::CreateConnectedClientInstance(const ConnectedClientInfo& info)
    {
        return new ConnectedGameClient(info);
    }

    void GameServer::Update(float fTime)
    {
        for (auto itConnectedClient = m_ConnectedClients.begin(); itConnectedClient != m_ConnectedClients.end(); ++itConnectedClient)
        {
            itConnectedClient->Update(fTime);
        }
    }
}
