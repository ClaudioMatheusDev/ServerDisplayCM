using ServerDisplayCM.Agent.Hardware;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddSingleton<IHardwareMonitorService,
    HardwareMonitorService>();

var app = builder.Build();

app.MapGet("/", () => new
{
    application = "ServerDisplayCM.Agent",
    status = "online"
});

app.MapGet("/api/monitor",
    (IHardwareMonitorService hardwareMonitor) =>
{
    var metrics = hardwareMonitor.GetMetrics();

    return Results.Ok(metrics);
});

app.Run("http://0.0.0.0:5050");