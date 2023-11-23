using System.Net.Http.Headers;
using System.Text.Json;

namespace OpOverkillShared.Classes
{
    public sealed class ApiWrapper
    {
        private static readonly JsonSerializerOptions DefaultJsonSerializerOptions = new()
        {
            AllowTrailingCommas = true,
            WriteIndented = true,
        };

        private HttpClient CreateApiClient()
        {
            HttpClient httpClient = new();

            httpClient.DefaultRequestHeaders.Accept.Add(
                new MediaTypeWithQualityHeaderValue("application/json"));
            httpClient.DefaultRequestHeaders.UserAgent.Clear();
            httpClient.DefaultRequestHeaders.UserAgent.Add(
                new ProductInfoHeaderValue("OpOverkill", "1.0"));

#if DEBUG
            httpClient.Timeout = TimeSpan.FromMinutes(5);
#else
     
            httpClient.Timeout = TimeSpan.FromMilliseconds(2000);
#endif

            return httpClient;
        }

        public T CallGetApi<T>(Uri uri)
        {
            try
            {
                using HttpClient httpClient = CreateApiClient();
                string address = uri.ToString();

                using HttpResponseMessage response = httpClient.GetAsync(address).Result;

                if (!response.IsSuccessStatusCode)
                    return default;

                string jsonData = response.Content.ReadAsStringAsync().Result;

                if (String.IsNullOrWhiteSpace(jsonData))
                    return default;

                return (T)JsonSerializer.Deserialize(jsonData, typeof(T), DefaultJsonSerializerOptions);
            }
            catch
            {
                return default;
            }
        }
    }
}
