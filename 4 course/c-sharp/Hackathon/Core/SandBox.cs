using Hackathon.Contracts;
using Hackathon.Utils;

namespace Hackathon.Core;

public class SandBox(Hackathon hackathon)
{
    private const int HACKATHON_REPEATS = 1000;

    private readonly List<Employee> _juniors = EmployeesReader.ReadEmployees("resources/Juniors20.csv");
    private readonly List<Employee> _teamLeads = EmployeesReader.ReadEmployees("resources/Teamleads20.csv");

    public void Start()
    {
        foreach (var junior in _juniors)
        {
            Console.WriteLine($"Junior Id: {junior.Id}, Name: {junior.Name}");
        }

        foreach (var teamlead in _teamLeads)
        {
            Console.WriteLine($"Teamlead Id: {teamlead.Id}, Name: {teamlead.Name}");
        }
        
        var sumScore = 0.0;
        for (var i = 0; i < HACKATHON_REPEATS; i++)
        {
            var score = hackathon.RunHackathon(_teamLeads, _juniors);
            Console.WriteLine($"score [i={i}]: {score}");
            sumScore += score;
        }

        Console.WriteLine($"Average score: {sumScore / HACKATHON_REPEATS}");
    }
}