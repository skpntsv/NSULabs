using Shared.Model.Records;

namespace Shared.Model.Utils;

public abstract class WishListGenerator
{
    public static Wishlist GenerateEmployeeWishList(Employee employee, IEnumerable<Employee> preferredEmployees)
    {
        var shuffledPreferredEmployees = preferredEmployees.ToArray();
        Random.Shared.Shuffle(shuffledPreferredEmployees);
        var wishList = new Wishlist(employee, shuffledPreferredEmployees);
        return wishList;
    }
}