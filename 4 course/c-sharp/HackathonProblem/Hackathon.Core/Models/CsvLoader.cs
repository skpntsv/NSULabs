using System;
using System.Collections.Generic;
using System.IO;

namespace Hackathon.Core.Models;

public class CsvLoader
{
    public static List<Employee> LoadEmployeesFromCsv(string filePath, Func<int, string, Employee> employeeFactory)
    {
        var employees = new List<Employee>();
            
        using (var reader = new StreamReader(filePath))
        {
            reader.ReadLine();

            while (!reader.EndOfStream)
            {
                var line = reader.ReadLine();
                if (line == null) break;
                var values = line.Split(';');
                    
                int id = int.Parse(values[0]);
                string name = values[1];

                var employee = employeeFactory(id, name);
                employees.Add(employee);
            }
        }
            
        return employees;
    }

    public static List<string> LoadEmployeeNamesFromCsv(string filePath)
    {
        List<string> names = [];
        using (var reader = new StreamReader(filePath))
        {
            reader.ReadLine();

            while (!reader.EndOfStream)
            {
                var line = reader.ReadLine();
                if (line == null) break;
                var values = line.Split(';');
                    
                string name = values[1];
                names.Add(name);
            }
        }
            
        return names;
    }
}