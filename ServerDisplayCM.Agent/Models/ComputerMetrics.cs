namespace ServerDisplayCM.Agent.Models;

public sealed class ComputerMetrics
{
    public string MachineName { get; set; } = Environment.MachineName;

    public CpuMetrics Cpu { get; set; } = new();

    public MemoryMetrics Memory { get; set; } = new();

    public GpuMetrics Gpu { get; set; } = new();

    public string Status { get; set; } = "online";

    public DateTimeOffset CollectedAt { get; set; } = DateTimeOffset.Now;
}

public sealed class CpuMetrics
{
    public float UsagePercent { get; set; }

    public float? TemperatureCelsius { get; set; }
}

public sealed class MemoryMetrics
{
    public float UsedGb { get; set; }

    public float TotalGb { get; set; }

    public float UsagePercent { get; set; }
}

public sealed class GpuMetrics
{
    public string Name { get; set; } = "Não encontrada";

    public float? UsagePercent { get; set; }

    public float? TemperatureCelsius { get; set; }
}
