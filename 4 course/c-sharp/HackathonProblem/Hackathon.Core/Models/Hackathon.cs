using System;
using System.Collections.Generic;
using System.Linq;

namespace Hackathon.Core.Models
{
    public class Hackathon
    {
        public List<Employee> TeamLeads { get; }
        public List<Employee> Juniors { get; }

        public Hackathon(string teamLeadsCsvPath, string juniorsCsvPath)
        {
            TeamLeads = CsvLoader.LoadEmployeesFromCsv(teamLeadsCsvPath, (id, name) => new Employee(id, name))
                .ToList();
            Juniors = CsvLoader.LoadEmployeesFromCsv(juniorsCsvPath, (id, name) => new Employee(id + 20, name))
                .ToList();

            if (TeamLeads.Count != Juniors.Count) 
            {
                throw new Exception("Тимлидов и джунов должно быть одинаковое количество!");
            }

            if (TeamLeads.Count == 0) 
            {
                throw new Exception("В хакатоне должно учавствовать хотябы 2 участника!");
            }
        }

        public Hackathon(List<Employee> teamLeads, List<Employee> juniors)
        {
            TeamLeads = teamLeads;
            Juniors = juniors;
        }
    }
}
