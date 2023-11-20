using Microsoft.Extensions.Hosting;

namespace OpOverkillShared
{
    public sealed class ArduinoWorkerService : BackgroundService
    {
        private readonly IArduinoProcessor _arduinoProcessor;

        public ArduinoWorkerService(IArduinoProcessor arduinoProcessor)
        {
            _arduinoProcessor = arduinoProcessor ?? throw new ArgumentNullException(nameof(arduinoProcessor));
        }

        protected override async Task ExecuteAsync(CancellationToken stoppingToken)
        {
            while (!stoppingToken.IsCancellationRequested)
            {
                try
                {
                    _arduinoProcessor.Connect();
                }
                catch (Exception err)
                {
                    Console.WriteLine(err.Message);
                }

                await Task.Delay(1000, stoppingToken);
            }
        }
    }
}