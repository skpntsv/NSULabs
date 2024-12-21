using Hackathon.Core.Models;

namespace Hackathon.Tests;

public class HrDirectorTests
{
    [Fact]
    public void CalculateMaxTeamsScoreTest()
    {
        var hrDirector = new HrDirector();

        var teams = new List<Team>
        {
            new(new Employee(1, "TeamLead 1"), new Employee(3, "Junior 1")),
            new(new Employee(2, "TeamLead 2"), new Employee(4, "Junior 2"))
        };

        var wishlists = new List<Wishlist>
        {
            new(1, [3, 4]),
            new(2, [4, 3]),
            new(3, [1, 2]),
            new(4, [2, 1])
        };

        hrDirector.ReceiveTeamsAndWishlists(teams, wishlists);

        var score = hrDirector.CalculateTeamsScore();

        Assert.Equal(2, score);
    }

    [Fact]
    public void CalculateMiddleTeamsScore()
    {
        var hrDirector = new HrDirector();

        var teams = new List<Team>
        {
            new(new Employee(1, "TeamLead 1"), new Employee(3, "Junior 1")),
            new(new Employee(2, "TeamLead 2"), new Employee(4, "Junior 2"))
        };

        var wishlists = new List<Wishlist>
        {
            new(1, [3, 4]),
            new(2, [3, 4]),
            new(3, [2, 1]),
            new(4, [2, 1])
        };

        hrDirector.ReceiveTeamsAndWishlists(teams, wishlists);

        var score = hrDirector.CalculateTeamsScore();

        Assert.InRange(score, 1.3332, 1.3334);
    }
}