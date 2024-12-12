namespace Hackathon.Core.Api;

public interface IWishlist
{
    public int EmployeeId { get; }
    public int[] DesiredEmployees { get; }
}