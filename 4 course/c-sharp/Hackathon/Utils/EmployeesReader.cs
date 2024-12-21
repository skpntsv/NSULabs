using System.Globalization;
using CsvHelper;
using CsvHelper.Configuration;
using Hackathon.Contracts;

namespace Hackathon.Utils;

public static class EmployeesReader
{
    public static List<Employee> ReadEmployees(string filePath)
    {
        using var streamReader = new StreamReader(filePath);
        using var csvReader = new CsvReader(
            streamReader,
            new CsvConfiguration(CultureInfo.CurrentCulture) { Delimiter = ";" }
        );
        return csvReader.GetRecords<Employee>().ToList();
    }
}