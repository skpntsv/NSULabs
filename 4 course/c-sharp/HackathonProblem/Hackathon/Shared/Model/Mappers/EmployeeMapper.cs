using Shared.Model.DataBase;
using Shared.Model.Records;

namespace Shared.Model.Mappers;

public static class EmployeeMapper
{
    public static EmployeeDB ToEmployeeDB(this Employee employee, int experimentId, int hackathonCount)
    {
        ArgumentNullException.ThrowIfNull(employee);

        return new EmployeeDB
        {
            ExperimentId = experimentId,
            HackathonCount = hackathonCount,
            Employee = employee
        };
    }

    public static Employee ToEmployee(this EmployeeDB employeeDb)
    {
        ArgumentNullException.ThrowIfNull(employeeDb);

        return new Employee(employeeDb.Employee.Id, employeeDb.Employee.Name, employeeDb.Employee.Type);
    }
}