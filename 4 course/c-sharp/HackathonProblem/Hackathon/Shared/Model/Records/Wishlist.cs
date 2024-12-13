namespace Shared.Model.Records;

public record Wishlist(Employee Employee, IEnumerable<Employee> PreferredEmployees);