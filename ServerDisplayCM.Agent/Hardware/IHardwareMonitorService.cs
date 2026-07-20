using ServerDisplayCM.Agent.Models;

namespace ServerDisplayCM.Agent.Hardware;

public interface IHardwareMonitorService
{
    ComputerMetrics GetMetrics();
}