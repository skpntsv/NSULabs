namespace Shared.Model.DataBase;

public enum ExperimentStatus
{
    Undefined = 0,
    Opened = 1,
    InProgress = 2,
    Completed = 3,
    Failed = 4
}

public static class ExperimentStatusExtensions
{
    public static string ToString(this ExperimentStatus status)
    {
        return status switch
        {
            ExperimentStatus.Undefined => "Undefined",
            ExperimentStatus.Opened => "Opened",
            ExperimentStatus.InProgress => "In Progress",
            ExperimentStatus.Completed => "Completed",
            ExperimentStatus.Failed => "Failed",
            _ => "Unknown"
        };
    }
}