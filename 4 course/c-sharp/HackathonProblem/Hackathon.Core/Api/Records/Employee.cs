using Hackathon.Core.Utils;

namespace Hackathon.Core.Api.Records;

public class Employee
{
    public Employee(int Id, string Name, EmployeeTypes Type)
    {
        this.Id = Id;
        this.Name = Name;
        this.Type = Type;
    }

    public int Id { get; set; }
    public string Name { get; set; }
    public EmployeeTypes Type { get; set; }
    public Wishlist GenerateWishList(IEnumerable<Employee> preferredEmployees)
    {
        return WishListGenerator.GenerateEmployeeWishList(this, preferredEmployees);
    }
}