using Hackathon.Contracts;
using Hackathon.Utils;

namespace HackathonTest;

public class WishlistTest
{
    [Fact]
    public void Wishlist_Should_Match_Employee_Count()
    {
        // Arrange
        string juniorsFile = "Resources/Juniors5.csv";
        string teamLeadsFile = "Resources/Teamleads5.csv";
        var juniors = EmployeesReader.ReadEmployees(juniorsFile);
        var teamLeads = EmployeesReader.ReadEmployees(teamLeadsFile);

        var rand = new Random();
        var juniorsWishlists = WishListGenerator.GenerateWishLists(juniors, teamLeads, rand);
        var teamleadsWishlists = WishListGenerator.GenerateWishLists(juniors, teamLeads, rand);
        
        // Assert
        Assert.All(juniorsWishlists, juniorsWishlist => Assert.Equal(teamLeads.Count(), juniorsWishlist.DesiredEmployees.Count()));
        Assert.All(teamleadsWishlists, teamleadsWishlist => Assert.Equal(juniors.Count(), teamleadsWishlist.DesiredEmployees.Count()));
    }

    [Fact]
    public void Wishlist_Should_Include_Specified_Employee()
    {
        // Arrange
        string juniorsFile = "Resources/Juniors5.csv";
        string teamLeadsFile = "Resources/Teamleads5.csv";
        var juniors = EmployeesReader.ReadEmployees(juniorsFile);
        var teamLeads = EmployeesReader.ReadEmployees(teamLeadsFile);

        var rand = new Random();
        var juniorsWishlists = WishListGenerator.GenerateWishLists(juniors, teamLeads, rand);
        var teamleadsWishlists = WishListGenerator.GenerateWishLists(juniors, teamLeads, rand);

        var specifiedEmployee = teamLeads.First();

        // Act
        var juniorWishlist = juniorsWishlists.First();

        // Assert
        Assert.Contains(specifiedEmployee.Id, juniorWishlist.DesiredEmployees);
    }
}
