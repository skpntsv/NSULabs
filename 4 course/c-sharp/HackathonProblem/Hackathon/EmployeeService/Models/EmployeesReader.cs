using System.Globalization;
using CsvHelper;
using CsvHelper.Configuration;
using Shared.Model.Records;

namespace EmployeeService.Models;

public class EmployeesReader
{
    public List<Employee> ReadJuniors(string filePath)
    {
        return ReadEmployee(filePath, EmployeeTypes.Junior);
    }

    public List<Employee> ReadTeamLeads(string filePath)
    {
        return ReadEmployee(filePath, EmployeeTypes.TeamLead);
    }

    private record EmployeeRaw(int Id, string Name);

    private static List<Employee> ReadEmployee(string filePath, EmployeeTypes type)
    {
        using var reader = new StreamReader(filePath);
        using var csvReader =
            new CsvReader(reader, new CsvConfiguration(CultureInfo.CurrentCulture) { Delimiter = ";" });
        var employees = csvReader.GetRecords<EmployeeRaw>()
            .Select(e => new Employee(e.Id, e.Name, type))
            .ToList();

        return employees;
    }
}