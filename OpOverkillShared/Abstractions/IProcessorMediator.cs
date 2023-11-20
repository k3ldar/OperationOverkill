using System.Net.WebSockets;

namespace OpOverkillShared
{
    public interface IProcessorMediator
    {
        Task ProcessClientCommunications(WebSocket webSocket, IArduinoProcessor arduinoProcessor, string clientId);

        CancellationToken CancellationToken { get; }
    }
}
