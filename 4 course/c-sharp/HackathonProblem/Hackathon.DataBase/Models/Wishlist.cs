using System.ComponentModel.DataAnnotations;

namespace Hackathon.DataBase.Models;

public class Wishlist
{
    [Key]
    public int WishlistId { get; set; }

    public int EmployeeId { get; set; }

    public int[] DesiredEmployeeIds { get; set; } = Array.Empty<int>();

    public Wishlist() { }

    public Wishlist(int employeeId, int[] desiredEmployeeIds)
    {
        EmployeeId = employeeId;
        DesiredEmployeeIds = desiredEmployeeIds ?? Array.Empty<int>();
    }

    public override string ToString()
    {
        return $"Wishlist(EmployeeId: {EmployeeId}, DesiredEmployeeIds: [{string.Join(", ", DesiredEmployeeIds)}])";
    }
}