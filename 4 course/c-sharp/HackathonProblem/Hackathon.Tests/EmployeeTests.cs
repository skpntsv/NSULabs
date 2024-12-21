using Hackathon.Core.Models;

namespace Hackathon.Tests
{
    public class EmployeeTests
    {
        [Fact]
        public void GenerateWishlistSizesTest()
        {
            var employee = new Employee(1, "Partner 1");
            var predefinedPartner1 = new Employee(2, "Partner 2");
            var predefinedPartner2 = new Employee(3, "Partner 3");
            var predefinedPartner3 = new Employee(4, "Partner 4");
            var potentialPartners = new List<Employee>
            {
                predefinedPartner1,
                predefinedPartner2,
                predefinedPartner3
            };

            var wishlist = employee.GenerateWishlist(potentialPartners);

            Assert.Equal(potentialPartners.Count, wishlist.DesiredEmployees.Length);
        }

        [Fact]
        public void GenerateWishlistContainsEmployeeTest()
        {
            var employee = new Employee(1, "Partner 1");
            var predefinedPartner1 = new Employee(2, "Partner 2");
            var predefinedPartner2 = new Employee(3, "Partner 3");
            var predefinedPartner3 = new Employee(4, "Partner 4");
            var potentialPartners = new List<Employee>
            {
                predefinedPartner1,
                predefinedPartner2,
                predefinedPartner3
            };

            var wishlist = employee.GenerateWishlist(potentialPartners);

            Assert.Contains(predefinedPartner1.Id, wishlist.DesiredEmployees);
            Assert.Contains(predefinedPartner2.Id, wishlist.DesiredEmployees);
            Assert.Contains(predefinedPartner3.Id, wishlist.DesiredEmployees);
        }
    }
}
