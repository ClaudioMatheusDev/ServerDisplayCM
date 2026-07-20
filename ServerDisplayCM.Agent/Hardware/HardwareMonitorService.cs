using LibreHardwareMonitor.Hardware;
using ServerDisplayCM.Agent.Models;

namespace ServerDisplayCM.Agent.Hardware;

public sealed class HardwareMonitorService :
    IHardwareMonitorService,
    IDisposable
{
    private readonly Computer _computer;
    private readonly ILogger<HardwareMonitorService> _logger;
    private bool _inventoryLogged;

    public HardwareMonitorService(
        ILogger<HardwareMonitorService> logger)
    {
        _logger = logger;
        _computer = new Computer
        {
            IsCpuEnabled = true,
            IsMemoryEnabled = true,
            IsGpuEnabled = true
        };

        _computer.Open();
    }

    public ComputerMetrics GetMetrics()
    {
        LogHardwareInventoryIfNeeded();

        var metrics = new ComputerMetrics();

        foreach (var hardware in _computer.Hardware)
        {
            hardware.Update();

            foreach (var subHardware in hardware.SubHardware)
            {
                subHardware.Update();
            }

            switch (hardware.HardwareType)
            {
                case HardwareType.Cpu:
                    ReadCpu(hardware, metrics);
                    break;

                case HardwareType.Memory:
                    ReadMemory(hardware, metrics);
                    break;

                case HardwareType.GpuNvidia:
                case HardwareType.GpuAmd:
                case HardwareType.GpuIntel:
                    ReadGpu(hardware, metrics);
                    break;
            }
        }

        metrics.CollectedAt = DateTimeOffset.Now;

        return metrics;
    }

    private void LogHardwareInventoryIfNeeded()
    {
        if (_inventoryLogged)
        {
            return;
        }

        _inventoryLogged = true;

        foreach (var hardware in _computer.Hardware)
        {
            _logger.LogInformation(
                "Hardware detectado: {HardwareType} - {HardwareName}",
                hardware.HardwareType,
                hardware.Name);

            LogSensors(hardware);

            foreach (var subHardware in hardware.SubHardware)
            {
                _logger.LogInformation(
                    "Subhardware detectado: {HardwareType} - {HardwareName}",
                    subHardware.HardwareType,
                    subHardware.Name);

                LogSensors(subHardware);
            }
        }
    }

    private void LogSensors(IHardware hardware)
    {
        foreach (var sensor in hardware.Sensors)
        {
            _logger.LogInformation(
                "Sensor detectado em {HardwareName}: {SensorType} - {SensorName} = {SensorValue}",
                hardware.Name,
                sensor.SensorType,
                sensor.Name,
                sensor.Value);
        }
    }

    private static void ReadCpu(
        IHardware hardware,
        ComputerMetrics metrics)
    {
        metrics.Cpu.Name = hardware.Name;

        var totalLoad = hardware.Sensors.FirstOrDefault(sensor =>
            sensor.SensorType == SensorType.Load &&
            sensor.Name.Contains("CPU Total",
                StringComparison.OrdinalIgnoreCase));

        var packageTemperature = hardware.Sensors.FirstOrDefault(sensor =>
            sensor.SensorType == SensorType.Temperature &&
            (
                sensor.Name.Contains("Package",
                    StringComparison.OrdinalIgnoreCase) ||
                sensor.Name.Contains("Core",
                    StringComparison.OrdinalIgnoreCase)
            ));

        metrics.Cpu.UsagePercent = totalLoad?.Value ?? 0;
        metrics.Cpu.TemperatureCelsius =
            packageTemperature?.Value;
    }

    private static void ReadMemory(
        IHardware hardware,
        ComputerMetrics metrics)
    {
        var usedMemory = hardware.Sensors.FirstOrDefault(sensor =>
            sensor.SensorType == SensorType.Data &&
            sensor.Name.Contains("Used",
                StringComparison.OrdinalIgnoreCase));

        var availableMemory = hardware.Sensors.FirstOrDefault(sensor =>
            sensor.SensorType == SensorType.Data &&
            sensor.Name.Contains("Available",
                StringComparison.OrdinalIgnoreCase));

        var memoryLoad = hardware.Sensors.FirstOrDefault(sensor =>
            sensor.SensorType == SensorType.Load);

        var usedGb = usedMemory?.Value ?? 0;
        var availableGb = availableMemory?.Value ?? 0;

        metrics.Memory.UsedGb = usedGb;
        metrics.Memory.TotalGb = usedGb + availableGb;
        metrics.Memory.UsagePercent = memoryLoad?.Value ?? 0;
    }

    private void ReadGpu(
        IHardware hardware,
        ComputerMetrics metrics)
    {
        metrics.Gpu.Name = hardware.Name;

        var gpuLoad = hardware.Sensors
            .Where(sensor => sensor.SensorType == SensorType.Load)
            .OrderByDescending(sensor => sensor.Value ?? float.MinValue)
            .FirstOrDefault();

        var gpuTemperature = hardware.Sensors.FirstOrDefault(sensor =>
            sensor.SensorType == SensorType.Temperature &&
            sensor.Name.Contains("GPU Core",
                StringComparison.OrdinalIgnoreCase));

        metrics.Gpu.UsagePercent = gpuLoad?.Value;
        metrics.Gpu.TemperatureCelsius =
            gpuTemperature?.Value;

        if (gpuLoad is null)
        {
            _logger.LogInformation(
                "Nenhum sensor de Load foi encontrado para a GPU {GpuName}. Sensores disponíveis: {Sensors}",
                hardware.Name,
                string.Join(", ", hardware.Sensors.Select(sensor =>
                    $"{sensor.SensorType} - {sensor.Name}")));
        }
    }

    public void Dispose()
    {
        _computer.Close();
    }
}
