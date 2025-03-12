using System.ComponentModel.DataAnnotations;

namespace Hackathon.DataBase.Models;

public class Employee
{
    [Key]
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;

    public Employee() { }

    public Employee(int id, string name)
    {
        Id = id;
        Name = name;
    }

    public Wishlist GenerateWishlist(IEnumerable<Employee> potentialPartners)
    {
        var random = new Random();
        var shuffledPartners = potentialPartners
            .OrderBy(_ => random.Next())
            .Select(e => e.Id)
            .ToArray();

        return new Wishlist
        {
            EmployeeId = Id,
            DesiredEmployeeIds = shuffledPartners
        };
    }

    public override string ToString()
    {
        return $"Employee({Id} : {Name})";
    }
}