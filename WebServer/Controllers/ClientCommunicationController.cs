using System;
using System.Net.WebSockets;
using System.Threading.Tasks;

using OpOverkillShared;

using Microsoft.AspNetCore.Mvc;

using SharedPluginFeatures;
using Microsoft.Extensions.DependencyInjection;

namespace OpOverkill.Controllers
{
    public class ClientCommunicationController : BaseController
    {
        private readonly IProcessorMediator _processorMediator;

        public ClientCommunicationController(IProcessorMediator processorMediator)
        {
            _processorMediator = processorMediator ?? throw new ArgumentNullException(nameof(processorMediator));
        }

        [Route("/client2/")]
        [Route("/webclient")]
        public async Task ClientConnection2(string clientId)
        {
            if (HttpContext.WebSockets.IsWebSocketRequest)
            {
                try
                {
                    using WebSocket webSocket = await HttpContext.WebSockets.AcceptWebSocketAsync();
                    await _processorMediator.ProcessClientCommunications(webSocket, HttpContext.RequestServices.GetRequiredService<IArduinoProcessor>(), clientId);
                }
                catch (Exception)
                {
                    // do not throw the exception
                }
            }
            else
            {
                HttpContext.Response.StatusCode = SharedPluginFeatures.Constants.HtmlResponseBadRequest;
            }
        }
    }
}
