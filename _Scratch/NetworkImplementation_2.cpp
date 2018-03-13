namespace GameFacilities
{
	class GameServer : public NetServer
	{
	public: // NetServer:
		virtual void CreateSnapshot(ISerContainer* container) const;
	}
}

namespace Net
{
	class NetServer
	{
	public:
		// Creates a snapshot from the current scene state by serializing it into a
		// serialization container. As this container will be hashed bytewise to
		// accomplish delta-compression, this method should be idempotent.
		virtual void CreateSnapshot(ISerContainer* container) const = 0;

		// Periodically creates a snapshot of the scene that will be queued for
		// ConnectedClient::DoClientThread()s to be delta-compressed and sent to the clients.
		// Should be called from the server application's Update method
		//
		// TODO: Really have a thread per client?
		//	--> Apache DDoS issue?
		//
		void Update();

		// Listens on socket for incoming client messages and routes them to their
		// corresponding ConnectedClients
		void DoServerThread();
	}
}