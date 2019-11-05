using System.Text.RegularExpressions;
using System.Threading;
using System;
using System.IO.Ports;
using Microsoft.Azure.EventHubs;
using System.Text;
using Newtonsoft.Json;
using System.Threading.Tasks;

namespace Azure203EventHubWithJSON
{
    class Program
    {
        private static EventHubClient eventHubClient;
        private const string EventHubConnectionString = "Endpoint=sb://sillahub.servicebus.windows.net/;SharedAccessKeyName=RootManageSharedAccessKey;SharedAccessKey=g8d3f9b3aO4lNS2zHY2CSdZrpdSQQBqFDIkaWMHO2IA=";
        private const string EventHubName = "hub01"; // Event Hub path/name
        
        static float temperature = 0.0f;
        static float humidity = 0.0f;

        static void Main(string[] args)
        {
            MainAsync(args).GetAwaiter().GetResult();
        }

        private static async Task MainAsync(string[] args)
        {
            var connectionStringBuilder = new EventHubsConnectionStringBuilder(EventHubConnectionString)
            {
                EntityPath = EventHubName
            };
            eventHubClient = EventHubClient.CreateFromConnectionString(connectionStringBuilder.ToString());
            
            StringBuilder builderA = new StringBuilder();

            while(true){
                try {
                    
                    Int32 unixTimestamp = (Int32)(DateTime.UtcNow.Subtract(new DateTime(1970, 1, 1))).TotalSeconds;

                    builderA.Append("{");

                    // device -> Partition Key
                    builderA.Append("\"device:\"");
                    builderA.Append("\"0001\"");
                    builderA.Append(",");

                    // time -> row Key
                    builderA.Append("\"time:\"");
                    builderA.Append(unixTimestamp);
                    builderA.Append(",");

                    // First Data
                    builderA.Append("\"temperature:\"");
                    builderA.Append(temperature+=1);
                    builderA.Append(",");

                    // Second Data
                    builderA.Append("\"humidity:\"");
                    builderA.Append(humidity+=1);

                    builderA.Append("}\n");

                    Console.WriteLine(builderA);
                    await SendMessagesToEventHub(builderA.ToString());
                    builderA.Clear();

                    await Task.Delay(1000); 

                } catch (TimeoutException) { }
            }
        }
        private static async Task SendMessagesToEventHub(string message)
        {         
            try {
                await eventHubClient.SendAsync(new EventData(Encoding.UTF8.GetBytes(message)));
            } catch (Exception exception) {
                Console.WriteLine($"{DateTime.Now} > Exception: {exception.Message}");
            }
        }
    }
}
