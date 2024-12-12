namespace Hackathon.Core.Models;

public record Wishlist(int EmployeeId, int[] DesiredEmployees) {
    public Wishlist(Wishlist wishlist) 
    {
        EmployeeId = wishlist.EmployeeId;
        DesiredEmployees = wishlist.DesiredEmployees;
    }
        
    public override string ToString()
    {
        return $"WishList({EmployeeId})";
    }
}