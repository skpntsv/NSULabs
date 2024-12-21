namespace Hackathon.Core.Api.Records;

public record Wishlist(Employee Employee, IEnumerable<Employee> PreferredEmployees);