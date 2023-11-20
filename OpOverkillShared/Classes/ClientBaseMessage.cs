namespace OpOverkillShared
{
    public class ClientBaseMessage
    {
        public ClientBaseMessage()
        {
        }

        public ClientBaseMessage(string req)
        {
            success = true;
            request = req;
        }

        public ClientBaseMessage(string req, object msg)
            : this(req)
        {
            message = msg;
        }

        public string Identifier { get; set; }

        public bool success { get; set; }

        public string request { get; set; }

        public object message { get; set; }

        public decimal ServerCpuStatus { get; set; }

        public bool IsConnected { get; set; }
    }
}
